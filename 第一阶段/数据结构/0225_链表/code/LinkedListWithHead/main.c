#include "LinkedListWithHead.h"

/*
	func:在一个带头节点的单链表list中找到倒数第k个节点，将其节点的地址返回	
*/
struct node*func(struct LinkedList*list,int k)
{
	if(list->num < k)
	{
		return NULL;
	}
	int n = list->num-k;
	struct node*p = list->first;
	while(n--)
	{
		p = p->next;
	}
	
	return p;
}	
int main()
{
	struct LinkedList*l = Create_LinkedListWithHead();
	//printf("%d\n%d\n%d\n%s\n",l->first->data,l->last->data,l->num,l->name);
	//printf("%d %s\n",__LINE__,__FUNCTION__);
	printf("num = %d\n",l->num);
	//l = add_x_node(l,2,3);
	//printf("%d\n%d\n%d\n%s\n",l->first->data,l->last->data,l->num,l->name);
	//l = delete_all_xnode(l,2);
	//printf("num = %d\n",l->num);
	Print(l);
	struct node*p = func(l,3);
	if(p)
		printf("p->data = %d\n",p->data);
	else
		printf("linkedlist is NULL or num < k\n");
	
}