#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <time.h>

		
int main(int argc,char**argv)
{
	//stat是用来获取path指定的文件的状态信息，获取的信息保存到buf所指向的内存空间中
	 struct stat buf;
     int r = stat(argv[1],&buf);
	 if(r == -1)
	 {
		perror("stat error");
		return -1;
	 }
	 
	 char str[512] = {0};
	 int re = 0;
	 //判断文件类型
	 if(S_ISREG(buf.st_mode))
	 {
		re = sprintf(str,"-");
	 }
	 else if(S_ISDIR(buf.st_mode))
	 {
		re = sprintf(str,"d");
	 }
	  else if(S_ISCHR(buf.st_mode))
	 {
		re = sprintf(str,"c");
	 }
	  else if(S_ISBLK(buf.st_mode))
	 {
		re = sprintf(str,"b");
	 }
	  else if(S_ISFIFO(buf.st_mode))
	 {
		re = sprintf(str,"p");
	 }
	  else if(S_ISLNK(buf.st_mode))
	 {
		re = sprintf(str,"l");
	 }
	 else
	 {
		re = sprintf(str,"s");
	 }
	 
	 //判断文件权限
	 //用户
	 if(buf.st_mode & S_IRUSR)
	 {
		re += sprintf(str+re,"r");
	 }
	 else
	 {
		re += sprintf(str+re,"-");
	 }
	  if(buf.st_mode & S_IWUSR)
	 {
		re += sprintf(str+re,"w");
	 }
	 else
	 {
		re += sprintf(str+re,"-");
	 }
	 if(buf.st_mode & S_IXUSR)
	 {
		re += sprintf(str+re,"x");
	 }
	 else
	 {
		re += sprintf(str+re,"-");
	 }
	 //组用户
	  if(buf.st_mode & S_IRGRP)
	 {
		re += sprintf(str+re,"r");
	 }
	 else
	 {
		re += sprintf(str+re,"-");
	 }
	  if(buf.st_mode & S_IWGRP)
	 {
		re += sprintf(str+re,"w");
	 }
	 else
	 {
		re += sprintf(str+re,"-");
	 }
	 if(buf.st_mode & S_IXGRP)
	 {
		re += sprintf(str+re,"x");
	 }
	 else
	 {
		re += sprintf(str+re,"-");
	 }
	 //其他用户
	  if(buf.st_mode & S_IROTH)
	 {
		re += sprintf(str+re,"r");
	 }
	 else
	 {
		re += sprintf(str+re,"-");
	 }
	  if(buf.st_mode & S_IWOTH)
	 {
		re += sprintf(str+re,"w");
	 }
	 else
	 {
		re += sprintf(str+re,"-");
	 }
	 if(buf.st_mode & S_IXOTH)
	 {
		re += sprintf(str+re,"x");
	 }
	 else
	 {
		re += sprintf(str+re,"-");
	 }
	 
	 //硬链接数
	 re += sprintf(str+re," %d",buf.st_nlink);
	 
	 //用户ID和组ID
	 re += sprintf(str+re," %d %d",buf.st_uid,buf.st_gid);
	 
	 //文件大小
	 re += sprintf(str+re," %ld",buf.st_size);
	 
	 //最后修改时间
	 re += sprintf(str+re," %s",ctime(&buf.st_mtime));
	 str[re-1] = ' ';
	 
	 //文件名
	 re += sprintf(str+re,"%s",argv[1]);
	 
	 printf("%s\n",str);
	 
}