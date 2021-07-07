#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <string.h>
#include <fcntl.h>
#include <unistd.h>
#include <errno.h>


int main()
{
	int r = mkfifo("/home/gec/fifo",0777);
	if(r == -1)
	{
		if(errno != EEXIST)//如果出错不是因为已经存在
		{
			perror("mkfifo error:");
			return -1;
		}
		
	}
	
	int fd = open("/home/gec/fifo",O_RDONLY);
	if(fd == -1)
	{
		perror("open error:");
		return -1;
	}
	
	printf("open right\n");
	
	char s[32] = {0};
	
	read(fd,s,32);
	printf("read :%s\n",s);
	
	
	close(fd);
}