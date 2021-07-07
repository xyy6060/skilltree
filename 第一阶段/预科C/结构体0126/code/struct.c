#include <stdio.h>
struct student
{
	int num;
	char name[32];
	char sex;
	int age;
	char addr[32];
};
struct student1
{
	//char a;
	//int num;
	//char name[32];
	//char sex;
	//int age;
	//char addr[32];
	//double a;
	//int age;
	char c;
	char a;
	int b;
	
};

int main()
{
	printf("%d\n",sizeof(struct student1));
}