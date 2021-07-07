#include <stdio.h>
#include <dirent.h>
#include <sys/types.h>
#include <string.h>
#include <unistd.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <pthread.h>
#include <stdlib.h>
pthread_t tid[100];
int i = 0;
struct filepath
{
	char file1[256];
	char file2[256];
};
/*
	cp_file:拷贝普通文件
*/
void* cp_file(void*arg)
{
	puts(((struct filepath*)arg)->file1);
	puts(((struct filepath*)arg)->file2);
	int fd1 = open(((struct filepath*)arg)->file1,O_RDONLY);
	if(fd1 == -1)
	{
		perror("open fd1 error:");
		return NULL;
	}
	int fd2 = open(((struct filepath*)arg)->file2,O_WRONLY|O_CREAT,0664);
	if(fd2 == -1)
	{
		perror("open fd2 error:");
		return NULL;
	}
	
	char str[128];
	while(1)
	{
		int r = read(fd1,str,128);
		if(r <= 0)
			break;
		write(fd2,str,r);
	}
	
	close(fd1);
	close(fd2);
	free(arg);
}

/*
	cp_dir：拷贝目录
*/
int cp_dir(char*src,char*dest)
{
	DIR*dir = opendir(src);
	if(dir == NULL)
	{
		perror("opendir error:");
		return -1;
	}
	 struct dirent *dirent;

	while(dirent = readdir(dir))
	{	
		if(strcmp(dirent->d_name,".") == 0 || strcmp(dirent->d_name,"..") == 0)
		{
			continue;
		}
		
		char pathname1[256];
		char pathname2[256];
		sprintf(pathname1,"%s/%s",src,dirent->d_name);
		sprintf(pathname2,"%s/%s",dest,dirent->d_name);
		//puts(pathname1);
	//	puts(pathname2);
		//普通文件
		if(dirent->d_type == DT_REG)
		{
			//文件拷贝 
			//创建一条线程，让线程来帮你拷贝文件
			//cp_file(pathname1,pathname2);
			struct filepath *fpath = malloc(sizeof(struct filepath));
			strcpy(fpath->file1,pathname1);
			strcpy(fpath->file2,pathname2);
			
			pthread_create(&tid[i++],NULL,cp_file,(void*)fpath);
		}
		else if(dirent->d_type == DT_DIR)	
		{
			//子目录
			mkdir(pathname2,0666);
			cp_dir(pathname1,pathname2);
			//cp_dir(pathname1,dirname);
		}
			
		
	}
	
	closedir(dir);
}

//
int main(int argc,char**argv)
{
	mkdir(argv[2],0666); 
	
	int l = strlen(argv[1]);
	char*p = argv[1]+l-1;
	while(*p != '/')
	{
		p--;
	}
	char dirname[128];
	sprintf(dirname,"%s%s",argv[2],p);
	mkdir(dirname,0666);
 	
	cp_dir(argv[1],dirname);
	
	
	int n;
	for(n = 0;n < i;n++)
	{
		pthread_join(tid[n],NULL);//等待子线程退出
	}
}

// ./a.out  ../dir  ./code