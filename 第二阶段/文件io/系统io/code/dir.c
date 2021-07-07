#include<stdio.h>
#include <dirent.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
int n = 0;
int size = 0;
//求一个目录的子目录数
void dirnum(char*pathname)
{
	 //打开目录
	 DIR * dir = opendir(pathname);  
	 if(dir == NULL)
	 {
		perror("opendir error:");
		return ;
	 }
	 
	 //读取目录，求子目录数
	 struct dirent*dirent = NULL;
	 while(dirent = readdir(dir))
	 {
		 char filename[258] = {0};
		 sprintf(filename,"%s/%s",pathname,dirent->d_name);
		if(dirent->d_type == DT_DIR)	
		{
			if(strcmp(dirent->d_name,".") == 0 || strcmp(dirent->d_name,"..") == 0)
			{
				continue;
			}
			n++;
			printf("%s\n",filename);
			dirnum(filename);
		}
		else if(dirent->d_type == DT_REG)
		{
			struct stat st;
			stat(filename,&st);
			size += st.st_size;
			printf("%s\n",filename);
		}
	
	 }
	 
	 closedir(dir);
}

int main(int argc,char**argv)
{
	
	dirnum(argv[1]);
	printf("n = %d\n",n);
	printf("size = %d\n",size);
}