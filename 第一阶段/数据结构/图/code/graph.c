#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "SqQueue.h"
#define BIG 65535
#define MAX 100
typedef char VType;

struct Graph
{
	VType V[MAX];//顶点的集合
	int A[MAX][MAX];//关系的集合 
	int vexnum;//图中的顶点个数
	int arcnum;//图中边或弧的个数
	//...
};
int visit[MAX];//0 =>未被访问 1 => 已访问
/*
	get_vertex_index:在图g的顶点数组中找到顶点元素为v的下标
*/
int get_vertex_index(struct Graph*g,VType v)
{
	int i;
	for(i = 0;i < g->vexnum;i++)
	{
		if(g->V[i] == v)
			return i;
	}
	
	return -1;
	
}
/*
	create_graph:创建一个图
*/

struct Graph*create_graph()
{
	//step1:开辟图的空间,赋值
	struct Graph*g = malloc(sizeof(*g));
	g->vexnum = 0;
	g->arcnum = 0;
	
	char ch;//临时存放顶点位置
	
	//step2:一个一个输入顶点字符
	while(1)
	{
		scanf("%c",&ch);
		if(ch == '#')
			break;
		
		g->V[g->vexnum++] = ch;
	}
	
	//step3:先要对关系数组进行最初的赋值
	int i,j;
	for(i = 0;i <g->vexnum;i++ )
	{
		for(j = 0;j < g->vexnum;j++)
		{
			g->A[i][j] = BIG;
		}
	}
	
	getchar();//用来吸收\n
	//输入关系 ： 起点  终点  权	
	//eg :AB2 
	//##0  结束输入
	
	while(1)
	{
		//
		char start,stop;
		int w;
		scanf("%c%c%d",&start,&stop,&w);
		getchar();
		if(start == '#')
			break;
		
		int start_index = get_vertex_index(g,start);//起点的下标
		int stop_index = get_vertex_index(g,stop);//起点的下标
		
		g->A[start_index][stop_index] = w;
		
		g->arcnum++;
	}
	
	
	return g;
}
/*
	First_index:在图g中获取下标为v的顶点的第一个邻接点的下标
*/
int First_index(struct Graph*g,int v)
{
	int i;
	for(i = 0;i < g->vexnum;i++)
	{
		if(g->A[v][i] != BIG)
		{
			return i;
		}
	}
	
	return -1;
}
/*
	Next_index:在图g中获取下标为v的顶点的以l+1列后邻接点的下标
*/
int Next_index(struct Graph*g,int v,int l)
{
	int i;
	for(i = l+1;i < g->vexnum;i++)
	{
		if(g->A[v][i] != BIG)
		{
			return i;
		}
	}
	
	return -1;
}
/*
	DFS:对图g中下标为v的顶点进行深度优先搜索
*/
void DFS(struct Graph*g,int v)
{
	visit[v] = 1;
	printf("%c ",g->V[v]);
	
	int l;
	for(l = First_index(g,v);l >= 0;l = Next_index(g,v,l))
	{
		if(!visit[l])
		{
			
			DFS(g,l);
		}
	}
	
}
/*
	DFS_search:对图g进行深度优先搜索
*/
void DFS_search(struct Graph*g)
{
	printf("DFS :========\n");
	int i;
	for(i = 0;i < g->vexnum;i++)
	{
		visit[i] = 0;
	}
	
	for(i = 0;i < g->vexnum;i++)
	{
		if(visit[i] == 0)
		{
			DFS(g,i);
		}
	}
	
	printf("\n");
}

/*
	BFS:广度优先搜索整个图
*/
void BFS(struct Graph*g)
{
	struct SqQueue*q = InitQueue(MAX);
	printf("BFS:=======\n");
	int i;
	for(i = 0;i < g->vexnum;i++)
	{
		visit[i] = 0;
	}
	
	for(i = 0;i < g->vexnum;i++)
	{
		if(!visit[i])
		{
			visit[i] = 1;
			printf("%c ",g->V[i]);
			
			EnQueue(q,i);
			while(!IsEmptyQueue(q))
			{
			
				int j;
				DeQueue(q,&j);
				int l;
				//让出队元素的未被访问邻接点入队
				for(l = First_index(g,j);l >= 0;l = Next_index(g,j,l))
				{
					if(!visit[l])
					{
						EnQueue(q,l);
						printf("%d\n ",l);
					}
				}
			}
		}
	}
	
	printf("\n");
	DestroyQueue(q);
}
/*
	print_graph() :打印图
*/
void print_graph(struct Graph*g)
{
	int i,j;
	for(i = 0;i < g->vexnum;i++)
	{
		printf("%c ",g->V[i]);
	}
	printf("\n");
	
	for(i = 0;i < g->vexnum;i++)
	{
		for(j = 0;j < g->vexnum;j++)
		{
			if(g->A[i][j] == BIG)
			{
				printf("~ ");
			}
			else
			{
				printf("%d ",g->A[i][j]);
			}
		}
		printf("\n");
	}
}
/*
（1）向量s[n]
		s[i] = 1;//源点v到i的最短路径求出来
		s[i] = 0;//源点v到i的最短路径没有求出来
		
		初始时s[v] = 1,其他的s[i] = 0
		
(2)向量dist[n]
	dist[i] =>存放的源点v到i的最短路径的距离（长度）
	
	dist[i] = <v,i>上的权w，若P(v,i)存在，v到i之间有一条直接路径
	dist[i] = 无穷大 若 P(v,i)不存在，v到i之间有没有直接路径
	
(3)向量path[n]

	path[i] 存放的是源点v到i的最短路径（v.....i）

	初始时path[i] = {v}; (表示从v出发)
*/	
/*
	Dijkstra:迪杰斯特拉算法
*/
int s[MAX];
int dist[MAX];
char path[MAX][MAX] = {'\0'};

void Dijkstra(struct Graph*g,int v)
{
	//step1:初始化这三个辅助向量
	int i,n;
	for(i = 0;i < g->vexnum;i++)
	{
		s[i] = (i == v) ? 1 : 0;
		dist[i] = g->A[v][i];
		path[i][0] = g->V[v];
	}
	
	for(n = 0;n < g->vexnum-1;n++)
	{
		//step1:在所有s[w] = 0中从源点到其他各个顶点的第一条最短路径  dist[u]
		int min = BIG;
		int j;//下标（所找到的最小的下标）
		for(i = 0;i < g->vexnum;i++)
		{
			if(s[i] == 0)
			{
				if(min > dist[i])
				{
					min = dist[i];
					j = i;
				}
			}
		}
		s[j] = 1;
		path[j][strlen(path[j])] = g->V[j];
		
		//step2:更新其他的dist
		for(i = 0;i < g->vexnum;i++)
		{
			if(s[i] == 0)
			{
				if(dist[j] + g->A[j][i] < dist[i])
				{
					dist[i] = dist[j] + g->A[j][i];
					strcpy(path[i],path[j]);
				}
			}
		}
	}
	
	for(i = 0;i < g->vexnum;i++)
	{
		printf("%d :",dist[i]);
		printf("%s\n",path[i]);
	}		
}
int main()
{
	struct Graph*g = create_graph();
	
	//print_graph(g);
	
	//DFS_search(g);
	//BFS(g);
	Dijkstra(g,0);
}


