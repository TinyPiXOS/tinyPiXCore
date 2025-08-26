#ifndef _TP_BLUETOOTH_SERVICE_DISCOVERY_H_
#define _TP_BLUETOOTH_SERVICE_DISCOVERY_H_

#include "TpUtils.h"
#include "TpBluetoothDevice.h"
#include "TpBluetoothAddress.h"
#include "TpBluetoothService.h" 

TP_DEF_VOID_TYPE_VAR(ItpBluetoothServiceDiscoveryData);


class TpBluetoothServiceDiscovery{
public:
	TpBluetoothServiceDiscovery(const TpBluetoothAddress &addr);
	~TpBluetoothServiceDiscovery();

public:
	int start();
	int stop();
	int setRemoteAddress(const TpBluetoothAddress &addr);
	TpBluetoothAddress getRemoteAddress() const;
	int setUuidFilter(const TpString &uuid);
	int setUuidFilter(const TpList<TpString> &uuid);
	TpString getUuidFilter() const;
	TpList<TpBluetoothService> discoveredServices();
private:

private:
	ItpBluetoothServiceDiscoveryData *data_;
};




#endif