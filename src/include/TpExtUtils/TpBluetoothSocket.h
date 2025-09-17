#ifndef _TP_BLUETOOTH_SOCKET_H_
#define _TP_BLUETOOTH_SOCKET_H_

#include "TpUtils.h"
#include "TpSignalSlot.h"
#include "TpBluetoothDevice.h"
#include "TpBluetoothAddress.h"
#include "TpBluetoothService.h"

TP_DEF_VOID_TYPE_VAR(ItpBluetoothSocketData);

/// @brief 蓝牙设备的连接通信
class TpBluetoothSocket{
public:
	TpBluetoothSocket(const TpString& name,TpBluetoothService::Protocol type=TpBluetoothService::TP_BLUET_UNKNOWN_PROTOCOL);
	TpBluetoothSocket(const TpString& name,int sockfd,TpBluetoothService::Protocol type=TpBluetoothService::TP_BLUET_UNKNOWN_PROTOCOL);
	~TpBluetoothSocket();
public:
	/// @brief 连接到远端蓝牙设备(暂不支持)
	/// @param service 远端蓝牙的服务
	/// @return 
	int connectToService(const TpBluetoothService& service);

	/// @brief 连接到远端蓝牙设备(暂不支持)
	/// @param addr 
	/// @param uuid 服务的uuid(注意不能使用protocol的uuid)
	/// @return 
	int connectToService(const TpBluetoothAddress& addr, const TpString& uuid);

	/// @brief 连接到远端蓝牙设备
	/// @param addr 远端地址
	/// @param port 通道号或psm号
	/// @return 
	int connectToService(const TpBluetoothAddress& addr, tpUInt16 port);

	/// @brief 断开连接
	/// @return 
	int disconnectFromService();

	tpUInt64 send(const tpUInt8 *buff, tpUInt64 size);
	tpUInt64 recv(tpUInt8 *buff, tpUInt64 size);

	TpBluetoothAddress getPeerAddress();
	TpString getPeerName();
	tpUInt16 getPeerPort();

public
signals:
	declare_signal(connected);
	declare_signal(disconnected,TpBluetoothSocket *);
	declare_signal(readyRead,TpBluetoothSocket *);
private:
	void handleWrite();
	void handleRead();
	void handleDisconnected();
	tpBool checkDisconnected();
	void setDisconnectedInfo();
private:
	ItpBluetoothSocketData *data_;
};




#endif