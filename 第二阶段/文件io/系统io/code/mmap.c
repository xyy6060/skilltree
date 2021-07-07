#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <sys/mman.h>
#include <string.h>
int main(int argc,char**argv)
{
	//打开文件
	int fd = open(argv[1],O_RDWR);
	if(fd == -1)
	{
		perror("open error:");
		return -1;
	}
	
	//映射
	 char*s = mmap(NULL, 4096,PROT_READ |PROT_WRITE , MAP_SHARED,fd,0);
	 if(s == MAP_FAILED)
	 {
		perror("mmap error:");
		close(fd);
		return -1;
	 }
	 
	 //
	 strcpy(s,"hello world");
	 
	 //解映射
	 munmap(s,4096);
	 
	 //关闭文件
	 close(fd);
}