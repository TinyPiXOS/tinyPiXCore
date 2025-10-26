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
#include "TpSocket.h"


struct TpSocketData
{
	TpSockfd sockfd;
	TpString addr_r;	//远程ip地址
	tpUInt16 port_r;	//远程端口

	TpString addr_d;	//本地ip地址
	tpUInt16 port_d;	//本地端口
	TpSocketData()
	{
		sockfd=-1;
	}
};


TpSocket::TpSocket()
{
	data_=new TpSocketData();
}

TpSocket::TpSocket(TpSockfd sockfd, TpString addr_r, tpUInt16 port_r)
{
	data_=new TpSocketData();
	TpSocketData *sock=static_cast<TpSocketData *>(data_);
	sock->sockfd=sockfd;
	sock->addr_r=addr_r;
	sock->port_r=port_r;
}

TpSocket::~TpSocket()
{
	TpSocketData *sock=static_cast<TpSocketData *>(data_);
	close();
	sock->sockfd=-1;
}

TpSocket &TpSocket::operator=(const TpSocket &other)
{
	TpSocketData *sock=static_cast<TpSocketData *>(data_);
	TpSocketData *sock_other=static_cast<TpSocketData *>(other.data_);
	sock->addr_r=sock_other->addr_r;
	sock->port_r=sock_other->port_r;
	sock->sockfd=sock_other->sockfd;
	//添加其他需要深拷贝的内容
	return *this;
}

TpSockfd TpSocket::socket(TpSocketType type)
{
	TpSocketData *sock=static_cast<TpSocketData *>(data_);

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

tpInt32 TpSocket::bind(const TpString &addr,tpUInt16 port)
{
	TpSocketData *sock=static_cast<TpSocketData *>(data_);
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

tpInt32 TpSocket::close()
{
	TpSocketData *sock=static_cast<TpSocketData *>(data_);
	int sockfd=sock->sockfd;
	if(sockfd<0)
	{
		std::cerr<<"连接不存在\n";
		return -1;
	}
	return ::close((int)sockfd);	
}
tpInt32 TpSocket::close(TpSockfd sockfd)
{
	if(sockfd<0)
	{
		std::cerr<<"连接不存在\n";
		return -1;
	}
	return ::close((int)sockfd);	
}

tpInt64 TpSocket::sendTo(const tpUInt8 *data,tpUInt64 size,const TpString &addr,tpUInt16 port)
{
	TpSocketData *sock=static_cast<TpSocketData *>(data_);
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

tpInt64 TpSocket::recvFrom(tpUInt8 *data,tpUInt64 size, TpString &addr,tpUInt16 *port)
{
	TpSocketData *sock=static_cast<TpSocketData *>(data_);
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
	addr=TpString(c_addr);

	printf("recv ip:%s  port:%d\n",addr.c_str(),*port);
//	free(addr_r);
	return length;
}

TpSocket *TpSocket::connectToHost(const TpString &addr ,tpUInt16 port, tpBool block)
{
	TpSocketData *sock=static_cast<TpSocketData *>(data_);
	if(!block)
	{
		int flags = fcntl(sock->sockfd, F_GETFL, 0);
		fcntl(sock->sockfd, F_SETFL, flags | O_NONBLOCK);
	}
	char *c_addr = new char[addr.length() + 1];
	std::strcpy(c_addr, addr.c_str());
	int ret=-1;
	struct sockaddr_in servaddr;
	struct in_addr _addr;
	inet_pton(AF_INET,c_addr,(void*)&_addr);
	servaddr.sin_family=AF_INET;
	servaddr.sin_addr=_addr;
	servaddr.sin_port=htons(port);
	if((ret=::connect(sock->sockfd,(struct sockaddr*)(&servaddr),sizeof(struct sockaddr)))==0)
	{
		return this;
	}
	if (!block && errno == EINPROGRESS) {
        return this;
    }
	
	return nullptr;
}

//监听(默认以非阻塞的模式接受连接)
tpInt32 TpSocket::listen(tpInt32 size)
{
	TpSocketData *sock=static_cast<TpSocketData *>(data_);

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

tpInt64 TpSocket::send(const tpUInt8 *data,tpUInt64 size)
{
	TpSocketData *sock=static_cast<TpSocketData *>(data_);
	int length=0;
	if(sock->sockfd<0)
	
		return -1;
	if((length=::send(sock->sockfd,data,size,0))<0)
	{
		;
	}
	return length;
}

tpInt64 TpSocket::recv(tpUInt8 *data,tpUInt64 size,tpInt32 flag)
{
	TpSocketData *sock=static_cast<TpSocketData *>(data_);
	int length;
	if(sock->sockfd<0)
		return -1;
	if((length=::recv(sock->sockfd,data,size,flag))<0)
	{
		std::cerr << "recv data error\n";
	}
	return length;
}

TpSockfd TpSocket::getSocket()
{
	TpSocketData *sock=static_cast<TpSocketData *>(data_);
	return sock->sockfd;
}

TpString TpSocket::getLocalAddress()
{
	TpSocketData *sock=static_cast<TpSocketData *>(data_);
	return sock->addr_d;
}

tpUInt16 TpSocket::getLocalPort()
{
	TpSocketData *sock=static_cast<TpSocketData *>(data_);
	return sock->port_d;
}

TpString TpSocket::getPeerAddress()
{
	TpSocketData *sock=static_cast<TpSocketData *>(data_);
	return sock->addr_r;
}

tpUInt16 TpSocket::getPeerPort()
{
	TpSocketData *sock=static_cast<TpSocketData *>(data_);
	return sock->port_r;
}


/*tpBool TpSocket::waitForBytesWritten(int msecs = 10000)
{
	TpSocketData *sock=static_cast<TpSocketData *>(data_);

	struct pollfd pfd;
    pfd.fd = sock->sockfd;
    pfd.events = POLLOUT | POLLERR | POLLHUP;
    pfd.revents = 0;
    
    // 转换超时时间为毫秒
    int timeout = msecs;
    
    // 调用 poll
    int ret = poll(&pfd, 1, timeout);
    
    if (ret < 0) {
        // 错误
        return -1;
    } else if (ret == 0) {
        // 超时
        return 0;
    }
    
    // 检查事件
    if (pfd.revents & (POLLERR | POLLHUP | POLLNVAL)) {
        // 连接错误
        return -1;
    }
    
    return (pfd.revents & POLLOUT) ? 1 : 0;
}

tpBool TpSocket::waitForReadyRead(int msecs = 10000)
{
	TpSocketData *sock=static_cast<TpSocketData *>(data_);
	if (socket_fd_ < 0) return false;
        
        // 检查是否已有数据可读
        if (hasPendingData()) {
            return true;
        }
        
        // 使用 poll 等待数据可读
        struct pollfd pfd;
        pfd.fd = socket_fd_;
        pfd.events = POLLIN | POLLERR | POLLHUP;
        pfd.revents = 0;
        
        int ret = poll(&pfd, 1, msecs);
        if (ret < 0) {
            // 错误
            return false;
        } else if (ret == 0) {
            // 超时
            return false;
        }
        
        // 检查事件
        if (pfd.revents & (POLLERR | POLLHUP | POLLNVAL)) {
            // 连接错误
            return false;
        }
        
        return (pfd.revents & POLLIN) != 0;
}*/
