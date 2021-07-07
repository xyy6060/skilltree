 #include <unistd.h>
 #include <stdio.h>
 int main()
 {
	char buf[128];
	getwd(buf);
	
	printf("dir:%s\n",buf);
	
	chdir("/home/gec/");
	perror("error");
	getwd(buf);
	printf("dir:%s\n",buf);
	//while(1);
 }