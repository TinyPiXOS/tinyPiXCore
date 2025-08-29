#include "TpGpio.h"



int main ()
{
	TpGpio gpio(16);
	if(!gpio.open())
	{
		printf("gpio打开失败\n");
		return -1;
	}
	while(1)
	{
		gpio.setHeight();
		sleep(1);
		gpio.setLow();
		sleep(1);
	}
	gpio.close();
}