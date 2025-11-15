#include "TpHardwarePwm.h"

int main()
{
	TpList<tpUInt8>list=TpHardwarePwm::getPwmNumbers();
	for(auto &it : list)
	{
		;
	}
	TpHardwarePwm pwm(0);	//默认打开通道0
	if(pwm.open()<0)
		return -1;
	pwm.setPeriod(1000000);
	float duty=10;
	while(1)
	{
		pwm.setDutyCycle(duty);
		usleep(100000);
		duty+=1;
		if(duty==90)
			duty=10;
	}
	pwm.close();
}
