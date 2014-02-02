// InterceptDll.cpp : 定义 DLL 应用程序的入口点。
//


#include "stdafx.h"
#include <stdio.h> 
#include <winsock2.h>
#include <windows.h>
#include <iostream>
#include <io.h>
#include <stdio.h>
#include <tlhelp32.h>

//#pragma comment(lib, "wsock32.lib") 
//DllImport("user32.dll");
extern BOOL 	PoeIntercept();
//extern HINSTANCE	_hinstWSASendToReplaced;
extern int	_create_win();

BOOL InterceptAPI(HMODULE hLocalModule, const char* c_szDllName, const char* c_szApiName,
        DWORD dwReplaced, DWORD dwTrampoline, int offset);

BOOL InterceptAPI_fog1(HMODULE hLocalModule, const char* c_szDllName, const char* c_szApiName,
        DWORD dwReplaced, DWORD dwTrampoline, int offset);




HWND  WINAPI  GetFindwindowReplaced(LPCTSTR a1,LPCTSTR a2);
HWND  WINAPI  GetSendReplaced(SOCKET,char*,int,int);
HWND WINAPI WSASendToReplaced ( SOCKET s, const char*
  lpBuffers, DWORD dwBufferCount, LPDWORD
  lpNumberOfBytesSent, int iFlags, LPVOID lpTo, int
  iToLen, LPWSAOVERLAPPED lpOverlapped,
  LPWSAOVERLAPPED_COMPLETION_ROUTINE
  lpCompletionRoutine );

HWND  WINAPI  GetFogReplaced1(unsigned char*,int);
HWND   __stdcall Trampoline_findwindow(LPCTSTR,LPCTSTR);
HWND   __stdcall Trampoline_send(SOCKET,const char*,int,int);
HWND   __stdcall Trampoline_wsasendto( SOCKET s, const char*
  lpBuffers, DWORD dwBufferCount, LPDWORD
  lpNumberOfBytesSent, int iFlags, LPVOID lpTo, int
  iToLen, LPWSAOVERLAPPED lpOverlapped,
  LPWSAOVERLAPPED_COMPLETION_ROUTINE
  lpCompletionRoutine );
HWND   __stdcall Trampoline_fog1(unsigned char*, int);

HWND  WINAPI  GetFindwindowReplaced(LPCTSTR c1,LPCTSTR c2)
{
	static BOOL	DONE=FALSE;
	BOOL	tag=TRUE;

	if(DONE)		//only once since only block diablo.exe first check
		return Trampoline_findwindow(c1,c2);
	
	DONE=TRUE; 
	if(c1)
	{
		if(!strcmp(c1,"Diablo II")) tag= FALSE;
		if(!strcmp(c1,"Diablo_II"))
			return Trampoline_findwindow("Diablo II",c2); 
	}
	if(tag==FALSE)
	{ 
		return FALSE;
	}
	return Trampoline_findwindow(c1,c2);
	
}


HWND  WINAPI GetFogReplaced1(unsigned char* n1, int len)
{
	
	int i;
	 char str1[102400];
	 char str2[102400];


	 if(len<=0) return Trampoline_fog1(n1,len);
	sprintf(str1,"%4x |",len);
	sprintf(str2,"%4x |",len);
	for(i=0;i<len;i++)
	{
		sprintf(str1,"%s %2x",str1,n1[i]);	
//		if(n1[i]>='A' && n1[i]<='Z' || n1[i]>='a'&&n1[i]<='z')
		if(isprint(n1[i]))
			sprintf(str2,"%s %2c",str2,n1[i]);	
		else
			sprintf(str2,"%s %2c",str2,'*');	

	}
		FILE	*fp;
		fp=fopen("c:\\recv.txt","a");
		fprintf(fp,str1);
		fprintf(fp,"\n");
		fprintf(fp,str2);
		fprintf(fp,"\n");
		fprintf(fp,"\n");
		fclose(fp);

	return Trampoline_fog1(n1,len);
}

HWND  WINAPI  GetSendReplaced(SOCKET s,char* buf,int len, int flag)
{

	return Trampoline_send(s,buf,len,flag);
}

HWND WINAPI WSASendToReplaced ( SOCKET s, const char*
  buf, DWORD len, LPDWORD
  lpNumberOfBytesSent, int iFlags, LPVOID lpTo, int
  iToLen, LPWSAOVERLAPPED lpOverlapped,
  LPWSAOVERLAPPED_COMPLETION_ROUTINE
  lpCompletionRoutine )
{
	return Trampoline_wsasendto(s,buf,len,lpNumberOfBytesSent,iFlags,lpTo,iToLen,lpOverlapped,lpCompletionRoutine);

}
BOOL InterceptAPI(HMODULE hLocalModule, const char* c_szDllName, const char* c_szApiName,
        DWORD dwReplaced, DWORD dwTrampoline, int offset)
{

    int i;
    DWORD dwOldProtect;
        DWORD dwAddressToIntercept = (DWORD)GetProcAddress(
            GetModuleHandle((char*)c_szDllName), (char*)c_szApiName);

    BYTE *pbTargetCode = (BYTE *) dwAddressToIntercept;
    BYTE *pbReplaced = (BYTE *) dwReplaced;
    BYTE *pbTrampoline = (BYTE *) dwTrampoline;

    // Change the protection of the trampoline region
    // so that we can overwrite the first 5 + offset bytes.
    VirtualProtect((void *) dwTrampoline, 5+offset, PAGE_WRITECOPY, &dwOldProtect);
    for (i=0;i<offset;i++)
        *pbTrampoline++ = *pbTargetCode++;
    pbTargetCode = (BYTE *) dwAddressToIntercept;

    // Insert unconditional jump in the trampoline.
    *pbTrampoline++ = 0xE9;        // jump rel32
    *((signed int *)(pbTrampoline)) = (pbTargetCode+offset) - (pbTrampoline + 4);
    VirtualProtect((void *) dwTrampoline, 5+offset, PAGE_EXECUTE, &dwOldProtect);
    
    // Overwrite the first 5 bytes of the target function
    VirtualProtect((void *) dwAddressToIntercept, 5, PAGE_WRITECOPY, &dwOldProtect);
    *pbTargetCode++ = 0xE9;        // jump rel32
    *((signed int *)(pbTargetCode)) = pbReplaced - (pbTargetCode +4);
    VirtualProtect((void *) dwAddressToIntercept, 5, PAGE_EXECUTE, &dwOldProtect);
    
    // Flush the instruction cache to make sure 
    // the modified code is executed.
    FlushInstructionCache(GetCurrentProcess(), NULL, NULL);
    return TRUE;


}

BOOL InterceptAPI_fog1(HMODULE hLocalModule, const char* c_szDllName, const char* c_szApiName,
        DWORD dwReplaced, DWORD dwTrampoline, int offset)
{

    int i;
    DWORD dwOldProtect;
        DWORD dwAddressToIntercept = 0X6FF5DB60;

    BYTE *pbTargetCode = (BYTE *) dwAddressToIntercept;
    BYTE *pbReplaced = (BYTE *) dwReplaced;
    BYTE *pbTrampoline = (BYTE *) dwTrampoline;

    // Change the protection of the trampoline region
    // so that we can overwrite the first 5 + offset bytes.
    VirtualProtect((void *) dwTrampoline, 5+offset, PAGE_WRITECOPY, &dwOldProtect);
    for (i=0;i<offset;i++)
        *pbTrampoline++ = *pbTargetCode++;
    pbTargetCode = (BYTE *) dwAddressToIntercept;

    // Insert unconditional jump in the trampoline.
    *pbTrampoline++ = 0xE9;        // jump rel32
    *((signed int *)(pbTrampoline)) = (pbTargetCode+offset) - (pbTrampoline + 4);
    VirtualProtect((void *) dwTrampoline, 5+offset, PAGE_EXECUTE, &dwOldProtect);
    
    // Overwrite the first 5 bytes of the target function
    VirtualProtect((void *) dwAddressToIntercept, 5, PAGE_WRITECOPY, &dwOldProtect);
    *pbTargetCode++ = 0xE9;        // jump rel32
    *((signed int *)(pbTargetCode)) = pbReplaced - (pbTargetCode +4);
    VirtualProtect((void *) dwAddressToIntercept, 5, PAGE_EXECUTE, &dwOldProtect);
    
    // Flush the instruction cache to make sure 
    // the modified code is executed.
    FlushInstructionCache(GetCurrentProcess(), NULL, NULL);
    return TRUE;


}

HWND  __stdcall Trampoline_send(SOCKET s,const char *buf,int len, int flag)
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
		nop
		nop
	}
}

HWND  __stdcall Trampoline_wsasendto( SOCKET s, const char*
  lpBuffers, DWORD dwBufferCount, LPDWORD
  lpNumberOfBytesSent, int iFlags, LPVOID lpTo, int
  iToLen, LPWSAOVERLAPPED lpOverlapped,
  LPWSAOVERLAPPED_COMPLETION_ROUTINE
  lpCompletionRoutine )
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
		nop
		nop
	}
}

HWND  __stdcall Trampoline_fog1(unsigned char* s,int len)
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
		nop
		nop
	}
}


HWND  __stdcall Trampoline_findwindow(LPCTSTR C1,LPCTSTR C2)
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
		nop
		nop
	}
} 
 
  
BOOL is_inject(char* str)
{
	if(strstr(str,"game.exe")) return TRUE;
	if(strstr(str,"Game.exe")) return TRUE;
	if(strstr(str,"lotroclient.exe")) return TRUE;
	if(strstr(str,"Diablo III.exe")) return TRUE;
	if(strstr(str,"Client.exe")) return TRUE;
//	if(strstr(str,"note")) return TRUE;
//	if(strstr(str,"Diablo")) return TRUE;
//	if(strstr(str,"diablo")) return TRUE;
//	if(strstr(str,"bot")) return TRUE;
//	if(strstr(str,"mule")) return TRUE;
	return FALSE;
 
}
#include <stdlib.h>


BOOL APIENTRY	DllMain(HINSTANCE hInst,DWORD dwReason,LPVOID reserved)
{
//	hDllModule=hInst;
	if(dwReason== DLL_PROCESS_ATTACH){
		char	m_name[1000];  

		strcpy(m_name,GetCommandLine());
		 
		FILE	*fp;
		fp=fopen("c:\\ttt.txt","a");
		fprintf(fp,m_name);
		fprintf(fp,"\n"); 
		fclose(fp);
//		exit(0); 

//		GetModuleFileName(hInst,m_name,sizeof(m_name));
//			MessageBox(NULL,m_name,m_name,0);
//		_hInst=hInst;	
//		InterceptAPI(hInst,"user32.dll","FindWindowA",(DWORD) GetFindwindowReplaced,(DWORD)&Trampoline_findwindow,5);
//		InterceptAPI(hInst,"wsock32.dll","send",(DWORD) GetSendReplaced,(DWORD)&Trampoline_send,5);
//		InterceptAPI(hInst,"ws2_32.dll","WSASendTo",(DWORD) WSASendToReplaced,(DWORD)&Trampoline_wsasendto,5);
//		InterceptAPI(hInst,"wsock32.dll","send",(DWORD) GetSendReplaced,(DWORD)&Trampoline_send,5);
//		InterceptAPI_fog1(hInst,"","",(DWORD) GetFogReplaced1,(DWORD)&Trampoline_fog1,5);
//		InterceptAPI_fog2(hInst,"","",(DWORD) GetFogReplaced2,(DWORD)&Trampoline_fog2,5);
//		Intercept_Wsock32();	//make some room before return
//////////////////////////////		
//		_hinst=hInst;
		 _create_win(); 
//////////////////////////////////           
  //
		//InterceptAPI(hInst,"ws2_32.dll","recv",(DWORD) WSASendToReplaced,(DWORD)&Trampoline_wsasendto,5);
//PoeInterceptWinsock();
	} 
	else if(dwReason==DLL_PROCESS_DETACH){
//		::MessageBox(NULL,"AA","AA",MB_OK);
	}
	return TRUE;
} 
/*
LRESULT CALLBACK MyShellProc (int nCode, WPARAM wParam, LPARAM lParam)
{	
	//	MessageBox(NULL,"myshell","mbot",MB_OK);		
		if(nCode   ==   HSHELL_WINDOWCREATED) 
		{
  	if ( bTryed == false ){
		bTryed = true;
//		MessageBox(NULL,"myshell","mbot",MB_OK);		
		InterceptAPI(NULL,"user32.dll","FindWindowA",(DWORD) GetFindwindowReplaced,(DWORD)&Trampoline,5);
//		Intercept();
		bIntercepted= true;
	}
		}
	return ::CallNextHookEx( hhook, nCode, wParam, lParam );
}

extern"C" __declspec( dllexport ) HRESULT InstallHook( HWND hWnd )
{
	HRESULT hr = E_FAIL;

	hhook = ::SetWindowsHookEx( WH_SHELL , MyShellProc ,(HINSTANCE)hDllModule, 0);
	if ( hhook == NULL ){		
		hr =  E_FAIL;
	}
	else{				
		hr =  S_OK;		
	}
	return hr;
}

extern"C" __declspec( dllexport ) HRESULT UninstallHook()
{	
	::UnhookWindowsHookEx( hhook );
	hhook = NULL;
	return S_OK;	
}	
*/