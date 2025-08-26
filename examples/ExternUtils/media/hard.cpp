#include <iostream>
#include "TpSound.h"

//硬件全局音量设置
//硬件音量和实际设置值可讷讷个会有略微偏差
int main()
{
	
	TpList<TpString> cards=TpSound::getDevices();
	for(auto &it : cards)
	{
		std::cout << "device:" << it << std::endl;
	}

	TpString card("hw:0,0 device");
	TpSound audio(card);

	std::cout << "当前系统音量" <<audio.getSystemVolume()<<std::endl;
//	std::cout << "设置系统音量为100\n";
	audio.setSystemVolume(50);
	audio.getSystemVolume();
	std::cout << "当前系统音量" <<audio.getSystemVolume()<<std::endl;

	
	TpSound::setUsedDevice(card);
	std::cout << "当前使用的声卡" <<TpSound::getUsedDevice()<<std::endl;



	return 0;
}