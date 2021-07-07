#include "LinkedListWithHead.h"

/*
	Create_LinkedListWithHead():创建一个带头节点的链表
*/

struct LinkedList*Create_LinkedListWithHead()
{
	//step1:创建一个头节点，并赋值
	 struct LinkedList*list= malloc(sizeof(struct LinkedList));
	 list->first = list->last = NULL;
	 list->num = 0;
	 strcpy(list->name,"pxl");
	ElemType d;
	while(1)
	{
		//step2:获取数据，创建一个数据节点并赋值
		scanf("%d",&d);
		if(d == 0)
			break;
			
		struct node*pnew = malloc(sizeof(*pnew));
		pnew->next = NULL;
		pnew->data = d;
		//step3:把数据节点加入到链表中
		if(list->first == NULL)//空链表
		{
			list->first = pnew;
			list->last = pnew;
		}
		else
		{
			list->last->next = pnew;
			list->last = pnew;
		}
		
		list->num++;
	}
	
	
	return list;
}
/*
	add_x_node:在一个带头节点的单链表list中找到值为x的数据节点，在其前面增加一个值为y的数据节点。
			如果有多个值为x的节点，就在找到的第一个节点前面增加，若没有，就加在最后。
			返回新链表头节点的地址
*/
struct LinkedList*add_x_node(struct LinkedList*list,int x,int y)
{
	struct node*p = list->first;
	struct node*pre = NULL;
	//step1:创建一个数据节点并赋值
	struct node*pnew = malloc(sizeof(*pnew));
	pnew->next = NULL;
	pnew->data = y;
	
	
	//step2:查找x所在的位置
	while(p)
	{
		if(p->data == x)
			break;
		pre = p;
		p = p->next;
	}
	//step3:把节点加入
	if(p)//找到x
	{
		if(p == list->first)
		{
			pnew->next = list->first;
			list->first = pnew;
		}
		else
		{
			pnew->next = p;
			pre->next = pnew;
		}
	}
	else
	{
		if(list->first == NULL)
		{
			list->first = pnew;
			list->last = pnew;
		}
		else
		{
			list->last->next = pnew;
			list->last = pnew;
		}
	}
	list->num++;
	
	return list;
}
/*
	delete_all_xnode：在一个带头节点的单链表list中找到值为x的数据节点，将其删除
		如果有多个值为x的节点，就全部删除，若没有，不删除。
		返回新链表的首节点的地址
*/
struct LinkedList*delete_all_xnode(struct LinkedList*list,int x)
{
	struct node*p = list->first;
	struct node*pre = NULL;
	//step1：找到x所在的位置
	while(p)
	{
		if(p->data == x)
		{
			//step2:删除（找到了）
			if(p == list->first)
			{
				list->first = list->first->next;
				p->next = NULL;
				free(p);
				p = list->first;
			}
			else if(p == list->last)
			{
				list->last = pre;
				pre->next = NULL;
				free(p);
				p = NULL;
			}
			else
			{
				pre->next = p->next;
				p->next = NULL;
				free(p);
				p = pre->next;
			}
			list->num--;
		}
		else
		{
			pre = p;
			p = p->next;
		}
	}
	
	return list;
}
/*
	Print:打印链表
*/
void Print(struct LinkedList*list)
{
	printf("================\n");
	struct node*p = list->first;
	while(p)
	{
		printf("%d ",p->data);
		p = p->next;
	}
	
	printf("\n");
}