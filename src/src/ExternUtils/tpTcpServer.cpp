/*///------------------------------------------------------------------------------------------------------------------------//
		TCP服务端
说 明 :
日 期 : 2024.12.24

/*///------------------------------------------------------------------------------------------------------------------------//

#include <iostream>
#include <thread>
#include <poll.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>             //地址转换
#include "tpTcpServer.h"
#include "tpSocketNotifier.h"

#define MAX_CONNECTS	32		//最大允许连接数量

struct tpTcpServerData
{
	tpSocket *sock;						//服务端tcp
	tpList<tpTcpSocket *>tcp_connect;	//tcp连接列表
	tpSocket::tpSocketStatus status;	//当前的socket状态
	tpInt32 connect_max;				//最大允许连接数量
	std::atomic<bool> wait_connect;		//是否有待连接的客户端
	tpSocketNotifier *notifier;
	tpTcpServerData(){
		status=tpSocket::TP_SOCK_DISCONNECT;
		connect_max=MAX_CONNECTS;
		notifier=nullptr;
	}
};

tpTcpServer::tpTcpServer()
{
	data_=new tpTcpServerData();
	tpTcpServerData *tcp=static_cast<tpTcpServerData *>(data_);
	tcp->sock=new tpSocket();
	if((tcp->sock->socket(tpSocket::TP_SOCK_STREAM))<0)
		std::cerr << "socket creat error\n";
}

tpTcpServer::~tpTcpServer()
{
	tpTcpServerData *tcp=static_cast<tpTcpServerData *>(data_);
	if(!tcp)
		return;

	close();
	if(tcp->sock)
	{
		delete(tcp->sock);
		tcp->sock=nullptr;
	}

	delete(tcp);
}

void tpTcpServer::setMaxPendingConnects(tpInt32 max)
{
	tpTcpServerData *tcp=static_cast<tpTcpServerData *>(data_);
	tcp->connect_max=max;
}

tpInt32 tpTcpServer::close()
{
	tpTcpServerData *tcp=static_cast<tpTcpServerData *>(data_);
	if (tcp->notifier) {
        delete tcp->notifier; 
		tcp->notifier = nullptr;
    }
	for(auto it : tcp->tcp_connect)
	{
		if(it==nullptr)
			continue;
		it->close();
		delete(it);
	}
	tcp->sock->close();
	return 0;
}

tpBool tpTcpServer::listen(tpString &addr, tpUInt16 port)
{
	tpTcpServerData *tcp=static_cast<tpTcpServerData *>(data_);
	if(isListening())
		return TP_FALSE;
	if(tcp->sock->bind(addr,port)<0)
		return TP_FALSE;
	tcp->status=tpSocket::TP_SOCK_BIND;

	if(tcp->sock->listen(tcp->connect_max)<0)
		return TP_FALSE;

	tcp->notifier = new tpSocketNotifier(tcp->sock->getSocket(), tpSocketNotifier::Read, 
		[this]() {handleNewConnection();}
	);
	
	tcp->status=tpSocket::TP_SOCK_LISTEN;
	return TP_TRUE;
}

tpBool tpTcpServer::isListening()
{
	tpTcpServerData *tcp=static_cast<tpTcpServerData *>(data_);
	return (tcp->status==tpSocket::TP_SOCK_LISTEN ? TP_TRUE : TP_FALSE);
}

//接受一个新的连接
tpSocket *tpTcpServer::accept()
{
	tpTcpServerData *tcp=static_cast<tpTcpServerData *>(data_);
	char c_addr[64];
	tpSockfd acceptfd;
	struct sockaddr_in clieaddr;
	int size_clieaddr;
	size_clieaddr=sizeof(struct sockaddr_in);
	if((acceptfd=::accept(tcp->sock->getSocket(),(struct sockaddr*)(&clieaddr), (socklen_t *)&size_clieaddr))<0)
	{
		//std::cerr << "accept error\n";
		return nullptr;
	}
	inet_ntop(AF_INET,(const void *)&clieaddr.sin_addr,c_addr,INET_ADDRSTRLEN);  //网络用二进制转换为普通十进制(支持ipv6)
	printf("accetp new connect %s\n",c_addr);
	tpSocket *sock_r=new tpSocket(acceptfd,tpString(c_addr),ntohs(clieaddr.sin_port));

	return sock_r;
}


//接受一个连接
tpTcpSocket *tpTcpServer::acceptConnect()
{
	tpTcpServerData *tcp=static_cast<tpTcpServerData *>(data_);
	tpSocket *sock=accept();
	if(sock==nullptr)
		return nullptr;
	tpTcpSocket *client=new tpTcpSocket(sock);
//	delete(sock);
	tcp->tcp_connect.push_back(client);
	return client;
}



tpTcpSocket* tpTcpServer::nextPendingConnection() 
{
	tpTcpServerData *tcp=static_cast<tpTcpServerData *>(data_);
    if (tcp->tcp_connect.empty()) 
		return nullptr;
    tpTcpSocket* client = tcp->tcp_connect.front();
    tcp->tcp_connect.pop_front();
    return client;
}

// 内部处理新连接
void tpTcpServer::handleNewConnection() 
{
	tpTcpServerData *tcp=static_cast<tpTcpServerData *>(data_);
	// 使用 tpSocket 接受
	tpSocket *tcp_sock = accept();

	if (!tcp_sock) {
		std::cerr << "accept failed" << std::endl;
		return;
	}
	// 创建 tpTcpSocket
	tpTcpSocket* tcp_c = new tpTcpSocket(tcp_sock);
	tcp->tcp_connect.push_front(tcp_c);

	connect(tcp_c, tpTcpSocket::disconnected, [=](tpTcpSocket *client) {
        tcp->tcp_connect.remove(client);
		std::thread([client]() {
           // 等 epoll 事件处理完再删除
            std::this_thread::sleep_for(std::chrono::milliseconds(1));
            delete client;
        }).detach();
    });
	
	newConnection.emit();
}
