#include <stdio.h>
#define N 5
int main()
{
	int a[N];
	int l,r,mid,x,i,j;
	
	for(i = 0;i < N;i++)
	{
		scanf("%d",&a[i]);
		
		//排序
		//找插入位置
			//二分法/二分查找法
			l = 0;
			r = i;
			while(l <= r)
			{
				mid = (l+r)/2;// 1
				if(a[mid] == a[i])
				{
					break;
				}
				else if(a[mid] < a[i])
				{
					l = mid+1;
				}
				else
				{
					r = mid-1;
				}
			}
			if(a[i] > a[mid])
			{
				mid++;
			}
			
			//插入操作
			//把插入位置及后面的元素，每一个都往后移
			x = a[i];
			//mid,...i-1
			for(j = i-1;j >= mid;j--)
			{
				a[j+1] = a[j];
			}
			a[mid] = x;
	}
	
	for(i = 0;i < N;i++)
	{
		printf("%d ",a[i]);
	}
	printf("\n");
}

1 5 
 
 












