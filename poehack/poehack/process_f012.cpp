#include "stdafx.h"
#include "myCode.h"

BYTE	GetObjectType(char*);
char	_objTypeStr[1024];
char	_itemTypeStr[1024];
BOOL	IsMonster(BYTE);

BYTE process_f0(BYTE* aPacket,DWORD len)
{
	char	str[100];
	static	int box_status=-1;
	static	int big_box_status=-1;
	DWORD	id;
	static	int socket_count=0;
	static	int blue=0,green=0,red=0;

	str[0]=0;
	if(len==4 && aPacket[0]==0x40 && aPacket[1]==0x80 && aPacket[2]==0 && aPacket[3]==0) //dead monster
	{
		//RemoveMonster(_monster.id);
		_monster.id=0;
	}
	if(aPacket==NULL && len==0) //packet over
	{
		if(_monster.type==0) _monster.id=0;
		if(_monster.quality== ITEM_RARE || _monster.quality==ITEM_UNIQUE)
			_monster.good_item=1;
		if(strstr(_itemTypeStr,"Wand") && _monster.total_sockets==3 && _monster.sockets_link==3)
			_monster.good_item=1;
//		if(_monster.total_sockets>2 && _monster.sockets_link>2)
//			_monster.good_item=1;
		if(blue+green+red==3 && _monster.sockets_link>2)
			_monster.good_item=1;
		if(_monster.total_sockets>4)
			_monster.good_item=1;
		if(strstr(_itemTypeStr,"Items/Flask") && _monster.quality==ITEM_MAGIC)
			_monster.good_item=1;
		if(strstr(_itemTypeStr,"Items/Currency"))
			_monster.good_item=1;
		if(strstr(_itemTypeStr,"Items/Gems"))
			_monster.good_item=1;
		if(_show=='3') {
			if(_monster.type==OBJ_ITEM)
				showinfo("type:%d, id:%x, quality:%d, socket:%d, link%d, type:%s",_monster.type,_monster.id,_monster.quality,_monster.total_sockets,_monster.sockets_link,_itemTypeStr);
			if(_monster.type==OBJ_MONSTER)
				showinfo("monster type:%d, %s",_monster.type,_objTypeStr);
		}

		/*
		if(_action.count<25) //can be box,wp,door,flask,jewel && _monster.type==OBJ_ITEM)	//special object
		{
			if(box_status==1)	//opened boxpe
				_monster.id=0;[0
			else if(box_status==0)
				_monster.type = OBJ_BOX;
			else if(big_box_status==1)	//opened box
				_monster.id=0;
			else if(big_box_status==0)
				_monster.type = OBJ_BIGBOX;
			else if(_monster.type == OBJ_NOT_SURE)
				_monster.type = OBJ_DOOR;
		}

		if(_monster.id!=0)
			AddMonster(&_monster);
		box_status=-1;
		big_box_status=-1;
		return 0;
		*/	
		if(_monster.id!=0)
		{
			_monster.type=GetObjectType(_objTypeStr);
			AddMonster(&_monster);
			memset(&_monster,0,sizeof(MONSTER));
			_itemTypeStr[0]=0;
			_objTypeStr[0]=0;
	//		showinfo("add 1, type:%d", _monster.type);
		}
//		showinfo("total colore:%d", blue+green+red);
		socket_count=0;
		blue=0;green=0;red=0;
	}
	if(aPacket==NULL && len==1) //from total number of sockets call
	{
		socket_count=1;
		return 0;
	}

	if(socket_count>0) socket_count++;
	switch(socket_count)
	{
		case 2:
		case 4:
		case 6:
		case 8:
		case 10:
		case 12:
			if(aPacket[0]==3) blue=1;
			if(aPacket[0]==2) green=1;
			if(aPacket[0]==1) red=1;
//			showinfo("socket_count:%d,color: %d",socket_count,aPacket[0]);
			if(socket_count>=_monster.total_sockets*2) socket_count=0;
			break;
	}

	switch(_action.count)
	{
		case 1:
			_monster.object_type_id = htonl(*((DWORD*)(aPacket)));
			_objTypeStr[0]=0;
			readObjectType(_monster.object_type_id);
			break;
		case 2:
			_monster.type=GetObjectType(_objTypeStr);
//			showinfo("%d, %s",_objTypeStr);
//			showinfo("%d, %s",_monster.type,_objTypeStr);
			_monster.id=htons(*((WORD*)(aPacket+2)));

			sprintf_s(str,"id: %x",_monster.id);
			break;
		case 4:
			_monster.pos.x = htons(*((WORD*)(aPacket+2)));
			sprintf_s(str,"pos.x: %x",_monster.pos.x);
			break;
		case 5: 
			_monster.pos.y = htons(*((WORD*)(aPacket+2)));
			sprintf_s(str,"pos.y: %x",_monster.pos.y);
//			showinfo("new monster,id:%x,x=%d,y=%d",_monster.id,_monster.pos.x,_monster.pos.y);
			break;
		case 7:
			if(_monster.type==OBJ_PLAYER  && _player.id==0) //player
			{
				_player.id=_monster.id;
				_player.pos.x=_monster.pos.x;
				_player.pos.y=_monster.pos.y;
				strcpy_s(str,"this is player");
				showinfo("player id:%x",_player.id);
			}
			break;
		case 8:
			if(!(aPacket[0]==0x3f && aPacket[1]==0x80 && aPacket[2]==0 && aPacket[3]==0)) //unique
			{
				//_monster.quality = MONSTER_UNIQUE;
			}
			break;
		case 11:
//			if(len==1) _monster.type=OBJ_ITEM;
//			if(len==2) _monster.type=OBJ_MONSTER;
//			if(len==4) _monster.type=OBJ_NOT_SURE; //door,wp,big coffin
			break;
		case 12:
			if(len==1 && _monster.type==OBJ_CHEST) 
			{
				box_status=aPacket[0];
				if(box_status==1) _monster.id=0;
			}
			break;
		case 14:
			if(_monster.type==OBJ_ITEM && len==4)
			{
			_monster.item_type_id = htonl(*((DWORD*)(aPacket)));
			readItemType(_monster.item_type_id);
			}
			break;
		case 16:
			if(len==1 && _monster.type==OBJ_CHEST) 
			{
				big_box_status=aPacket[0];
				if(big_box_status==1) _monster.id=0;
			}
			break;
		case 24:
			break;
			if(_monster.type == OBJ_ITEM)
			{
				if(len==1 && aPacket[0]==2) 
				{
					_monster.quality=ITEM_RARE;
//					UpdateMonster(&_monster);
				}
				if(len==1 && aPacket[0]==1) 
				{
					_monster.quality=ITEM_MAGIC;
//					UpdateMonster(&_monster);
				}
			}
			break;
		case 50:
//			_monster.type = OBJ_MONSTER;
			strcpy_s(str,"should be monster");
//			UpdateMonster(&_monster);
			break;
	}
		FormatOutput((char*)aPacket,_tempbuf,len);
		sprintf_s(_tempbuf,"%s *** %s (%d) ,type:%d",_tempbuf,str,_action.count,_monster.type);
//		showinfo(_tempbuf);
	return 0;
}




BYTE process_f1(BYTE* aPacket,DWORD len)
{
//	if(len==1 && *aPacket==0)
//		{showinfo("done from f1, id:%x,x=%d,y=%d---x=%d,y=%d",_monster.id,_monster.pos.x,_monster.pos.y,_monster.pos1.x,_monster.pos1.y);return 0;}
	//showinfo("in f1, command:%rnd",command);
/*
		case 3:	//life mana update
			if(_player.id==0) _player.id=object_id;
			if(_player.id==object_id)
			{
				_player.life = *(aPacket+8) * 256+*(aPacket+9);
				_player.mana = *(aPacket+17) * 256+*(aPacket+18);
				_player.shield = *(aPacket+26) * 256+*(aPacket+27);
				if(_player.mana_max<_player.mana) _player.mana_max=_player.mana;
				if(_player.life_max<_player.life) _player.life_max=_player.life;
				if(_player.life<_player.life_max/1.5) 
					if(DrinkLifePotion()) {
							sprintf_s(_tempbuf,"drink life potion, life: %d, life_max:%d",_player.life,_player.life_max);
							return _tempbuf;}
					if(_player.mana<_player.mana_max/4)
						if(DrinkManaPotion()) return "drink mana potion22";
					sprintf_s(_tempbuf,"player life: %d, mana: %d, shield:%d ",_player.life,_player.mana,_player.shield);
					
					}*/
	switch(_action.count)
	{
		case 0:
			break;
		case 1:
			_monster.id=htons(*((WORD*)(aPacket+2)));
//			showinfo("monster xx id:%x,%x",_monster.id,aPacket[0]);
			break;
		case 2:
//			showinfo("monster id:%x",_monster.id);
			_monster.action = *aPacket;	//7 behave;1 drink;3 life update;5 die
			if(_monster.action == 4) 
				{
//					showinfo("mouseter died:%x",_monster.id);
					MonsterDie(_monster.id);
					RemoveMonster(_monster.id);	
			}
			break;
		case 3:
			if(_monster.action==7) _monster.action1 = *aPacket;
			//action1:
			//2 stop and hit
			//0 stop and do nothing
			//3 hit
			//1 move
			if(_monster.action ==3)  _monster.life = *(aPacket+2)*256+*(aPacket+3);//life/mana/shield update
			break;
		case 4:
			if(_monster.action1 == 1 || _monster.action1==3) _monster.pos.x = htons(*((WORD*)(aPacket+2)));	//move pos
			break;
		case 5:
			if(_monster.action1 == 1  || _monster.action1==3)
			{
				_monster.pos.y = htons(*((WORD*)(aPacket+2)));
				//_monster.type = OBJ_MONSTER;
				UpdateMonster(&_monster);
			}
			
			break;
		case 6:
			if(_monster.action ==3)  _monster.mana = htons(*((WORD*)(aPacket+2)));//life/mana/shield update
			break;
		case 9:
			if(_monster.action1 == 1)	_monster.pos1.x = htons(*((WORD*)(aPacket+2)));
			if(_monster.action ==3) 
			{
				_monster.shield = htons(*((WORD*)(aPacket+2)));//life/mana/shield update
				if(_player.id==_monster.id)
				{
					_player.id=_monster.id;
					_player.life=_monster.life;
					_player.mana=_monster.mana;
					_player.shield=_monster.shield;

					if(_player.mana_max<_player.mana) _player.mana_max=_player.mana;
					if(_player.life_max<_player.life) _player.life_max=_player.life;
					if(_player.life<_player.life_max/1.5) 
					if(DrinkLifePotion()) showinfo("drink life potion, life: %d, life_max:%d",_player.life,_player.life_max);
							
					if(_player.mana<_player.mana_max/4) if(DrinkManaPotion()) showinfo("drink mana");


					for(int i=0;i<MAX_monster_array;i++)
					{
						if(_monster.id==_monster_array[i].id)
						{
							_player.pos.x=_monster_array[i].pos.x;
							_player.pos.y=_monster_array[i].pos.y;
							break;
						}
					}
				}
			}
			break;
		case 10:
			if(_monster.action1 == 1)	
			{
				_monster.pos1.y = htons(*((WORD*)(aPacket+2)));
				if(_monster.id==_player.id)
				{
					_player.pos.x=_monster.pos.x;
					_player.pos.y=_monster.pos.y;
				}
				//UpdateMonster(_monster.id,_monster.pos.x,_monster.pos.y);
				//showinfo("moving");
			}
			break;
		default:
			switch(_monster.action)				
			{
				case 7:
//					return process_f1_action7(aPacket,len,command,_monster);
					break;
				case 3: //life mana update
//					return process_f1_action3(aPacket,len,command,monster);
					break;
				case 1: // drink
//					return process_f1_action1(aPacket,len,command,monster);
					break;
			}
	}
//	showinfo("in f1: count:%d,action:%d",_action.count,_monster.action);
	return 0;
}


BYTE process_f2(BYTE* aPacket,DWORD len)
{
	if(_action.count == 1)
	{
		_monster.id=htons(*((WORD*)(aPacket+2)));
		RemoveMonster(_monster.id);
//		showinfo("monster removed, id:%x",_monster.id);
	}
	return 0;
}

BYTE	GetObjectType(char* str)
{
	
	if(strstr(str,"Metadata/NPC")) return OBJ_NPC;
	if(strstr(str,"Metadata/Characters")) return OBJ_PLAYER;
	if(strstr(str,"Quest")) return OBJ_QUEST;
	if(strstr(str,"Waypoint")) return OBJ_WP;
	if(strstr(str,"AreaTransition")) return OBJ_DOOR;
	if(strstr(str,"Metadata/Monsters") && strstr(str,"Boss")) return OBJ_BOSS;
	if(strstr(str,"Metadata/Monsters") && strstr(str,"Big")) return OBJ_MONSTER_BIG;
	if(strstr(str,"Metadata/Monsters") && strstr(str,"Raised")) return OBJ_MINION;
	if(strstr(str,"Metadata/Monsters")) return OBJ_MONSTER;
	if(strstr(str,"WorldItem")) return OBJ_ITEM;
	if((strstr(str,"Barrel") || strstr(str,"Boulder") || strstr(str,"Crate")  || strstr(str,"Urn")) && strstr(str,"Chests")) return OBJ_USELESS_CHEST;
	if(strstr(str,"Metadata/Chests")) return OBJ_CHEST;
	return 0;
}

BOOL	IsMonster(BYTE type)
{
	if(type==OBJ_MONSTER || type==OBJ_MONSTER_BIG || type==OBJ_BOSS) return TRUE;
	return FALSE;

}
