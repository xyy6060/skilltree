#include <stdio.h>
#include <sqlite3.h>
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
				
	//const char *zSql ="insert into stuinfo(num,name,score) values(1001,'pxl',98);";
	
	sqlite3_stmt *ppStmt = NULL;
	const char *pzTail = NULL;	
	
	while(1)
	{
		int num;
		char name[256] = {0};
		int score;
		scanf("%d%s%d",&num,name,&score);
		if(num == 0)
		{
			break;
		}
		
		char sql[256] = {0};
		sprintf(sql,"insert into stuinfo(num,name,score) values(%d,'%s',%d);",num,name,score);
		//编译SQL语句，创建一个SQL语句对象
		re = sqlite3_prepare_v2(
							ppDb,            /*数据库连接对象，表示要操作哪个数据库*/
							sql,       /* 要编译的SQL语句 ，多条SQL语句用 ;隔开*/
							-1,              /*指示要编译的zSql语句到哪个位置 */
												//<0  ：编译到zSql指向的SQL语句到第一个\0为止
												//>0   :编译zSql指向的SQL语句前面n字节
												//=0 :什么都不编译
												
							&ppStmt,  //* OUT: Statement handle 用来保存编译好的SQL语句对象
							
							&pzTail     // OUT: Pointer to unused portion of zSql 
														//第一条完整SQL语句后面的第一个字符
														
														//为NULL，不保存/
								);
								
		if(re != SQLITE_OK)
		{
			printf("sqlite3_prepare_v2 error:%s\n",sqlite3_errmsg(ppDb));
			return -1;
		}
		
		//printf("*pzTail = %c\n",*pzTail);
		printf("sql = %s\n",sql);
		//执行SQL语句
		re = sqlite3_step(ppStmt);
		if(re == SQLITE_DONE)
		{
			printf("step success\n");
		}
	
	}
	
	sqlite3_finalize(ppStmt);	
	
	sqlite3_close(ppDb);
	
}