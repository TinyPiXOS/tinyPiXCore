/*///------------------------------------------------------------------------------------------------------------------------//
		蓝牙通信相关接口
说 明 : 一个类只允许连接到一个蓝牙设备 
日 期 : 2025.5.9

/*///------------------------------------------------------------------------------------------------------------------------//

#include <iostream>
#include <stdio.h>
#include "tpBluetoothSocket.h"
#include "bluetooth/include/bluetooth_inc.h"
#include ""

struct tpBluetoothSocketData{
	Adapter *adapter;	//当前网络连接使用的蓝牙适配器
			//连接标志
	tpString uuid;
	tpBluetoothAddress address;
	BluetDevice *device;
	tpBool is_connect;
	tpBluetoothSocketData(){
		adapter=NULL;
		uuid=NULL;
		device=NULL;
		is_connect=TP_FALLSE;
	};
};

tpBluetoothSocket::tpBluetoothSocket(const char *name)
{
	data_ = new tpBluetoothSocketData();
	tpBluetoothSocketData *data = static_cast<tpBluetoothSocketData *>(data_);
	if(tpDbusConnectManage::instance().connection()!=TP_TRUE)
	{
		fprintf(stderr,"connect to dbus error\n");
		return ;
	}
	data->adapter=find_adapter(name);
	if(data->adapter)
	{
		fprintf(stderr,"设备不存在\n");
	}
}

tpBluetoothSocket::~tpBluetoothSocket()
{
	tpBluetoothSocketData *data = static_cast<tpBluetoothSocketData *>(data_);
	if(!data)
		return ;
	delete(data)
}

int tpBluetoothSocket::connect(tpBluetoothAddress address)
{
	tpBluetoothSocketData *data = static_cast<tpBluetoothSocketData *>(data_);
	if(!data->adapter)
		return -1;
	if(data->is_connect)
	{
		fprintf(stderr,"Repeatedly establish connection\n");
		return -1;
	}
	bluet_device_creat(data->adapter,address.toString().c_str());
	bluet_connect_remote_device(data->device,NULL);
	return 0;
}

int tpBluetoothSocket::connect(tpBluetoothAddress address,tpString &uuid)
{
	tpBluetoothSocketData *data = static_cast<tpBluetoothSocketData *>(data_);
	if(!data->adapter)
		return -1;
	if(data->is_connect)
	{
		fprintf(stderr,"Repeatedly establish connection\n");
		return -1;
	}
	data.uuid=uuid;
	data->address=address;
	bluet_connect_remote_device(data->device,uuid.empty()?NULL:uuid.c_str());
	return 0;
}

int tpBluetoothSocket::disconnect()
{
	tpBluetoothSocketData *data = static_cast<tpBluetoothSocketData *>(data_);
	if(!data->adapter)
		return -1;
	//bluet_disconnect_remote(data->adapter,data.address.toString().c_str(),uuid.empty()?NULL:uuid.c_str());
	return 0;
}




