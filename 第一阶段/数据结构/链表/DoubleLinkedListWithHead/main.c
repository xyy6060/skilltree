#include "DoubleLinkedListWithHead.h"
/*
	FUNC:链表中存的是整数，有正有负，写个函数把链表list中的负数放在正数前面
*/
struct LinkedList*FUNC(struct LinkedList*list)
{
	struct node*p = list->first;
	struct node*psave = NULL;
	while(p)
	{
		//step1:找为负数的节点
		while(p)
		{
			if(p->data < 0)
				break;
			p = p->next;
		}
		
		//step2:把关系断掉，按照“头插法”加入到链表中
		if(p)
		{
			if(p == list->first)
			{
				p = p->next;
			}
			else if(p == list->last)
			{
				p->prev->next = NULL;
				list->last = p->prev;
				p->prev = NULL;
				p->next = list->first;
				list->first->prev = p;
				list->first = p;
				p = NULL;
			}
			else//找到的负数在中间
			{
				psave = p->next;
				p->prev->next = p->next;
				p->next->prev = p->prev;
				p->next = NULL;
				p->prev = NULL;
				p->next = list->first;
				list->first->prev = p;
				list->first = p;
				p = psave;
			}
		}
	}
	return list;
}
/*
	SonList:链表A,B.判断A 是否为B的子序列
		子序列：连续的一部分
	返回值 ：是 1
			不是  0
*/
int SonList(struct LinkedList*list1,struct LinkedList*list2)
{
	//step1：排除掉list1的节点数大于list2的节点数的这种情况
	if(list1->num > list2->num)
	{
		return 0;
	}
	
	//step2:判断是否是list2的子序列
	struct node*p1 = list1->first;
	struct node*p2 = list2->first;
	struct node*p2save = p2->next;//指向p2下一次开始的节点
	/*
	while(p1 && p2)
	{
		p2save = p2->next;
		while(p1 && p2)
		{
			if(p1->data == p2->data)
			{
				p1 = p1->next;
				p2 = p2->next;
			}
			else
			{
				p1 = list1->first;
				p2 = p2save;
				break;
			}
		}
	}
	if(p1 == NULL)
	{
		return 1;
	}
	else
	{
		return 0;
	}
	*/
	while(p1 && p2)
	{
		if(p1->data == p2->data)
		{
			p1 = p1->next;
			p2 = p2->next;
		}
		else
		{
			p2 = p2save;
			p1 = list1->first;
			p2save = p2save->next;//一直在更新
		}
	}
	if(p1 == NULL)
	{
		return 1;
	}
	else
	{
		return 0;
	}
}
/*
	Same_node:求两个升序链表的交集。A，B都有的元素
	把交集组成一个新链表C，原链表A,B保持不变,且C也要升序
*/
struct LinkedList*Same_node(struct LinkedList*list1,struct LinkedList*list2)
{
	struct node*p1 = list1->first;
	struct node*p2 = list2->first;
	//step1：创建一个新链表的头结点
	struct LinkedList*list = malloc(sizeof(*list));
	list->first = list->last = NULL;
	list->num = 0;
	strcpy(list->name,"qwe");
	
	//step2:找交集
	while(p1 && p2)
	{
		if(p1->data == p2->data)
		{
			if(list->first == NULL || list->last->data != p1->data)
			{
				struct node*pnew = malloc(sizeof(*pnew));
				pnew->next = pnew->prev = NULL;
				pnew->data = p1->data;
				if(list->first == NULL)
				{
					list->first = pnew;
					list->last = pnew;
				}
				else
				{
					list->last->next = pnew;
					pnew->prev = list->last;
					list->last = pnew;
				}
				list->num++;
			}
			p1 = p1->next;
			p2 = p2->next;
		}
		else if(p1->data > p2->data)
		{
			p2 = p2->next;
		}
		else
		{
			p1 = p1->next;
		}
	}
	
	return list;
}
int main()
{
	struct LinkedList*l1 = Create_Sort_Double_ListWithHead();
	struct LinkedList*l2 = Create_Sort_Double_ListWithHead();
	//struct LinkedList*l = Create_Sort_Double_ListWithHead();
	//l = add_x_node(l,2,3);
	//printf("line = %d\n",__LINE__);
	//l = delete_all_xnode(l,2);
	//l = FUNC(l);
	//printf("line = %d\n",__LINE__);
	//printf("return :%d\n",SonList(l1,l2));
	struct LinkedList*l = Same_node(l1,l2);
	Print(l1);
	Print(l2);
	Print(l);
}