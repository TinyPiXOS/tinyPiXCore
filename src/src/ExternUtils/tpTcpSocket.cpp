/*///------------------------------------------------------------------------------------------------------------------------//
		TCP客户端
说 明 :
日 期 : 2024.12.24

/*///------------------------------------------------------------------------------------------------------------------------//

#include <sys/types.h>
#include <sys/socket.h>  // <-- 这里定义了 MSG_PEEK
#include "tpTcpSocket.h"
#include "tpSocketNotifier.h"

struct tpTcpSocketData{
	tpSocket *sock;		//本地的sock
//	tpSocket sock_r;	//远程的sock连接
	tpSocket::tpSocketStatus status;	//当前的socket状态
	tpSocketNotifier *notifier_read;
	tpSocketNotifier *notifier_write;
	tpTcpSocketData()
	{
		sock=nullptr;
		status=tpSocket::TP_SOCK_DISCONNECT;
		notifier_read=nullptr;
		notifier_write=nullptr;
	}
};


tpTcpSocket::tpTcpSocket(tpSocket *sock)
{
	data_=new tpTcpSocketData();
	tpTcpSocketData *tcp=static_cast<tpTcpSocketData *>(data_);

	if(!sock)
	{
		tcp->sock=new tpSocket();
		if(!tcp->sock)
			return ;
		tcp->sock->socket(tpSocket::TP_SOCK_STREAM);
	}
	else
		tcp->sock=sock;

	tcp->status=tpSocket::TP_SOCK_CONNECT;

	tcp->notifier_read = new tpSocketNotifier(tcp->sock->getSocket(), tpSocketNotifier::Read, 
		[this]() { handleRead(); },
		[this]() { handleDisconnected(); }
	);
}

tpTcpSocket::~tpTcpSocket()
{
	tpTcpSocketData *tcp=static_cast<tpTcpSocketData *>(data_);
	if(!tcp)
		return ;
	if(tcp->status==tpSocket::TP_SOCK_CONNECT)
		close();
	if(tcp->sock)
	{
		delete(tcp->sock);
		tcp->sock=nullptr;
	}
	delete(tcp);
	tcp=nullptr;
}

tpInt32 tpTcpSocket::bind(const tpString &addr, tpUInt16 port)
{
	tpTcpSocketData *tcp=static_cast<tpTcpSocketData *>(data_);
	return tcp->sock->bind(addr,port);
}

tpInt32 tpTcpSocket::bind(tpUInt16 port)
{
	tpTcpSocketData *tcp=static_cast<tpTcpSocketData *>(data_);
	return tcp->sock->bind("127.0.0.1",port);
}

tpInt32 tpTcpSocket::connectToHost(const tpString &addr, tpUInt16 port)
{
	tpTcpSocketData *tcp=static_cast<tpTcpSocketData *>(data_);
	tpSocket *ret=nullptr;
	ret = tcp->sock->connectToHost(addr,port,TP_FALSE);	

	if(ret!=nullptr)
	{
		//tcp->sock_r=*ret;
		tcp->status=tpSocket::TP_SOCK_CONNECT;
		return 0;
	}
	return -1;
}

tpInt32 tpTcpSocket::close()
{
	tpTcpSocketData *tcp=static_cast<tpTcpSocketData *>(data_);
	if (tcp->notifier_read) {
        delete tcp->notifier_read; 
		tcp->notifier_read = nullptr;
    }
	tcp->sock->close();
	tcp->status=tpSocket::TP_SOCK_DISCONNECT;
	disconnected.emit(this);		//发送断开连接的信号
	return 0;
}

tpInt32 tpTcpSocket::close(tpSockfd sock)
{
	if(sock)
		::close(sock);
	return 0;
}

tpInt64 tpTcpSocket::send(const tpUInt8 *buff, tpUInt64 size)
{
	tpTcpSocketData *tcp=static_cast<tpTcpSocketData *>(data_);
	if(tcp->status!=tpSocket::TP_SOCK_CONNECT)
		return -1;
	tpInt32 ret = tcp->sock->send(buff,size);
	if(ret==0)
	{
		tcp->status=tpSocket::TP_SOCK_DISCONNECT;
	}
	printf("send return %d\n",ret);
	return ret;
}

tpInt64 tpTcpSocket::recv(tpUInt8 *buff, tpUInt64 size)
{
	tpTcpSocketData *tcp=static_cast<tpTcpSocketData *>(data_);
	if(tcp->status!=tpSocket::TP_SOCK_CONNECT)
		return -1;
	tpInt32 ret = tcp->sock->recv(buff,size);
	if(ret==0)
		tcp->status=tpSocket::TP_SOCK_DISCONNECT;
	return ret;
}

tpString tpTcpSocket::getPeerAddress()
{
	tpTcpSocketData *tcp=static_cast<tpTcpSocketData *>(data_);
	return tcp->sock->getPeerAddress();
}
tpUInt16 tpTcpSocket::getPeerPort()
{
	tpTcpSocketData *tcp=static_cast<tpTcpSocketData *>(data_);
	return tcp->sock->getPeerPort();
}

void tpTcpSocket::handleRead() 
{
//	if(checkDisconnected())
//		return ;
    printf("debug:有可读事件，但不读出数据，只发信号\n");
    readyRead.emit(this);	
}

tpBool tpTcpSocket::checkDisconnected() 
{
	tpTcpSocketData *tcp = static_cast<tpTcpSocketData *>(data_);
	char c;
	int ret = tcp->sock->recv((tpUInt8*)&c, 1, MSG_PEEK);
	if (ret == 0) {
		tcp->status = tpSocket::TP_SOCK_DISCONNECT;
		disconnected.emit(this);
		return TP_TRUE;
	}
	return TP_FALSE;
}


void tpTcpSocket::handleDisconnected() 
{	
	tpTcpSocketData* tcp = static_cast<tpTcpSocketData*>(data_);
    tcp->status = tpSocket::TP_SOCK_DISCONNECT;
	printf("断开连接\n");
	disconnected.emit(this);
}