#include <iostream>
#include "ap3216.h"

int main()
{
	TpAp3216Manager ap3216c(1);

	if(!ap3216c.open())
	{
		printf("打开失败\n");
		return -1;
	}

	TpAp3216 data=ap3216c.getSampleData();
	if(data.isNull())
	{
		printf("获取数据失败\n");
	}

	printf("光照强度：%f流明\n",data.getLux());
	printf("红外强度：%d\n",data.getIr());
	printf("接近距离：%d\n",data.getPs());
	printf("是否靠近：%d流明\n",data.isCloser()==TP_TRUE? 1:0);
}

