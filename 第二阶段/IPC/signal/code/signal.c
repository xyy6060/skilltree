#include <stdio.h>
#include <signal.h>

//typedef void (*sighandler_t)(int);
void Sig_Handler(int sig)
{
	printf("SIGALRM\n");
}

void Sig_Handler1(int sig)
{
	printf("SIGINT\n");
}
int main()
{
	alarm(10);
	
	signal(SIGALRM,Sig_Handler);
	signal(SIGINT,Sig_Handler);
	
	
	sleep(20);
	/*while(1)
	{
		sleep(5);
		printf("123\n");
	}*/
	
	//pause();//暂停进程，等待信号
}