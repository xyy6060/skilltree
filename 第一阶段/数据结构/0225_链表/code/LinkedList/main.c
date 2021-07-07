#include "LinkedList.h"
/*
删除：
		delete_all_xnode:在一个单链表f中找到值为x的数据节点，将其删除
		如果有多个值为x的节点，就全部删除，若没有，不删除。
		返回新链表的首节点的地址
*/
struct node*delete_all_xnode(struct node*f,int x)
{
	struct node*p = f;//遍历指针
	struct node*pre = NULL;//指向p这个节点前面的那个节点
	
	while(p)
	{
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
				p = f;
			}
			else
			{
				pre->next = p->next;
				p->next = NULL;
				free(p);
				p = pre->next;
			}
		}
	}
	
	
	return f;
}
/*
	Sort_LinkedList():将一个链表排序（升序）
*/
struct node*Sort_LinkedList(struct node*f)
{
	struct node*pnew = f;//指向拆下来的节点
	struct node*first = NULL;
	while(pnew)
	{
		//step1：把原链表的节点一个一个拆下来
		f = f->next;
		pnew->next = NULL;
	
		//step2:重新组合
		if(first == NULL)
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
	
		pnew = f;//要拆的节点从原链表的第一个节点开始
	}
	
	return first;	
}
/*
	Merge:归并两个有序链表（升序）
		将两个升序链表合成一个升序链表
*/
struct node*Merge(struct node*fa,struct node*fb)
{
	struct node*pa = fa;
	struct node*pb = fb;
	struct node*first = NULL;//保存新组合的链表的第一个节点的地址
	struct node*last = NULL;
	struct node*pnew = NULL;//保存每次拿到的最小的节点
	while(pa && pb)
	{
		if(pa->data < pb->data)
		{
			pnew = pa;
			pa = pa->next;
		}
		else
		{
			pnew = pb;
			pb = pb->next;
		}
		if(first == NULL)
		{
			first = pnew;
			last = pnew;
		}
		else
		{
			last->next = pnew;
			last = pnew;
		}
	}
	if(last == NULL)//假如有一者为NULL
	{
		first = pa ? pa : pb;
	}
	else
	{
		last->next = pa ? pa : pb;
	}
	
	
	return first;
}
int main()
{
	printf("hgiruehg\n");
	struct node*pa = Create_Sort_Linkedlist();
	struct node*pb = Create_Sort_Linkedlist();
	
	struct node*p = Merge(pa,pb);
	Print(p);
}