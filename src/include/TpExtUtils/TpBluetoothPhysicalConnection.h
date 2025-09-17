#ifndef _TP_BLUETOOTH_PHYSICAL_CONNECTION_H_
#define _TP_BLUETOOTH_PHYSICAL_CONNECTION_H_

#include "TpUtils.h"
#include "TpSignalSlot.h"
#include "TpBluetoothDevice.h"
#include "TpBluetoothAddress.h"

TP_DEF_VOID_TYPE_VAR(ItpBluetoothPhysicalConnectionData);

/// @brief 蓝牙设备的连接通信
class TpBluetoothPhysicalConnection{
public:
	enum ConnectionState {
		TP_BLUET_DISCONNECTED,
		TP_BLUET_CONNECTING,
		TP_BLUET_CONNECTED,
		TP_BLUET_DISCONNECTING,
    };
public:
	TpBluetoothPhysicalConnection(const TpBluetoothAddress &addr);
	~TpBluetoothPhysicalConnection();

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