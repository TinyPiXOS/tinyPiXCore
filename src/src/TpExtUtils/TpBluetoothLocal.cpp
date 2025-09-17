/*///------------------------------------------------------------------------------------------------------------------------//
		本地蓝牙相关接口
说 明 : 
日 期 : 2025.4.23

/*///------------------------------------------------------------------------------------------------------------------------//

#include "TpBluetoothLocal.h"
#include "TpBluetoothAddress.h"
#include "TpDbusConnectManage.h"
#include "bluetooth/include/blt_hard.h"
#include "bluetooth/include/blt_device.h"
#include "bluetooth/include/blt_agent.h"
#include "bluetooth/include/blt_sdp.h"


struct TpBluetoothLocalData{
	TpBluetoothAddress address;
	TpString name;
	Adapter *adapter;
	BluetAgent *agent;
	tpBool power;

	TpList<BluetDevice *> device_list;	//
	TpBluetoothLocalData(){
		adapter=NULL;
		agent=NULL;
		power=TP_FALSE;
	};
	~TpBluetoothLocalData(){
		//delete(address);	//address为指针时需要调用
		//if(adapter)

	};
};

//
static void adapterListCallback(const BluetoothAdapter* adapter, void* user_data) 
{
    TpList<TpBluetoothLocal > *adapter_list = static_cast<TpList<TpBluetoothLocal >*>(user_data);
//	TpBluetoothLocal *local=new TpBluetoothLocal(adapter->id, adapter->address, adapter->name);
//	printf("adapter:%s %s\n",adapter->name,adapter->address);
    adapter_list->emplace_back(adapter->id, adapter->address, adapter->name);
}

static void deviceListCallback(const BluetoothRemote *remote, void *user_data)
{
	TpList<TpBluetoothAddress> *remote_list = static_cast<TpList<TpBluetoothAddress>*>(user_data);

	remote_list->emplace_back(TpString(remote->address));
	//如有需要可以继续添加其他属性
}

TpBluetoothLocal::TpBluetoothLocal(int id, const char *address, const char *name)
{
	data_ = new TpBluetoothLocalData();
	TpBluetoothLocalData *data = static_cast<TpBluetoothLocalData *>(data_);
	if(TpDbusConnectManage::instance().connection()!=TP_TRUE)
	{
		fprintf(stderr,"[Error]: connect to dbus error\n");
		return ;
	}
	data->address=TpBluetoothAddress(TpString(address));
	data->name=TpString(name);
	if(!getAdapter())
		fprintf(stderr,"[Error]: Adapter does not exist\n");
}

TpBluetoothLocal::TpBluetoothLocal(const TpString& name):TpBluetoothLocal(name.c_str())
{
}

TpBluetoothLocal::TpBluetoothLocal(const char *name)
{
	data_ = new TpBluetoothLocalData();
	TpBluetoothLocalData *data = static_cast<TpBluetoothLocalData *>(data_);
	if(TpDbusConnectManage::instance().connection()!=TP_TRUE)
	{
		fprintf(stderr,"[Error]: connect to dbus error\n");
		return ;
	}
	data->name=TpString(name);
	if(!getAdapter())
		fprintf(stderr,"[Error]: Adapter does not exist\n");
	printf("[Debug]: bluet_agent_creat\n");
	data->agent=bluet_agent_creat();
	printf("[Debug]: bluet_agent_creat ok\n");
	if(!data->agent)
	{
		fprintf(stderr,"[Error]: Bluetooth service did not start successfully\n");
	}
}

TpBluetoothLocal::~TpBluetoothLocal()
{
	TpBluetoothLocalData *data = static_cast<TpBluetoothLocalData *>(data_);
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

TpList<TpBluetoothLocal> TpBluetoothLocal::getAllDevice()
{
	TpList<TpBluetoothLocal> list;
	bluet_get_adapters(adapterListCallback,(void *)(&list));
	return list;
}


TpString TpBluetoothLocal::getName()
{
	TpBluetoothLocalData *data = static_cast<TpBluetoothLocalData *>(data_);
	return data->name;
}

TpBluetoothAddress TpBluetoothLocal::getAddress()
{
	TpBluetoothLocalData *data = static_cast<TpBluetoothLocalData *>(data_);
	return data->address;
}

//此函数世纪返回类型为 Adapter *
void *TpBluetoothLocal::getAdapter()
{
	TpBluetoothLocalData *data = static_cast<TpBluetoothLocalData *>(data_);
	if(!data->adapter)
		data->adapter=find_adapter(data->name.c_str(),NULL);
	if(!data->adapter)	
		return NULL;
	return data->adapter;
}

TpList<TpBluetoothAddress> TpBluetoothLocal::getPairedDevices()
{
	TpBluetoothLocalData *data = static_cast<TpBluetoothLocalData *>(data_);
	TpList<TpBluetoothAddress> device_list;
	bluet_adapter_get_paired_device_list(data->adapter,deviceListCallback,(void *)(&device_list));
	return device_list;
}

TpList<TpBluetoothAddress> TpBluetoothLocal::getConnectedDevices()
{
	TpBluetoothLocalData *data = static_cast<TpBluetoothLocalData *>(data_);
	TpList<TpBluetoothAddress> device_list;
	bluet_adapter_get_connected_device_list(data->adapter,deviceListCallback,(void *)(&device_list));
	return device_list;
}

//请求配对
int TpBluetoothLocal::requestPairing(TpBluetoothAddress &address,TpBluetoothLocal::TpLocalPair pair)
{
	TpBluetoothLocalData *data = static_cast<TpBluetoothLocalData *>(data_);
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
		case TpBluetoothLocal::TP_LOCAL_PAIRED:
			bluet_device_pair_with_remote(device,0);
			break;
		case TpBluetoothLocal::TP_LOCAL_AUTHORIZED_PAIRED:			
			bluet_device_pair_with_remote(device,1);
			break;
		case TpBluetoothLocal::TP_LOCAL_UNPAIRED:
			bluet_cancel_paie_with_remote(device);
			break;
	}

//	int bluet_disconnect_remote(Adapter *adapter,const char *name);
	return 0;
}

int TpBluetoothLocal::removeDevice(TpBluetoothAddress &address)
{
	TpBluetoothLocalData *data = static_cast<TpBluetoothLocalData *>(data_);
	if(!getAdapter())
		return -1;
	return bluet_remove_remote(data->adapter,address.toString().c_str());
}

TpBluetoothLocal::TpLocalPair TpBluetoothLocal::getPairStatus(TpBluetoothAddress &address)
{
	TpBluetoothLocalData *data = static_cast<TpBluetoothLocalData *>(data_);
	if(!getAdapter())
		return TpBluetoothLocal::TP_LOCAL_UNPAIRED;
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
int TpBluetoothLocal::setTrusted(TpBluetoothAddress &address, tpBool trusted)
{
	TpBluetoothLocalData *data = static_cast<TpBluetoothLocalData *>(data_);
	return bluet_adapter_set_trusted(data->adapter,address.toString().c_str(),trusted==TP_FALSE?0:1);
}


tpBool TpBluetoothLocal::isPowerOn()
{
	TpBluetoothLocalData *data = static_cast<TpBluetoothLocalData *>(data_);
	if(!data->adapter)
	{
		fprintf(stderr,"Adapter exception");
		return TP_FALSE;
	}
	return (bluet_adapter_get_powered(data->adapter)==0?TP_FALSE:TP_TRUE);
}

int TpBluetoothLocal::setDiscoverable(tpBool discoverable)
{
	TpBluetoothLocalData *data = static_cast<TpBluetoothLocalData *>(data_);
	return bluet_adapter_set_discoverable(data->adapter,discoverable==TP_FALSE?0:1);
}


int TpBluetoothLocal::setDiscoverableTimeout(tpUInt32 timeout)
{
	TpBluetoothLocalData *data = static_cast<TpBluetoothLocalData *>(data_);
	return bluet_adapter_set_discoverable_timeout(data->adapter,timeout);
}


TpList<TpBluetoothUuid> TpBluetoothLocal::getUuids()
{
	TpBluetoothLocalData *data = static_cast<TpBluetoothLocalData *>(data_);
	TpList<TpBluetoothUuid> uuid_list;
	char **uuids=bluet_adapter_get_service_uuids(data->adapter);
	for (int i = 0; uuids[i] != NULL; i++)
	{
		uint8_t uuid128[16];
		bluet_uuidstr_to_uuid128(uuids[i], uuid128);
		TpBluetoothUuid uuid(uuid128);
		uuid_list.emplace_back(uuid);
	}
	return uuid_list;
}

tpBool TpBluetoothLocal::isHaveUuid(TpBluetoothUuid& uuid)
{
	TpBluetoothLocalData *data = static_cast<TpBluetoothLocalData *>(data_);
	TpList<TpBluetoothUuid> uuid_list=getUuids();
	for(auto &it : uuid_list)
	{
		if(it==uuid)
			return TP_TRUE;
	}
	return TP_FALSE;
}


int TpBluetoothLocal::powerOn()
{
	TpBluetoothLocalData *data = static_cast<TpBluetoothLocalData *>(data_);
	if(!data->adapter)
	{
		fprintf(stderr,"Adapter exception");
		return TP_FALSE;
	}
	return bluet_adapter_set_powered(data->adapter,1);
}

int TpBluetoothLocal::powerOff()
{
	TpBluetoothLocalData *data = static_cast<TpBluetoothLocalData *>(data_);
	if(!data->adapter)
	{
		fprintf(stderr,"Adapter exception");
		return TP_FALSE;
	}
	return bluet_adapter_set_powered(data->adapter,0);
}

