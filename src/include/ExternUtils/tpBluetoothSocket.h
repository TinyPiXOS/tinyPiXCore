#ifndef _TP_BLUETOOTH_SOCKET_H_
#define _TP_BLUETOOTH_SOCKET_H_

#include "tpUtils.h"
#include "tpBluetoothDevice.h"
#include "tpBluetoothAddress.h"

TP_DEF_VOID_TYPE_VAR(ItpBluetoothSocketData);

/// @brief 蓝牙设备的连接通信
class tpBluetoothSocket{
public:
	tpBluetoothSocket(const char *name);
	~tpBluetoothSocket();
public:
	/// @brief 连接到远端蓝牙设备
	/// @param address 远端蓝牙地址
	/// @return 
	int connect(tpBluetoothAddress address);
	int connect(tpBluetoothAddress address,tpString &uuid);

	/// @brief 断开连接
	/// @return 
	int disconnect();
private:
	ItpBluetoothSocketData *data_;
};




#endif