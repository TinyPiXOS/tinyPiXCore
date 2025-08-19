

#include <iostream>
#include "tpApp.h"
#include "tpFixScreen.h"
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

	
	discovery.stop();
	while(1);
	return 0 ;
}

int example_socket_client(int32_t argc, char *argv[],const tpString& adapter)
{
	tpApp app(argc, argv);
	tpFixScreen *vScreen = new tpFixScreen();
	vScreen->setBackGroundColor(_RGBA(128, 128, 128, 255));
	vScreen->setVisible(true); // vScreen setvisible will be update display weekly
	app.bindVScreen(vScreen);

	tpUInt8 buff[10]="senddata\n";

	tpString adapter_=adapter;
	tpBluetoothLocal local(adapter_.c_str());
	local.powerOn();

	tpBluetoothSocket bt_client(adapter,tpBluetoothService::TP_BLUET_RFCOMM_PROTOCOL);

	//扫描
//	example_list_device(adapter);

	bt_client.connectToService(tpBluetoothAddress("E4:5F:01:37:58:93"), 1);

	

	connect(&bt_client, connected, [&bt_client, &buff]( )
        { 
			std::cout << "connect to remote ok" << std::endl; 
			while(1)
			{
				bt_client.send((const tpUInt8 *)buff,sizeof(buff));
				sleep(1);
			}
				
		});
	connect(&bt_client, disconnected, [=](tpBluetoothSocket *bt)
        {	
			std::cout << "disconnect from remote" << std::endl; 
		});
			
	
	app.run();
	return 0 ;
}

int example_socket_server(int32_t argc, char *argv[],const tpString& adapter)
{
	tpApp app(argc, argv);
	tpFixScreen *vScreen = new tpFixScreen();
	vScreen->setBackGroundColor(_RGBA(128, 128, 128, 255));
	vScreen->setVisible(true); // vScreen setvisible will be update display weekly
	app.bindVScreen(vScreen);

	tpBluetoothServer bt_server(adapter,tpBluetoothService::TP_BLUET_RFCOMM_PROTOCOL);
	bt_server.listen(tpBluetoothAddress::any(),1);
	printf("服务端已启动\n");

	connect(&bt_server, newConnection,[&]()
	{
		tpBluetoothSocket *bt_c=bt_server.nextPendingConnection();
		if(bt_c)
		{
			std::cout << "accept new connect" << 
				bt_c->getPeerAddress().toString() << " : " << bt_c->getPeerPort() << std::endl; 

			connect(bt_c, tpBluetoothSocket::readyRead, [=](tpBluetoothSocket *client) {
				tpUInt8 buf[1024];
				buf[20]='\0';
				tpInt64 n = client->recv(buf, sizeof(buf));
				if (n > 0) {
					std::cout << "Received: " << buf << std::endl;
					client->send(buf,n);
				}

			});
			connect(bt_c, tpBluetoothSocket::disconnected, [=](tpBluetoothSocket *client) {
				std::cout << "Client disconnected: " << std::endl;
							
			});			
		}
			
	});
	
	app.run();
	return 0;
}


int main(int32_t argc, char *argv[])
{
	tpList<tpBluetoothLocal> adapter_list=tpBluetoothLocal::getAllDevice();
	for(auto &it:adapter_list)
	{
		std::cout << "name=" << it.getName() << std::endl;
		std::cout << "addr=" << it.getAddress().toString() << std::endl ;
		std::cout << std::endl;
	}
	printf("蓝牙客户端/服务端收发数据测试\n");
//	example_socket_client(argc,argv,tpString("hci1"));
	example_socket_server(argc,argv,tpString("hci1"));
}


