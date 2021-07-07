#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
int main(int argc ,char**argv)
{
	//打开文件
	int fd = open(argv[1],O_RDONLY);
	if(fd == -1)
	{
		perror("open error:");
		return -1;
	}
	
	
	//判断是否是bmp文件
	char a[2];
	read(fd,a,2);
	if(a[0] != 0x42 || a[1] != 0x4d)
	{
		printf("It is not bmp\n");
		close(fd);
		return -1;
	}
	
	printf("It is bmp\n");
	
	//分辨率
	char w[4];
	char h[4];
	lseek(fd,0x12,SEEK_SET);
	read(fd,w,4);
	int w1 = (w[0]&0xff) | (w[1]&0xff)<<8|(w[2]&0xff)<<16|(w[3]&0xff)<<24;
	read(fd,h,4);
	printf("w1 = %d\n",w1);
	//printf("%d*%d\n",w,h);
	
	//size
	int size;
	lseek(fd,2,SEEK_SET);
	read(fd,&size,4);
	printf("size = %d\n",size);
	
	//depth
	short depth;
	lseek(fd,0x1c,SEEK_SET);
	read(fd,&depth,2);
	printf("depth = %d\n",depth/8);
	
	//addr
	int addr;
	lseek(fd,0x0a,SEEK_SET);
	read(fd,&addr,4);
	printf("addr = %d\n",addr);
	
	
	close(fd);
}