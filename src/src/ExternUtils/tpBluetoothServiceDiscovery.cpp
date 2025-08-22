

#include "tpBluetoothServiceDiscovery.h"	
#include "bluetooth/include/blt_sdp.h"

struct tpBluetoothServiceDiscoveryData{
	tpBluetoothAddress addr;
	tpList<tpString> uuids_filter;
	tpBluetoothServiceDiscoveryData(const tpBluetoothAddress &addr_):addr(addr_){

	};
};

	
tpBluetoothServiceDiscovery::tpBluetoothServiceDiscovery(const tpBluetoothAddress &addr)
{
	data_ = new tpBluetoothServiceDiscoveryData(addr);
	tpBluetoothServiceDiscoveryData *data = static_cast<tpBluetoothServiceDiscoveryData *>(data_);
	if(!data)
	{
		fprintf(stderr,"[Error]: tpBluetoothServiceDiscovery\n");
		return ;
	}
	
}

tpBluetoothServiceDiscovery::~tpBluetoothServiceDiscovery()
{
	tpBluetoothServiceDiscoveryData *data = static_cast<tpBluetoothServiceDiscoveryData *>(data_);
	if(!data)
		return ;
	

	delete(data);
}

//开始扫描
int tpBluetoothServiceDiscovery::start()
{

	struct SdpAttrValue *attr_data=(struct SdpAttrValue *)malloc(sizeof(struct SdpAttrValue)*16);
	memset(attr_data,0,sizeof(struct SdpAttrValue)*16);
	uint16_t uuid=0x0003;
	const char bt_addr[18]="E4:5F:01:37:58:93";

	printf("开始扫描\n");
//	bluet_quere_profile_attr((const char *)bt_addr, uuid,  attr_data,16);
	printf("开始扫描\n");
//	scan_device_services((const char *)bt_addr);
	scan_device_services(NULL);
	/*memset(attr_data,0,sizeof(struct SdpAttrValue)*16);	
	int count = sdp_query_device(bt_addr, uuid, attr_data, 16);
	if (count < 0) {
        printf("查询失败或未找到服务。\n");
    } 
	else {
        printf("查询到 %d 个属性:\n", count);		

    }*/

    free(attr_data);



	return 0;
}

//停止扫描
int tpBluetoothServiceDiscovery::stop()
{
	return 0;
}


int tpBluetoothServiceDiscovery::setRemoteAddress(const tpBluetoothAddress &addr)
{
	return 0;
}

tpBluetoothAddress tpBluetoothServiceDiscovery::getRemoteAddress() const
{
	return tpBluetoothAddress();
}

int tpBluetoothServiceDiscovery::setUuidFilter(const tpString &uuid)
{
	return 0;
}

int tpBluetoothServiceDiscovery::setUuidFilter(const tpList<tpString> &uuid)
{
	return 0;
}

tpString tpBluetoothServiceDiscovery::getUuidFilter() const
{
	return nullptr;
}

tpList<tpBluetoothService> tpBluetoothServiceDiscovery::discoveredServices()
{
	tpList<tpBluetoothService> list;
	return list;
}


