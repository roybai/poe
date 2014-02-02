#include "stdafx.h"
#include "myCode.h"

char	_init_file[]="c:\\bot\\d3\\poehack.ini";
WORD	_life_flask;

void LoadBotSetting()
{
	_life_flask=GetPrivateProfileInt("Setting","life_potion",3,_init_file);
}

DWORD readObjectType(DWORD id)
{
//	char idBuf[20];
//	sprintf(idBuf,"%x",id);
//	GetPrivateProfileString("Object Type",idBuf,"",_objTypeStr,1024,_init_file);
//	if(_objTypeStr[0]==0) return 0;
	_objTypeStr[0]=0;
	char *str=_poe_link_get_obj(id);
	if(str==NULL) return 0;
	strcpy(_objTypeStr,str);
//	showinfo("got object: %s",str);
	return 1;
}

DWORD readItemType(DWORD id)
{
//	char idBuf[20];
//	sprintf(idBuf,"%x",id);
//	GetPrivateProfileString("Item Type",idBuf,"",_itemTypeStr,1024,_init_file);
//	if(_itemTypeStr[0]==0) return 0;
	_itemTypeStr[0]=0;
	char *str=_poe_link_get_item(id);
	if(str==NULL) return 0;
	strcpy(_itemTypeStr,str);
//	showinfo("got item: %s",str);
	return 1;
}

void writeObjectType(DWORD id, char*buf)
{
	if(id==0 || buf==NULL) return;
	char idBuf[20];
	sprintf(idBuf,"%x",id);
	WritePrivateProfileString("Object Type",idBuf,buf,_init_file);
	_add_object_link(id,buf);
}

void writeItemType(DWORD id, char*buf)
{
	if(id==0 || buf==NULL) return;
	char idBuf[20];
	sprintf(idBuf,"%x",id);
	WritePrivateProfileString("Item Type",idBuf,buf,_init_file);
	_add_item_link(id,buf);
}
