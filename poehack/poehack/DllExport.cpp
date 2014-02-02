#include "stdafx.h"
#include "Dbghelp.h"
#include "memory.h"
#include "time.h"
#include "myCode.h"
#include "PassValue.h"
#include "iat.h"

#define CLASSNAME	"Direct3DWindowClass"
extern "C" __declspec(dllexport) char* ProcessSerialRecvPacket(BYTE*,DWORD);
extern "C" __declspec(dllexport) char* ProcessOrigRecvPacket(BYTE*,DWORD);
extern "C" __declspec(dllexport) char* ProcessOrigSendPacket(BYTE*,DWORD);
//extern "C" __declspec(dllexport) char* GetPlayerStatus();
//extern "C" __declspec(dllexport) char* GetMessageBuffer();
/*
extern "C" __declspec(dllexport) void NewPacket();
extern "C" __declspec(dllexport) void PoeObjType(char*);
*/
extern "C" __declspec(dllexport) void PassValue(VALUE*);
extern void MyLoop();
void Unload(); 
HWND _chwnd;

char * ProcessOrigSendPacket(BYTE *aPacket, DWORD len)
{
	ProcessPacketBeforeSend(aPacket,len);
	return NULL;
}
/*
void NewPacket()
{
	if(_action.comm == 0xf0) process_f0(NULL,0);
	_action.comm=0;
	_action.count=0;
	memset(&_monster,0,sizeof(MONSTER));
	//showinfo("new packet");
}

void PoeObjType(char* type)
{
//	showinfo(type);
	strcpy(_objTypeStr,type);
}
*/
void PassValue(VALUE *v)
{
//	showinfo(type);
	switch(v->id)
	{
		case POE_OBJECT_TYPE:
			if(_objTypeStr[0]==0 && _monster.object_type_id!=0)
			{
				strcpy(_objTypeStr,v->value);
				writeObjectType(_monster.object_type_id,_objTypeStr);
				showinfo("write to ini: %s",_objTypeStr);
			}
			break;
		case NEW_SERIALIZED_PACKET:
			if(_action.comm == 0xf0) process_f0(NULL,0);
			_action.comm=0;
			_action.count=0;
			_objTypeStr[0]=0;
			_itemTypeStr[0]=0;
			memset(&_monster,0,sizeof(MONSTER));
			break;
		case NUMBER_OF_SOCKS:
			_monster.total_sockets = v->value[0];
			if(_action.comm == 0xf0) process_f0(NULL,1);
			break;
		case SOCKETS_LINK:
			_monster.sockets_link = v->value[0];
			break;
		case ITEM_QUALITY:
			_monster.quality = v->value[0];
			break;
		case ITEM_TYPE:
			if(_itemTypeStr[0]==0 && _monster.item_type_id!=0)
			{
				strcpy(_itemTypeStr, v->value);
				writeItemType(_monster.item_type_id,_itemTypeStr);
				showinfo("write to ini: %s",_itemTypeStr);
			}
			break;
		case DO_UNLOAD:
			Unload();
			break;
	}
}

//void showinfo(LPCTSTR lpszFormat,...);

/*
//called from findwin.dll
void newCommand(BYTE*,BYTE*,MONSTER*);
void newCommand(BYTE *action,BYTE *command,MONSTER* monster)
{
	*action=0;
	*command=0;
	memset(monster,0,sizeof(MONSTER));
	return;
}
*/
char* ProcessSerialRecvPacket(BYTE* aPacket, DWORD len)
{
	char	tempbuf[10240];
	if(len>10240) len=10240;
	static char show_buf[10240*2]="";

	if(len==1 && aPacket[0]==0x0c) return NULL;
	if(_action.comm==0)
	{
		if(_show=='r') {showinfo(show_buf);}
		_action.comm=aPacket[0];
		_action.count = 0;
		show_buf[0]=0;
//		showinfo("new _action: %x",_action.comm);
	}

	FormatOutput((char*)aPacket,tempbuf,len);
	if(tempbuf[0]!=0) tempbuf[0]='*';
	strcat(show_buf,tempbuf);

	switch(_action.comm)
	{	/*
		case 0x3c://player update
			break;
		case 0x34:
			break;
		case 0x5d:
			//show_flag=TRUE;
			//strcat_s(_tempbuf,"object die\n");
			//MonsterDie(aPacket,len);
			break;
			*/
		case 0x07:	//someone talk
			break;
		case 0x1a://change act
			memset(&_player,0,sizeof(PLAYER));
			InitMonsterMap();
			break;
		case 0x22:	//interact with wp,stash
			break;
		case 0x2d:	//interact open, list wps,stash
			break;
		case 0x5b://pick up something
			break;
		case 0x5c://stash opened
			break;
		case 0x60://stash list end
			break;
		case 0xf0:	//new object
			process_f0(aPacket,len);
			if(_show=='0') {FormatOutput((char*)aPacket,tempbuf,len); showinfo(tempbuf);}
			break;
		case 0xf1:		//update
			process_f1(aPacket,len);
			if(_show=='1') {FormatOutput((char*)aPacket,tempbuf,len); showinfo(tempbuf);}
			break;
		case 0xf2:
			process_f2(aPacket,len);
			if(_show=='2') {FormatOutput((char*)aPacket,tempbuf,len); showinfo(tempbuf);}
			break;
		default:
			if(_show=='a')
			{
				if(_action.count==0) showinfo("xxxxxxxx");
				FormatOutput((char*)aPacket,tempbuf,len); showinfo(tempbuf);
			}			
			break;
	}
	_action.count++;
//	if(_show=='a') {FormatOutput((char*)aPacket,tempbuf,len); showinfo(tempbuf);}
	return NULL;
}
int myHookFunc()
{
	return 0;
}


void MyTimer1()
{
	static DWORD tick=0;
	if(tick % 5==0 ) MyLoop();
	tick++;
}

void init()
{
	_hwnd = FindWindow(CLASSNAME,NULL);	//POE hwnd
	_chwnd = FindWindow("MainWClass",NULL);
	_hmodule = GetModuleHandle("findwin.dll");
//	HHOOK myhook = SetWindowsHookEx(WH_KEYBOARD,(HOOKPROC)myHookFunc,NULL,0);
	SetTimer(_chwnd,1,100,(TIMERPROC)MyTimer1);
//	showinfo("hwnd:%x, chwnd:%x, _hmodule:%x, myhook:%x",_hwnd,_chwnd,_hmodule,myhook);
	showinfo("hwnd:%x, chwnd:%x, _hmodule:%x",_hwnd,_chwnd,_hmodule);
	InitMonsterMap();
	LoadBotSetting();
	_poe_link_contruct();
//	ShowProcMemInfo();
//	HookAPI();
	//MessageBox(NULL,"test111","test2",0);

//	mywindow();
}

//orignal recv packet
char* ProcessOrigRecvPacket(BYTE* aPacket, DWORD len)
{
	static int	init_flag=0;
	if(init_flag==0)
	{
		init_flag=1;
		init();
	}
	_tempbuf[0]=0;
	char temp[10];
	BOOL show_flag=FALSE;;

	for(DWORD i=0;i<len;i++)
	{
	 	sprintf_s(temp,"-%x",aPacket[i]);	
		if(i%10==0) 		strcat_s(_tempbuf," * ");
		strcat_s(_tempbuf,temp);
	}
//	if(aPacket[0]==0x5d) show_flag=1;
	if(show_flag)showinfo(_tempbuf);

	return NULL;

}

void Unload()
{
	restoreAllHook();
	_poe_link_destruct();
}


