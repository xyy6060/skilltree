#include "LinkedList.h"
/*
	Create_Linkedlist:创建一个不带头节点的单向链表
*/
struct node*Create_Linkedlist()
{
	struct node*first = NULL;
	struct node*last = NULL;
	ElemType d;
	while(1)
	{
		//step1:获取数据，开辟数据节点的空间
		scanf("%d",&d);
		if(d == 0)
			break;
		
		struct node*pnew = (struct node*)malloc(sizeof(struct node));
		
		//step2:给数据节点赋值
		pnew->data = d;
		pnew->next = NULL;
		
		//step3：把数据节点加入到链表中
		if(first == NULL)//空链表
		{
			first = pnew;
			last = pnew;
		}
		else//尾插法
		{
			last->next = pnew;
			last = pnew;
		}
		#if 0
		else//头插法
		{
			pnew->next = first;
			first = pnew;
		}
		#endif
	}
	
	
	return first;
}
/*
	Create_Sort_Linkedlist:创建一个升序的不带头节点的单向链表
*/
struct node*Create_Sort_Linkedlist()
{
	struct node*first = NULL;
	ElemType d;
	while(1)
	{
		//step1:获取数据，开辟数据节点的空间
		scanf("%d",&d);
		if(d == 0)
			break;
		
		struct node*pnew = (struct node*)malloc(sizeof(struct node));
		
		//step2:给数据节点赋值
		pnew->data = d;
		pnew->next = NULL;
		
		//step3：把数据节点加入到链表中
		if(first == NULL)//空链表
		{
			first = pnew;
		}
		else
		{
			struct node*p = first;//遍历指针
			struct node*pre = NULL;//指向p这个节点前面的那个节点
			while(p)
			{
				if(p->data > pnew->data)
				{
					break;
				}
				pre = p;
				p = p->next;
			}
			if(p)//找到了
			{
				if(p == first)
				{
					pnew->next = first;
					first = pnew;
				}
				else
				{
					pnew->next = p;
					pre->next = pnew;
				}
			}
			else
			{
				pre->next = pnew;
			}
			
		}
	
	}
	
	
	return first;
}
/*
	Change_xnode:
	在一个单链表f中找到值为x的数据节点，将其值改为y。如果有多个值为x的节点，
			改找到的第一个节点，若没有，不需要改
			返回新链表的首节点的地址		
*/
struct node*Change_xnode(struct node*f,int x,int y)
{
	struct node*p = f;
	//step1:找到值为x的节点
	while(p)
	{
		if(p->data == x)
		{
			//step2：修改
			p->data = y;
			break;
		}
		p = p->next;//更新p，把p往后移
	}
	
	return f;
	
}
/*
	add_a_node:
	在一个单链表f中找到值为x的数据节点，在其前面增加一个值为y的数据节点。
			如果有多个值为x的节点，就在找到的第一个节点前面增加，若没有，就加在最后。
			返回新链表的首节点的地址	
*/
struct node*add_a_node(struct node*f,int x,int y)
{
	struct node*p = f;//遍历指针
	struct node*pre = NULL;//指向p这个节点前面的那个节点
	//step1:开辟新的数据节点的空间，并赋值
	struct node* pnew = malloc(sizeof(*pnew));
	pnew->data = y;
	pnew->next = NULL;
	
	//step2:找到值为x的节点
	while(p)
	{
		if(p->data == x)
		{
			break;
		}
		pre = p;
		p = p->next;
	}
	
	//step3:加入
	if(p)//找到
	{
		if(p == f)//p在首节点
		{
			pnew->next = f;
			f = pnew;
		}
		else
		{
			pnew->next = p;
			pre->next = pnew;
		}
	}
	else
	{
		if(f == NULL)
		{
			f = pnew;
		}
		else
		{
			pre->next = pnew;
		}
	}
		
	return f;	
}
/*
	add_an_element:
	在一个单链表f中找到值为x的数据节点，在其后面增加一个值为y的数据节点。
			如果有多个值为x的节点，就在找到的第一个节点前面增加，若没有，就加在最后。
			返回新链表的首节点的地址	
*/
struct node*add_an_element(struct node*f,int x,int y)
{
	struct node*p = f;//遍历指针
	struct node*pre = NULL;//指向p这个节点前面的那个节点
	//step1:开辟新的数据节点的空间，并赋值
	struct node* pnew = malloc(sizeof(*pnew));
	pnew->data = y;
	pnew->next = NULL;
	
	//step2:找到值为x的节点
	while(p)
	{
		if(p->data == x)
		{
			break;
		}
		pre = p;
		p = p->next;
	}
	
	//step3:加入
	if(p)//找到
	{
		pnew->next= p->next;
		p->next = pnew;
	}
	else
	{
		if(f == NULL)
		{
			f = pnew;
		}
		else
		{
			pre->next = pnew;
		}
	}
		
	return f;	
}
/*
	删除：
			delete_x_node:在一个单链表f中找到值为x的数据节点，将其删除
			如果有多个值为x的节点，就删除找到的第一个节点，若没有，不删除。
			返回新链表的首节点的地址
			
*/
struct node*delete_x_node(struct node*f,int x)
{
	struct node*p = f;//遍历指针
	struct node*pre = NULL;//指向p这个节点前面的那个节点
	//step1:找到值为x的节点
	while(p)
	{
		if(p->data == x)
		{
			break;
		}
		pre = p;
		p = p->next;
	}
	
	//step2:删除（找到了）
	if(p)
	{
		if(p == f)//p在首节点
		{
			f = f->next;
			p->next = NULL;
			free(p);
			p = NULL;
		}
		else
		{
			pre->next = p->next;
			p->next = NULL;
			free(p);
			p = NULL;
		}
	}
	
	return f;
}

/*
	Print ：打印整个链表
*/
void Print(struct node*f)
{
	printf("==================\n");
	struct node*p = f;//遍历指针
	
	while(p)
	{
		printf("%d ",p->data);
		p = p->next;
	}
	
	printf("\n");
}


		