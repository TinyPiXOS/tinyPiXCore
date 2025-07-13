/*///------------------------------------------------------------------------------------------------------------------------//
		Socket（TCP和UDP基础的网络连接通信）
说 明 :
日 期 : 2024.12.25

/*///------------------------------------------------------------------------------------------------------------------------//

#include <iostream>
#include <cstring>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>             //地址转换
#include "tpSocket.h"


struct tpSocketData
{
	tpSockfd sockfd;
	tpString addr_r;	//远程ip地址
	tpUInt16 port_r;	//远程端口

	tpString addr_d;	//本地ip地址
	tpUInt16 port_d;	//本地端口
	tpSocketData()
	{
		sockfd=-1;
	}
};


tpSocket::tpSocket()
{
	data_=new tpSocketData();
}

tpSocket::tpSocket(tpSockfd sockfd, tpString addr_r, tpUInt16 port_r)
{
	data_=new tpSocketData();
	tpSocketData *sock=static_cast<tpSocketData *>(data_);
	sock->sockfd=sockfd;
	sock->addr_r=addr_r;
	sock->port_r=port_r;
}

tpSocket::~tpSocket()
{
	tpSocketData *sock=static_cast<tpSocketData *>(data_);
	close();
	sock->sockfd=-1;
}

tpSocket &tpSocket::operator=(const tpSocket &other)
{
	tpSocketData *sock=static_cast<tpSocketData *>(data_);
	tpSocketData *sock_other=static_cast<tpSocketData *>(other.data_);
	sock->addr_r=sock_other->addr_r;
	sock->port_r=sock_other->port_r;
	sock->sockfd=sock_other->sockfd;
	//添加其他需要深拷贝的内容
	return *this;
}

tpSockfd tpSocket::socket(tpSocketType type)
{
	tpSocketData *sock=static_cast<tpSocketData *>(data_);

	int sock_type=-1;
	if(sock->sockfd>=0)
		close();

	switch(type)
	{
		case TP_SOCK_STREAM:
			sock_type=SOCK_STREAM;
			break;
		case TP_SOCK_DGRAM:
			sock_type=SOCK_DGRAM;
			break;
		default:
			return -1;	
			break;
	}
	if ((sock->sockfd = ::socket(AF_INET, sock_type, 0)) < 0) 
	{
		std::cerr << "socket creation failed\n";
		return -1;
	}
	return sock->sockfd;
}

tpInt32 tpSocket::bind(const tpString &addr,tpUInt16 port)
{
	tpSocketData *sock=static_cast<tpSocketData *>(data_);
	int ret=0;
	struct sockaddr_in servaddr;
	char *c_addr = new char[addr.length() + 1];
	std::strcpy(c_addr, addr.c_str());

	memset(&servaddr, 0, sizeof(servaddr));
	servaddr.sin_family = AF_INET; 
//	INADDR_ANY
	inet_pton(AF_INET,c_addr,&servaddr.sin_addr);	//INADDR_ANY;
	servaddr.sin_port = htons(port);

	if ((ret=::bind(sock->sockfd, (const struct sockaddr *)&servaddr, sizeof(servaddr))) < 0) 
	{
		std::cerr << "bind failed\n";
		close();
	}
	sock->addr_d=addr;
	sock->port_d=port;
	free(c_addr);
	return ret;
}

tpInt32 tpSocket::close()
{
	tpSocketData *sock=static_cast<tpSocketData *>(data_);
	int sockfd=sock->sockfd;
	if(sockfd<0)
	{
		std::cerr<<"连接不存在\n";
		return -1;
	}
	return ::close((int)sockfd);	
}
tpInt32 tpSocket::close(tpSockfd sockfd)
{
	if(sockfd<0)
	{
		std::cerr<<"连接不存在\n";
		return -1;
	}
	return ::close((int)sockfd);	
}

tpInt64 tpSocket::sendTo(const tpUInt8 *data,tpUInt64 size,const tpString &addr,tpUInt16 port)
{
	tpSocketData *sock=static_cast<tpSocketData *>(data_);
	if(sock->sockfd<0)
		return -1;
	char *c_addr = new char[addr.length() + 1];
	std::strcpy(c_addr, addr.c_str());

	int ret=0;
	struct sockaddr_in sock_addr;
	int len=sizeof(sock_addr);
	memset(&sock_addr, 0, len);
	sock_addr.sin_family = AF_INET; 
//	sock_addr.sin_addr.s_addr = inet_pton(addr);
	inet_pton(AF_INET,c_addr,&sock_addr.sin_addr);
	sock_addr.sin_port = htons(port);

	ret=sendto(sock->sockfd,(void *)data, size, MSG_CONFIRM, (const struct sockaddr *) &sock_addr, len);
	if(ret<0)
	{
		std::cerr << "UDP Send failed\n";
	}
	free(c_addr);
	return ret;
}

tpInt64 tpSocket::recvFrom(tpUInt8 *data,tpUInt64 size, tpString &addr,tpUInt16 *port)
{
	tpSocketData *sock=static_cast<tpSocketData *>(data_);
	char c_addr[64];
	int length=0;
	struct sockaddr_in from_addr;
//	char *addr_r=malloc(40);
	socklen_t addr_len=sizeof(struct sockaddr_in);
	if((length=recvfrom(sock->sockfd,data,size,0,(struct sockaddr*)(&from_addr),&addr_len))<0)
	{
		//std::cerr << "UDP Receive failed\n";
		return -1;
	}
	inet_ntop(AF_INET,(const void *)&from_addr.sin_addr,c_addr,INET_ADDRSTRLEN);  //网络用二进制转换为普通十进制(支持ipv6)
	*port=ntohs(from_addr.sin_port);
	addr=tpString(c_addr);

	printf("recv ip:%s  port:%d\n",addr.c_str(),*port);
//	free(addr_r);
	return length;
}

tpSocket *tpSocket::connectToHost(const tpString &addr ,tpUInt16 port)
{
	tpSocketData *sock=static_cast<tpSocketData *>(data_);
	char *c_addr = new char[addr.length() + 1];
	std::strcpy(c_addr, addr.c_str());
	int ret=-1;
	struct sockaddr_in servaddr;
	struct in_addr _addr;
	inet_pton(AF_INET,c_addr,(void*)&_addr);
	servaddr.sin_family=AF_INET;
	servaddr.sin_addr=_addr;
	servaddr.sin_port=htons(port);
	std::cerr << "connect to "<< addr<<":"<<port<<"\n";
	if((ret=::connect(sock->sockfd,(struct sockaddr*)(&servaddr),sizeof(struct sockaddr)))<0)
	{
		std::cerr << "connect to "<< addr<<":"<<port<<"error\n";
		return nullptr;
	}
	else
	{
		printf("connect ok %d\n",ret);
	}
	return this;
}

//监听(默认以非阻塞的模式接受连接)
tpInt32 tpSocket::listen(tpInt32 size)
{
	tpSocketData *sock=static_cast<tpSocketData *>(data_);

	tpInt32 flags = fcntl(sock->sockfd, F_GETFL, 0);
	if (flags == -1) {
		std::cerr << "fcntl F_GETFL failed\n";
		return -1;
	}

	if (fcntl(sock->sockfd, F_SETFL, flags | O_NONBLOCK) == -1) {
		std::cerr << "fcntl F_SETFL failed\n";
		return -1;
	}

	::listen(sock->sockfd,size);
	return 0;
}

tpInt64 tpSocket::send(const tpUInt8 *data,tpUInt64 size)
{
	tpSocketData *sock=static_cast<tpSocketData *>(data_);
	int length=0;
	if(sock->sockfd<0)
		return -1;
	printf("send to  %d\n",sock->sockfd);
	if((length=::send(sock->sockfd,data,size,0))<0)
	{
		std::cerr << "send data error\n";
	}
	return length;
}

tpInt64 tpSocket::recv(tpUInt8 *data,tpUInt64 size,tpInt32 flag)
{
	tpSocketData *sock=static_cast<tpSocketData *>(data_);
	int length;
	if(sock->sockfd<0)
		return -1;
	if((length=::recv(sock->sockfd,data,size,flag))<0)
	{
		std::cerr << "recv data error\n";
	}
	return length;
}

tpSockfd tpSocket::getSocket()
{
	tpSocketData *sock=static_cast<tpSocketData *>(data_);
	return sock->sockfd;
}

tpString tpSocket::getLocalAddress()
{
	tpSocketData *sock=static_cast<tpSocketData *>(data_);
	return sock->addr_d;
}

tpUInt16 tpSocket::getLocalPort()
{
	tpSocketData *sock=static_cast<tpSocketData *>(data_);
	return sock->port_d;
}

tpString tpSocket::getPeerAddress()
{
	tpSocketData *sock=static_cast<tpSocketData *>(data_);
	return sock->addr_r;
}

tpUInt16 tpSocket::getPeerPort()
{
	tpSocketData *sock=static_cast<tpSocketData *>(data_);
	return sock->port_r;
}