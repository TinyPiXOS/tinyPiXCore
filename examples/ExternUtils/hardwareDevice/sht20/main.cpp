#include "sht20.h"

int main()
{
	TpSht20 sht20(1);

	sht20.open();
	
	float t=sht20.getTemperature(NULL);
	float h=sht20.getHumidity(NULL);

	printf("温度：%fC  湿度：%f%\n",t,h);
}
