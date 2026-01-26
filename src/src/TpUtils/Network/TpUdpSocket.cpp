/*///------------------------------------------------------------------------------------------------------------------------//
		UDP通信
说 明 :
日 期 : 2024.12.24

/*///------------------------------------------------------------------------------------------------------------------------//

#include <iostream>
#include <vector>
#include <cstring>
#include <vector>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>             //地址转换
#include "TpUdpSocket.h"
#include "TpSocket.h"
#include "TpNetworkDatagram.h"
#include "TpSocketNotifier.h"
#include "TpSocketNotifierNew.h"

struct TpUdpSocketData{
	TpSocket *sock;
	TpSocketNotifierNew *notifier;
	mutable tpBool hasData;
	TpUdpSocketData(){
		sock=nullptr;
		notifier=nullptr;
		hasData=TP_FALSE;
	}
};


TpUdpSocket::TpUdpSocket()
{
	data_=new TpUdpSocketData();
	TpUdpSocketData *sockData=static_cast<TpUdpSocketData *>(data_);
	sockData->sock=new TpSocket();
	sockData->sock->socket(TpSocket::TP_SOCK_DGRAM);
}

TpUdpSocket::~TpUdpSocket()
{
	TpUdpSocketData *sockData=static_cast<TpUdpSocketData *>(data_);
	if(!sockData)
		return ;
	if(sockData->sock)
		delete(sockData->sock);
	if(sockData->notifier)
		delete(sockData->notifier);
	delete(sockData);
}

tpInt32 TpUdpSocket::bind(const TpString &addr, tpUInt16 port)
{
	TpUdpSocketData *sockData=static_cast<TpUdpSocketData *>(data_);
	tpInt32 err=sockData->sock->bind(addr,port);
	if(err<0)
		return err;

	sockData->notifier = new TpSocketNotifierNew(sockData->sock->getSocket(), TpSocketNotifierNew::Read, [this]() { handleReadyRead(); });
	return 0;
}

tpInt32 TpUdpSocket::bind(tpUInt16 port)
{
	TpUdpSocketData *sockData=static_cast<TpUdpSocketData *>(data_);
	return bind(TpString("127.0.0.1"),port);
}

tpInt64 TpUdpSocket::sendTo(const tpUInt8 *data, tpUInt64 size, const TpString &addr, tpUInt16 port)
{
	TpUdpSocketData *sockData=static_cast<TpUdpSocketData *>(data_);
	return sockData->sock->sendTo(data,size,addr,port);
}

tpInt64 TpUdpSocket::sendTo(const TpNetworkDatagram &datagram) {
    return sendTo(datagram.data(), datagram.size(),
                         datagram.destinationAddress(), datagram.destinationPort());
}

tpInt64 TpUdpSocket::recvFrom(tpUInt8 *data, tpUInt64 size, TpString &addr, tpUInt16 *port)
{
	TpUdpSocketData *sockData=static_cast<TpUdpSocketData *>(data_);
//	tpUInt32 len=sock->recvFrom(data,size,addr,port);
//	TpNetworkDatagram datap(data,len,addr,port);
	return sockData->sock->recvFrom(data,size,addr,port);
}

TpNetworkDatagram TpUdpSocket::recvDatagram(tpUInt64 size)
{
	TpUdpSocketData *sockData=static_cast<TpUdpSocketData *>(data_);
	
	if (!sockData->hasData) 
		return nullptr;
	sockData->hasData = TP_FALSE;

	std::vector<tpUInt8> buffer(size);
	TpString addr;
    tpUInt16 port;

	int len=sockData->sock->recvFrom(buffer.data(),size,addr,&port);

	const tpUInt8 *data=buffer.data();
	TpNetworkDatagram datagram(data, len, sockData->sock->getLocalAddress(),  sockData->sock->getLocalPort());
	datagram.setSender(addr, port);
	return datagram;
}

tpBool TpUdpSocket::hasPendingDatagrams()
{
	TpUdpSocketData *sockData=static_cast<TpUdpSocketData *>(data_);
	
	return sockData->hasData;
}

void TpUdpSocket::handleReadyRead() 
{
	TpUdpSocketData *sockData=static_cast<TpUdpSocketData *>(data_);
    sockData->hasData = TP_TRUE;
	readyRead.emit();
}
