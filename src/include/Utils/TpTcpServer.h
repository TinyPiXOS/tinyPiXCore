#ifndef __TP_TCP_SERVER_H
#define __TP_TCP_SERVER_H

#include "TpUtils.h"
#include "TpSignalSlot.h"
#include "TpHostAddress.h"
#include "TpSocket.h"
#include "TpTcpSocket.h"

TP_DEF_VOID_TYPE_VAR(ItpTcpServerData);

class TpTcpServer
{
public:
	TpTcpServer();
	~TpTcpServer();

public:
	/// @brief 关闭TCP服务端
	/// @return 返回0
	tpInt32 close();
	/// @brief 设置最大可以连接的数量，需要在监听之前调用
	/// @param max 最大连接数量
	void setMaxPendingConnects(tpInt32 max);
	/// @brief 获取最大可以连接的数量
	/// @return 
	tpInt32 getMaxPendingConnects();
	/// @brief 开始监听客户端连接
	/// @param addr 
	/// @param port 
	/// @return 
	tpBool listen(TpString &addr, tpUInt16 port);
	/// @brief 开始监听客户端连接
	/// @param address 
	/// @param port 
	/// @return 
	//tpBool listen(const TpHostAddress &address, tpUInt16 port);
	/// @brief 查看当前是否在监听
	/// @return 返回监听状态
	tpBool isListening();
	/// @brief 接受一个新的客户端连接(此函数以非阻塞的方式等待新的连接)
	/// @return 返回一个客户端连接
	TpTcpSocket *acceptConnect();
	/// @brief 接收所有连接的数据(暂不需要，未实现)
	/// @return 
	tpInt32 recvAll();
	TpTcpSocket *nextPendingConnection();

public signals:
	declare_signal(newConnection);

private:
	void handleNewConnection();
	TpSocket *accept();

private:
	ItpTcpServerData *data_;
};








#endif
