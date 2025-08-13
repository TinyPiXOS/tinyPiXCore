

#include <iostream>
#include "tpBluetoothLocal.h"
#include "tpBluetoothDiscovery.h"
#include "tpBluetoothDevice.h"
#include "tpBluetoothSocket.h"
#include "tpBluetoothServer.h"
#include "tpBluetoothService.h"

//扫描蓝牙
int example_list_device(const tpString& adapter)
{
	tpBluetoothDiscovery discovery(adapter);

	discovery.start();
	
	connect(&discovery, bluetoothDeviceRemove, [=](tpBluetoothAddress address)
            { std::cout << "[Signal]设备消失：" << address.toString() << std::endl; });

	connect(&discovery, bluetoothDeviceAdd, [=](const tpBluetoothDevice &device)
            { std::cout << "[Signal]设备新增：" << device.getAddress().toString() << std::endl; });

	while(1);
	discovery.stop();
}

int example_socket_client(const tpString& adapter)
{
	uint8_t buff[10]="senddata\n";

	tpString adapter_=adapter;
	tpBluetoothLocal local(adapter_.c_str());
	local.powerOn();
	
	tpBluetoothSocket bt_client(adapter,tpBluetoothService::TP_BLUET_RFCOMM_PROTOCOL);

	//扫描
	example_list_device(adapter);

	bt_client.connectToService(tpBluetoothAddress("6C:D1:99:69:BF:F0"), 0);

	

	/*connect(&bt_client, connected, [=]( )
        { 
			std::cout << "connect to remote ok" << std::endl; 
			connect(&bt_client, disconnected, [=](tpBluetoothSocket *bt)
            {	

			});
			bt_client.send(buff,sizeof(buff));
			sleep(1);
			
		});*/
	
	while(1);
	return 0 ;
}

int example_socket_server(const tpString& adapter)
{
	tpBluetoothServer bt_server(adapter,tpBluetoothService::TP_BLUET_RFCOMM_PROTOCOL);


	return 0;
}


int main()
{
	/*tpList<tpBluetoothLocal> adapter_list=tpBluetoothLocal::getAllDevice();
	for(auto &it:adapter_list)
	{
		std::cout << "name=" << it.getName() << std::endl;
		std::cout << "addr=" << it.getAddress().toString() << std::endl ;
		std::cout << std::endl;
	}*/
	printf("蓝牙客户端/服务端收发数据测试\n");
	example_socket_client(tpString("hci0"));
}


