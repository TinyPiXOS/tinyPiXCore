

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

}

//停止扫描
int tpBluetoothServiceDiscovery::stop()
{

}


int tpBluetoothServiceDiscovery::setRemoteAddress(const tpBluetoothAddress &addr)
{

}

tpBluetoothAddress tpBluetoothServiceDiscovery::getRemoteAddress() const
{

}

int tpBluetoothServiceDiscovery::setUuidFilter(const tpString &uuid)
{

}

int tpBluetoothServiceDiscovery::setUuidFilter(const tpList<tpString> &uuid)
{

}

tpString tpBluetoothServiceDiscovery::getUuidFilter() const
{

}

tpList<tpBluetoothService> tpBluetoothServiceDiscovery::discoveredServices()
{

}


