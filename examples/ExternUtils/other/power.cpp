//电源测试
//关机/重启/挂起测试
#include <iostream>
#include "tpSystemBatteryInfo.h"
#include "tpLogin.h"


void example_power()
{
	printf("电源测试\n");
	tpSystemBatteryInfo bat;
	printf("电量：%d\n",bat.getBatteryLevel());
	printf("状态：%d\n",bat.getState());
	printf("电压：%lf\n",bat.getVoltage());
}


void example_login()
{
	printf("关机/重启/挂起测试\n");
	tpLogin login;

	login.reboot();
}

int main()
{	
	example_power();
	example_login();
}