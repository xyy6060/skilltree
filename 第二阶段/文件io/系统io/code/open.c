#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
int main()
{
	//step1:open file
	 int fd = open("1.txt",O_RDWR | O_CREAT,0666);
	if(fd == -1)//出错处理
	{
		perror("open error:");
		return -1;
	}
	perror("open ");
	//step2:
	
	
	//step3:close file
	close(fd);
}