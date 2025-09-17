#ifndef _TP_BLUETOOTH_SERVER_H_
#define _TP_BLUETOOTH_SERVER_H_

#include "TpUtils.h"
#include "TpSignalSlot.h"
#include "TpBluetoothDevice.h"
#include "TpBluetoothAddress.h"
#include "TpBluetoothService.h"
#include "TpBluetoothSocket.h"

TP_DEF_VOID_TYPE_VAR(ItpBluetoothServerData);


class TpBluetoothServer{
public:

	TpBluetoothServer(const TpString& name, TpBluetoothService::Protocol type);
	~TpBluetoothServer();

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
	tpBool listen(const TpBluetoothAddress &addr=TpBluetoothAddress(), tpUInt16 port=0);
	/// @brief 开始监听客户端连接
	/// @param uuid profile类型
	/// @return 
	tpBool listen(const TpString &uuid);
	/// @brief 获取服务端蓝牙端口
	/// @return 
	tpUInt16 getServerPort();
	/// @brief 获取服务端蓝牙地址
	/// @return 
	TpBluetoothAddress getServerAddress();
	/// @brief 获取服务端协议类型
	/// @return 
	TpBluetoothService::Protocol getServerType();
	/// @brief 查看当前是否在监听
	/// @return 返回监听状态
	tpBool isListening();
	/// @brief 获取下一个可用连接
	/// @return 
	TpBluetoothSocket *nextPendingConnection();
public signals:
	declare_signal(newConnection);

private:
	void handleNewConnection();
	int accept(TpString &client_addr, int &client_port);
private:
	ItpBluetoothServerData *data_;
};




#endif