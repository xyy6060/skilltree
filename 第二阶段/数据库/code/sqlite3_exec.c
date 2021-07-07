#include <stdio.h>
#include <sqlite3.h>
#include <string.h>
int flag = 0;
/*
	select_table:解析每一行
	@arg :
	@n :多少列
	@col_value:每一列的值
	@col_name:每一列的名字
*/
int select_table(void*arg,int n,char*col_value[],char*col_name[])
{
	int i;
	if(flag == 0)
	{
		for(i = 0;i < n;i++)
		{
			printf("%s\t",col_name[i]);
		}
		printf("\n");
		flag = 1;
	}
	
	
	for(i = 0;i < n;i++)
	{
		printf("%s\t",col_value[i]);
	}
	printf("\n");
	
	
	return 0;
}

//-sqlite3
int main(int argc,char**argv)
{
	if(argc != 2)
	{
		printf("argc number is error\n");
		return -1;
	}
	
	//创建一个数据库连接对象
	sqlite3 *ppDb = NULL;
	int  re = sqlite3_open(argv[1],&ppDb);
	if(re != SQLITE_OK)
	{
		printf("sqlite3_open error:%s\n",sqlite3_errmsg(ppDb));
		return -1;
	}
	//const char *zSql = "create table stuinfo(\
					num int primary key,\
					name text not null,\
					score int\
					);insert into stuinfo(num,name,score) values(1001,'pxl',98);";
				
	//nst char *zSql ="insert into stuinfo(num,name,score) values(1010,'zhi',56);";
	const char *zSql ="select * from stuinfo;";
	
	
	//执行SQL语句
	char *errmsg = NULL;
	re =  sqlite3_exec(
						  ppDb,                                  /* 指向数据库连接对象 */
						  zSql,                           /* 指向要指向的SQL语句(一般不带参数) */
						  select_table,  /* 回调函数 */
						  NULL,                                    /* 这个参数，将作为回调函数的第一个参数传入 */
						  &errmsg                              /* 用来保存sqlite3_exec出错信息 */
						);
	
	if(re != 0)
	{
		printf("sqlite3_exec error:%s\n",errmsg);
		sqlite3_close(ppDb);
		return -1;
	}
	//关闭一个数据库连接对象
	sqlite3_close(ppDb);
	
	
	
}