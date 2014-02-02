
typedef struct
{
	long x;
	long y;
} POS;

typedef struct
{
	DWORD id;
	BYTE type;
	BYTE action;
	BYTE action1;
	POS pos;
	POS pos1;
	WORD count;
	DWORD life;
	DWORD mana;
	DWORD shield;
	DWORD life_max;
	DWORD mana_max;
	BYTE quality;
	POS	old_pos;
	BYTE active;
	BYTE total_sockets;
	BYTE sockets_link;
//	char item_type[128];
	WORD good_item;
	DWORD item_type_id;
	DWORD object_type_id;
} PLAYER,MONSTER,OBJECT;

typedef struct {
	BYTE	comm;
	WORD	count;
} ACTION;
#define MAX_monster_array 1024



#define OBJ_NPC		1
#define OBJ_PORTAL	2
#define OBJ_PLAYER	3
#define OBJ_WP		4
#define OBJ_DOOR	5
#define OBJ_MONSTER	6
#define OBJ_CHEST	7
#define OBJ_USELESS_CHEST	8
#define OBJ_BOSS	9
#define OBJ_QUEST	10
#define OBJ_ITEM	11
#define OBJ_MONSTER_BIG	12
#define OBJ_MINION	13


/*
#define OBJ_NOT_SURE	0
#define OBJ_MONSTER 2
#define OBJ_DOOR	3
#define OBJ_BOX		4
#define OBJ_BIGBOX  9
#define OBJ_CONSUME	10
*/
#define ITEM_MAGIC 1
#define ITEM_RARE 2
#define ITEM_UNIQUE 3
//#define MONSTER_UNIQUE	8
//#define OBJ_UNIQUE_MONSTER	9


#define COLOR_BLACK	RGB(0,0,0)
#define COLOR_GREY	RGB(160,160,160)
#define COLOR_RED	RGB(255,0,0)
#define COLOR_GREEN	RGB(0,255,0)
#define COLOR_BLUE	RGB(0,0,255)
#define COLOR_YELLOW RGB(255,255,0)
#define COLOR_WHITE RGB(255,255,255)

//CommonFunction
void SaveFile(LPCTSTR lpszFormat,...);
extern void showinfo(LPCTSTR lpszFormat,...);
extern void FormatOutput(char *in, char* out, DWORD len);

//MonsterMap
extern void AddMonster(MONSTER*);
extern void UpdateMonster(MONSTER *);
extern void RemoveMonster(DWORD);
extern void InitMonsterMap();
extern MONSTER 	_monster_array[];
extern MONSTER 	_nearest_monster;
extern void MonsterDie(DWORD id);
extern void DrawMapFlag(HWND);
extern DWORD	GetMonsterType(DWORD id);

//mywindow
extern int mywindow();
extern HWND _chwnd;
extern BOOL _draw_map;
extern char	_objTypeStr[];
extern char	_itemTypeStr[];
//ReceivePacket
extern char* ObjectUpdate(BYTE*, DWORD);
//extern char* ObjectAdd(BYTE*, DWORD);
extern char* ObjectRemove(BYTE*, DWORD);
extern char* MonsterDie(BYTE*, DWORD);
extern char* ItemOnMouse(BYTE*,DWORD);
extern HWND	_hwnd;
extern HMODULE	_hmodule;
extern time_t	_time_drink_life_potion,_time_drink_mana_potion;
extern PLAYER _player;
extern ACTION _action;
extern MONSTER _monster;
extern char _tempbuf[102400];

extern BYTE	process_f0(BYTE*,DWORD);
extern BYTE	process_f1(BYTE*,DWORD);
extern BYTE	process_f2(BYTE*,DWORD);
extern BOOL DrinkLifePotion();
extern BOOL DrinkManaPotion();

//SearchMemory
extern BOOL ShowProcMemInfo();
extern char *SearchMemory1();
extern HANDLE	_hProcess;

//SendPacket
extern char *ProcessPacketBeforeSend(BYTE*,DWORD);
extern BOOL _auto_attack;

extern void MyLoop();
extern "C" __declspec(dllexport) void DrawMap(HWND);

extern BYTE	_show;

extern DWORD readObjectType(DWORD id);
extern DWORD readItemType(DWORD id);
extern void writeObjectType(DWORD id, char*buf);
extern void writeItemType(DWORD id, char*buf);
extern void LoadBotSetting();
extern WORD _life_flask;


//LinkNode.cpp
extern void _poe_link_destruct();
extern void _poe_link_contruct();
extern char* _poe_link_get_obj(DWORD);
extern char* _poe_link_get_item(DWORD);
extern BOOL _add_object_link(DWORD id,char* buf);
extern BOOL _add_item_link(DWORD id,char* buf);
