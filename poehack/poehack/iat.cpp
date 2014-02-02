#include "stdafx.h"

typedef struct tag_HOOKAPI 
{ 
	LPCSTR szFunc;//被HOOK的API函数名称。
	PROC pNewProc;//替代函数地址。
	PROC pOldProc;//原API函数地址。
	PIMAGE_THUNK_DATA iatAddr;
}HOOKAPI, *LPHOOKAPI; 

extern "C" __declspec(dllexport)PIMAGE_IMPORT_DESCRIPTOR LocationIAT(HMODULE, LPCSTR);
extern "C" __declspec(dllexport)BOOL HookAPIByName(HMODULE, LPCSTR, LPHOOKAPI);

class objHook {
	HOOKAPI hook;

public:
	void initHook(LPCSTR dllName, LPCSTR funcName, PROC myFuncAddr )
	{
		memset(&hook,0,sizeof(HOOKAPI));
		hook.szFunc = funcName;
		hook.pNewProc = myFuncAddr;
		HookAPIByName(GetModuleHandle(NULL),dllName, &hook);
	}
	void stopHook()
	{
		DWORD	protect,dummy;
		if(hook.pOldProc==0) return;

		VirtualProtect((LPVOID)hook.iatAddr,4, PAGE_READWRITE, &protect); 
//		showinfo("iatAddr:%x, old:%x, new:%x",hook.iatAddr,hook.pOldProc,hook.pNewProc);
		hook.iatAddr->u1.Function = (DWORD)hook.pOldProc;
		VirtualProtect((LPVOID)hook.iatAddr,4, protect, &dummy); 
	}
};
objHook mytesthook,myMallocHook;

//HOOKAPI recvHook;
//void restoreHook(HOOKAPI *);


PIMAGE_IMPORT_DESCRIPTOR LocationIAT(HMODULE hModule, LPCSTR szImportMod)
//其中，hModule为进程模块句柄；szImportMod为输入库名称。
{ 
	PIMAGE_DOS_HEADER pDOSHeader = (PIMAGE_DOS_HEADER) hModule; 

	//检查是否为DOS程序，如是返回NULL，因DOS程序没有IAT。
	if(pDOSHeader->e_magic != IMAGE_DOS_SIGNATURE) return NULL; 
	
	//检查是否为NT标志，否则返回NULL。
	PIMAGE_NT_HEADERS pNTHeader = (PIMAGE_NT_HEADERS)((DWORD)pDOSHeader+ (DWORD)(pDOSHeader->e_lfanew)); 
    if(pNTHeader->Signature != IMAGE_NT_SIGNATURE) return NULL; 
    
	//没有IAT表则返回NULL。
	if(pNTHeader->OptionalHeader.DataDirectory[IMAGE_DIRECTORY_ENTRY_IMPORT].VirtualAddress == 0) return NULL; 
    
	//定位第一个IAT位置。 
	PIMAGE_IMPORT_DESCRIPTOR pImportDesc = (PIMAGE_IMPORT_DESCRIPTOR)((DWORD)pDOSHeader + (DWORD)(pNTHeader->OptionalHeader.DataDirectory[IMAGE_DIRECTORY_ENTRY_IMPORT].VirtualAddress));

	//根据输入库名称循环检查所有的IAT，如匹配则返回该IAT地址，否则检测下一个IAT。
	while (pImportDesc->Name) 
	{ 
     //获取该IAT描述的输入库名称。
		PSTR szCurrMod = (PSTR)((DWORD)pDOSHeader + (DWORD)(pImportDesc->Name)); 
//		showinfo("pImportDesc->Name: %s",szCurrMod);
		if (stricmp(szCurrMod, szImportMod) == 0) break; 
		pImportDesc++; 
		
	} 
	if(pImportDesc->Name == NULL) return NULL; 
	return pImportDesc; 
} 


BOOL HookAPIByName( HMODULE hModule, LPCSTR szImportMod, LPHOOKAPI pHookApi)
//其中，hModule为进程模块句柄；szImportMod为输入库名称；pHookAPI为HOOKAPI结构指针。
{ 
	//定位szImportMod输入库在输入数据段中的IAT地址。
	PIMAGE_IMPORT_DESCRIPTOR pImportDesc = LocationIAT(hModule, szImportMod); 
	if (pImportDesc == NULL) return FALSE; 

/*
typedef struct _IMAGE_THUNK_DATA32 {
    union {
        DWORD ForwarderString;      // PBYTE 
        DWORD Function;             // PDWORD
        DWORD Ordinal;
        DWORD AddressOfData;        // PIMAGE_IMPORT_BY_NAME
    } u1;
} IMAGE_THUNK_DATA32;
typedef IMAGE_THUNK_DATA32 * PIMAGE_THUNK_DATA32;

typedef struct _IMAGE_IMPORT_DESCRIPTOR {
    union {
        DWORD   Characteristics;            // 0 for terminating null import descriptor
        DWORD   OriginalFirstThunk;         // RVA to original unbound IAT (PIMAGE_THUNK_DATA)
    } DUMMYUNIONNAME;
    DWORD   TimeDateStamp;                  // 0 if not bound,
                                            // -1 if bound, and real date\time stamp
                                            //     in IMAGE_DIRECTORY_ENTRY_BOUND_IMPORT (new BIND)
                                            // O.W. date/time stamp of DLL bound to (Old BIND)

    DWORD   ForwarderChain;                 // -1 if no forwarders
    DWORD   Name;
    DWORD   FirstThunk;                     // RVA to IAT (if bound this IAT has actual addresses)
} IMAGE_IMPORT_DESCRIPTOR;
typedef IMAGE_IMPORT_DESCRIPTOR UNALIGNED *PIMAGE_IMPORT_DESCRIPTOR;


*/

	//第一个Thunk地址。
	PIMAGE_THUNK_DATA pOrigThunk = (PIMAGE_THUNK_DATA)((DWORD)hModule + (DWORD)(pImportDesc->OriginalFirstThunk)); 
	
	//第一个IAT项的Thunk地址。
	PIMAGE_THUNK_DATA pRealThunk = (PIMAGE_THUNK_DATA)((DWORD)hModule + (DWORD)(pImportDesc->FirstThunk)); 
	
	MEMORY_BASIC_INFORMATION mbi_thunk;
	DWORD dwOldProtect; 

	//循环查找被截API函数的IAT项，并使用替代函数地址修改其值。
	while(pOrigThunk->u1.Function) 
	{
//		showinfo("pOrigThunk->u1.Ordinal: %x, %x, %x",pOrigThunk->u1.Ordinal, IMAGE_ORDINAL_FLAG,pRealThunk->u1.Function);
		//检测此Thunk是否为IAT项。
		if((pOrigThunk->u1.Ordinal & IMAGE_ORDINAL_FLAG) != IMAGE_ORDINAL_FLAG) 
		{
			//获取此IAT项所描述的函数名称。
			PIMAGE_IMPORT_BY_NAME pByName =(PIMAGE_IMPORT_BY_NAME)((DWORD)hModule+(DWORD)(pOrigThunk->u1.AddressOfData)); 
			if(pByName->Name[0] == '\0')
			{
//				showinfo("no name: addr:%x", pRealThunk->u1.Function);
					return FALSE; 
			}
			//检测是否为挡截函数。
//			showinfo("pByName->Name: %s,%x",(char*)pByName->Name,(DWORD)hModule+(DWORD)(pOrigThunk->u1.AddressOfData));
			if(strcmpi(pHookApi->szFunc, (char*)pByName->Name) == 0) 
			{
//				showinfo("found pByName->Name: %s",(char*)pByName->Name);
				//查询修改页的信息。
				VirtualQuery(pRealThunk, &mbi_thunk, sizeof(MEMORY_BASIC_INFORMATION)); 
				//改变修改页保护属性为PAGE_READWRITE。

				pHookApi->iatAddr = pRealThunk;

				VirtualProtect(mbi_thunk.BaseAddress,mbi_thunk.RegionSize, PAGE_READWRITE, &mbi_thunk.Protect); 
				//保存原来的API函数地址。
				//if(pHookApi->pOldProc == NULL) 
					pHookApi->pOldProc = (PROC)pRealThunk->u1.Function; 
  
				//修改API函数IAT项内容为替代函数地址。
//				showinfo("old address: %x , new address: %x",pRealThunk->u1.Function,pHookApi->pNewProc);
				pRealThunk->u1.Function = (DWORD)pHookApi->pNewProc; 
				//恢复修改页保护属性。
				VirtualProtect(mbi_thunk.BaseAddress, mbi_thunk.RegionSize, mbi_thunk.Protect, &dwOldProtect); 
			}
			
		}
		else if(strcmpi(szImportMod,"WS2_32.dll")==0)
		{
			
			switch(pOrigThunk->u1.Ordinal & 0xff)
			{
				case 16: //recv
				//查询修改页的信息。
				VirtualQuery(pRealThunk, &mbi_thunk, sizeof(MEMORY_BASIC_INFORMATION)); 
				//改变修改页保护属性为PAGE_READWRITE。
				VirtualProtect(mbi_thunk.BaseAddress,mbi_thunk.RegionSize, PAGE_READWRITE, &mbi_thunk.Protect); 
				//保存原来的API函数地址。
				pHookApi->pOldProc = (PROC)pRealThunk->u1.Function; 
  				pHookApi->iatAddr = pRealThunk;

				//修改API函数IAT项内容为替代函数地址。
//				showinfo("old address: %x , new address: %x",pRealThunk->u1.Function,pHookApi->pNewProc);
				pRealThunk->u1.Function = (DWORD)pHookApi->pNewProc; 
				//恢复修改页保护属性。
				VirtualProtect(mbi_thunk.BaseAddress, mbi_thunk.RegionSize, mbi_thunk.Protect, &dwOldProtect); 
				break;

			}
			
			//showinfo("ordinal: %d", pOrigThunk->u1.Ordinal & 0xff);
		}
		pOrigThunk++; //两个整数数组而已
		pRealThunk++; 
	}
	SetLastError(ERROR_SUCCESS); //设置错误为ERROR_SUCCESS，表示成功。
	return TRUE; 
}

static int WINAPI MessageBoxA1 (HWND hWnd , LPCTSTR lpText, LPCTSTR lpCaption, UINT uType)
{
	//过滤掉原MessageBoxA的正文和标题内容，只显示如下内容。
	return MessageBox(hWnd, "Hook API OK!", "Hook API", uType); 
} 
static int WINAPI recv1(SOCKET s, char FAR *buf, int len, int flags )
{
	//此处可以挡截游戏服务器发送来的网络数据包，可以加入分析和处理数据代码。
	int ret = recv(s,buf,len,flags);
	char tempbuf[102400];
	if(ret>1 && buf[0]==0xf1)
	{
//		FormatOutput(buf, tempbuf, len);
	}
	return ret;
} 

static PVOID WINAPI HeapAlloc1(PVOID handle,ULONG flags,SIZE_T size )
{
	return NULL;
	//此处可以挡截游戏服务器发送来的网络数据包，可以加入分析和处理数据代码。
//	return RtlAllocateHeap(handle,flags,size);
} 

/*
static PVOID WINAPI HeapAlloc1(PVOID handle,ULONG flags,SIZE_T size )
{
	//此处可以挡截游戏服务器发送来的网络数据包，可以加入分析和处理数据代码。
	static int i=0;
	char	*buf=(char*)malloc(1024);
	free(buf);
	if(i==0) showinfo("got malloc");
	i=1;
	LPVOID ret =NULL;
	return ret;
} 
*/
static LPVOID WINAPI malloc1(SIZE_T dwBytes )
{
	return malloc(dwBytes);
}
/*
LPVOID WINAPI HeapAlloc(
  _In_  HANDLE hHeap,
  _In_  DWORD dwFlags,
  _In_  SIZE_T dwBytes
);
*/
void HookAPI()
{
	mytesthook.initHook("WS2_32.dll","recv",(PROC)recv1);
//	myMallocHook.initHook("ntdll.dll","RtlAllocateHeap",(PROC)HeapAlloc1);
//	myMallocHook.initHook("MSVCR100.dll","malloc",(PROC)malloc1);
//	showinfo("111");
	return;
	//设置挡截User32.dll库中的MessageBoxA函数。
	//	HookAPIByName(GetModuleHandle(NULL),"User32.dll",&api[0]);
	/*
	//设置挡截Wsock32.dll库中的recv函数。
	recvHook.szFunc ="recv";//设置被挡截函数的名称。
	recvHook.pNewProc = (PROC)recv1; //设置替代函数的地址。
	recvHook.pOldProc = 0;
	HookAPIByName(GetModuleHandle(NULL),"WS2_32.dll",&recvHook);
	*/
}
/*
void restoreHook(HOOKAPI *p)
{
	DWORD	protect,dummy;
	VirtualProtect((LPVOID)p->iatAddr,4, PAGE_READWRITE, &protect); 
	showinfo("iatAddr:%x, old:%x, new:%x",p->iatAddr,p->pOldProc,p->pNewProc);
	if(p->pOldProc!=0)
		p->iatAddr->u1.Function = (DWORD)p->pOldProc;
	VirtualProtect((LPVOID)p->iatAddr,4, protect, &dummy); 
}
*/

void restoreAllHook()
{ 
	//mytesthook.stopHook();
	//myMallocHook.stopHook();
	//return;
//	showinfo("unloading recvhook");
//	restoreHook(&recvHook);
}
