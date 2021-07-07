#include <stdio.h>
#include <unistd.h>


int main()
{
	printf("start:_________\n");
	
	system("./sum 123 567");
	int r = execlp("add","add","123","345",NULL);
	
	perror("execlp error :");
	printf("r = %d\n",r);
	
	printf("I am back  \n");
}