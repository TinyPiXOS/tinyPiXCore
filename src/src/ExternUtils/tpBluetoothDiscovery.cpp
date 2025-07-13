/*///------------------------------------------------------------------------------------------------------------------------//
		蓝牙扫描相关接口
说 明 : 
日 期 : 2025.4.22

/*///------------------------------------------------------------------------------------------------------------------------//

#include <iostream>
#include <cstring>
#include <algorithm>
#include <functional>
#include "tpDbusConnectManage.h"
#include "tpSystemDataManage.h"
#include "bluetooth/include/blt_hard.h"
#include "bluetooth/include/blt_dbussignal.h"
#include "bluetooth/include/bluetooth_inc.h"
#include "tpBluetoothDiscovery.h"
#include "tpBluetoothLocal.h"

struct tpBluetoothListData{
	tpList<tpBluetoothDevice *> device_list;
	tpSystemDataManage data;
};


struct tpBluetoothDiscoveryData{
	tpList<tpBluetoothDevice *> device_list;
	tpSystemDataManage data;
	BluetDeviceScan *scan;
	BluetDbusSignal *sig_add;
	BluetDbusSignal *sig_remove;
	BluetDbusSignal *sig_cha;
	
	tpBluetoothDiscoveryData(){
		sig_add=NULL;
		sig_cha=NULL;
		scan=NULL;
	};

	~tpBluetoothDiscoveryData() {
		// 释放 device_list 中的每个 tpBluetoothDevice*
		for (auto* device : device_list) {
			delete device;
		}
		device_list.clear();

		// 释放其他指针
		if(scan) delete scan;
		if(sig_add) delete sig_add;
		if(sig_cha) delete sig_cha;
		std::cout << "struct free ok\n\n";
	}
};


//蓝牙信号列表新增设备
static void deviceListAddCallback(const BluetoothRemote *remote, void* user_data) 
{
	tpList<tpBluetoothDevice *> *device_list = static_cast<tpList<tpBluetoothDevice *>*>(user_data);
	if(!remote)
	{
		std::cout << "new device error\n";
		return ;
	}
	tpBluetoothDevice *device=new tpBluetoothDevice(remote->name,
									remote->address,
									remote->rssi,
									remote->class_type,
									remote->paired,
									remote->legacy_pairing,
									remote->alias,
									remote->icon);
	device_list->emplace_back(device);
}

//蓝牙信号列表减少设备
static void deviceListRemoveCallback(const BluetoothRemote *remote, void* user_data) 
{
	tpList<tpBluetoothDevice *> *device_list = static_cast<tpList<tpBluetoothDevice *>*>(user_data);
	if(!remote)
	{
		std::cout << "del device error\n";
		return ;
	}

	auto it = find_if(device_list->begin(), device_list->end(), [remote](tpBluetoothDevice *bt){ 
		tpString bt_remove(remote->address);  
		return (bt->getAddress()==bt_remove); });
	if (it != device_list->end()) {
		delete *it; 
    	it = device_list->erase(it); // 更新迭代器指向下一元素
	}
}

extern "C" void deviceAddCallback(const BluetoothRemote *remote, void *user_data) {
    auto self = static_cast<tpBluetoothDiscovery *>(user_data);
    if (self) self->onDeviceAdd(remote);
}

extern "C" void deviceRemoveCallback(const BluetoothRemote *remote, void *user_data) {
    auto self = static_cast<tpBluetoothDiscovery *>(user_data);
    if (self) self->onDeviceRemove(remote);
}





tpBluetoothDiscovery::tpBluetoothDiscovery(const tpString& local)
{
	char* ptr = new char[local.size() + 1];      // 分配内存
	local.copy(ptr, local.size());                 // 复制内容
	ptr[local.size()] = '\0';                    // 手动添加终止符
	tpBluetoothDiscovery((const char *)ptr);
}


tpBluetoothDiscovery::tpBluetoothDiscovery(const char *local)
{
	tpBluetoothLocal adapter(local);
	if(!adapter.isPowerOn())
	{
		fprintf(stderr,"[Error]:Adapter is power down\n");
		//return;
	}

	data_ = new tpBluetoothDiscoveryData();
	tpBluetoothDiscoveryData *data = static_cast<tpBluetoothDiscoveryData *>(data_);

	if(tpDbusConnectManage::instance().connection()!=TP_TRUE)
	{
		fprintf(stderr,"[Error]:connect to dbus error\n");
		return ;
	}


	data->scan=bluet_adapter_scan_creat(local);
	if(!data->scan)
		return ;
	std::cout << "tpBluetoothDiscovery creat ok" << std::endl;
}


tpBluetoothDiscovery::~tpBluetoothDiscovery()
{
	tpBluetoothDiscoveryData *data = static_cast<tpBluetoothDiscoveryData *>(data_);
	if(!data)
		return;
	if(data->scan)
	{
		stop();
	}
	std::cout << "析构tpBluetoothDiscovery\n";
	if (data->data.running == true)
	{
		data->data.running = false;
		if (data->data.thread_t.joinable())
			data->data.thread_t.join(); // 等待线程完成
	}

	bluet_adapter_scan_delete(data->scan);
	std::cout << "析构tpBluetoothDiscovery\n";
	delete(data);
}

void tpBluetoothDiscovery::start()
{
	tpBluetoothDiscoveryData *data = static_cast<tpBluetoothDiscoveryData *>(data_);

//	data->sig_add =		bluet_adapter_interfaces_added(data->scan, deviceListAddCallback, &data->device_list);
//	data->sig_remove = 	bluet_adapter_interfaces_removed(data->scan, deviceListRemoveCallback, &data->device_list);

	data->sig_add =		bluet_adapter_interfaces_added(data->scan, deviceAddCallback, this);
	data->sig_remove = 	bluet_adapter_interfaces_removed(data->scan, deviceRemoveCallback, this);

	data->sig_cha =		bluet_adapter_properties_changed(data->scan,NULL,NULL);
	
	if(data->sig_add)
	{
		std::cout << "sig add ok" << std::endl;
	}
	if(data->sig_remove)
	{
		std::cout << "sig remove ok" << std::endl;
	}
	if(data->sig_cha)
	{
		std::cout << "sig cha ok" << std::endl;
	}

	data->data.running = true;
	data->data.thread_t = std::thread(&tpBluetoothDiscovery::discovery,this);
}

void tpBluetoothDiscovery::onDeviceAdd(const void *remote) {
    auto dev = static_cast<const BluetoothRemote *>(remote);
    if (!dev) return;
	std::cout << "sig：onDeviceAdd" << std::endl;
	tpBluetoothDevice bt_device(dev->name,
									dev->address,
									dev->rssi,
									dev->class_type,
									dev->paired,
									dev->legacy_pairing,
									dev->alias,
									dev->icon);

	bluetoothDeviceAdd.emit(bt_device);
}

void tpBluetoothDiscovery::onDeviceRemove(const void *remote) {
    auto dev = static_cast<const BluetoothRemote *>(remote);
    if (!dev) return;
	std::cout << "sig： onDeviceRemove" << std::endl;
	tpBluetoothAddress addr(tpString(dev->address));
	bluetoothDeviceRemove.emit(addr);
}


void tpBluetoothDiscovery::discovery()
{
	tpBluetoothDiscoveryData *data = static_cast<tpBluetoothDiscoveryData *>(data_);
	bluet_adapter_start_discovery(data->scan);		//此接口为阻塞扫描
/*	while(data->data.running)
	{
		std::this_thread::sleep_for(std::chrono::milliseconds(1000));
		
	}*/
}


void tpBluetoothDiscovery::stop()
{
	tpBluetoothDiscoveryData *data = static_cast<tpBluetoothDiscoveryData *>(data_);
	bluet_adapter_stop_discovery(data->scan);
	data->scan=NULL;
	bluet_adapter_sbus_signal_delete(data->sig_add);
	data->sig_add=NULL;
	bluet_adapter_sbus_signal_delete(data->sig_remove);
	data->sig_remove=NULL;
	bluet_adapter_sbus_signal_delete(data->sig_cha);
	data->sig_cha=NULL;
}


/*
由于改为了信号的方式，暂不支持直接获取列表，需要用户手动生成
tpList<tpBluetoothDevice *> tpBluetoothDiscovery::getDeviceList()
{
	tpBluetoothDiscoveryData *data = static_cast<tpBluetoothDiscoveryData *>(data_);
	return data->device_list;
}
*/

int tpBluetoothDiscovery::setInquiryType(tpDiscoveryInquiryType type)
{
	return 0;
}

