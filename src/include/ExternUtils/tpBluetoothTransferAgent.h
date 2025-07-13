#ifndef _TP_BLUETOOTH_TRANSFER_AGENT_H_
#define _TP_BLUETOOTH_TRANSFER_AGENT_H_

#include "tpUtils.h"
#include "tpBluetoothDevice.h"
#include "tpBluetoothAddress.h"

TP_DEF_VOID_TYPE_VAR(ItpBluetoothTransferAgentData);

/// @brief 蓝牙接收文件服务
class tpBluetoothTransferAgent{
public:
	tpBluetoothTransferAgent();
	~tpBluetoothTransferAgent();
public:
	int startServer();
	int stopServer();
	void setSaveDirectory(const tpString &directory);
	tpBool isTransferring() const;
	tpInt64 currentProgress() const;
	tpInt64 totalBytes() const;
private:
	ItpBluetoothTransferAgentData *data_;
};




#endif
