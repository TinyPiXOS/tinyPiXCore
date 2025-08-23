#ifndef _TP_BLUETOOTH_SERVICE_DISCOVERY_H_
#define _TP_BLUETOOTH_SERVICE_DISCOVERY_H_

#include "tpUtils.h"
#include "tpBluetoothDevice.h"
#include "tpBluetoothAddress.h"
#include "tpBluetoothService.h" 

TP_DEF_VOID_TYPE_VAR(ItpBluetoothServiceDiscoveryData);


class tpBluetoothServiceDiscovery{
public:
	tpBluetoothServiceDiscovery(const tpBluetoothAddress &addr);
	~tpBluetoothServiceDiscovery();

public:
	int start();
	int stop();
	int setRemoteAddress(const tpBluetoothAddress &addr);
	tpBluetoothAddress getRemoteAddress() const;
	int setUuidFilter(const tpString &uuid);
	int setUuidFilter(const tpList<tpString> &uuid);
	tpString getUuidFilter() const;
	tpList<tpBluetoothService> discoveredServices();
private:

private:
	ItpBluetoothServiceDiscoveryData *data_;
};




#endif