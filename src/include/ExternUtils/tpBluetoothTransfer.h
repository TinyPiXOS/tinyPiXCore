#ifndef _TP_BLUETOOTH_TRANSFER_H_
#define _TP_BLUETOOTH_TRANSFER_H_

#include "tpUtils.h"
#include "tpBluetoothDevice.h"
#include "tpBluetoothAddress.h"

TP_DEF_VOID_TYPE_VAR(ItpBluetoothTransferData);

/// @brief 蓝牙发送文件
class tpBluetoothTransfer{
public:

public:
	tpBluetoothTransfer();
	~tpBluetoothTransfer();
public:
	/// @brief 蓝牙发送文件到远端蓝牙设备
	/// @param address 
	/// @param file 
	/// @return 
	int sendFile(tpBluetoothAddress address,const char *file);
	int sendFile(tpBluetoothAddress address,const tpString& file);

	/// @brief 获取传输状态(暂为封装接口)
	/// @return 
	int getStatus();

	/// @brief 获取传输进度(暂为封装接口)
	/// @return 
	int getProgress();
private:
	ItpBluetoothTransferData *data_;
};




#endif
