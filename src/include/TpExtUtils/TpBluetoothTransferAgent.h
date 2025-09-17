#ifndef _TP_BLUETOOTH_TRANSFER_AGENT_H_
#define _TP_BLUETOOTH_TRANSFER_AGENT_H_

#include "TpUtils.h"
#include "TpBluetoothDevice.h"
#include "TpBluetoothAddress.h"

TP_DEF_VOID_TYPE_VAR(ItpBluetoothTransferAgentData);

/// @brief 蓝牙接收文件服务
class TpBluetoothTransferAgent{
public:
	TpBluetoothTransferAgent();
	~TpBluetoothTransferAgent();
public:
	int startServer();
	int stopServer();
	void setSaveDirectory(const TpString &directory);
	tpBool isTransferring() const;
	tpInt64 currentProgress() const;
	tpInt64 totalBytes() const;
private:
	ItpBluetoothTransferAgentData *data_;
};




#endif
