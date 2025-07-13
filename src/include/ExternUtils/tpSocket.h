#ifndef __TP_SOCKET_H_
#define __TP_SOCKET_H_

#include "tpString.h"

TP_DEF_VOID_TYPE_VAR(ItpSocketData);

typedef int tpSockfd;

class tpSocket
{
public:
	enum tpSocketType{
		TP_SOCK_STREAM	,
		TP_SOCK_DGRAM	,
	};
	enum tpSocketStatus{
		TP_SOCK_DISCONNECT,
		TP_SOCK_BIND,
		TP_SOCK_LISTEN	,
		TP_SOCK_CONNECT,
	};

public:
	tpSocket();
	tpSocket(tpSockfd sockfd, tpString addr_r, tpUInt16 port_r);
	~tpSocket();
	tpSocket &operator=(const tpSocket &other);

public:
	/// @brief 创建新的socket
	/// @param type socket类型
	/// @return 返回创建好的socket fd
	tpSockfd socket(tpSocketType type);
	/// @brief 绑定到指定地址和端口
	/// @param addr 地址
	/// @param port 端口
	/// @return 
	tpInt32 bind(const tpString &addr,tpUInt16 port);
	/// @brief 关闭连接
	/// @return 
	tpInt32 close();
	/// @brief 关闭指定连接
	/// @param sock 
	/// @return 
	tpInt32 close(tpSockfd sock);
	/// @brief 发送数据到指定地址和端口(UDP)
	/// @param data 发送数据缓存区
	/// @param size 数据长度
	/// @param addr 地址
	/// @param port 端口
	/// @return 发送长度或错误码
	tpInt64 sendTo(const tpUInt8 *data,tpUInt64 size,const tpString &addr,tpUInt16 port);
	/// @brief 从指定端口和地址接收数据(UDP)
	/// @param data 接收的数据缓存区
	/// @param size 最大接收长度
	/// @param addr 数据来源地址
	/// @param port 数据来源端口
	/// @return 接收长度或错误码
	tpInt64 recvFrom(tpUInt8 *data,tpUInt64 size,tpString &addr,tpUInt16 *port);
	/// @brief 连接到目标地址和端口
	/// @param addr 地址
	/// @param port 端口
	/// @return 
	tpSocket *connectToHost(const tpString &addr ,tpUInt16 port);
	/// @brief 监听连接
	/// @param size 
	/// @return 
	tpInt32 listen(tpInt32 size);
	/// @brief 发送数据(TCP)
	/// @param data 发送数据缓存区
	/// @param size 数据长度
	/// @return 发送长度或错误码
	tpInt64 send(const tpUInt8 *data,tpUInt64 size);
	/// @brief 接收数据(TCP)
	/// @param data 接收的数据缓存区
	/// @param size 最大接收长度
	/// @param flag 模式，可以不设置，默认为0
	/// @return 接收长度或错误码
	tpInt64 recv(tpUInt8 *data,tpUInt64 size,tpInt32 flag=0);
	/// @brief 获取socket连接文件描述符
	/// @return 文件描述符
	tpSockfd getSocket();
	/// @brief 获取socket连接主机地址
	/// @return 地址
	tpString getLocalAddress();
	/// @brief 获取socket连接主机端口
	/// @return 端口
	tpUInt16 getLocalPort();
	/// @brief 获取远端主机地址
	/// @return 地址
	tpString getPeerAddress();
	/// @brief 获取短短主机端口
	/// @return 端口
	tpUInt16 getPeerPort();
	

protected:
//	tpSockfd sockfd;
	ItpSocketData *data_;
};



#endif
