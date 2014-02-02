#include "stdafx.h"
#include "myCode.h"

BOOL	_auto_attack=FALSE;
BYTE	_show=0;

char* ProcessPacketBeforeSend(BYTE* aPacket, DWORD len)
{

	if(len==1) return NULL;


	BOOL flag=TRUE;
	WORD id;
	POS	pos;
	WORD counter;
	BYTE action;
	WORD skill;
	BYTE belt;
	char dummy[100];
	switch(aPacket[0])
	{
		case 0x05: //talk
			showinfo("taaaaaaaaaaaaaaaaaaaaaaaaaaaalk");
			_show=aPacket[3];
			showinfo("show: %c",_show);
			break;
		
		case 0x10:	//interact with object
			strcpy(dummy,"0x1000000000000000000000000000000000000");
			id = htons(*((WORD*)(aPacket+3)));
			skill = htons(*((WORD*)(aPacket+5)));	
			//0x0266 interact
			//0x0280 firestorm
			//0x0289 fireball

			counter=htons(*((WORD*)(aPacket+7)));
//			showinfo("id:%x, counter:%x,skill %x",id,counter,skill);
			//if the object is box, remove the box
			if(skill== 0x0266 && (GetMonsterType(id) == OBJ_CHEST))
			{
				RemoveMonster(id);
			}
			break;
			//return NULL;
		case 0x11:
			strcpy(dummy,"0x1111111111111111111111111111111111111");
			pos.x = htons(*((WORD*)(aPacket+3)));
			pos.y = htons(*((WORD*)(aPacket+7)));
			skill = htons(*((WORD*)(aPacket+9)));
			counter=htons(*((WORD*)(aPacket+11)));
			action = aPacket[13];

			//showinfo("run to:%x,%x, counter:%x, action:%x,skill:%x",pos.x,pos.y,counter,action,skill);
			if(action==8 && _nearest_monster.id!=0)
			{
				char	buf1[1000];
				memcpy(buf1,aPacket,len);
				WORD xx=ntohs(_nearest_monster.pos.x);
				WORD yy=ntohs(_nearest_monster.pos.y);
//				memcpy(aPacket+3,&xx,2);
//				memcpy(aPacket+7,&yy,2);
//				showinfo("change!!");
//	FormatOutput(buf1,_tempbuf,len);
//	showinfo(_tempbuf);

			}
			break;
		case 0x14:
			break;
		case 0x16:	//put item in inventory
			break;
		case 0x90: // drink belt
			belt = aPacket[4]; //0-4
			break;
		default:
			break;
	}


	FormatOutput((char*)aPacket,_tempbuf,len);
	if(_show=='s') showinfo(_tempbuf);


	return NULL;

}