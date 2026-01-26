#include <iostream>
#include "Network/TpNetworkInterface.h"
#include "Network/TpWirelessInfo.h"
#include "TpString.h"


int example_wireless()
{
	TpNetworkInterface device("wlx502b73e07098");
	if(device.isWireless())
		std::cout << "this is wireless" << std::endl;
	else	
		return 0;
	if(device.isOnline())
	{
		std::cout << "已连接网络" << std::endl;
	}

	if(device.startScan()<0)
		std::cout << "不能扫描\n";

	connect(&device, wirelessRemove, [=](TpString name)
            { std::cout << "[Signal]网络消失：" << name<< std::endl; }, Tp::DirectConnection);

	connect(&device, wirelessAdd, [=](TpWirelessInfo wifi)
            { std::cout << "[Signal]新添网络：" << wifi.getSsid() << "  \tLevel: "<< wifi.getLevel()<<std::endl; }, Tp::DirectConnection);

	while(1);

//	std::cout << "网络SSID:" << device.getWirelessSsid()<< std::endl;
	std::cout << "准备连接\n";
	device.connectWireless("哲思亿佳","zerseager@");
	std::cout << "连接完成\n";
	while(1);
}

int example_hotspot()
{
	TpNetworkInterface device("wlx502b73e07098");
	TpString ssid("TinyPiX WIFI");
	TpString pwd("TinyPiX");
	printf("设置ssid\n");
	device.setHotspotSsid(ssid);
	printf("设置pwd\n");
	device.setHotspotPwd(pwd);
	printf("热点开启\n");
	device.openHotspot();
	return 0;
}

int main()
{
	example_wireless();
	return 0;
}
