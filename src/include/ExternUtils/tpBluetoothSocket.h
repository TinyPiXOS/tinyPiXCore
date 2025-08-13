#ifndef _TP_BLUETOOTH_SOCKET_H_
#define _TP_BLUETOOTH_SOCKET_H_

#include "tpUtils.h"
#include "tpSignalSlot.h"
#include "tpBluetoothDevice.h"
#include "tpBluetoothAddress.h"
#include "tpBluetoothService.h"

TP_DEF_VOID_TYPE_VAR(ItpBluetoothSocketData);

/// @brief 蓝牙设备的连接通信
class tpBluetoothSocket{
public:
	tpBluetoothSocket(const tpString& name,tpBluetoothService::Protocol type=tpBluetoothService::TP_BLUET_UNKNOWN_PROTOCOL);
	tpBluetoothSocket(int sockfd,tpBluetoothService::Protocol type=tpBluetoothService::TP_BLUET_UNKNOWN_PROTOCOL);
	~tpBluetoothSocket();
public:
	/// @brief 连接到远端蓝牙设备(暂不支持)
	/// @param service 远端蓝牙的服务
	/// @return 
	int connectToService(const tpBluetoothService& service);
	/// @brief 
	/// @param addr 
	/// @param uuid 服务的uuid(注意不能使用protocol的uuid)
	/// @return 
	int connectToService(const tpBluetoothAddress& addr, const tpString& uuid);

	int connectToService(const tpBluetoothAddress& addr, tpUInt16 port);


	/// @brief 断开连接
	/// @return 
	int disconnectFromService();

	tpUInt64 send(const tpUInt8 *buff, tpUInt64 size);
	tpUInt64 recv(tpUInt8 *buff, tpUInt64 size);

public
signals:
	declare_signal(connected);
	declare_signal(disconnected,tpBluetoothSocket *);
	declare_signal(readyRead,tpBluetoothSocket *);
private:
	void handleWrite();
	void handleRead();
	void handleDisconnected();
	tpBool checkDisconnected();
private:
	ItpBluetoothSocketData *data_;
};




#endif