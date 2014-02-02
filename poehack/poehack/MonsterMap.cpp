#include "stdafx.h"
#include "myCode.h"

MONSTER	_monster_array[MAX_monster_array];
MONSTER _nearest_monster;
void GetNearestMonster();
void AddMonster(MONSTER*);
void UpdateMonster(MONSTER *);
void RemoveMonster(DWORD);
void InitMonsterMap();
DWORD	GetMonsterType(DWORD id);
BOOL	_map=FALSE;
extern PLAYER _player;
#define DEADMONSTER 32*1024
WORD _dead_monster[DEADMONSTER];
DWORD _dead_monster_count=0;
BOOL isMonsterDie(DWORD id);

BOOL isObjectMoved(MONSTER *ptr)
{
	if(ptr->pos.x== ptr->old_pos.x &&
		ptr->pos.y == ptr->old_pos.y)
		return FALSE;
	return TRUE;
}

void InitMonsterMap()
{
	_player.id=0;
	memset(_monster_array,0,sizeof(_monster_array));
	_map=TRUE;
	showinfo("map reset");
}

DWORD	GetMonsterType(DWORD id)
{
	for(int i=0;i<MAX_monster_array;i++)
		if(_monster_array[i].id == id)
			return _monster_array[i].type;
	return 0XFFFF;
}

void MonsterMapInfo()
{
	int count=0;
			showinfo("palyer id:%x, x:%d,y:%d",_player.id,_player.pos.x,_player.pos.y);
	for(int i=0;i<MAX_monster_array;i++)
	{
		if(_monster_array[i].id!=0)
		{/*
			_monster_array[i].old_pos.x = _monster_array[i].pos.x;
			_monster_array[i].old_pos.y = _monster_array[i].pos.y;
			_monster_array[i].pos.x = x;
			_monster_array[i].pos.y = y;
		*/
			showinfo("id:%x, type:%d,x:%d,y:%d",_monster_array[i].id,_monster_array[i].type,_monster_array[i].pos.x,_monster_array[i].pos.y);
		count ++;
			
		}
	}
	showinfo("total monster on map:%d",count);
}

void UpdateMonster(MONSTER *m)
{
	int i;
	/*
	for(i=0;i<MAX_monster_array;i++)
	{
		if(abs(_player.pos.y-_monster_array[i].pos.y) + abs(_player.pos.x-_monster_array[i].pos.x) >100)
			_monster_array[i].id=0;
		if(_monster_array[i].id == _player.id)
			_monster_array[i].id=0;
	}
	*/
	//MonsterMapInfo();
	BOOL found=FALSE;
	for(i=0;i<MAX_monster_array;i++)
		if(_monster_array[i].id==m->id)
		{
			
			_monster_array[i].old_pos.x = _monster_array[i].pos.x;
			_monster_array[i].old_pos.y = _monster_array[i].pos.y;
			_monster_array[i].pos.x = m->pos.x;
			_monster_array[i].pos.y = m->pos.y;
			_monster_array[i].active=1;

			//_monster_array[i].type=m->type;
			//if(m->quality!=0) _monster_array[i].item_quality = m->item_quality;
			//if(m->ique_monster!=0) _monster_array[i].unique_monster = m->unique_monster;
			
			//memcpy(&_monster_array[i], m, sizeof(MONSTER));
			DrawMapFlag(_chwnd);
			//found=TRUE;
			return;
		}
//	showinfo("add");
//	AddMonster(m);
}
void AddMonster(MONSTER *m)
{
	if(isMonsterDie(m->id)) return;
	for(int i=0;i<MAX_monster_array;i++)
		if(_monster_array[i].id==0)
		{
			/*
			_monster_array[i].id = m->id;
			_monster_array[i].pos.x = m->pos.x;
			_monster_array[i].pos.y = m->pos.y;
			_monster_array[i].type = m->type;
			*/
			memcpy(&_monster_array[i], m, sizeof(MONSTER));
//			MonsterMapInfo();
			DrawMapFlag(_chwnd);
			return;
		}
}
void RemoveMonster(DWORD id)
{
	for(int i=0;i<MAX_monster_array;i++)
		if(_monster_array[i].id==id)
		{
			_monster_array[i].id = 0;
//			showinfo("RR");
			DrawMapFlag(_chwnd);
			return;
		}
}

WORD distance(POS *p1, POS *p2)
{
	return abs(p1->x-p2->x) + abs(p1->y-p1->y);
}
void GetNearestMonster()
{
	int i;
	_nearest_monster.id=0;

	for(i=0;i<MAX_monster_array;i++)
	{
		if(_monster_array[i].id==0) continue;
		if(_monster_array[i].type==OBJ_ITEM) continue;
		if(_monster_array[i].id == _player.id) continue;
		if(_nearest_monster.id==0 || distance(&(_nearest_monster.pos),&_player.pos) > distance(&_monster_array[i].pos,&_player.pos))
		{
			if( distance(&_monster_array[i].pos,&_player.pos)>60) continue;
			_nearest_monster.id=_monster_array[i].id;
			_nearest_monster.pos.x=_monster_array[i].pos.x;
			_nearest_monster.pos.y=_monster_array[i].pos.y;
		}
	}
}

void DrawMapFlag(HWND hwnd)
{
	//DrawMap(hwnd);
	_draw_map = TRUE;
}
void DrawMap(HWND hwnd)
{
//	if(!_draw_map) return;
//	_draw_map=FALSE;
    HDC         hdc ;    
    PAINTSTRUCT ps ;
	DWORD	i,x,y;
	if(!_map) return;
//	MonsterMapInfo();
	hdc = BeginPaint (hwnd, &ps) ;
	char buf[100];
	GetNearestMonster();
	for(i=0;i<MAX_monster_array;i++)
	{
		buf[0]=0;
		if(_monster_array[i].id == 0 || (_monster_array[i].id == _player.id)) continue;
		x=320-(_player.pos.x-_monster_array[i].pos.x)/1;
		y=(_player.pos.y-_monster_array[i].pos.y)/1+200;
//		if(isObjectMoved(&_player) || (isObjectMoved(&_monster_array[i])))
		{
			SetTextColor(hdc,COLOR_GREY);
			if(_monster_array[i].type==OBJ_MONSTER)
			{
				strcpy_s(buf,"x");
				if(_show=='i') 
					sprintf(buf,"%x,%x,%x,x=%d,y=%d",_monster_array[i].type,_monster_array[i].id,i,_monster_array[i].pos.x,_monster_array[i].pos.y);
				if(_monster_array[i].type == OBJ_BOSS) {SetTextColor(hdc,RGB(255,0,0));strcpy_s(buf,"X");}
			}
			if(_monster_array[i].type==OBJ_ITEM && _monster_array[i].good_item==1)
			{
				strcpy_s(buf,"*");
			}
			if(_monster_array[i].type==OBJ_CHEST) strcpy_s(buf,"o");
			if(_monster_array[i].type==OBJ_DOOR || _monster_array[i].type==OBJ_WP) 
			{
				strcpy_s(buf,"O");
				if(_show=='i') 
					sprintf(buf,"%x,%x,%x,x=%d,y=%d",_monster_array[i].type,_monster_array[i].id,i,_monster_array[i].pos.x,_monster_array[i].pos.y);
			}
			if(_nearest_monster.id == _monster_array[i].id && _monster_array[i].id != _player.id) {
			}
//			sprintf_s(buf,"x(%d,%d)",_monster_array[i].pos.x,_monster_array[i].pos.y);
//			TextOut (hdc, x, y,buf,strlen(buf));			
			if(buf[0])TextOut (hdc, x, y,buf,strlen(buf));
			//showinfo("drawing: %d, %d, %s",x,y,buf);
		}
	}
//			TextOut (hdc, x, y,buf,1);
	SetTextColor(hdc,COLOR_BLACK);
	TextOut(hdc,320,200,"^",1);
//			sprintf_s(buf,"%d,%d",_player.pos.x,_player.pos.y);
//	TextOut(hdc,320,200,buf,strlen(buf));

	EndPaint (hwnd, &ps) ;
//	InvalidateRect (hwnd, NULL, TRUE) ;
	return;
}

void MonsterDie(DWORD id)
{
	return;
	if(_dead_monster_count>DEADMONSTER) return;
	_dead_monster[_dead_monster_count]=id/0x10000;
//	showinfo("die monster:id:%x, %x",id,_dead_monster[_dead_monster_count]);
	_dead_monster_count++;
	return;
}
BOOL isMonsterDie(DWORD id)
{
	return FALSE;
	DWORD i;
	for(i=0;i<_dead_monster_count;i++)
		if(_dead_monster[i]==id/0x10000)
			return TRUE;
	return FALSE;
}

