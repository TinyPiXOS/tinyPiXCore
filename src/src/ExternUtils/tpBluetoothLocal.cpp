/*///------------------------------------------------------------------------------------------------------------------------//
		本地蓝牙相关接口
说 明 : 
日 期 : 2025.4.23

/*///------------------------------------------------------------------------------------------------------------------------//

#include "tpBluetoothLocal.h"
#include "tpBluetoothAddress.h"
#include "tpDbusConnectManage.h"
#include "bluetooth/include/blt_hard.h"
#include "bluetooth/include/blt_device.h"
#include "bluetooth/include/blt_agent.h"

struct tpBluetoothLocalData{
	tpBluetoothAddress address;
	tpString name;
	Adapter *adapter;
	BluetAgent *agent;
	tpBool power;

	tpList<BluetDevice *> device_list;	//
	tpBluetoothLocalData(){
		adapter=NULL;
		agent=NULL;
		power=TP_FALSE;
	};
	~tpBluetoothLocalData(){
		//delete(address);	//address为指针时需要调用
		//if(adapter)

	};
};

//
static void adapterListCallback(const BluetoothAdapter* adapter, void* user_data) 
{
    tpList<tpBluetoothLocal > *adapter_list = static_cast<tpList<tpBluetoothLocal >*>(user_data);
//	tpBluetoothLocal *local=new tpBluetoothLocal(adapter->id, adapter->address, adapter->name);
    adapter_list->emplace_back(adapter->id, adapter->address, adapter->name);
}

static void deviceListCallback(const BluetoothRemote *remote, void *user_data)
{
	tpList<tpBluetoothAddress> *remote_list = static_cast<tpList<tpBluetoothAddress>*>(user_data);

	remote_list->emplace_back(tpString(remote->address));
	//如有需要可以继续添加其他属性
}

tpBluetoothLocal::tpBluetoothLocal(int id, const char *address, const char *name)
{
	data_ = new tpBluetoothLocalData();
	tpBluetoothLocalData *data = static_cast<tpBluetoothLocalData *>(data_);
	if(tpDbusConnectManage::instance().connection()!=TP_TRUE)
	{
		fprintf(stderr,"connect to dbus error\n");
		return ;
	}
	data->address=tpBluetoothAddress(tpString(address));
	data->name=tpString(name);
	if(!getAdapter())
		fprintf(stderr,"Adapter does not exist\n");
}

tpBluetoothLocal::tpBluetoothLocal(const char *name)
{
	data_ = new tpBluetoothLocalData();
	tpBluetoothLocalData *data = static_cast<tpBluetoothLocalData *>(data_);
	if(tpDbusConnectManage::instance().connection()!=TP_TRUE)
	{
		fprintf(stderr,"connect to dbus error\n");
		return ;
	}
	data->name=tpString(name);
	if(!getAdapter())
		fprintf(stderr,"Adapter does not exist\n");
	data->agent=bluet_agent_creat();
	if(!data->agent)
	{
		fprintf(stderr,"Bluetooth service did not start successfully\n");
	}
}

tpBluetoothLocal::~tpBluetoothLocal()
{
	tpBluetoothLocalData *data = static_cast<tpBluetoothLocalData *>(data_);
	if(!data)
		return ;
	if(!data->agent)
		bluet_agent_delete(data->agent);

	if(data->adapter)
		bluet_object_free(data->adapter);

	for (auto it = data->device_list.begin(); it != data->device_list.end(); ) {
		BluetDevice* ptr = *it;      // 获取当前结构体指针
		delete ptr;               // 释放结构体内存
		it = data->device_list.erase(it);    // 删除链表节点，并更新迭代器
	}

	delete(data);
}

tpList<tpBluetoothLocal> tpBluetoothLocal::getAllDevice()
{
	tpList<tpBluetoothLocal> list;
	bluet_get_adapters(adapterListCallback,(void *)(&list));
	return list;
}


tpString tpBluetoothLocal::getName()
{
	tpBluetoothLocalData *data = static_cast<tpBluetoothLocalData *>(data_);
	return data->name;
}

tpBluetoothAddress tpBluetoothLocal::getAddress()
{
	tpBluetoothLocalData *data = static_cast<tpBluetoothLocalData *>(data_);
	return data->address;
}

//此函数世纪返回类型为 Adapter *
void *tpBluetoothLocal::getAdapter()
{
	tpBluetoothLocalData *data = static_cast<tpBluetoothLocalData *>(data_);
	if(!data->adapter)
		data->adapter=find_adapter(data->name.c_str(),NULL);
	if(!data->adapter)	
		return NULL;
	return data->adapter;
}

tpList<tpBluetoothAddress> tpBluetoothLocal::getPairedDevices()
{
	tpBluetoothLocalData *data = static_cast<tpBluetoothLocalData *>(data_);
	tpList<tpBluetoothAddress> device_list;
	bluet_adapter_get_paired_device_list(data->adapter,deviceListCallback,(void *)(&device_list));
	return device_list;
}

tpList<tpBluetoothAddress> tpBluetoothLocal::getConnectedDevices()
{
	tpBluetoothLocalData *data = static_cast<tpBluetoothLocalData *>(data_);
	tpList<tpBluetoothAddress> device_list;
	bluet_adapter_get_connected_device_list(data->adapter,deviceListCallback,(void *)(&device_list));
	return device_list;
}

//请求配对
int tpBluetoothLocal::requestPairing(tpBluetoothAddress &address,tpBluetoothLocal::tpLocalPair pair)
{
	tpBluetoothLocalData *data = static_cast<tpBluetoothLocalData *>(data_);
	if(!getAdapter())
		return -1;

	BluetDevice *device=bluet_device_creat(data->adapter,address.toString().c_str());
	if(!device)
	{
		fprintf(stderr,"Target device not found\n");
		return -1;
	}
	data->device_list.emplace_back(device);

	switch(pair)
	{
		case tpBluetoothLocal::TP_LOCAL_PAIRED:
			bluet_device_pair_with_remote(device,0);
			break;
		case tpBluetoothLocal::TP_LOCAL_AUTHORIZED_PAIRED:			
			bluet_device_pair_with_remote(device,1);
			break;
		case tpBluetoothLocal::TP_LOCAL_UNPAIRED:
			bluet_cancel_paie_with_remote(device);
			break;
	}

//	int bluet_disconnect_remote(Adapter *adapter,const char *name);
	return 0;
}

int tpBluetoothLocal::removeDevice(tpBluetoothAddress &address)
{
	tpBluetoothLocalData *data = static_cast<tpBluetoothLocalData *>(data_);
	if(!getAdapter())
		return -1;
	return bluet_remove_remote(data->adapter,address.toString().c_str());
}

tpBluetoothLocal::tpLocalPair tpBluetoothLocal::getPairStatus(tpBluetoothAddress &address)
{
	tpBluetoothLocalData *data = static_cast<tpBluetoothLocalData *>(data_);
	if(!getAdapter())
		return tpBluetoothLocal::TP_LOCAL_UNPAIRED;
	Device *device=find_device(data->adapter,address.toString().c_str(),NULL);
	
	int paired=bluet_device_get_paired(device);
	int trusted=bluet_device_get_trusted(device);

	if(paired==1 && trusted==1)
		return TP_LOCAL_AUTHORIZED_PAIRED;
	else if(paired==1)
		return TP_LOCAL_PAIRED;
	else	
		return TP_LOCAL_UNPAIRED;
}

//设置信任
int tpBluetoothLocal::setTrusted(tpBluetoothAddress &address, tpBool trusted)
{
	tpBluetoothLocalData *data = static_cast<tpBluetoothLocalData *>(data_);
	return bluet_adapter_set_trusted(data->adapter,address.toString().c_str(),trusted==TP_FALSE?0:1);
}


tpBool tpBluetoothLocal::isPowerOn()
{
	tpBluetoothLocalData *data = static_cast<tpBluetoothLocalData *>(data_);
	if(!data->adapter)
	{
		fprintf(stderr,"Adapter exception");
		return TP_FALSE;
	}
	return (bluet_adapter_get_powered(data->adapter)==0?TP_FALSE:TP_TRUE);
}

int tpBluetoothLocal::setDiscoverable(tpBool discoverable)
{
	tpBluetoothLocalData *data = static_cast<tpBluetoothLocalData *>(data_);
	return bluet_adapter_set_discoverable(data->adapter,discoverable==TP_FALSE?0:1);
}


int tpBluetoothLocal::setDiscoverableTimeout(tpUInt32 timeout)
{
	tpBluetoothLocalData *data = static_cast<tpBluetoothLocalData *>(data_);
	return bluet_adapter_set_discoverable_timeout(data->adapter,timeout);
}


int tpBluetoothLocal::powerOn()
{
	tpBluetoothLocalData *data = static_cast<tpBluetoothLocalData *>(data_);
	if(!data->adapter)
	{
		fprintf(stderr,"Adapter exception");
		return TP_FALSE;
	}
	return bluet_adapter_set_powered(data->adapter,1);
}

int tpBluetoothLocal::powerOff()
{
	tpBluetoothLocalData *data = static_cast<tpBluetoothLocalData *>(data_);
	if(!data->adapter)
	{
		fprintf(stderr,"Adapter exception");
		return TP_FALSE;
	}
	return bluet_adapter_set_powered(data->adapter,0);
}

