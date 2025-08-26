//蓝牙测试程序
//适配器电源开关
//适配器扫描，蓝牙扫描

#include <iostream>
#include "TpBluetoothLocal.h"
#include "TpBluetoothDiscovery.h"
#include "TpBluetoothDevice.h"
#include "blt_service.h"

//本地蓝牙设备获取
int example_list_adapter()
{
	TpList<TpBluetoothLocal> adapter_list=TpBluetoothLocal::getAllDevice();
	for(auto &it:adapter_list)
	{
		std::cout << "name=" << it.getName() << std::endl;
		std::cout << "addr=" << it.getAddress().toString() << std::endl ;
		std::cout << std::endl;
	}
}

//扫描蓝牙
int example_list_device()
{
	TpBluetoothDiscovery discovery("hci0");
	discovery.start();
	
	connect(&discovery, bluetoothDeviceRemove, [=](TpBluetoothAddress address)
            { std::cout << "[Signal]设备消失：" << address.toString() << std::endl; });

	connect(&discovery, bluetoothDeviceAdd, [=](const TpBluetoothDevice &device)
            { std::cout << "[Signal]设备新增：" << device.getAddress().toString() << std::endl; });

	while(1);
	discovery.stop();
}

int example_pair()
{
	TpBluetoothLocal local("hci0");
	TpBluetoothAddress remote(TpString("6C:D1:99:69:BF:F0"));
	local.requestPairing(remote,TpBluetoothLocal::TP_LOCAL_UNPAIRED);
	sleep(10);
}

//适配器电源开关
int example_power()
{
	TpBluetoothLocal local("hci0");
	if(local.isPowerOn())
		std::cout << "power is on" << std::endl;
	local.powerOff();
	sleep(5);
	local.powerOn();
}




int main()
{
	example_list_adapter();
	example_list_device();
//	example_power();
//	example_pair();
}