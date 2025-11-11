#ifndef __TP_UDP_SOCKET_H
#define __TP_UDP_SOCKET_H

#include <TpCore.h>
#include "TpSignalSlot.h"
#include "TpNetworkDatagram.h"


TP_DEF_VOID_TYPE_VAR(ItpUdpSocketData);

class TpUdpSocket
{
public:
	TpUdpSocket();
	~TpUdpSocket();

public:
	/// @brief 绑定到指定地址和端口
	/// @param addr 地址
	/// @param port 端口
	/// @return 绑定成功返回0,失败返回-1
	tpInt32 bind(tpUInt16 port);
	tpInt32 bind(const TpString &addr, tpUInt16 port);
	/// @brief 发送数据到指定地址和端口
	/// @param data 数据
	/// @param size 数据的长度
	/// @param addr 目标地址
	/// @param port 目标端口
	/// @return 成功返回发送的字节数,失败返回-1
	tpInt64 sendTo(const tpUInt8 *data, tpUInt64 size, const TpString &addr, tpUInt16 port);
	/// @brief 发送数据到指定地址和端口
	/// @param datagram 数据
	/// @return 
	tpInt64 sendTo(const TpNetworkDatagram &datagram);
	/// @brief 接收数据
	/// @param data 数据
	/// @param size 允许的最大接收长度
	/// @param addr 目标地址
	/// @param port 目标端口
	/// @return 成功返回发送的字节数,失败返回-1
	tpInt64 recvFrom(tpUInt8 *data, tpUInt64 size, TpString &addr, tpUInt16 *port);
	/// @brief 接收数据
	/// @param size 允许的最大接收长度
	/// @return 
	TpNetworkDatagram recvDatagram(tpUInt64 size);

	tpBool hasPendingDatagrams();

public signals:
    declare_signal(readyRead);

private:
	void handleReadyRead();
	ItpUdpSocketData *data_;
};











#endif