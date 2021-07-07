#include <stdio.h>
#include <unistd.h>
#include <sys/wait.h>
#include <stdlib.h>
int main()
{
	printf("haha");
	pid_t pid = fork();
	if(pid > 0 )
	{
		printf("I am father\n");
		
		int status;
		wait(&status);
		if(WIFEXITED(status))
		{
			printf("status = %d\n",WEXITSTATUS(status));
		}
		if(WIFSIGNALED(status))
		{
			printf("signal = %d\n",WTERMSIG(status));
		}
		
	}
	else if(pid == 0)
	{
		printf("I am son\n");
		//while(1);
		
		//exit(-1);
		
	}
	else
	{
		perror("fork error:");
		return -1;
	}
	
	printf("We are family\n");
}
