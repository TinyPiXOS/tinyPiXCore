#ifndef _TP_BLUETOOTH_PHYSICAL_CONNECTION_H_
#define _TP_BLUETOOTH_PHYSICAL_CONNECTION_H_

#include "tpUtils.h"
#include "tpSignalSlot.h"
#include "tpBluetoothDevice.h"
#include "tpBluetoothAddress.h"

TP_DEF_VOID_TYPE_VAR(ItpBluetoothPhysicalConnectionData);

/// @brief 蓝牙设备的连接通信
class tpBluetoothPhysicalConnection{
public:
	enum ConnectionState {
		TP_BLUET_DISCONNECTED,
		TP_BLUET_CONNECTING,
		TP_BLUET_CONNECTED,
		TP_BLUET_DISCONNECTING,
    };
public:
	tpBluetoothPhysicalConnection(const tpBluetoothAddress &addr);
	~tpBluetoothPhysicalConnection();

public:
	int connect();



public
signals:
    declare_signal(connected);
	declare_signal(disconnect);
	declare_signal(disconnected, ConnectionState);

private:
	ItpBluetoothPhysicalConnectionData *data_;
};




#endif