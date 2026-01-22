//蓝牙测试程序
//适配器电源开关
//适配器扫描，蓝牙扫描

#include <iostream>
#include "Bluetooth/TpBluetoothHostInfo.h"
#include "Bluetooth/TpBluetoothLocal.h"
#include "Bluetooth/TpBluetoothDiscovery.h"
#include "Bluetooth/TpBluetoothDevice.h"
//#include "blt_service.h"


void printf_paired_status(TpBluetoothLocal::TpLocalPair status)
{
	switch(status)
	{
		case TpBluetoothLocal::TP_LOCAL_PAIRED:
			std::cout << "未信任已配对" << std::endl;
			break;
		case TpBluetoothLocal::TP_LOCAL_UNPAIRED:
			std::cout << "未配对" << std::endl;
			break;
		case TpBluetoothLocal::TP_LOCAL_AUTHORIZED_PAIRED:
			std::cout << "已配对且已信任" << std::endl;
			break;
	}
}

//本地蓝牙设备获取
void example_list_adapter()
{
	TpList<tpShared<TpBluetoothHostInfo>> adapter_list=TpBluetoothLocal::allDevice();
	for(auto &it:adapter_list)
	{
		std::cout << "name=" << it->name() << std::endl;
		std::cout << "addr=" << it->address().toString() << std::endl ;
		std::cout << std::endl;
	}
}

//扫描蓝牙
void example_list_device()
{
	TpBluetoothDiscovery discovery("hci0");
	discovery.start();
	
	connect(&discovery, bluetoothDeviceRemove, [=](TpBluetoothAddress address)
            { std::cout << "[Signal]设备消失：" << address.toString() << std::endl; },Tp::DirectConnection);

	connect(&discovery, bluetoothDeviceAdd, [=](const TpBluetoothDevice &device)
            { std::cout << "[Signal]设备新增：" << device.getAddress().toString() << std::endl; },Tp::DirectConnection);

	while(1);
	discovery.stop();
}

void example_pair()
{
	TpBluetoothLocal local("hci0");
	TpBluetoothAddress remote(TpString("6C:D1:99:69:BF:F0"));
	
	TpBluetoothLocal::TpLocalPair status = local.getPairStatus(remote);
	printf_paired_status(status);
/*	printf("开始配对\n");
	local.requestPairing(remote,TpBluetoothLocal::TP_LOCAL_AUTHORIZED_PAIRED);
	printf_paired_status(status);
	sleep(20);
	printf("取消配对\n");
	local.requestPairing(remote,TpBluetoothLocal::TP_LOCAL_UNPAIRED);
	printf_paired_status(status);*/
}

//适配器电源开关
void example_power()
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
//	example_list_device();
//	example_power();
	example_pair();
	return 0;
}