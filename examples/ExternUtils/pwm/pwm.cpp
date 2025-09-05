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
	pwm.setPeriod(10000);
	pwm.setDutyCycle(50);
	while(1);
	pwm.close();
}
