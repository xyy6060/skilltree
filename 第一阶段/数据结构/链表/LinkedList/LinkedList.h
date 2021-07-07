#ifndef _LINKEDLIST_H_
#define _LINKEDLIST_H_
#include <stdio.h>
#include <stdlib.h>
typedef int  ElemType;
struct node
{
	ElemType data;//数据域 =》保存数据
	struct node*next;//指针域  =》保存逻辑关系上的下一个的地址
};
struct node*Create_Linkedlist();
/*
	Create_Sort_Linkedlist:创建一个升序的不带头节点的单向链表
*/
struct node*Create_Sort_Linkedlist();
/*
	Change_xnode:
	在一个单链表f中找到值为x的数据节点，将其值改为y。如果有多个值为x的节点，
			改找到的第一个节点，若没有，不需要改
			返回新链表的首节点的地址		
*/
struct node*Change_xnode(struct node*f,int x,int y);
/*
	add_a_node:
	在一个单链表f中找到值为x的数据节点，在其前面增加一个值为y的数据节点。
			如果有多个值为x的节点，就在找到的第一个节点前面增加，若没有，就加在最后。
			返回新链表的首节点的地址	
*/
struct node*add_a_node(struct node*f,int x,int y);
/*
	add_an_element:
	在一个单链表f中找到值为x的数据节点，在其后面增加一个值为y的数据节点。
			如果有多个值为x的节点，就在找到的第一个节点前面增加，若没有，就加在最后。
			返回新链表的首节点的地址	
*/
struct node*add_an_element(struct node*f,int x,int y);
/*
	删除：
			delete_x_node:在一个单链表f中找到值为x的数据节点，将其删除
			如果有多个值为x的节点，就删除找到的第一个节点，若没有，不删除。
			返回新链表的首节点的地址
			
*/
struct node*delete_x_node(struct node*f,int x);
/*
	Print ：打印整个链表
*/
void Print(struct node*f);


#endif