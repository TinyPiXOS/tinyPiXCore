#ifndef _TP_BLUETOOTH_SERVER_H_
#define _TP_BLUETOOTH_SERVER_H_

#include "tpUtils.h"
#include "tpSignalSlot.h"
#include "tpBluetoothDevice.h"
#include "tpBluetoothAddress.h"
#include "tpBluetoothService.h"
#include "tpBluetoothSocket.h"

TP_DEF_VOID_TYPE_VAR(ItpBluetoothServerData);


class tpBluetoothServer{
public:

	tpBluetoothServer(const char *name, tpBluetoothService::Protocol type);
	~tpBluetoothServer();

public:
	/// @brief 关闭服务端
	/// @return 返回0
	tpInt32 close();
	/// @brief 设置最大可以连接的数量，需要在监听之前调用
	/// @param max 最大连接数量
	void setMaxPendingConnects(tpInt32 max);
	/// @brief 获取最大可连接的数量
	/// @return 
	tpInt32 getMaxPendingConnects();
	/// @brief 开始监听客户端连接
	/// @param addr 
	/// @param port 非传统意义上的端口，当RFCOMM时表示channel，当使用L2CAP时表示psm。未来可能扩展其他协议
	/// @return 
	tpBool listen(const tpBluetoothAddress &addr, tpUInt16 port=0);
	tpBool listen(const tpString &uuid);
	tpUInt16 getServerPort();
	tpBluetoothAddress getServerAddress();
	tpBluetoothService::Protocol getServerType();
	/// @brief 查看当前是否在监听
	/// @return 返回监听状态
	tpBool isListening();
	tpBluetoothSocket *nextPendingConnection();
public signals:
	declare_signal(newConnection);

private:
	void handleNewConnection();
	int accept();
private:
	ItpBluetoothServerData *data_;
};




#endif