#ifndef __TP_TCP_SERVER_H
#define __TP_TCP_SERVER_H

#include "tpUtils.h"
#include "tpSignalSlot.h"
#include "tpHostAddress.h"
#include "tpSocket.h"
#include "tpTcpSocket.h"

TP_DEF_VOID_TYPE_VAR(ItpTcpServerData);

class tpTcpServer
{
public:
	tpTcpServer();
	~tpTcpServer();

public:
	/// @brief 关闭TCP服务端
	/// @return 返回0
	tpInt32 close();
	/// @brief 设置最大可以连接的数量，需要在监听之前调用
	/// @param max 最大连接数量
	void setMaxPendingConnects(tpInt32 max);
	/// @brief 开始监听客户端连接
	/// @param addr 
	/// @param port 
	/// @return 
	tpBool listen(tpString &addr, tpUInt16 port);
	/// @brief 开始监听客户端连接
	/// @param address 
	/// @param port 
	/// @return 
	//tpBool listen(const tpHostAddress &address, tpUInt16 port);
	/// @brief 查看当前是否在监听
	/// @return 返回监听状态
	tpBool isListening();
	/// @brief 接受一个新的客户端连接(此函数以非阻塞的方式等待新的连接)
	/// @return 返回一个客户端连接
	tpTcpSocket *acceptConnect();
	/// @brief 接收所有连接的数据(暂不需要，未实现)
	/// @return 
	tpInt32 recvAll();
	tpTcpSocket *nextPendingConnection();

public signals:
	declare_signal(newConnection);

private:
	void handleNewConnection();
	tpSocket *accept();

private:
	ItpTcpServerData *data_;
};








#endif
