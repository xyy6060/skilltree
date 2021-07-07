#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>


int main(int argc,char**argv)
{
	 int fd1 = open(argv[1],O_RDWR);
	 int fd2 = open(argv[2],O_RDWR | O_CREAT | O_TRUNC,0777);
	 if(fd1 == -1 || fd2 == -1)
	 {
		perror("open error:");
		return -1;
	 }
	 
	 char s[10];
	 while(1)
	 {
		ssize_t r = read(fd1,s,10);
		if(r == 0)
		{
			break;
		}
		else if(r == -1)
		{
			perror("read error");
			break;
		}
		write(fd2,s,r);
	 }
	 
	 
	 close(fd1);
	 close(fd2);
}
