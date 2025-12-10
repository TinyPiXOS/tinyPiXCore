#ifndef _TP_BLUETOOTH_TRANSFER_H_
#define _TP_BLUETOOTH_TRANSFER_H_

#include <TpCore.h>
#include "TpBluetoothDevice.h"
#include "TpBluetoothAddress.h"

TP_DEF_VOID_TYPE_VAR(ITpBluetoothTransferData);

/// @brief 蓝牙发送文件
class TpBluetoothTransfer{
public:

public:
	TpBluetoothTransfer();
	~TpBluetoothTransfer();
public:
	/// @brief 蓝牙发送文件到远端蓝牙设备
	/// @param address 
	/// @param file 
	/// @return 
	int sendFile(TpBluetoothAddress address,const char *file);
	int sendFile(TpBluetoothAddress address,const TpString& file);

	/// @brief 获取传输状态(暂为封装接口)
	/// @return 
	int getStatus();

	/// @brief 获取传输进度(暂为封装接口)
	/// @return 
	int getProgress();
private:
	ITpBluetoothTransferData *data_;
};




#endif
