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
#include "TpTcpServer.h"
#include "TpSocketNotifier.h"

#define MAX_CONNECTS	32		//最大允许连接数量

struct TpTcpServerData
{
	TpSocket *sock;						//服务端tcp
	TpList<TpTcpSocket *>tcp_connect;	//tcp连接列表
	TpSocket::TpSocketStatus status;	//当前的socket状态
	tpInt32 connect_max;				//最大允许连接数量
	std::atomic<bool> wait_connect;		//是否有待连接的客户端
	TpSocketNotifier *notifier;
	TpTcpServerData(){
		status=TpSocket::TP_SOCK_DISCONNECT;
		connect_max=MAX_CONNECTS;
		notifier=nullptr;
	}
};

TpTcpServer::TpTcpServer()
{
	data_=new TpTcpServerData();
	TpTcpServerData *tcp=static_cast<TpTcpServerData *>(data_);
	tcp->sock=new TpSocket();
	if((tcp->sock->socket(TpSocket::TP_SOCK_STREAM))<0)
		std::cerr << "socket creat error\n";
}

TpTcpServer::~TpTcpServer()
{
	TpTcpServerData *tcp=static_cast<TpTcpServerData *>(data_);
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

void TpTcpServer::setMaxPendingConnects(tpInt32 max)
{
	TpTcpServerData *tcp=static_cast<TpTcpServerData *>(data_);
	tcp->connect_max=max;
}

tpInt32 TpTcpServer::getMaxPendingConnects()
{
	TpTcpServerData *tcp=static_cast<TpTcpServerData *>(data_);
	return tcp->connect_max;
}

tpInt32 TpTcpServer::close()
{
	TpTcpServerData *tcp=static_cast<TpTcpServerData *>(data_);
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

tpBool TpTcpServer::listen(TpString &addr, tpUInt16 port)
{
	TpTcpServerData *tcp=static_cast<TpTcpServerData *>(data_);
	if(isListening())
		return TP_FALSE;
	if(tcp->sock->bind(addr,port)<0)
		return TP_FALSE;
	tcp->status=TpSocket::TP_SOCK_BIND;

	if(tcp->sock->listen(tcp->connect_max)<0)
		return TP_FALSE;

	tcp->notifier = new TpSocketNotifier(tcp->sock->getSocket(), TpSocketNotifier::Read, 
		[this]() {handleNewConnection();}
	);
	
	tcp->status=TpSocket::TP_SOCK_LISTEN;
	return TP_TRUE;
}

tpBool TpTcpServer::isListening()
{
	TpTcpServerData *tcp=static_cast<TpTcpServerData *>(data_);
	return (tcp->status==TpSocket::TP_SOCK_LISTEN ? TP_TRUE : TP_FALSE);
}

//接受一个新的连接
TpSocket *TpTcpServer::accept()
{
	TpTcpServerData *tcp=static_cast<TpTcpServerData *>(data_);
	char c_addr[64];
	TpSockfd acceptfd;
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
	TpSocket *sock_r=new TpSocket(acceptfd,TpString(c_addr),ntohs(clieaddr.sin_port));

	return sock_r;
}


//接受一个连接
TpTcpSocket *TpTcpServer::acceptConnect()
{
	TpTcpServerData *tcp=static_cast<TpTcpServerData *>(data_);
	TpSocket *sock=accept();
	if(sock==nullptr)
		return nullptr;
	TpTcpSocket *client=new TpTcpSocket(sock);
//	delete(sock);
	tcp->tcp_connect.push_back(client);
	return client;
}



TpTcpSocket* TpTcpServer::nextPendingConnection() 
{
	TpTcpServerData *tcp=static_cast<TpTcpServerData *>(data_);
    if (tcp->tcp_connect.empty()) 
		return nullptr;
    TpTcpSocket* client = tcp->tcp_connect.front();
    tcp->tcp_connect.pop_front();
    return client;
}

// 内部处理新连接
void TpTcpServer::handleNewConnection() 
{
	TpTcpServerData *tcp=static_cast<TpTcpServerData *>(data_);
	// 使用 TpSocket 接受
	TpSocket *tcp_sock = accept();

	if (!tcp_sock) {
		std::cerr << "accept failed" << std::endl;
		return;
	}
	// 创建 TpTcpSocket
	TpTcpSocket* tcp_c = new TpTcpSocket(tcp_sock);
	tcp->tcp_connect.push_front(tcp_c);
	// 调试点1：检查对象地址
    std::cout << "Created TpTcpSocket at: " << tcp_c << std::endl;
    
    // 调试点2：检查信号地址
    std::cout << "disconnected signal address: " 
              << &TpTcpSocket::disconnected << std::endl;
    
    // 调试点3：检查信号成员偏移
    std::cout << "Signal offset: " 
              << reinterpret_cast<char*>(&tcp_c->disconnected) - reinterpret_cast<char*>(tcp_c)
              << std::endl;
    
    // 调试点4：尝试调用成员函数
    std::cout << "Peer address: " << tcp_c->getPeerAddress() << std::endl;
    
    // 将对象添加到列表
    tcp->tcp_connect.push_front(tcp_c);
    
    // 调试点5：在连接信号前暂停
    std::cout << "About to connect signal..." << std::endl;
    std::this_thread::sleep_for(std::chrono::milliseconds(100));
	connect(tcp_c, TpTcpSocket::disconnected, [=](TpTcpSocket *client) {
		printf("[DEBUG] Creating new connection: %p\n", client);
        tcp->tcp_connect.remove(client);
		std::thread([client]() {
           // 等 epoll 事件处理完再删除
            std::this_thread::sleep_for(std::chrono::milliseconds(1));
            delete client;
        }).detach();
    });
	
	newConnection.emit();
}
