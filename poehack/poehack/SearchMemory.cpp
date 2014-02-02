
#include "stdafx.h"
#include "myCode.h"

#define MAX_MEMORY_BLOCK 10240
#define MAX_FOUND	500
#define MAX_BUF_LEN	1024*64
extern "C" __declspec(dllexport) void Execute(DWORD);
void SaveFile(LPCTSTR lpszFormat,...);
char *SearchMemory();
char *SearchMemory1();
DWORD search_buf(BYTE *hays, BYTE* needle, DWORD len1, DWORD len2,DWORD start,DWORD base);
BOOL ReadProcessBYTES(HANDLE hProcess, DWORD lpAddress, void* buf, int len);
DWORD DoubleByteString(char *in, char *out);
BOOL is_in_base_list(DWORD baseAddr);

MEMORY_BASIC_INFORMATION _mbi[MAX_MEMORY_BLOCK];
DWORD	_mbi_count=0;
HANDLE _hProcess=NULL;
BOOL	_draw_map=NULL;
typedef struct {
	DWORD	address;
	DWORD	data_base;
}ADDRESS_BOOK;

DWORD	life_address[10240];
DWORD	life_address_count=0;

ADDRESS_BOOK _address_book[MAX_FOUND];
int	_address_count=0;

ADDRESS_BOOK _address_book1[MAX_FOUND];
int	_address_count1=0;

ADDRESS_BOOK _address_book2[MAX_FOUND];
int	_address_count2=0;

BOOL ShowProcMemInfo()
{
	DWORD pid=GetCurrentProcessId();

   _hProcess = OpenProcess(PROCESS_ALL_ACCESS,FALSE,pid);
   if(_hProcess == NULL)
      return FALSE;
   MEMORY_BASIC_INFORMATION mbi;
   PBYTE pAddress = NULL;
   TCHAR szInfo[200] = _T("BaseAddr     Size        Type        State         Protect \n");
//   _tprintf(szInfo);
 //  showinfo(szInfo);
   int i=0;
   while(TRUE)
   {
      if(VirtualQueryEx(_hProcess, pAddress, &mbi, sizeof(mbi)) != sizeof(mbi))
      {
         break;
      }
      if((mbi.AllocationBase != mbi.BaseAddress) && (mbi.State != MEM_FREE))
      {
         _stprintf(szInfo, _T("  %08X  %8dK  "), 
              mbi.BaseAddress,
              mbi.RegionSize>>10);
      }
      else
      {
         _stprintf(szInfo, _T("%08X    %8dK  "), 
              mbi.BaseAddress,
              mbi.RegionSize>>10);
      }
      PCTSTR pStr = _T("");
      switch(mbi.Type)
      {
      case MEM_IMAGE:      pStr = _T("MEM_IMAGE  "); break;
      case MEM_MAPPED:   pStr = _T("MEM_MAPPED "); break;
      case MEM_PRIVATE:   pStr = _T("MEM_PRIVATE"); break;
      default:         pStr = _T("-----------"); break;
      }
      _tcscat(szInfo, pStr);
      _tcscat(szInfo, _T("  "));

      switch(mbi.State)
      {
      case MEM_COMMIT:   pStr = _T("MEM_COMMIT "); break;
      case MEM_RESERVE:   pStr = _T("MEM_RESERVE"); break;
      case MEM_FREE:      pStr = _T("MEM_FREE   "); break;
      default:         pStr = _T("-----------"); break;
      }
      _tcscat(szInfo, pStr);
      _tcscat(szInfo, _T("  "));

      switch(mbi.AllocationProtect)
      {
      case PAGE_READONLY:            pStr = _T("PAGE_READONLY         "); break;   
      case PAGE_READWRITE:         pStr = _T("PAGE_READWRITE        "); break;   
      case PAGE_WRITECOPY:         pStr = _T("PAGE_WRITECOPY        "); break;   
      case PAGE_EXECUTE:            pStr = _T("PAGE_EXECUTE          "); break;   
      case PAGE_EXECUTE_READ:         pStr = _T("PAGE_EXECUTE_READ     "); break;   
      case PAGE_EXECUTE_READWRITE:   pStr = _T("PAGE_EXECUTE_READWRITE"); break;   
      case PAGE_EXECUTE_WRITECOPY:   pStr = _T("PAGE_EXECUTE_WRITECOPY"); break;   
      case PAGE_GUARD:            pStr = _T("PAGE_GUARD            "); break;   
      case PAGE_NOACCESS:            pStr = _T("PAGE_NOACCESS         "); break;   
      case PAGE_NOCACHE:            pStr = _T("PAGE_NOCACHE          "); break;   
      default:                  pStr = _T("----------------------"); break;
      }
      _tcscat(szInfo, pStr);
      _tcscat(szInfo, _T("\n"));
	  i++;
	  SaveFile(szInfo);
 //  showinfo(szInfo);
//      _tprintf(szInfo);

      pAddress = ((PBYTE)mbi.BaseAddress + mbi.RegionSize);
	  if((mbi.AllocationProtect == PAGE_READONLY || mbi.AllocationProtect == PAGE_READWRITE) && mbi.State != MEM_RESERVE && mbi.Protect != 0x104)
		  //&& mbi.Type != MEM_PRIVATE )
	  {
		memcpy(&_mbi[_mbi_count],&mbi,sizeof(MEMORY_BASIC_INFORMATION));
		_mbi_count++;
	  }
	  if(i>10240) break;
   }
   SearchMemory();
   _address_count1=_address_count;
   memcpy(_address_book1,_address_book,sizeof(ADDRESS_BOOK)*MAX_FOUND);
   CloseHandle(_hProcess);
   return TRUE;
}
char *SearchMemory()
{

	DWORD i,j,m,n,offset=0;
	BOOL	f;
	char	mem_buf[MAX_BUF_LEN];
	char output[1024];
	char buf[1024];
	DWORD	lpAddr;
	SIZE_T size;
	showinfo("mbicount: %d",_mbi_count);
	char string_buf[10240];
	DWORD string_len;
//	string_len = DoubleByteString("afwergtt",string_buf);
	string_len = DoubleByteString("tarazon",string_buf);
//	strcpy_s(string_buf,"is program");
//	string_len=7;
	_address_count=0;
	for(i=0;i<_mbi_count;i++)
	{
		if(_address_count>MAX_FOUND) break;
		lpAddr = (DWORD)_mbi[i].BaseAddress;
		size = _mbi[i].RegionSize;
		n= _mbi[i].RegionSize/(MAX_BUF_LEN);
		m=  _mbi[i].RegionSize%(MAX_BUF_LEN);
//		DWORD oldprot,dummy;
//		showinfo("searching %i, %x",i,lpAddr);
		j=0;
	//			showinfo(":%x -- %x,%x,%x,%x, %x",(DWORD)lpAddr+j*MAX_BUF_LEN,_mbi[i].Type,_mbi[i].State,_mbi[i].Protect,_mbi[i].AllocationProtect,_mbi[i].RegionSize);
		for(j=0;j<n;j++)
		{
			if(!ReadProcessMemory(_hProcess, (void*)(lpAddr+j*MAX_BUF_LEN), (void*)mem_buf, MAX_BUF_LEN, 0))
			{
//				showinfo(":%x -- %x,%x,%x,%x, ",(DWORD)lpAddr+j*MAX_BUF_LEN,_mbi[i].Type,_mbi[i].State,_mbi[i].Protect,_mbi[i].AllocationProtect);
				break;
			}
		//	showinfo("reading %x bytes from :%x",MAX_BUF_LEN,(DWORD)lpAddr+j*MAX_BUF_LEN);
			
			offset=search_buf((BYTE*)mem_buf,(BYTE*)string_buf,MAX_BUF_LEN,string_len,lpAddr+j*MAX_BUF_LEN,lpAddr);
			/*
			output[0]=0;
			for(i=0;i<100;i++)
			{
				sprintf_s(output,"%s-%x",output,(BYTE)mem_buf[i]);
			}
			showinfo(output);
			return NULL;
			*/
			if(offset>0) 
			{
//					showinfo("found one, address: %x",lpAddr+j*MAX_BUF_LEN+offset);
					//found++;
					//return NULL;
			}
		}
		if(m==0) continue;
			if(!ReadProcessMemory(_hProcess, (void*)(lpAddr+j*MAX_BUF_LEN), (void*)mem_buf, m, 0))
			{
//				showinfo(":%x -- %x,%x,%x,%x, ",(DWORD)lpAddr+j*MAX_BUF_LEN,_mbi[i].Type,_mbi[i].State,_mbi[i].Protect,_mbi[i].AllocationProtect);
				//DWORD nn= GetLastError();
				//showinfo("reading failed %x bytes from :%x, error code %d,%x",m,(DWORD)lpAddr+j*MAX_BUF_LEN,nn,nn);
			}
			else 
			{
				offset=search_buf((BYTE*)mem_buf,(BYTE*)string_buf,m,string_len,lpAddr+j*MAX_BUF_LEN,lpAddr);
				if(offset>0)
			{
					//showinfo("found one, address: %x",lpAddr+j*MAX_BUF_LEN+offset);
					//return NULL;
			}

			}
	}
	//we found some addresses, let's double check and get rid of the address from this function
	for(i=0;i<_address_count;i++)
	{
		ReadProcessBYTES(_hProcess, _address_book[i].address, output, string_len);
		for(j=0;j<string_len;j++)
		{
			if(output[j] != string_buf[j])
			{
				_address_book[i].address = 0;
				break;
			}
		}
	}

	showinfo("total found %d",_address_count);
	for(i=0;i<_address_count;i++)
	{
		if(_address_book[i].address==0) continue;
		showinfo("reading from %x",_address_book[i].address);
		ReadProcessBYTES(_hProcess, _address_book[i].address, output, 10);
		FormatOutput(output,buf,20);
		showinfo(buf);
	}
	return NULL;
}

char *SearchMemory1()
{

	DWORD i,j,m,n,offset=0;
	BOOL	f;
	char	mem_buf[MAX_BUF_LEN];
	char output[1024];
	DWORD	lpAddr;
	SIZE_T size;
	showinfo("mbicount: %d",_mbi_count);

	_address_count=0;
	for(i=0;i<_address_count1;i++)
	{
//		showinfo("xxxxxxx  address:%x, %x",_address_book1[i].address,_address_book1[i].data_base);
	}

	char string_buf[10240];
	DWORD string_len;
	DWORD	number=167;
	string_len=sizeof(DWORD);
	memcpy((void*)string_buf,(void*)&number,string_len);	
//	string_len = DoubleByteString("tarazon",string_buf);
//	strcpy_s(string_buf,"is program");
//	string_len=7;

	for(i=0;i<_mbi_count;i++)
	{
		if(_address_count>MAX_FOUND) break;
		lpAddr = (DWORD)_mbi[i].BaseAddress;
		if(!is_in_base_list(lpAddr)){
			continue;
		}
		showinfo("searching base: %x...",lpAddr);
		size = _mbi[i].RegionSize;
		n= _mbi[i].RegionSize/(MAX_BUF_LEN);
		m=  _mbi[i].RegionSize%(MAX_BUF_LEN);
		DWORD oldprot,dummy;
//		showinfo("searching %i, %x",i,lpAddr);
		j=0;
	//			showinfo(":%x -- %x,%x,%x,%x, %x",(DWORD)lpAddr+j*MAX_BUF_LEN,_mbi[i].Type,_mbi[i].State,_mbi[i].Protect,_mbi[i].AllocationProtect,_mbi[i].RegionSize);
		for(j=0;j<n;j++)
		{
			if(!ReadProcessMemory(_hProcess, (void*)(lpAddr+j*MAX_BUF_LEN), (void*)mem_buf, MAX_BUF_LEN, 0))
			{
//				showinfo(":%x -- %x,%x,%x,%x, ",(DWORD)lpAddr+j*MAX_BUF_LEN,_mbi[i].Type,_mbi[i].State,_mbi[i].Protect,_mbi[i].AllocationProtect);
				break;
			}
		//	showinfo("reading %x bytes from :%x",MAX_BUF_LEN,(DWORD)lpAddr+j*MAX_BUF_LEN);
			
			offset=search_buf((BYTE*)mem_buf,(BYTE*)string_buf,MAX_BUF_LEN,string_len,lpAddr+j*MAX_BUF_LEN,lpAddr);
			/*
			output[0]=0;
			for(i=0;i<100;i++)
			{
				sprintf_s(output,"%s-%x",output,(BYTE)mem_buf[i]);
			}
			showinfo(output);
			return NULL;
			*/
			if(offset>0) 
			{
//					showinfo("found one, address: %x",lpAddr+j*MAX_BUF_LEN+offset);
					//found++;
					//return NULL;
			}
		}
		if(m==0) continue;
			if(!ReadProcessMemory(_hProcess, (void*)(lpAddr+j*MAX_BUF_LEN), (void*)mem_buf, m, 0))
			{
//				showinfo(":%x -- %x,%x,%x,%x, ",(DWORD)lpAddr+j*MAX_BUF_LEN,_mbi[i].Type,_mbi[i].State,_mbi[i].Protect,_mbi[i].AllocationProtect);
				//DWORD nn= GetLastError();
				//showinfo("reading failed %x bytes from :%x, error code %d,%x",m,(DWORD)lpAddr+j*MAX_BUF_LEN,nn,nn);
			}
			else 
			{
				offset=search_buf((BYTE*)mem_buf,(BYTE*)string_buf,m,string_len,lpAddr+j*MAX_BUF_LEN,lpAddr);
				if(offset>0)
			{
					//showinfo("found one, address: %x",lpAddr+j*MAX_BUF_LEN+offset);
					//return NULL;
			}

			}
	}
	//we found some addresses, let's double check and get rid of the address from this function
	for(i=0;i<_address_count;i++)
	{
		ReadProcessBYTES(_hProcess, _address_book[i].address, output, string_len);
		for(j=0;j<string_len;j++)
		{
			if(output[j] != string_buf[j])
			{
				_address_book[i].address = 0;
				break;
			}
		}
	}

	string_buf[0]=0;
	for(i=0;i<_address_count;i++)
	{
		if(_address_book[i].address==0) continue;
		sprintf_s(string_buf,"%s %x",string_buf,_address_book[i].address);
		if(i%10==0) strcat_s(string_buf,"\n");
	//	showinfo("reading from %x",_address_book[i].address);
	//	ReadProcessBYTES(_hProcess, _address_book[i].address, output, 10);
	//	FormatOutput(output, 20);
	}
	showinfo("total found %d",_address_count);
	showinfo(string_buf);
	return NULL;
}

DWORD DoubleByteString(char *in, char *out)
{
	DWORD len = strlen(in);
	for(int i=0;i<len;i++)
	{
		out[i*2]=0;
		out[i*2+1]=in[i];
	}
	return len*2;
}

DWORD search_buf(BYTE *hays, BYTE* needle, DWORD len1, DWORD len2,DWORD start,DWORD base)
{
	if(len1<len2) return 0;
	int i,j,match_flag;
	for(i=0;i<len1-len2;i++) 
	{
		match_flag=1;
		for(j=0;j<len2;j++)
		{
			if(hays[i+j]!=needle[j]) {match_flag=0;break;}
		}
		if(match_flag==1) //found match
		{
			_address_book[_address_count].address=start+i;
			_address_book[_address_count].data_base=base;
			_address_count ++;
		}
	}

	return 0;
}

BOOL ReadProcessBYTES(HANDLE hProcess, DWORD lpAddress, void* buf, int len)
{
	DWORD oldprot, dummy = 0,ret;
	VirtualProtectEx(hProcess, (void*) lpAddress, len, PAGE_READWRITE, &oldprot);
	ret=ReadProcessMemory(hProcess, (void*) lpAddress, buf, len, 0);
	VirtualProtectEx(hProcess, (void*) lpAddress, len, oldprot, &dummy);
	return ret;
}



BOOL is_in_base_list(DWORD baseAddr)
{
	for(int i=0;i<_address_count1;i++)
	{

		if(_address_book1[i].address==0) continue;
		if(_address_book1[i].data_base == baseAddr) return TRUE;
	}
	return FALSE;
}

void Execute(DWORD dummy)
{
	if(dummy!=0)
		_hwnd = (HWND) dummy;
	return;
		DWORD pid=GetCurrentProcessId();
		
		_hProcess = OpenProcess(PROCESS_ALL_ACCESS, FALSE,pid);
		if(_hProcess == NULL) return;
		SearchMemory1();
		CloseHandle(_hProcess);
}
