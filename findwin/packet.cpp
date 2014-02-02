#include "stdafx.h"
#include <stdio.h> 
DWORD packet(int len, BYTE*p);
void save_packet(void);


extern void	savefile(LPCTSTR lpszFormat,...);
extern void _showinfo(LPCTSTR lpszFormat,...);
extern int _create_win();

void show_place(char* buf, float f1,float f2, float f3, float a,DWORD id);

BOOL _create_win_tag=FALSE;
BOOL	_write_tag=FALSE;
int	_len=0;
BYTE _packet_buf[10240];

int	_old_len=0;
BYTE _old_packet_buf[10240];
int	tick=0;
DWORD	trace;
DWORD	_id;
BYTE	filter[100];
void read_filter(void)
{
	int i;
	FILE	*fp;

	for(i=0;i<100;i++)
		filter[i]=-1;
	fp=fopen("c:\\filter.txt","r");
	if(fp==NULL) return;
	
	i=0;
	while(1)
	{
		if(fscanf(fp,"%x",&filter[i])==EOF)
			break;
//		_showinfo("filter: %x",filter[i]);
		i++;
		if(i>=100) break;
	}
	fclose(fp);

	trace=0;
	_id=0;
	fp=fopen("c:\\trace.txt","r");
	if(fp==NULL) return;
	
	fscanf(fp,"%x",&trace);
	if(fscanf(fp,"%x",&_id)==EOF)
		_id=0;
	fclose(fp);

}
BOOL is_useless(BYTE ch)
{
	for(int i=0;i<100;i++)
		if(ch==filter[i])
			return TRUE;
	return FALSE;
}

DWORD	isTrace(DWORD tr,DWORD id)
{
	if(tr==trace && trace!=0 && id==_id) return 1;

	return 0;

}

DWORD packet(int len, BYTE*p)
{
	tick++;
	char buf[4096];
	char buf1[4096];
//	return len;
	if(tick>100)
	{
//		read_filter();
		tick=0;
	}

		float	f1;
		float	f2;
		float	f3;
		float	f4;
		BYTE	angel;
		DWORD	id;
	sprintf(buf,"%x ",len);
	strcpy(buf1,buf);
	_showinfo(buf);
//	savefile(buf);
return 0;
//	if(len==0x1b0) return 0;
	if(len<8) return 0;
	_len=len;
	memcpy(_packet_buf,p,len);

	id=0;
	if(is_useless(p[1])) return 0;
	
	switch(p[1])
	{
	case 0x8a:	//npc 
	case 0x82:
		return 0;
	case 0x7b:
	case 0x7c:
	case 0x7d:
		return 0;
	}
	

	if(p[40]==0xff &&p[41]==0xff && p[42]==0x7f && p[43]==0xff)	//just a counter useless
		return 0;

	sprintf(buf,"%x ",len);
	strcpy(buf1,buf);
	_showinfo(buf);
	savefile(buf);
	return 0;
	for(int i=0;i<len;i++)
	{
		sprintf(buf,"%s %2x",buf,p[i]);
		if(isprint(p[i]))
			sprintf(buf1,"%s %2c",buf1,p[i]);	
		else
			sprintf(buf1,"%s %2c",buf1,'*');	
	}
	if(p[1]==0x21)	//walking in instance
	{

		id=p[11];
		memcpy(&f1,p+24,4);
		memcpy(&f2,p+28,4);
		memcpy(&f3,p+32,4);
		angel=*(p+36);
		f4=360*(float)angel;
		f4=f4/255;

		show_place("instance:", f1,f2,f3,f4,id);
		return 0;

	}
	if(p[1]==0x09 && len==0x24)
	{
		id=p[11];
		memcpy(&f1,p+23,4);
		memcpy(&f2,p+27,4);
		memcpy(&f3,p+31,4);
		angel=*(p+35);
		f4=360*(float)angel;
		f4=f4/255;
	
		show_place("at:", f1,f2,f3,f4,id);
	}
	if(p[1]==0x29 && len==0x28)	//walking in room
	{
		id=p[11];
		
		memcpy(&f1,p+27,4);
		memcpy(&f2,p+31,4);
		memcpy(&f3,p+35,4);
		angel=*(p+39);
		f4=360*(float)angel;
		f4=f4/255;
	
		show_place("to:", f1,f2,f3,f4,id);
	return isTrace(p[1],id);
		

	}
	if(p[1]==0x09 && len==0x22)
	{
		memcpy(&f1,p+21,4);
		memcpy(&f2,p+25,4);
		memcpy(&f3,p+29,4);
		angel=*(p+33);
		id=p[11];
		f4=360*(float)angel;
		f4=f4/255;
	
		show_place("at:", f1,f2,f3,f4,id);
	}
	if(p[1]==0x29 && len==0x26)	//walking outside
	{
		
		id=p[11];
		memcpy(&f1,p+25,4);
		memcpy(&f2,p+29,4);
		memcpy(&f3,p+33,4);
		angel=*(p+37);
		f4=360*(float)angel;
		f4=f4/255;

		show_place("to:", f1,f2,f3,f4,id);
		
	return isTrace(p[1],id);
		

	}
	savefile(buf);
	savefile(buf1);
	_showinfo(buf);
//	if(p[1]==trace && trace!=0 && id==_id) return 1;
	return isTrace(p[1],id);
//	return 0;
}

void save_packet(void)
{
	if(_len<1) return;
	return;

	if(memcmp(_packet_buf,_old_packet_buf,_len)==0) return;
	
	_old_len=_len;
	memcpy(_old_packet_buf,_packet_buf,_len);

	char buf[10240];

	buf[0]=0;
	for(int i=0;i<_len;i++)
	sprintf(buf,"%s %2x",buf,_packet_buf[i]);

	savefile(buf);
}

void show_place(char* buf, float f1,float f2, float f3, float a,DWORD id)
{
	_showinfo("%s: id:%x, x=%5.1f, y=%5.1f, h=%5.1f, a=%5.1f",buf,id,f1,f2,f3,a);

}

