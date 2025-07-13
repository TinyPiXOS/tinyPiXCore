#include <iostream>
#include "tpSound.h"

//硬件全局音量设置
//硬件音量和实际设置值可讷讷个会有略微偏差
int main()
{
	
	tpList<tpString> cards=tpSound::getDevices();
	for(auto &it : cards)
	{
		std::cout << "device:" << it << std::endl;
	}

	tpString card("hw:0,0 device");
	tpSound audio(card);

	std::cout << "当前系统音量" <<audio.getSystemVolume()<<std::endl;
//	std::cout << "设置系统音量为100\n";
	audio.setSystemVolume(50);
	audio.getSystemVolume();
	std::cout << "当前系统音量" <<audio.getSystemVolume()<<std::endl;

	
	tpSound::setUsedDevice(card);
	std::cout << "当前使用的声卡" <<tpSound::getUsedDevice()<<std::endl;



	return 0;
}