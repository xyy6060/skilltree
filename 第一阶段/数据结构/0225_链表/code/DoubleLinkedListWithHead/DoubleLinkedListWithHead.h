#ifndef _DOUBLELISTWITHHEAD_H_
#define _DOUBLELISTWITHHEAD_H_
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
typedef int  ElemType;
struct node//数据节点
{
	ElemType data;//数据域 =》保存数据
	struct node*next;//指针域  =》保存逻辑关系上的下一个的地址
	struct node*prev;//指针域  =》保存逻辑关系上的上一个的地址
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
	Create_Double_ListWithHead:创建一个带头节点的双向链表
*/
struct LinkedList*Create_Double_ListWithHead();
/*
	Print:打印链表
*/
void Print(struct LinkedList*list);

struct LinkedList*add_x_node(struct LinkedList*list,int x,int y);
struct LinkedList*delete_all_xnode(struct LinkedList*list,ElemType x);
/*
	Create_Sort_Double_ListWithHead:创建一条升序的带头节点的双向链表(必做)
*/
struct LinkedList*Create_Sort_Double_ListWithHead();
#endif