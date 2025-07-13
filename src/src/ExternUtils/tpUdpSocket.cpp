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
#include "tpUdpSocket.h"
#include "tpSocket.h"
#include "tpNetworkDatagram.h"
#include "tpSocketNotifier.h"

struct tpUdpSocketData{
	tpSocket *sock;
	tpSocketNotifier *notifier;
	mutable tpBool hasData;
	tpUdpSocketData(){
		sock=nullptr;
		notifier=nullptr;
		hasData=TP_FALSE;
	}
};


tpUdpSocket::tpUdpSocket()
{
	data_=new tpUdpSocketData();
	tpUdpSocketData *sockData=static_cast<tpUdpSocketData *>(data_);
	sockData->sock=new tpSocket();
	sockData->sock->socket(tpSocket::TP_SOCK_DGRAM);
}

tpUdpSocket::~tpUdpSocket()
{
	tpUdpSocketData *sockData=static_cast<tpUdpSocketData *>(data_);
	if(!sockData)
		return ;
	if(sockData->sock)
		delete(sockData->sock);
	if(sockData->notifier)
		delete(sockData->notifier);
	delete(sockData);
}

tpInt32 tpUdpSocket::bind(const tpString &addr, tpUInt16 port)
{
	tpUdpSocketData *sockData=static_cast<tpUdpSocketData *>(data_);
	tpInt32 err=sockData->sock->bind(addr,port);
	if(err<0)
		return err;

	sockData->notifier = new tpSocketNotifier(sockData->sock->getSocket(), tpSocketNotifier::Read, [this]() { handleReadyRead(); });
	return 0;
}

tpInt32 tpUdpSocket::bind(tpUInt16 port)
{
	tpUdpSocketData *sockData=static_cast<tpUdpSocketData *>(data_);
	return bind(tpString("127.0.0.1"),port);
}

tpInt64 tpUdpSocket::sendTo(const tpUInt8 *data, tpUInt64 size, const tpString &addr, tpUInt16 port)
{
	tpUdpSocketData *sockData=static_cast<tpUdpSocketData *>(data_);
	return sockData->sock->sendTo(data,size,addr,port);
}

tpInt64 tpUdpSocket::sendTo(const tpNetworkDatagram &datagram) {
    return sendTo(datagram.data(), datagram.size(),
                         datagram.destinationAddress(), datagram.destinationPort());
}

tpInt64 tpUdpSocket::recvFrom(tpUInt8 *data, tpUInt64 size, tpString &addr, tpUInt16 *port)
{
	tpUdpSocketData *sockData=static_cast<tpUdpSocketData *>(data_);
//	tpUInt32 len=sock->recvFrom(data,size,addr,port);
//	tpNetworkDatagram datap(data,len,addr,port);
	return sockData->sock->recvFrom(data,size,addr,port);
}

tpNetworkDatagram tpUdpSocket::recvDatagram(tpUInt64 size)
{
	tpUdpSocketData *sockData=static_cast<tpUdpSocketData *>(data_);
	
	if (!sockData->hasData) 
		return nullptr;
	sockData->hasData = TP_FALSE;

	std::vector<tpUInt8> buffer(size);
	tpString addr;
    tpUInt16 port;

	int len=sockData->sock->recvFrom(buffer.data(),size,addr,&port);

	const tpUInt8 *data=buffer.data();
	tpNetworkDatagram datagram(data, len, sockData->sock->getLocalAddress(),  sockData->sock->getLocalPort());
	datagram.setSender(addr, port);
	return datagram;
}

tpBool tpUdpSocket::hasPendingDatagrams()
{
	tpUdpSocketData *sockData=static_cast<tpUdpSocketData *>(data_);
	
	return sockData->hasData;
}

void tpUdpSocket::handleReadyRead() 
{
	tpUdpSocketData *sockData=static_cast<tpUdpSocketData *>(data_);
    sockData->hasData = TP_TRUE;
	readyRead.emit();
}
