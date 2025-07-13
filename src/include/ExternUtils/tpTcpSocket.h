#ifndef __TP_TCP_SOCKET_H
#define __TP_TCP_SOCKET_H

#include "tpUtils.h"
#include "tpSignalSlot.h"
#include "tpSocket.h"

TP_DEF_VOID_TYPE_VAR(ItpTcpSocketData);

class tpTcpSocket
{
public:
	tpTcpSocket(tpSocket *sock=nullptr);
	~tpTcpSocket();

public:
	/// @brief 绑定到本地的端口，可以不调用次接口，系统将会随机分配端口
	/// @param addr 绑定到的地址
	/// @param port 绑定到的端口
	/// @return 绑定成功返回0,失败返回-1；
	tpInt32 bind(tpUInt16 port);
	tpInt32 bind(const tpString &addr, tpUInt16 port);
	/// @brief 连接到指定TCP服务器
	/// @param addr TCP服务器地址
	/// @param port TCP服务器端口
	/// @return 连接成功返回tpSockfd，失败返回负值
	tpSockfd connectToHost(const tpString &addr, tpUInt16 port);
	/// @brief 关闭连接
	/// @return 返回0
	tpInt32 close();
	/// @brief 关闭指定连接
	/// @param sock 要关闭的连接
	/// @return 返回0
	tpInt32 close(tpSockfd sock);
	/// @brief 发送数据
	/// @param buff 准备发送的数据
	/// @param size 发送的数据长度
	/// @return 成功返回长度，连接断开返回0,失败返回负值；
	tpInt64 send(const tpUInt8 *buff, tpUInt64 size);
	/// @brief 接收数据
	/// @param buff 接收的缓存区
	/// @param size 接收的缓存区大小
	/// @return 成功返回长度，连接断开返回0,失败返回负值；
	tpInt64 recv(tpUInt8 *buff, tpUInt64 size);

	tpString getPeerAddress();
	tpUInt16 getPeerPort();

public
signals:
	declare_signal(disconnected,tpTcpSocket *);
	declare_signal(readyRead,tpTcpSocket *);

private:
	tpBool checkDisconnected();
	void handleRead();
	void handleDisconnected();
	friend class tpTcpServer;
private:
	ItpTcpSocketData *data_;
};











#endif