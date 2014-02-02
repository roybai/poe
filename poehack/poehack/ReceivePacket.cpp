#include "stdafx.h"
#include "myCode.h"


HWND	_hwnd=NULL;
HMODULE	_hmodule=NULL;
time_t	_time_drink_life_potion,_time_drink_mana_potion;
PLAYER	_player;
char _tempbuf[102400];
ACTION _action;
MONSTER	_monster;
char* ObjectUpdate(BYTE*, DWORD);
char* ObjectRemove(BYTE*, DWORD);
char* ItemOnMouse(BYTE*,DWORD);



BOOL DrinkLifePotion();
BOOL DrinkManaPotion();

BOOL DrinkLifePotion()
{
	static int i=1;
	time_t time_now;
	time(&time_now);
//	SendMessage(_hwnd,WM_RBUTTONDOWN,0,MAKEWORD(1,1));
	if(time_now - _time_drink_life_potion < 5)
	{
			return FALSE;
	}
	time(&_time_drink_life_potion);
	SendMessage(_hwnd,WM_KEYDOWN,'0'+i,0);
	
	showinfo("_hwnd:%x , drinking %d",_hwnd,i);
	i++;
	if(i>_life_flask) i=1;
	return TRUE;
}

BOOL DrinkManaPotion()
{
	static int i=5;
	time_t time_now;
	time(&time_now);
	if(time_now - _time_drink_mana_potion < 5) return FALSE;
	time(&_time_drink_mana_potion);
	PostMessage(_hwnd,WM_KEYDOWN,'0'+i,0);
	showinfo("_hwnd:%x , drinking mana %d",_hwnd,i);
	i++;
	if(i>5) i=_life_flask+1;
	return TRUE;
}

/*
char* ObjectAdd(BYTE* aPacket, DWORD len)
{
	DWORD object_id = *((DWORD*)(aPacket+5));
	DWORD x = *(aPacket+12) * 256+*(aPacket+13);
	DWORD y = *(aPacket+16) * 256+*(aPacket+17);
	sprintf_s(_tempbuf,"new object id: %x, pos: %d,%d",object_id,x,y);
//	sprintf_s(_tempbuf,"new object pos: %x-%x ",x,y);
//	AddMonster(object_id,x,y);
	return _tempbuf;
}
*/
char* ObjectRemove(BYTE* aPacket, DWORD len)
{
	BYTE* p=aPacket;
	char buf[100];
	strcpy_s(_tempbuf,"obj remove:");
	for(int i=0;i<len;i=i+4)
	{
	DWORD object_id=*((DWORD*)(p+i+1)); 
	RemoveMonster(object_id);
	sprintf_s(buf,"%x ",object_id);
	strcat_s(_tempbuf,buf);
	}
	return _tempbuf;
}
char* ObjectUpdate(BYTE* aPacket, DWORD len)
{		
	DWORD object_id=*((DWORD*)(aPacket+1)); 
	DWORD	x,y;
	BYTE action;

	action = *(aPacket+5);
	switch(action) {
		case 3:	//life mana update
//			if(_player.id==0) _player.id=object_id;
			if(_player.id==object_id)
			{
				_player.life = *(aPacket+8) * 256+*(aPacket+9);
				_player.mana = *(aPacket+17) * 256+*(aPacket+18);
				_player.shield = *(aPacket+26) * 256+*(aPacket+27);
//				if(_player.mana_max<_player.mana) _player.mana_max=_player.mana;
//				if(_player.life_max<_player.life) _player.life_max=_player.life;
				if(_player.mana==0)  _player.mana_max=_player.mana;
				if(_player.life==0)  _player.life_max=_player.life;
			}
					//return NULL;
			return _tempbuf;
					break;
				case 7: //action
					
//					if(_player.id == object_id){
	//					return NULL;
		//			}
			//		return NULL;
					switch(*(aPacket +6)) {
						case 2: //stop and hit
							break;
						case 0: //stop and do nothing
							break;
							return NULL;
						case 3: //hit
							break;
							x = *(aPacket+28) * 256+*(aPacket+29);
							y = *(aPacket+32) * 256+*(aPacket+33);
							if(_player.id != object_id){
								//UpdateMonster(object_id,x,y);
								break;
							}
							sprintf_s(_tempbuf,"action to: %x,%x ",x,y);
							_player.old_pos.x=_player.pos.x;
							_player.old_pos.y=_player.pos.y;
							_player.pos.x=x;
							_player.pos.y=y;
							break;
						case 1:	//move
							x = *(aPacket+12) * 256+*(aPacket+13);
							y = *(aPacket+16) * 256+*(aPacket+17);
							if(_player.id != object_id){
								//UpdateMonster(object_id,x,y);
								break;
							}
				//			sprintf_s(_tempbuf,"player pos: %x,%x\n",x,y);
							_player.old_pos.x=_player.pos.x;
							_player.old_pos.y=_player.pos.y;
							_player.pos.x=x;
							_player.pos.y=y;
							return _tempbuf;
							break;
					}
					break;
				case 1: //life mana update by eating potion
					
					if(*(aPacket+9) == 0xa8 || (*(aPacket+7)==3 && *(aPacket+ 21)==0xa8)) //drink mana
					{
						time(&_time_drink_mana_potion);
						return "drink mana potion";
					}
					if(*(aPacket+9) == 0xa7 || (*(aPacket+7)==3 && *(aPacket+ 21)==0xa7))//drink life
					{
						time(&_time_drink_life_potion);
						return "drink life potion";
					}
					//else return NULL;
					break;
			}
	return _tempbuf;
}

char* ItemOnMouse(BYTE* aPacket,DWORD len)
{
	BYTE action;
	DWORD64 itemid;
	DWORD itemid_high,itemid_low;

	action = *(aPacket + 13);
	if(action == 1) // on mouse 
	{
		itemid= *((DWORD64*)(aPacket+46));
		itemid_high=itemid/0x100000000;
		itemid_low = itemid%0x100000000;
		sprintf_s(_tempbuf,"item:id %x-%x ",itemid_high,itemid_low);		
	}
	if(action == 0) // off mouse
	{
	//	return NULL;
		BYTE box_pos_x = *(aPacket+22);
		BYTE box_pos_y = *(aPacket+23);
	sprintf_s(_tempbuf,"%s,box x: %x,box y:%x\n ",_tempbuf,box_pos_x,box_pos_y);
	}
	return _tempbuf;
}



