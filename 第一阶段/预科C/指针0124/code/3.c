#include <stdio.h>

int main()
{
	int *p;//定义了一个指针变量，p会对应一个存储单元
			//p这个变量有一个值，值 ：未知
			
			//*p =>等价于地址所对应的那个变量，但是这个地址是未知
				//能不能访问也是未知的 =》很有可能造成内存的非法访问 =》段错误
	*p = 1024;
	
	printf("%d\n",*p);
	
	char *q;
	
	printf("%d,%d\n",sizeof(p),sizeof(q));
}


		