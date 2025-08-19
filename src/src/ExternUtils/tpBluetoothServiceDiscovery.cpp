

#include "tpBluetoothServiceDiscovery.h"	

struct tpBluetoothServiceDiscoveryData{
	tpBluetoothAddress remote_addr;
	tpList<tpString> uuids_filter;
	tpBluetoothServiceDiscoveryData(const tpBluetoothAddress &addr):remote_addr(addr){

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


