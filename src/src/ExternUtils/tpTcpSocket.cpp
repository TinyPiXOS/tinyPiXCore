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
	{
		tcp->sock=sock;
		tcp->status=tpSocket::TP_SOCK_CONNECT;
	}

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
	printf("析构\n");
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
	ret = tcp->sock->connectToHost(addr,port,TP_FALSE);		//非阻塞模式
	if(ret!=nullptr)
	{
		//tcp->sock_r=*ret;
		printf("立即连接成功\n");
		tcp->status=tpSocket::TP_SOCK_CONNECT;
		connected.emit();
		return 0;
	}
	tcp->notifier_write = new tpSocketNotifier(
			tcp->sock->getSocket(), tpSocketNotifier::Write,
			[this](){ handleWrite(); }
		);
	return 0;
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
	{
		return -1;
	}
	tpInt32 ret = tcp->sock->send(buff,size);
	if(ret==0)
	{
		tcp->status=tpSocket::TP_SOCK_DISCONNECT;
	}
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
//    printf("debug:有可读事件，但不读出数据，只发信号\n");
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
	disconnected.emit(this);
}

void tpTcpSocket::handleWrite() {
    tpTcpSocketData *tcp = static_cast<tpTcpSocketData*>(data_);
	printf("handlewrite触发\n");
    // 检查连接结果
    int err = 0;
    socklen_t len = sizeof(err);
    getsockopt(tcp->sock->getSocket(), SOL_SOCKET, SO_ERROR, &err, &len);

    if (err == 0) 
	{
		// 停掉写事件监听
		if (tcp->notifier_write) {
			delete tcp->notifier_write;
			tcp->notifier_write = nullptr;
		}
        tcp->status = tpSocket::TP_SOCK_CONNECT;
        connected.emit();
    }
	else
	{
		printf("debug 未知错误\n");
	}
    // 之后就继续依赖 notifier_read 处理数据和断开
}

void tpTcpSocket::handleConnectError() {
	 tpTcpSocketData *tcp = static_cast<tpTcpSocketData*>(data_);
        		// 走到这里说明 connect 被拒绝或有底层错误
				int err = 0;
				socklen_t len = sizeof(err);
				getsockopt(tcp->sock->getSocket(), SOL_SOCKET, SO_ERROR, &err, &len);
//printf("error 未知错误\n");
}