#include "stdafx.h"
#include "windows.h"
#define MAX_STR_LENGTH 1024

typedef struct POE_NODE{
	POE_NODE	*left;
	DWORD	id;
	char	*str;
	POE_NODE	*right;
}POE_LINK;

POE_LINK	_obj_link_head,_item_link_head;


char* poe_link_add(POE_LINK *head,POE_LINK	*link)
{
	POE_LINK	*ptr = head;

	while(ptr->right)
	{
		ptr=ptr->right;
		if(ptr->id == link->id) return ptr->str;
	}
	//link->str==NULL, Check if id in link only, return TRUE if not found
	if(link->str==NULL) return NULL;

	ptr->right=new POE_LINK;
	ptr->right->id=link->id;
	ptr->right->str=new char[MAX_STR_LENGTH];
	strcpy(ptr->right->str,link->str);
	ptr->right->left=ptr;
	ptr->right->right=NULL;
	return link->str;
}

BOOL poe_link_delete(POE_LINK *head,DWORD id)
{
	POE_LINK	*ptr = head;

	while(ptr)
	{
		if(ptr->id == id)
		{
			ptr->left->right=ptr->right;
			ptr->right->left=ptr->left;
			delete ptr->str;
			delete ptr;
			return TRUE;
		}
		ptr=ptr->right;
	}
	return FALSE;
}

void poe_link_delete_all(POE_LINK *head)
{
	POE_LINK	*ptr = head->right, *temp;
	while(ptr!=NULL)
	{
		temp=ptr;
		ptr=ptr->right;
		delete temp->str;
		delete temp;
	}
	head->right=NULL;
}

char* _poe_link_get_obj(DWORD id)
{
	POE_LINK	link;
	link.id=id;
	link.str=NULL;
	return poe_link_add(&_obj_link_head,&link);
}
char* _poe_link_get_item(DWORD id)
{
	POE_LINK	link;
	link.id=id;
	link.str=NULL;
	return poe_link_add(&_item_link_head,&link);
}

//return: 
//TRUE: added
//FALSE: already in the link
BOOL _add_object_link(DWORD id,char* buf)
{
	POE_LINK	*link = new POE_LINK;
	link->str = new char[MAX_STR_LENGTH];
	link->id=id;
	strcpy(link->str,buf);
	poe_link_add(&_obj_link_head,link);
	return TRUE;
}

//return: 
//TRUE: added
//FALSE: already in the link
BOOL _add_item_link(DWORD id,char* buf)
{
	POE_LINK	*link = new POE_LINK;
	link->str = new char[MAX_STR_LENGTH];
	link->id=id;
	strcpy(link->str,buf);
	poe_link_add(&_item_link_head,link);
	return TRUE;
}


void _poe_link_delete_all()
{
	poe_link_delete_all(&_obj_link_head);
	poe_link_delete_all(&_item_link_head);
}


char _line_buf[MAX_STR_LENGTH];
void parse_and_add_line(int type,char *buf)
{
	char*	ptr=strstr(buf,"=");
	if(ptr==NULL) return;
	char*	value=ptr+1;
	ptr[0]=0;

	DWORD	id=strtoul(buf,NULL,16);
	POE_LINK	link;
	char	tempbuf[1024];

	link.id=id;
	link.str=tempbuf;
	strncpy(link.str,value,MAX_STR_LENGTH);
	if(type==1) poe_link_add(&_obj_link_head,&link);
	if(type==2) poe_link_add(&_item_link_head,&link);
}

void _poe_link_destruct()
{
	poe_link_delete_all(&_obj_link_head);
	poe_link_delete_all(&_item_link_head);	
}

void _poe_link_contruct()
{
	memset(&_obj_link_head,0,sizeof(POE_LINK));
	memset(&_item_link_head,0,sizeof(POE_LINK));

	FILE	*fp=fopen("c:\\bot\\d3\\poehack.ini","r");
	if(fp==NULL) return;

	int	type=0;
	while( fgets(_line_buf,1024,fp) ) {
		if(!strcmp(_line_buf,"[Object Type]\n"))
		{
			type=1;
			continue;
		}
		if(!strcmp(_line_buf,"[Item Type]\n"))
		{
			type=2;
			continue;
		}
		parse_and_add_line(type,_line_buf);
    }
/*
	POE_LINK	*ptr=&_item_link_head;
	while(ptr->right)
	{
		ptr=ptr->right;
		printf("%x -- %s",ptr->id,ptr->str);
	}

*/
	fclose(fp);
	return;
}


