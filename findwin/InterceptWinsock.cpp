
#include "stdafx.h"
#include <stdio.h> 
#include <winsock2.h>
//#pragma comment(lib, "wsock32.lib") 

//CPU Disasm
//Address   Hex dump          Command                                  Comments
//00F32531    FF15 609C3401   CALL DWORD PTR DS:[<&WS2_32.#16>]
//00F32537    83F8 FF         CMP EAX,-1
//00F3253A    74 0B           JE SHORT 00F32547
//00F3253C    8B4D 10         MOV ECX,DWORD PTR SS:[EBP+10]
//00F3253F    8901            MOV DWORD PTR DS:[ECX],EAX
#define RECV_INTERCEPT_1	0x00F3253C
//62076DF1    83FF FF         CMP EDI,-1

#define INTERCEPT_WS_32_RECV	

#define COPY_PACKET_ADDR	0X00AD341D
//#define JUDGE_PACKET_ADDR	0X0041A80A
#define JUDGE_PACKET_ADDR	0X0041A817

#define RECVFROM_INTERCEPT_ADDR	0X71AD305C
#define FOG_1	0x6ff5d759
#define FOG_2	0x6ff5e000
#define NET_1	0X17e6cc6
#define NET_2	0x17e6d98
#define CDKEY	0x6FF2CA94

BOOL Intercept_Lotro(DWORD,DWORD,int);
BOOL Intercept_ws2_32_recv(DWORD,DWORD,int);
	void Intercept_Wsock32();	//make some room before return
BOOL InterceptWinsock();
void  RecvStub();
void  RecvFromStub();
void  GetLotroPacketStub();
void  GetPacketBeforeSendStub();
void  SavePacketStub();
void WriteToLog(int tag,BYTE* buf, int len, DWORD caller);
void PrintToLog(char *buf);
void ParsePacket(BYTE* buf, int len, DWORD caller);
void	savefile(LPCTSTR lpszFormat,...);
extern DWORD packet(int, BYTE*);
extern void save_packet(void);
extern void _showinfo(LPCTSTR lpszFormat,...);

DWORD fog1(BYTE*,int);
DWORD fog2(BYTE*,int);
DWORD net1(BYTE*,int);

////////////////
#define	NAME_PASS	1
#define SELECT_CHAR	2
#define IN_CHANNEL	3

DWORD	I_AM;
////////////// game tag
BOOL	is_in_game;
char _char_name[20];


#include <time.h>

BOOL Intercept_ws2_32_recv(DWORD dwAddressToIntercept, DWORD dwStub, int offset)
{

    int i;
    DWORD dwOldProtect;
    
    BYTE *pbTargetCode = (BYTE *) dwAddressToIntercept;
    BYTE *pbStub = (BYTE *) dwStub;

    VirtualProtect((void *) dwStub, offset, PAGE_WRITECOPY, &dwOldProtect);
	_showinfo("%x --- %x",(DWORD)pbStub,(DWORD)pbTargetCode);
	/*
    for (i=0;i<offset;i++)
	{
        *(pbStub+i) = *(pbTargetCode+i);
		_showinfo("%x", *(pbTargetCode+i));
	}
	*/
    VirtualProtect((void *) dwStub, offset, PAGE_EXECUTE, &dwOldProtect);

    pbTargetCode = (BYTE *) dwAddressToIntercept;

    // Overwrite the first 5 bytes of the target function
    VirtualProtect((void *) dwAddressToIntercept, offset, PAGE_WRITECOPY, &dwOldProtect);
	for(i=0;i<offset;i++)
	{
		*(pbTargetCode+i)=0x90;
	}
    *pbTargetCode++ = 0xE8;        // call 
    *((DWORD *)(pbTargetCode)) = dwStub-dwAddressToIntercept - 5;
//	*(DWORD*) (buffer + 1) = lpDest - (lpSource + 5);
    VirtualProtect((void *) dwAddressToIntercept, offset, PAGE_EXECUTE, &dwOldProtect);
    
    // Flush the instruction cache to make sure 
    // the modified code is executed.
    FlushInstructionCache(GetCurrentProcess(), NULL, NULL);
    return TRUE;
}

BOOL Intercept_Lotro(DWORD dwAddressToIntercept, DWORD dwStub, int offset)
{

    int i;
    DWORD dwOldProtect;
    
    BYTE *pbTargetCode = (BYTE *) dwAddressToIntercept;
    BYTE *pbStub = (BYTE *) dwStub;

    VirtualProtect((void *) dwStub, offset, PAGE_WRITECOPY, &dwOldProtect);
	_showinfo("%x --- %x",(DWORD)pbStub,(DWORD)pbTargetCode);
    for (i=0;i<offset;i++)
	{
        *(pbStub+i) = *(pbTargetCode+i);
		_showinfo("%x", *(pbTargetCode+i));
	}
    VirtualProtect((void *) dwStub, offset, PAGE_EXECUTE, &dwOldProtect);

    pbTargetCode = (BYTE *) dwAddressToIntercept;

    // Overwrite the first 5 bytes of the target function
    VirtualProtect((void *) dwAddressToIntercept, offset, PAGE_WRITECOPY, &dwOldProtect);
	for(i=0;i<offset;i++)
	{
		*(pbTargetCode+i)=0x90;
	}
    *pbTargetCode++ = 0xE8;        // call 
    *((DWORD *)(pbTargetCode)) = dwStub-dwAddressToIntercept - 5;
//	*(DWORD*) (buffer + 1) = lpDest - (lpSource + 5);
    VirtualProtect((void *) dwAddressToIntercept, offset, PAGE_EXECUTE, &dwOldProtect);
    
    // Flush the instruction cache to make sure 
    // the modified code is executed.
    FlushInstructionCache(GetCurrentProcess(), NULL, NULL);
    return TRUE;

}

BOOL	filtered(BYTE c1,BYTE c2)
{
//0x28 move
//0x23 move
//0x96 heartbeat, timer, len:0x34
//0x4a back to game, len:0x0a

//	if(c1==0x28 && c2==0x79) return TRUE;
//	if(c1==0x28 && c2==0x77) return TRUE;
//	if(c1==0x96 && c2==0x00) return TRUE;
//	return FALSE;
	FILE *fp=fopen("c:\\filter.txt","r");
	if(!fp) return FALSE;
	DWORD	a1,a2;
	BOOL	ret=FALSE;
	char	buf[100];
//	while(fscanf(fp,"%x %x",&a1,&a2)!=EOF)
	while(fgets(buf,10,fp))
	{
		sscanf(buf,"%x",&a1);

		if(a1==c1)
		{
			ret=TRUE;
			break;
		}
	}
	fclose(fp);
	return ret;
}
DWORD	get_dword(BYTE *a)
{
	return DWORD(a[3])+DWORD(a[2])*256+DWORD(a[1])*256*256+DWORD(a[0])*256*256*256;
	
}
DWORD __fastcall	GetLotroPacket(BYTE *buf,DWORD len)
{
//	packet(len,buf);
//	return len;
//	_showinfo("got one");
	if(len<8) return len;
	if(filtered(buf[4],buf[5]))
		return len;

	char tempbuf[10240];
	sprintf(tempbuf,"packet length:%x",len);
	if(len>0x100) len=0x100;
	for(int i=0;i<len;i++)
		sprintf(tempbuf,"%s-%x",tempbuf,buf[i]);
	if(buf[4]==0x28 && len==0x24)	//moving
	{
			DWORD	a1,a2,a3;
			static DWORD d1,d2,d3;
			a1=get_dword(buf+13);
			a2=get_dword(buf+26);
			a3=get_dword(buf+32);
		
//		sprintf(tempbuf,"---%d--%d--%d",a1,a2,a3);
		sprintf(tempbuf,"---%d--%d--%d--diff:%d--%d--%d",a1,a2,a3,a1-d1,a2-d2,a3-d3);
			d1=a1;
			d2=a2;
			d3=a3;
	}
	_showinfo(tempbuf);
//	_showinfo("info:%x, %x",DWORD(*buf),len);
	return len;
//	return packet(len,buf);;
}
DWORD __fastcall	SavePacket()
{
	save_packet();

	return 1;
}


void __declspec(naked) SavePacketStub()
{
	__asm {
		//first 5 bytes of targecode
		nop									
		nop
		nop
		nop
		nop
		nop
		pushad
		call SavePacket
		popad 
		ret

		nop
		nop
		nop
		nop
		nop

	}
}

void __declspec(naked) GetPacketBeforeSendStub()
{
	__asm {
		//first 5 bytes of targecode
		nop									
		nop
		nop
		nop
		nop
		nop
		nop
		nop
		nop									
		nop
		nop
		nop
		nop
		nop
		nop
		nop
		pushad
		mov edx, eax
		mov ecx, [esp+0x2c]
//		mov edx, esi
//		call GetPacketBeforeSend
//		cmp eax,0
//		jz  not_found
//		mov eax,0
		// Return to game
//not_found:				
		popad 
		ret

		nop
		nop
		nop
		nop
		nop

//		pushad
//		mov edx, esi
//		mov ecx, edx
//		call GetLotroPacket
//		popad 
//		ret
//		nop
	}
}


void __declspec(naked) GetLotroPacketStub()
{
	__asm {
		//first 5 bytes of targecode
		nop									
		nop
		nop
		nop
		nop
		nop
		nop
		nop
		nop									
		nop
		nop
		nop
		nop
		nop
		nop
		nop
		pushad
		mov edx, eax
		mov ecx, [esp+0x2c]
//		mov edx, esi
		call GetLotroPacket
//		cmp eax,0
//		jz  not_found
//		mov eax,0
		// Return to game
//not_found:				
		popad 
		ret

		nop
		nop
		nop
		nop
		nop

//		pushad
//		mov edx, esi
//		mov ecx, edx
//		call GetLotroPacket
//		popad 
//		ret
//		nop
	}
}

BOOL	InterceptWS2_32Send()
{
	Intercept_Lotro(RECV_INTERCEPT_1, (DWORD)&GetPacketBeforeSendStub, 5);
		return TRUE;
}
BOOL	InterceptWS2_32Recv()
{
		//can not intercept direct to recv because first 5 bytes of recv have modified bp. good thing is, recv has 5 empty bytes upfront
	/*
        DWORD InterceptAddr = (DWORD)GetProcAddress(GetModuleHandle((char*)"ws2_32.dll"), (char*)"recv")+11;
		Intercept_Lotro(InterceptAddr, (DWORD)&GetLotroPacketStub, 10);
	*/
	Intercept_Lotro(RECV_INTERCEPT_1, (DWORD)&GetLotroPacketStub, 5);
		return TRUE;
}
/*
battle.net.dll

CPU Disasm
Address   Hex dump          Command                                  Comments
62076DDF    FF15 B0F91662   CALL DWORD PTR DS:[<&WS2_32.#16>]
62076DE5    8B5C24 20       MOV EBX,DWORD PTR SS:[ESP+20]
62076DE9    8BF8            MOV EDI,EAX
62076DEB    C703 00000000   MOV DWORD PTR DS:[EBX],0
62076DF1    83FF FF         CMP EDI,-1
62076DF4    75 47           JNE SHORT 62076E3D
62076DF6    55              PUSH EBP
62076DF7    FF15 A8F91662   CALL DWORD PTR DS:[<&WS2_32.#111>]
62076DFD    8BE8            MOV EBP,EAX
62076DFF    81FD 33270000   CMP EBP,2733
62076E05    75 0D           JNE SHORT 62076E14
62076E07    5D              POP EBP

*/
BOOL 	InterceptWinsock()
{

//_showinfo("ok");

	InterceptWS2_32Recv();
//	InterceptWS2_32Send();
		return TRUE;
}



void	savefile(LPCTSTR lpszFormat,...)
{
	char _buf[4096];
	va_list args;
	va_start(args, lpszFormat);
	FILE *fp;
	fp=fopen("\\recv.txt","a");

	char	time_str[1000];
	struct tm	*timeinfo;
	
	time_t ltime;
	time( &ltime );
	
	timeinfo=localtime(&ltime);

	_strdate( time_str );
	_strtime( time_str );

	strftime (time_str,1000,"%w %H %M",timeinfo);
	

/*	_strdate( time_str );
 
	fwrite(time_str, strlen(time_str),1,fp );
	fwrite(" ", 1, 1, fp );
*/	_strtime( time_str );
	fwrite(time_str, strlen(time_str),1,fp );
	fwrite(" ", 1, 1, fp );



	_vsnprintf(_buf,(sizeof(_buf)/sizeof(_buf[0])),lpszFormat,args);
	
	fprintf(fp,"%s\n",_buf);
	fclose(fp);
	va_end(args);
}
