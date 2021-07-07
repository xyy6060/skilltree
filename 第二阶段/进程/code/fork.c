#include <stdio.h>
#include <unistd.h>
int main()
{
	printf("haha");
	printf("befor : pid = %d\n",getpid()); 
	pid_t pid = fork();
	if(pid > 0 )
	{
		printf("after : father pid = %d\n",getpid()); 	
		printf("after : father ppid = %d\n",getppid()); 	
		printf("I am father\n");
	}
	else if(pid == 0)
	{
		
		printf("I am son\n");
		execl();
		printf("after : son pid = %d\n",getpid()); 	
		printf("after : son ppid = %d\n",getppid()); 
		
	}
	else
	{
		perror("fork error:");
		return -1;
	}
	
	printf("We are family\n");
}