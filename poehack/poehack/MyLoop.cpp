#include "stdafx.h"
#include "myCode.h"

void MyLoop()
{	
//	if(_draw_map) {DrawMap(_chwnd);_draw_map=FALSE;}
//	static int i=0;	i++;if(i<3) return;i=0;
	if(_draw_map)
		InvalidateRect (_chwnd, NULL, TRUE) ;
	_draw_map=false;
//	showinfo("auto_attack:%d",_auto_attack);

	if(_player.life<_player.life_max/1.5) {DrinkLifePotion();return;}
	if(_player.mana<_player.mana_max/4) {DrinkManaPotion();return;}

//	if(_auto_attack) 	{
//		showinfo("auto_attacking");SendMessage(_hwnd,WM_KEYDOWN,0X54,0);SendMessage(_hwnd,WM_KEYDOWN,0X52,0);}
}
