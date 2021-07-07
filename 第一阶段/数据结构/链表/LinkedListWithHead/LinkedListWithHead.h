#ifndef _LISTWITHHEAD_H_
#define _LISTWITHHEAD_H_
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
typedef int  ElemType;
struct node//数据节点
{
	ElemType data;//数据域 =》保存数据
	struct node*next;//指针域  =》保存逻辑关系上的下一个的地址
};
struct LinkedList//头节点
{
	struct node*first;//指向链表中的第一个节点
	struct node*last;//指向链表中的最后一个节点
	int num;
	char name[32];
	//....
};
/*
	Create_LinkedListWithHead():创建一个带头节点的链表
*/

struct LinkedList*Create_LinkedListWithHead();
/*
	Print:打印链表
*/
void Print(struct LinkedList*list);
/*
	add_x_node:在一个带头节点的单链表list中找到值为x的数据节点，在其前面增加一个值为y的数据节点。
			如果有多个值为x的节点，就在找到的第一个节点前面增加，若没有，就加在最后。
			返回新链表头节点的地址
*/
struct LinkedList*add_x_node(struct LinkedList*list,int x,int y);

/*
	delete_all_xnode：在一个带头节点的单链表list中找到值为x的数据节点，将其删除
		如果有多个值为x的节点，就全部删除，若没有，不删除。
		返回新链表的首节点的地址
*/
struct LinkedList*delete_all_xnode(struct LinkedList*list,int x);
#endif