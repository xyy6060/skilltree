#include <stdio.h>
#include <unistd.h>

int main()
{
	 int pipefd[2];
	 int r = pipe(pipefd);
	 if(r == -1)
	 {
		perror("pipe error:");
		return -1;
	 }
	 
	pid_t pid = fork();
	if(pid == 0)
	{
		char buf[32] = {0};
		r = read(pipefd[0],buf,sizeof(buf));
		if(r > 0)
		{
			printf("%s\n",buf);
			write(pipefd[1],"hello ,father",14);
		}
		
	}
	else if(pid > 0)
	{
		write(pipefd[1],"hello ,son",11);
		
	}
	else
	{
		perror("fork error:");
		return -1;
	}
	
	
	
}