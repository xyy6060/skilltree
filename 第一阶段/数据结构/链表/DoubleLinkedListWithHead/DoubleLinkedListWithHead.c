#include "DoubleLinkedListWithHead.h"

/*
	Create_Double_ListWithHead:创建一个带头节点的双向链表
*/
struct LinkedList*Create_Double_ListWithHead()
{
	
	//step1：创建一个头结点，并赋值
	struct LinkedList*list = malloc(sizeof(*list));
	list->first = list->last = NULL;
	list->num = 0;
	strcpy(list->name,"pxl");
	
	
	ElemType d;
	while(1)
	{
		//step2:获取数据，就创建一个数据节点并赋值
		scanf("%d",&d);
		if(d == 0)
			break;
		
		struct node*pnew = malloc(sizeof(*pnew));
		pnew->prev = pnew->next = NULL;
		pnew->data = d;
		
		//step3:加入到链表中
		if(list->first == NULL)
		{
			list->first = pnew;
			list->last = pnew;
		}
		else//尾插法
		{
			pnew->prev = list->last;
			list->last->next = pnew;
			list->last = pnew;
		}
		
		list->num++;
	}
	
	return list; 
}
/*
	Create_Sort_Double_ListWithHead:创建一条升序的带头节点的双向链表(必做)
*/
struct LinkedList*Create_Sort_Double_ListWithHead()
{
	//step1：创建一个头结点，并赋值
	struct LinkedList*list = malloc(sizeof(*list));
	list->first = list->last = NULL;
	list->num = 0;
	strcpy(list->name,"pxl");
	
	
	ElemType d;
	while(1)
	{
		//step2:获取数据，就创建一个数据节点并赋值
		scanf("%d",&d);
		if(d == 0)
			break;
		
		struct node*pnew = malloc(sizeof(*pnew));
		pnew->prev = pnew->next = NULL;
		pnew->data = d;
		
		//step3:加入到链表中
		if(list->first == NULL)
		{
			list->first = pnew;
			list->last = pnew;
		}
		else//尾插法
		{
			struct node*p = list->first;
			while(p)
			{
				if(p->data > pnew->data)
					break;
				p = p->next;
			}
			if(p)
			{
				if(p == list->first)
				{
					pnew->next = list->first;
					list->first->prev = pnew;
					list->first = pnew;
				}
				else
				{
					pnew->next = p;
					pnew->prev = p->prev;
					p->prev = pnew;
					pnew->prev->next = pnew;
				}
			}
			else
			{
				list->last->next = pnew;
				pnew->prev = list->last;
				list->last = pnew;
			}
		}
		
		list->num++;
	}
	
	return list; 
}
/*
	add_x_node:在一个带头节点的双向链表list中找到值为x的数据节点，在其前面增加一个值为y的数据节点。
			如果有多个值为x的节点，就在找到的第一个节点前面增加，若没有，就加在最后。
			返回新链表头节点的地址
			
		
*/
struct LinkedList*add_x_node(struct LinkedList*list,int x,int y)
{
	struct node*p = list->first;//遍历指针
	//step1：创建新的数据节点并赋值
	struct node*pnew = malloc(sizeof(*pnew));
	pnew->next = pnew->prev = NULL;
	pnew->data = y;
	
	//step2:查找x所在的位置
	while(p)
	{
		if(p->data == x)
			break;
		p = p->next;
	}
	
	//step3:添加新的数据节点
	if(p)//找到
	{
		if(p == list->first)
		{
			pnew->next = p;
			p->prev = pnew;
			list->first = pnew;
		}
		else
		{
			pnew->next = p;
			pnew->prev = p->prev;	
			pnew->prev->next = pnew;
			p->prev = pnew;
		}
	}
	else//x未找到
	{
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
	}
	
	list->num++;
	
	return list;
}		
/*
	delete_all_xnode:在一个带头节点的双向链表list中找到值为x的数据节点，将其删除
		如果有多个值为x的节点，就全部删除，若没有，不删除。
		返回新链表的首节点的地址
*/	
struct LinkedList*delete_all_xnode(struct LinkedList*list,ElemType x)
{
	struct node*p = list->first;
	struct node*psave = NULL;//保存下一次p所指向的位置
	while(p)
	{
		//step1：找到要删除的节点
		while(p)
		{
			if(p->data == x)
				break;
			p = p->next;
		}
		//step2:删除（找到）
		if(p)
		{
			if(p == list->first)
			{
				if(list->num == 1)
				{
					list->first = NULL;
					list->last = NULL;
					free(p);
					p = NULL;
				}
				else
				{
					list->first = list->first->next;
					list->first->prev = NULL;
					p->next = NULL;
					free(p);
					p = list->first;
				}
				
			}
			else if(p == list->last)
			{
				list->last = p->prev;
				list->last->next = NULL;
				p->prev = NULL;
				free(p);
				p = NULL;
			}
			else
			{
				psave = p->next;
				p->prev->next = p->next;
				p->next->prev = p->prev;
				p->next = NULL;
				p->prev = NULL;
				free(p);
				p = psave;
				
			}
			
			list->num--;
		}
	}		
	
	
	return list;
}	   
/*
	Print:打印链表
*/
void Print(struct LinkedList*list)
{
	printf("==============\n");
	struct node*p = list->first;
	while(p)
	{
		printf("%d ",p->data);
		p = p->next;
	}
	printf("\n");
	p = list->last;
	while(p)
	{
		printf("%d ",p->data);
		p = p->prev;
	}
	printf("\n");
}