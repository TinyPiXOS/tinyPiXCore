

#include <iostream>
#include "TpApp.h"
#include "TpMainWindow.h"
#include "TpBluetoothLocal.h"
#include "TpBluetoothDiscovery.h"
#include "TpBluetoothDevice.h"
#include "TpBluetoothSocket.h"
#include "TpBluetoothServer.h"
#include "TpBluetoothService.h"

//扫描蓝牙
int example_list_device(const TpString& adapter)
{

	TpBluetoothDiscovery discovery(adapter);

	discovery.start();
	
	connect(&discovery, bluetoothDeviceRemove, [=](TpBluetoothAddress address)
            { std::cout << "[Signal]设备消失：" << address.toString() << std::endl; });

	connect(&discovery, bluetoothDeviceAdd, [=](const TpBluetoothDevice &device)
            { std::cout << "[Signal]设备新增：" << device.getAddress().toString() << std::endl; });

	
	discovery.stop();
	while(1);
	return 0 ;
}

int example_socket_client(int32_t argc, char *argv[],const TpString& adapter)
{
	TpApp app(argc, argv);
	TpMainWindow *vScreen = new TpMainWindow();
	vScreen->setBackGroundColor(_RGBA(128, 128, 128, 255));
	 weekly
	

	tpUInt8 buff[10]="senddata\n";

	TpString adapter_=adapter;
	TpBluetoothLocal local(adapter_.c_str());
	local.powerOn();

	TpBluetoothSocket bt_client(adapter,TpBluetoothService::TP_BLUET_RFCOMM_PROTOCOL);

	//扫描
//	example_list_device(adapter);

	std::cout << "connect to E4:5F:01:37:58:93 ..." << std::endl; 
	if(bt_client.connectToService(TpBluetoothAddress("E4:5F:01:37:58:93"), 1)<0)
	{
		std::cout << "connect error" << std::endl; 	
	}
	std::cout << "connect to E4:5F:01:37:58:93 ..." << std::endl; 
	

	connect(&bt_client, connected, [&bt_client, &buff]( )
        { 
			std::cout << "connect to remote ok" << std::endl; 
			while(1)
			{
				bt_client.send((const tpUInt8 *)buff,sizeof(buff));
				sleep(1);
			}
				
		});
	connect(&bt_client, disconnected, [=](TpBluetoothSocket *bt)
        {	
			std::cout << "disconnect from remote" << std::endl; 
		});
			
	
	app.run();
	return 0 ;
}

int example_socket_server(int32_t argc, char *argv[],const TpString& adapter)
{
	TpApp app(argc, argv);
	TpMainWindow *vScreen = new TpMainWindow();
	vScreen->setBackGroundColor(_RGBA(128, 128, 128, 255));
	 weekly
	

	TpBluetoothServer bt_server(adapter,TpBluetoothService::TP_BLUET_RFCOMM_PROTOCOL);
	bt_server.listen(TpBluetoothAddress::any(),1);
	printf("服务端已启动\n");

	connect(&bt_server, newConnection,[&]()
	{
		TpBluetoothSocket *bt_c=bt_server.nextPendingConnection();
		if(bt_c)
		{
			std::cout << "accept new connect" << 
				bt_c->getPeerAddress().toString() << " : " << bt_c->getPeerPort() << std::endl; 

			connect(bt_c, TpBluetoothSocket::readyRead, [=](TpBluetoothSocket *client) {
				tpUInt8 buf[1024];
				buf[20]='\0';
				tpInt64 n = client->recv(buf, sizeof(buf));
				if (n > 0) {
					std::cout << "Received: " << buf << std::endl;
					client->send(buf,n);
				}

			});
			connect(bt_c, TpBluetoothSocket::disconnected, [=](TpBluetoothSocket *client) {
				std::cout << "Client disconnected" << std::endl;
							
			});			
		}
			
	});
	
	app.run();
	return 0;
}


int main(int32_t argc, char *argv[])
{
	TpList<TpBluetoothLocal> adapter_list=TpBluetoothLocal::getAllDevice();
	for(auto &it:adapter_list)
	{
		std::cout << "name=" << it.getName() << std::endl;
		std::cout << "addr=" << it.getAddress().toString() << std::endl ;
		std::cout << std::endl;
	}
	printf("蓝牙客户端/服务端收发数据测试\n");
	example_socket_client(argc,argv,TpString("hci0"));
//	example_socket_server(argc,argv,TpString("hci1"));
}


