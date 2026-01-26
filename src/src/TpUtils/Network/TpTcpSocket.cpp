/*///------------------------------------------------------------------------------------------------------------------------//
		TCP客户端
说 明 :
日 期 : 2024.12.24

/*///------------------------------------------------------------------------------------------------------------------------//

#include <sys/types.h>
#include <sys/socket.h>  // <-- 这里定义了 MSG_PEEK
#include "TpTcpSocket.h"
#include "TpSocketNotifier.h"
#include "TpSocketNotifierNew.h"
#include "TpSocket.h"

struct TpTcpSocketData{
	TpSocket *sock;		//本地的sock
//	TpSocket sock_r;	//远程的sock连接
	TpSocket::TpSocketStatus status;	//当前的socket状态
	TpSocketNotifierNew *notifier_read;
	TpSocketNotifierNew *notifier_write;
	TpTcpSocketData()
	{
		sock=nullptr;
		status=TpSocket::TP_SOCK_DISCONNECT;
		notifier_read=nullptr;
		notifier_write=nullptr;
	}
};


TpTcpSocket::TpTcpSocket(TpSocket *sock)
{
	printf("TpTcpSocket 构造\n");
	data_=new TpTcpSocketData();
	TpTcpSocketData *tcp=static_cast<TpTcpSocketData *>(data_);

	if(!sock)
	{
		tcp->sock=new TpSocket();
		if(!tcp->sock)
			return ;
		tcp->sock->socket(TpSocket::TP_SOCK_STREAM);
	}
	else
	{
		tcp->sock=sock;
		tcp->status=TpSocket::TP_SOCK_CONNECT;
	}

	tcp->notifier_read = new TpSocketNotifierNew(tcp->sock->getSocket(), TpSocketNotifierNew::Read, 
		[this]() { handleRead(); },
		[this]() { handleDisconnected(); }
	);
}

TpTcpSocket::~TpTcpSocket()
{
	TpTcpSocketData *tcp=static_cast<TpTcpSocketData *>(data_);
	if(!tcp)
		return ;
	if(tcp->status==TpSocket::TP_SOCK_CONNECT)
		close();
	if(tcp->notifier_read) {
        tcp->notifier_read->deleteLater();
        tcp->notifier_read = nullptr;
    }

    if(tcp->notifier_write) {
        tcp->notifier_write->deleteLater();
        tcp->notifier_write = nullptr;
    }

	if(tcp->sock)
	{
		delete(tcp->sock);
		tcp->sock=nullptr;
	}
	delete(tcp);
	tcp=nullptr;
	printf("TpTcpSocket 析构\n");
}

tpInt32 TpTcpSocket::bind(const TpString &addr, tpUInt16 port)
{
	TpTcpSocketData *tcp=static_cast<TpTcpSocketData *>(data_);
	return tcp->sock->bind(addr,port);
}

tpInt32 TpTcpSocket::bind(tpUInt16 port)
{
	TpTcpSocketData *tcp=static_cast<TpTcpSocketData *>(data_);
	return tcp->sock->bind("127.0.0.1",port);
}

tpInt32 TpTcpSocket::connectToHost(const TpString &addr, tpUInt16 port)
{
	TpTcpSocketData *tcp=static_cast<TpTcpSocketData *>(data_);
	TpSocket *ret=nullptr;
	ret = tcp->sock->connectToHost(addr,port,TP_FALSE);		//非阻塞模式
	if(ret!=nullptr)
	{
		//tcp->sock_r=*ret;
		printf("立即连接成功\n");
		tcp->status=TpSocket::TP_SOCK_CONNECT;
		connected.emit();
		return 0;
	}
	tcp->notifier_write = new TpSocketNotifierNew(
			tcp->sock->getSocket(), TpSocketNotifierNew::Write,
			[this](){ handleWrite(); }
		);
	return 0;
}

tpInt32 TpTcpSocket::close()
{
	TpTcpSocketData *tcp=static_cast<TpTcpSocketData *>(data_);

	if (tcp->notifier_read) {
        delete tcp->notifier_read; 
		tcp->notifier_read = nullptr;
    }
	tcp->sock->close();
	tcp->status=TpSocket::TP_SOCK_DISCONNECT;
	disconnected.emit(this);		//发送断开连接的信号
	return 0;
}

tpInt32 TpTcpSocket::close(TpSockfd sock)
{
	if(sock)
		::close(sock);
	return 0;
}

tpInt64 TpTcpSocket::send(const tpUInt8 *buff, tpUInt64 size)
{
	TpTcpSocketData *tcp=static_cast<TpTcpSocketData *>(data_);
	if(tcp->status!=TpSocket::TP_SOCK_CONNECT)
	{
		return -1;
	}
	tpInt32 ret = tcp->sock->send(buff,size);
	if(ret==0)
	{
		tcp->status=TpSocket::TP_SOCK_DISCONNECT;
	}
	return ret;
}

tpInt64 TpTcpSocket::recv(tpUInt8 *buff, tpUInt64 size)
{
	TpTcpSocketData *tcp=static_cast<TpTcpSocketData *>(data_);
	if(tcp->status!=TpSocket::TP_SOCK_CONNECT)
		return -1;
	tpInt32 ret = tcp->sock->recv(buff,size);
	if(ret==0)
		tcp->status=TpSocket::TP_SOCK_DISCONNECT;
	return ret;
}

TpString TpTcpSocket::getPeerAddress()
{
	TpTcpSocketData *tcp=static_cast<TpTcpSocketData *>(data_);
	return tcp->sock->getPeerAddress();
}
tpUInt16 TpTcpSocket::getPeerPort()
{
	TpTcpSocketData *tcp=static_cast<TpTcpSocketData *>(data_);
	return tcp->sock->getPeerPort();
}

void TpTcpSocket::handleRead() 
{
//	if(checkDisconnected())
//		return ;
//    printf("debug:有可读事件，但不读出数据，只发信号\n");
    readyRead.emit(this);	
}

tpBool TpTcpSocket::checkDisconnected() 
{
	TpTcpSocketData *tcp = static_cast<TpTcpSocketData *>(data_);
	char c;
	int ret = tcp->sock->recv((tpUInt8*)&c, 1, MSG_PEEK);
	if (ret == 0) {
		tcp->status = TpSocket::TP_SOCK_DISCONNECT;
		disconnected.emit(this);
		return TP_TRUE;
	}
	return TP_FALSE;
}


void TpTcpSocket::handleDisconnected() 
{	
	TpTcpSocketData* tcp = static_cast<TpTcpSocketData*>(data_);
    tcp->status = TpSocket::TP_SOCK_DISCONNECT;
	disconnected.emit(this);
}

void TpTcpSocket::handleWrite() {
    TpTcpSocketData *tcp = static_cast<TpTcpSocketData*>(data_);
	printf("handlewrite触发\n");
    // 检查连接结果
    int err = 0;
    socklen_t len = sizeof(err);
    getsockopt(tcp->sock->getSocket(), SOL_SOCKET, SO_ERROR, &err, &len);

    if (err == 0) 
	{
		// 停掉写事件监听
		if (tcp->notifier_write) {
			//delete tcp->notifier_write;
			tcp->notifier_write->deleteLater();	//延迟删除
			tcp->notifier_write = nullptr;
		}
        tcp->status = TpSocket::TP_SOCK_CONNECT;
        connected.emit();
    }
	else
	{
		printf("debug 未知错误\n");
	}
    // 之后就继续依赖 notifier_read 处理数据和断开
}

void TpTcpSocket::handleConnectError() {
	 TpTcpSocketData *tcp = static_cast<TpTcpSocketData*>(data_);
        		// 走到这里说明 connect 被拒绝或有底层错误
				int err = 0;
				socklen_t len = sizeof(err);
				getsockopt(tcp->sock->getSocket(), SOL_SOCKET, SO_ERROR, &err, &len);
//printf("error 未知错误\n");
}