#include "stdafx.h"
#include "stdlib.h"
#include <stdio.h> 
#include <winsock2.h>
#include "PassValue.h"

//call ws2_32.recv from 00397b83 - 00130000
//
//#define POE_ORIG_SEND_ADDR	0x00267E14
/*
#define POE_ORIG_SEND_ADDR	0x00267DD3
#define POE_ORIG_RECV_ADDR	0x00267ADE
#define POE_SERIALIZED_PACKET_ADDR	0x00267B6B
#define POE_NEW_SERIALIZED_PACKET_ADDR	0x002A75e0
*/
/*
///1.0.5.b
#define POE_ORIG_SEND_ADDR	0x00267C03
#define POE_ORIG_RECV_ADDR	0x0026790E
#define POE_SERIALIZED_PACKET_ADDR	0x0026799B
#define POE_NEW_SERIALIZED_PACKET_ADDR	0x002A7160
#define POE_OBJECT_TYPE_ADDR	0x20DAAA
#define POE_NUMBER_OF_SOCKETS_ADDR	0x22eed6
#define POE_SOCKET_LINK_ADDR	0x22f05d
#define POE_ITEM_QUALITY_ADDR	0x236ab2
#define POE_ITEM_TYPE_ADDR	0x24205B
*/

//1.06
#define POE_ORIG_SEND_ADDR	0x0026A243
#define POE_ORIG_RECV_ADDR	0x00269F4E
#define POE_SERIALIZED_PACKET_ADDR	0x00269FDB
#define POE_NEW_SERIALIZED_PACKET_ADDR	0X520940
#define POE_OBJECT_TYPE_ADDR	0x210DEA
#define POE_NUMBER_OF_SOCKETS_ADDR	0x232226
#define POE_SOCKET_LINK_ADDR	0x2323AD
#define POE_ITEM_QUALITY_ADDR	0x239812
#define POE_ITEM_TYPE_ADDR	0x244F4B

extern DWORD	get_dword(BYTE *);
extern void _showinfo(LPCTSTR lpszFormat,...);
extern HMODULE poehackModule;

void OrigSendPacketStub();
void OrigRecvPacketStub();
void SerializedPacketStub();
void NewSerializedPacketStub();
//void MonsterDeadLiveStub();
void ObjectTypeStub();
void NumberOfSocketStub();
void SocketLinkStub();
void ItemQualityStub();
void ItemTypeStub();

pPassValue _PassValueAddr = NULL;
VALUE	_value;

DWORD __fastcall GetOrigSendPacket(BYTE*,DWORD);
DWORD __fastcall GetOrigSendPacket(BYTE*,DWORD);
DWORD __fastcall GetOrigRecvPacket(BYTE*,DWORD);
DWORD __fastcall GetSerializedPacket(BYTE*,DWORD,WORD*);
DWORD __fastcall GetNewSerializedPacket();
DWORD __fastcall GetPoeObjectType(wchar_t*,DWORD);
DWORD __fastcall GetNumberOfSocket(DWORD);
DWORD __fastcall GetSocketLink(DWORD);
DWORD __fastcall GetItemQuality(char);
DWORD __fastcall GetItemType(wchar_t*);

//DWORD __fastcall MonsterDeadLive(DWORD);
void* d2memcpy(DWORD lpDest, DWORD lpSource, int len);
BOOL  Intercept(int instruction, DWORD lpSource, DWORD lpDest, int len);
extern void read_filter(void);
//extern BOOL is_useless(BYTE);


BOOL 	PoeIntercept(HWND hwnd)
{
	DWORD baseAddr =(DWORD) GetModuleHandle(NULL);

	Intercept(0xe8, baseAddr + POE_ORIG_SEND_ADDR, (DWORD)&OrigSendPacketStub, 6);

	Intercept(0xe8, baseAddr + POE_ORIG_RECV_ADDR, (DWORD)&OrigRecvPacketStub, 6);

	Intercept(0xe8, baseAddr + POE_SERIALIZED_PACKET_ADDR, (DWORD)&SerializedPacketStub, 7);

	Intercept(0xe8, baseAddr + POE_NEW_SERIALIZED_PACKET_ADDR, (DWORD)&NewSerializedPacketStub, 6);

	
	Intercept(0xe8, baseAddr + POE_OBJECT_TYPE_ADDR, (DWORD)&ObjectTypeStub, 6);

	Intercept(0xe8, baseAddr + POE_NUMBER_OF_SOCKETS_ADDR, (DWORD)&NumberOfSocketStub, 7);

	Intercept(0xe8, baseAddr + POE_SOCKET_LINK_ADDR, (DWORD)&SocketLinkStub, 5);

	Intercept(0xe8, baseAddr + POE_ITEM_QUALITY_ADDR, (DWORD)&ItemQualityStub, 7);

	Intercept(0xe8, baseAddr + POE_ITEM_TYPE_ADDR, (DWORD)&ItemTypeStub, 5);

	return TRUE;
}	
/*
BOOL PoeInterceptNewpacket(DWORD myCodeAddr, int len)
{
	DWORD baseAddr =(DWORD) GetModuleHandle(NULL);
	DWORD hostAddr = baseAddr + POE_NEW_SERIALIZED_PACKET_ADDR;

	_showinfo("hostAddr:%x --- myCodeAddr:%x",hostAddr,myCodeAddr);
	Intercept(0xe8, hostAddr, myCodeAddr, len);
	return 0;

}
BOOL PoeInterceptRecvAfterSerialize(DWORD myCodeAddr, int len)
{
	
	DWORD baseAddr =(DWORD) GetModuleHandle(NULL);
	DWORD hostAddr = baseAddr + POE_SERIALIZED_PACKET_ADDR;

	_showinfo("hostAddr:%x --- myCodeAddr:%x",hostAddr,myCodeAddr);
	Intercept(0xe8, hostAddr, myCodeAddr, len);
	return 0;
}

BOOL PoeInterceptWS2_32Recv(DWORD myCodeAddr, int len)
{

	DWORD baseAddr =(DWORD) GetModuleHandle(NULL);
	DWORD hostAddr = baseAddr + POE_ORIG_RECV_ADDR;

	_showinfo("hostAddr:%x --- myCodeAddr:%x",hostAddr,myCodeAddr);
	Intercept(0xe8, hostAddr, myCodeAddr, len);
	return 0;

}
*/
void __declspec(naked) ItemTypeStub()
{
	__asm {
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
		mov ecx,[edi+4]
		call GetItemType
		popad 
		ret

		nop
		nop
		nop
		nop
		nop

	}
}
void __declspec(naked) SocketLinkStub()
{
	__asm {
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
		mov ecx,[esp+0x58]
		call GetSocketLink
		popad 
		ret

		nop
		nop
		nop
		nop
		nop

	}
}

void __declspec(naked) ItemQualityStub()
{
	__asm {
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
		mov eax,[esp+0x33]
		xor ecx,ecx
		mov cl,al

		call GetItemQuality
		popad 
		ret

		nop
		nop
		nop
		nop
		nop

	}
}

void __declspec(naked) NumberOfSocketStub()
{
	__asm {
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
		mov ecx,eax
		call GetNumberOfSocket
		popad 
		ret

		nop
		nop
		nop
		nop
		nop

	}
}


void __declspec(naked) ObjectTypeStub()
{
	__asm {
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
		mov ecx,[esp+0x38]
		mov edx,ecx
		sub edx,4
		mov ecx,[ecx]
		mov edx,[edx]
		call GetPoeObjectType
		popad 
		ret

		nop
		nop
		nop
		nop
		nop

	}
}

void __declspec(naked) SerializedPacketStub()
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
		mov ecx,edx
		mov edx,ebx
		push esi
		call GetSerializedPacket
		popad 
		ret

		nop
		nop
		nop
		nop
		nop

	}
}

/*
void __declspec(naked) MonsterDeadLiveStub()
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
		pushad
		mov ecx,eax
		call MonsterDeadLive
		popad 
		ret

		nop
		nop
		nop
		nop
		nop

	}
}
*/
void __declspec(naked) NewSerializedPacketStub()
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
		pushad
		call GetNewSerializedPacket
		popad 
		ret

		nop
		nop
		nop
		nop
		nop

	}
}
void __declspec(naked) OrigRecvPacketStub()
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
		pushad
		mov eax, esi
		add eax, dword ptr ds:[esi+0x90]
		sub eax, edi
here:
		add eax,0x20098
		mov ecx,eax
		mov edx,edi


		call GetOrigRecvPacket
		popad 
		ret

		nop
		nop
		nop
		nop
		nop
	}
}


void __declspec(naked) OrigSendPacketStub()
{
	__asm {
		nop
		nop
		nop									
		nop
		nop
		nop
		pushad
		lea ecx, [ebx+0x9c]
		mov edx, [EBX+0x80]
		call GetOrigSendPacket
		popad 
		ret

		nop
		nop
		nop
		nop
		nop
	}
}

DWORD __fastcall GetItemType(wchar_t* wcPtr)
{
	if(!poehackModule) return 0;
//	DWORD	n=(DWORD)wcPtr;
//	_showinfo("item type:%x",n);
//	return 0;
	char type[1024];
	wcstombs(type,wcPtr, 1024);
//	if(type[0]==0) return 0;
//	_showinfo("itemtype: %s",type);
	_value.id = ITEM_TYPE;
	_value.value=type;
	if(_PassValueAddr) _PassValueAddr(&_value);

	_value.id = ITEM_TYPE;
	_value.value=type;
//	if(_PassValueAddr) _PassValueAddr(&_value);
	return 0;
}

DWORD __fastcall GetPoeObjectType(wchar_t* wcPtr,DWORD typeID)
{
	if(!poehackModule) return 0;

	char type[1024];
	wcstombs(type,wcPtr, 1024);
//	_showinfo("objtype: %s , id:%x",type,typeID);

	_value.id = POE_OBJECT_TYPE;
	_value.value=type;
	if(_PassValueAddr) _PassValueAddr(&_value);
	return 0;
/*
	if(!poehackModule) return 0;

	typedef void(*pPoeObjType)(char*);
	pPoeObjType p;
	p=(pPoeObjType)GetProcAddress(poehackModule,"PoeObjType");
	
	p(type);

	return 0;
*/
}
DWORD __fastcall GetNewSerializedPacket()
{
	if(!poehackModule) return 0;
	_value.id = NEW_SERIALIZED_PACKET;
	_value.value=NULL;
	if(_PassValueAddr) _PassValueAddr(&_value);
	return 0;
/*
	if(!poehackModule) return 0;

	typedef void(*pNewPacket)();
	pNewPacket p;
	p=(pNewPacket)GetProcAddress(poehackModule,"NewPacket");
	
	p();
	return 1;
*/
}

/*
DWORD __fastcall MonsterDeadLive(DWORD flag)
{
	if(!poehackModule) return 0;

	_showinfo("dead flag:%x",flag);
	return 1;
}
*/
DWORD __fastcall	GetSerializedPacket(BYTE* buf, DWORD len,WORD* esi)
{
	if(!poehackModule) return 0;

	char	t[100];
	if(!poehackModule) return len;

//	WORD n1=*(esi+0x88),n2=*(esi+0x90);
//	_showinfo("n1:%d, n2:%d, len:%d",n1,n2,len);

	if(buf[0]==0xf1)
		strcpy(t,"f1  111111111111111111111111111");
	if(buf[0]==0xf0)
		strcpy(t,"f0  00000000000000000000000000000");
	if(buf[0]==0xf2)
		strcpy(t,"f2  22222222222222222222222222222");
	if(len==1)
		strcpy(t,"len=1  xxxxxxxxxxxxxxxxxxxxxxxxxxx");
	typedef char*(*pProcessSerialRecvPacket)(BYTE*,DWORD);
	pProcessSerialRecvPacket p;
	p=(pProcessSerialRecvPacket)GetProcAddress(poehackModule,"ProcessSerialRecvPacket");
	if(p) _showinfo(p(buf,len));
	/*
	else _showinfo("xxx");		
	
	if(len<2) return len;
	read_filter();
	if(is_useless(buf[0])) return len;


	char tempbuf[10240],temp[10];
	if(len>100) len=100;
	tempbuf[0]=0;
	for(int i=0;i<len;i++)
	{
		sprintf(temp,"-%x",buf[i]);	
		strcat(tempbuf,temp);
	}
	_showinfo(tempbuf);
	*/
	return len;


}
DWORD __fastcall GetItemQuality(char n)
{
	if(!poehackModule) return 0;
	_value.id = ITEM_QUALITY;
	_value.value=&n;
	if(_PassValueAddr) _PassValueAddr(&_value);
//	_showinfo("item quality:%d",n);
	return 0;
}

DWORD __fastcall GetSocketLink(DWORD n)
{
	if(!poehackModule) return 0;
	_value.id = SOCKETS_LINK;
	_value.value=(char*)&n;
	if(_PassValueAddr) _PassValueAddr(&_value);
//	_showinfo("socket link:%d",n);
	return 0;
}

DWORD __fastcall	GetNumberOfSocket(DWORD n)
{
	if(!poehackModule) return 0;
	_value.id = NUMBER_OF_SOCKS;
	_value.value=(char*)&n;
	if(_PassValueAddr) _PassValueAddr(&_value);
//	_showinfo("socket:%d",n);
	return 0;
}
DWORD __fastcall	GetOrigSendPacket(BYTE* buf, DWORD len)
{
	if(!poehackModule) return len;
	typedef char*(*pProcessOrigSendPacket)(BYTE*,DWORD);
	pProcessOrigSendPacket p;
	p=(pProcessOrigSendPacket)GetProcAddress(poehackModule,"ProcessOrigSendPacket");
	
	if(p) p(buf,len);
	return 1;
	char tempbuf[10240],temp[10];
	sprintf(tempbuf,"send len:%d",len);
	if(len>100) len=100;
	for(DWORD i=0;i<len;i++)
	{
		sprintf(temp,"-%x",buf[i]);	
		strcat(tempbuf,temp);
	}
	_showinfo(tempbuf);

	return len;


}

DWORD __fastcall	GetOrigRecvPacket(BYTE* buf, DWORD len)
{
//	return 0;

	if(!poehackModule) return len;
	/*
	char	t[100];
	if(buf[0]==0xf1)
		strcpy(t,"f1  xxxxxxxxxxxxxxxxxxxxxxxxxxx");
	if(buf[0]==0xf0)
		strcpy(t,"f0  xxxxxxxxxxxxxxxxxxxxxxxxxxx");
	if(buf[0]==0xf2)
		strcpy(t,"f2  xxxxxxxxxxxxxxxxxxxxxxxxxxx");
	if(len==1)
		strcpy(t,"len=1  xxxxxxxxxxxxxxxxxxxxxxxxxxx");
	*/
	typedef char*(*pProcessOrigRecvPacket)(BYTE*,DWORD);
	pProcessOrigRecvPacket p;
	p=(pProcessOrigRecvPacket)GetProcAddress(poehackModule,"ProcessOrigRecvPacket");
	
	if(p) _showinfo(p(buf,len));
	/*
	else _showinfo("xxx");		
	
	if(len<2) return len;
	read_filter();
	if(is_useless(buf[0])) return len;


	char tempbuf[10240],temp[10];
	if(len>100) len=100;
	tempbuf[0]=0;
	for(int i=0;i<len;i++)
	{
		sprintf(temp,"-%x",buf[i]);	
		strcat(tempbuf,temp);
	}
	_showinfo(tempbuf);
	*/
	return len;


}

/////////////////////////////////////////////////////////////////////////////
// Intercept()
// --------------------------------------------------------------------------
// Intercepts a code location to route it somewhere else.
// Also inserts original code at the destination.
/////////////////////////////////////////////////////////////////////////////

BOOL  Intercept(int instruction, DWORD lpSource, DWORD lpDest, int len)
{

	BYTE* buffer = new BYTE[len];
	buffer[0] = instruction;
	*(DWORD*) (buffer + 1) = lpDest - (lpSource + 5);
	memset(buffer + 5, 0x90, len - 5);		// nops
	d2memcpy(lpDest, lpSource, len);		// Patch in old code to new location
	d2memcpy(lpSource, (DWORD) buffer, len);// Route old location to new
	delete [] buffer;

	return TRUE;
}
VOID*  d2memcpy(DWORD lpDest, DWORD lpSource, int len)
{
	DWORD oldSourceProt,oldDestProt=0;
	VirtualProtect((void*)lpSource,len,PAGE_EXECUTE_READWRITE,&oldSourceProt);
 	VirtualProtect((void*)lpDest,len,PAGE_EXECUTE_READWRITE,&oldDestProt);
	memcpy((void*)lpDest,(void*)lpSource,len);
	VirtualProtect((void*)lpDest,len,oldDestProt,&oldDestProt);
	VirtualProtect((void*)lpSource,len,oldSourceProt,&oldSourceProt);
	return (void*)lpDest;
};

