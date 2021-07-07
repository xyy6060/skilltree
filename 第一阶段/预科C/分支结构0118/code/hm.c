#include <stdio.h>

int main()
{
		double p;
		long I;
		scanf("%ld",&I);
		switch(I/100000)
		{
			default:
			case 10:
				p = (I-1000000)*0.01;
				I = 1000000;
			case 9:
			case 8:
			case 7:
			case 6:
				p += (I-600000)*0.015;
				I = 600000;
			case 5:
			case 4:
				p += (I-400000)*0.03;
				I = 400000;
			case 3:
			case 2:
				p += (I-200000)*0.05;
				I = 200000;
			case 1:
				p += (I-100000)*0.075;
				I = 100000;
			case 0:
				p += I*0.1;
		}
		
		printf("p = %	\n",p);
	
}