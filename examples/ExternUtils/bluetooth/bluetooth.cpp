//蓝牙测试程序
//适配器电源开关
//适配器扫描，蓝牙扫描

#include <iostream>
#include "tpBluetoothLocal.h"
#include "tpBluetoothDiscovery.h"
#include "tpBluetoothDevice.h"

//本地蓝牙设备获取
int example_list_adapter()
{
	tpList<tpBluetoothLocal> adapter_list=tpBluetoothLocal::getAllDevice();
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
	tpBluetoothDiscovery discovery("hci1");
	discovery.start();
	
	connect(&discovery, bluetoothDeviceRemove, [=](tpBluetoothAddress address)
            { std::cout << "[Signal]设备消失：" << address.toString() << std::endl; });

	connect(&discovery, bluetoothDeviceAdd, [=](const tpBluetoothDevice &device)
            { std::cout << "[Signal]设备新增：" << device.getAddress().toString() << std::endl; });

	while(1);
	discovery.stop();
}

int example_pair()
{
	tpBluetoothLocal local("hci0");
	tpBluetoothAddress remote(tpString("6C:D1:99:69:BF:F0"));
	local.requestPairing(remote,tpBluetoothLocal::TP_LOCAL_UNPAIRED);
	sleep(10);
}

//适配器电源开关
int example_power()
{
	tpBluetoothLocal local("hci0");
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