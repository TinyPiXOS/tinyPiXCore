/*///------------------------------------------------------------------------------------------------------------------------//
		蓝牙音频服务管理相关接口
说 明 : 
日 期 : 2025.5.8

/*///------------------------------------------------------------------------------------------------------------------------//

#include <iostream>
#include "bluetooth/include/blt_hard.h"
#include "bluetooth/include/blt_device.h"
//#include "bluetooth/include/blt_dbus.h"
#include "bluetooth/include/bluetooth_inc.h"
#include "tpBluetoothAddress.h"
#include "tpDbusConnectManage.h"
#include "tpBluetoothAudioDevice.h"
#include "tpBluetoothLocal.h"

struct tpBluetoothAudioDeviceData{
	tpBluetoothAddress address;
	tpString name;
	Adapter *adapter;
	BluetDevice *device;
	tpBool is_connected;

	DbusMainThread *event;	//用于监测事件
	tpBluetoothAudioDeviceData(const tpBluetoothAddress &addr):address(addr){
		adapter=NULL;
		device=NULL;
		is_connected=TP_FALSE;
	};
};



tpBluetoothAudioDevice::tpBluetoothAudioDevice(const tpString &local,const tpBluetoothDevice &other)
{
	tpBluetoothAudioDevice(local,other.getAddress());
}

tpBluetoothAudioDevice::tpBluetoothAudioDevice(const tpString &local,const tpBluetoothAddress &address)
{
	data_ = new tpBluetoothAudioDeviceData(address);
	tpBluetoothAudioDeviceData *data = static_cast<tpBluetoothAudioDeviceData *>(data_);
	if(!data)
		return;
	tpBluetoothLocal adapter(local.c_str());
	if(!adapter.isPowerOn())
	{
		fprintf(stderr,"[Error]:Adapter is power down\n");
		//return;
	}
	if(tpDbusConnectManage::instance().connection()!=TP_TRUE)
	{
		fprintf(stderr,"connect to dbus error\n");
		delete(data);
		return ;
	}
	printf("find adapter\n");
	data->adapter=find_adapter(local.c_str(),NULL);
	if(!data->adapter)
	{
		tpDbusConnectManage::instance().disConnection();
		delete(data);
		return ;
	}

	data->device=bluet_device_creat(data->adapter,data->address.toString().c_str());

	data->event=dbus_main_thread_creat_once(DBUS_MAIN_THREAD_TYPE_ITERATION);
}


tpBluetoothAudioDevice::~tpBluetoothAudioDevice()
{
	tpBluetoothAudioDeviceData *data = static_cast<tpBluetoothAudioDeviceData *>(data_);
	if(!data)
		return ;
	if(data->is_connected)
	{
		disconnectDevice();
	}
	if(data->event)
		dbus_main_thread_delete_once(data->event);		//需要等待线程结束，会有延迟
	if(data->adapter)
		bluet_object_free(data->adapter);
	if(data->device)
		bluet_device_delete(data->device);
	
	delete(data);
}


int tpBluetoothAudioDevice::connectToDevice()
{
	tpBluetoothAudioDeviceData *data = static_cast<tpBluetoothAudioDeviceData *>(data_);
	if(!data)
		return -1;
	if(!data->adapter)
		return -1;
	if(!data->device)
		return -1;
	if(bluet_connect_remote_device(data->device,NULL)<0)
		return -1;
//	if(bluet_connect_remote_device(data->device,name_to_uuid("AudioSource"))<0)
	{
//		bluet_disconnect_remote_device(data->device,NULL);
//		return -1;
	}
	data->is_connected=TP_TRUE;
	return 0;
}


int tpBluetoothAudioDevice::disconnectDevice()
{
	tpBluetoothAudioDeviceData *data = static_cast<tpBluetoothAudioDeviceData *>(data_);
	if(!data)
		return -1;
	if(!data->adapter)
		return -1;
	if(!data->device)
		return -1;
	if(data->is_connected==TP_FALSE)
		return 0;
	//if(bluet_disconnect_remote_device(data->device,name_to_uuid("AudioSource"))<0)
		;
	if(bluet_disconnect_remote_device(data->device,NULL)<0)
		return -1;
	data->is_connected=TP_FALSE;
	return 0;
}

//或者用openDevice
tpString tpBluetoothAudioDevice::getDevice()
{
	tpBluetoothAudioDeviceData *data = static_cast<tpBluetoothAudioDeviceData *>(data_);
	tpString device="bluealsa:DEV=" + data->address.toString();
	return device;
}

