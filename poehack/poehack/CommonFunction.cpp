#include "stdafx.h"
#include "myCode.h"
	
void SaveFile(LPCTSTR lpszFormat,...)
{
	if(!lpszFormat) return;

	char _buf[409600];
	va_list args;
	va_start(args, lpszFormat);
	_vsnprintf_s(_buf,(sizeof(_buf)/sizeof(_buf[0])),lpszFormat,args);
	va_end(args);

	FILE	*fp=fopen("\\packet.txt","a");
	fprintf_s(fp,"%s\n",_buf);
	fclose(fp);
	return;
}

 void showinfo(LPCTSTR lpszFormat,...)
 {

 	if(!_hmodule) return;
	char _buf[64*1024];

	va_list args;
	va_start(args, lpszFormat);
	_vsnprintf(_buf,(sizeof(_buf)/sizeof(_buf[0])),lpszFormat,args);

	SaveFile(_buf);
	typedef void(*pShowinfo)(LPCTSTR lpszFormat,...);
	pShowinfo p;
	p=(pShowinfo)GetProcAddress(_hmodule,"_showinfo1");
	if(!p) return;
	p(_buf);
 }
 
void FormatOutput(char *in, char* out, DWORD len)
{
	DWORD	i;
	char charbuf[10];
	out[0]=0;
	for(i=0;i<len;i++)
	{
		if((BYTE)in[i]>0x0f)
			sprintf_s(charbuf," %x",(BYTE)in[i]);
		else
			sprintf_s(charbuf," 0%x",(BYTE)in[i]);
		strcat(out,charbuf);
	}
}