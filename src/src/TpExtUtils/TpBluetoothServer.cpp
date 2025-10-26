

#include <iostream>
#include <stdio.h>
#include <thread>
#include <bluetooth/bluetooth.h>
#include <bluetooth/rfcomm.h>
#include <bluetooth/l2cap.h>
#include "TpSocketNotifier.h"
#include "TpDbusConnectManage.h"
#include "bluetooth/include/bluetooth_inc.h"
#include "TpBluetoothServer.h"

struct TpBluetoothServerData{
	TpString adapter_name;
	tpInt32 max_connect;					//最大连接数量
	tpUInt16 port;							//监听端口
	TpBluetoothAddress address;				//监听地址
	TpBluetoothService::Protocol type;		//连接类型
	int sockfd;								//本地连接文件描述符
	TpSocket::TpSocketStatus status;		//连接状态
	TpSocketNotifier *notifier;				
	Adapter *adapter;						//适配器句柄
	TpList<TpBluetoothSocket *>connects;	//蓝牙连接列表												
	TpBluetoothServerData(){
		status=TpSocket::TP_SOCK_DISCONNECT;
		sockfd=-1;
		adapter=NULL;
		max_connect=32;
	};
};

//type：协议类型
//address：返回绑定到的地址
//channel:绑定到的端口
static int bluet_server_socket(TpBluetoothService::Protocol type,const char *address,uint16_t channel)
{
	int sock = -1;
    struct sockaddr* addr_ptr = nullptr;
    socklen_t addr_len = 0;

    // 根据协议创建不同类型的socket和地址结构

    if (type == TpBluetoothService::TP_BLUET_RFCOMM_PROTOCOL) {
        sock = socket(AF_BLUETOOTH, SOCK_STREAM, BTPROTO_RFCOMM);
        if (sock < 0) 
		{
			fprintf(stderr,"socket(AF_BLUETOOTH, SOCK_STREAM, BTPROTO_RFCOMM) error\n");
			return -1;
		}

		// 设置地址重用
		if (address) {
			int reuse = 1;
			setsockopt(sock, SOL_SOCKET, SO_REUSEADDR, &reuse, sizeof(reuse));
		}

        sockaddr_rc local_addr = {0};
        local_addr.rc_family = AF_BLUETOOTH;
		bdaddr_t any = {0};        // 或者 {{0,0,0,0,0,0}}
		local_addr.rc_bdaddr = any;
		local_addr.rc_channel = channel;
		// 绑定地址
		if (bind(sock, (struct sockaddr*)&local_addr, sizeof(local_addr)) < 0) {
			close(sock);
			fprintf(stderr,"bind error\n");
			return -1;
		}

		// 获取绑定地址，需要完善
		/*sockaddr_rc bound_addr;
		socklen_t len = sizeof(bound_addr);
		if (getsockname(sock, (struct sockaddr*)&bound_addr, &len) == 0) {
			memcpy(address, bound_addr.rc_bdaddr.b, 6);
		}*/
    }
    else if (type == TpBluetoothService::TP_BLUET_L2CAP_PROTOCOL) {
        sock = socket(AF_BLUETOOTH, SOCK_SEQPACKET, BTPROTO_L2CAP);
        if (sock < 0) 
			return -1;

        sockaddr_l2 local_addr = {0};
        local_addr.l2_family = AF_BLUETOOTH;
		bdaddr_t any = {0};        // 或者 {{0,0,0,0,0,0}}
		local_addr.l2_bdaddr = any;
        local_addr.l2_psm = htobs(channel);

        // 绑定地址
		if (bind(sock, (struct sockaddr*)&local_addr, sizeof(local_addr)) < 0) {
			close(sock);
			return -1;
		}
		
		// 获取绑定地址，需要完善
		/*sockaddr_l2 bound_addr;
		socklen_t len = sizeof(bound_addr);
		if (getsockname(sock, (struct sockaddr*)&bound_addr, &len) == 0) {
			memcpy(address, bound_addr.l2_bdaddr.b, 6);
		}*/
    }
    else {
		fprintf(stderr,"[Error]: Unsupported protocols\n");
        return -1; // 不支持的协议
    }
	return sock;
}



TpBluetoothServer::TpBluetoothServer(const TpString& name, TpBluetoothService::Protocol type)
{
	data_ = new TpBluetoothServerData();
	TpBluetoothServerData *data = static_cast<TpBluetoothServerData *>(data_);
	if(!data)
	{
		fprintf(stderr,"[Error]: TpBluetoothServerData\n");
		return ;
	}
	if(TpDbusConnectManage::instance().connection()!=TP_TRUE)
	{
		fprintf(stderr,"[Error]: connect to dbus error\n");
		return ;
	}
	data->adapter=find_adapter(name.c_str(),NULL);
	if(!data->adapter)
	{
		fprintf(stderr,"[Error]: 设备不存在\n");
	}
	data->adapter_name=name;
	data->type=type;
}

TpBluetoothServer::~TpBluetoothServer()
{
	TpBluetoothServerData *data = static_cast<TpBluetoothServerData *>(data_);
	if(!data)
		return ;
	close();

	if(data->adapter)
		bluet_object_free(data->adapter);
	
	delete(data);
}

/// @brief 关闭服务端
/// @return 返回0
tpInt32 TpBluetoothServer::close()
{
	TpBluetoothServerData *data = static_cast<TpBluetoothServerData *>(data_);
	if(!data)
		return -1;
	if (data->notifier) {
		delete data->notifier; 
		data->notifier = nullptr;
    }
	for(auto it : data->connects)
	{
		if(it==nullptr)
			continue;
		it->disconnectFromService();
		delete(it);
	}
	::close(data->sockfd);
	
	return 0;
}

/// @brief 设置最大可以连接的数量，需要在监听之前调用
/// @param max 最大连接数量
void TpBluetoothServer::setMaxPendingConnects(tpInt32 max)
{
	TpBluetoothServerData *data = static_cast<TpBluetoothServerData *>(data_);
	if(!data)
		return ;
	data->max_connect=max;
}

/// @brief 获取最大可连接的数量
/// @return 
tpInt32 TpBluetoothServer::getMaxPendingConnects()
{
	TpBluetoothServerData *data = static_cast<TpBluetoothServerData *>(data_);
		if(!data)
		return -1;
	return data->max_connect;
}

/// @brief 开始监听客户端连接
/// @param addr 
/// @param port 
/// @return 
tpBool TpBluetoothServer::listen(const TpBluetoothAddress &address, tpUInt16 port)
{
	TpBluetoothServerData *data = static_cast<TpBluetoothServerData *>(data_);
	if(!data)
		return TP_FALSE;
	
	TpBluetoothAddress addr=address;
	if(addr.isNull())
		data->sockfd=bluet_server_socket(data->type,NULL,(uint16_t)port);
	else
		data->sockfd=bluet_server_socket(data->type,addr.toString().c_str(),(uint16_t)port);
	if(data->sockfd<0)
	{
		fprintf(stderr,"[Error]: connect to local socket error\n");
		return TP_FALSE;
	}
	tpInt32 flags = fcntl(data->sockfd, F_GETFL, 0);
	if (flags == -1) {
		fprintf(stderr,"[Error]: fcntl F_GETFL failed\n");
		return TP_FALSE;
	}
	if (fcntl(data->sockfd, F_SETFL, flags | O_NONBLOCK) == -1) {
		fprintf(stderr,"[Error]: fcntl F_SETFL failed\n");
		return TP_FALSE;
	}

	if(::listen(data->sockfd,data->max_connect)<0)
	{
		fprintf(stderr,"[Error]: listen error\n");
		return TP_FALSE;
	}

	data->notifier = new TpSocketNotifier(data->sockfd, TpSocketNotifier::Read, 
		[this]() {handleNewConnection();}
	);

	printf("事件监测已打开\n");
	data->port=port;
	data->address=address;
	data->status=TpSocket::TP_SOCK_LISTEN;
	return TP_TRUE;
}

tpBool TpBluetoothServer::listen(const TpString &uuid)
{
	TpBluetoothServerData *data = static_cast<TpBluetoothServerData *>(data_);
	return TP_FALSE;
}

tpUInt16 TpBluetoothServer::getServerPort()
{
	TpBluetoothServerData *data = static_cast<TpBluetoothServerData *>(data_);
	return data->port;
}

TpBluetoothAddress TpBluetoothServer::getServerAddress()
{
	TpBluetoothServerData *data = static_cast<TpBluetoothServerData *>(data_);
	return data->address;
}

TpBluetoothService::Protocol TpBluetoothServer::getServerType()
{
	TpBluetoothServerData *data = static_cast<TpBluetoothServerData *>(data_);
	return data->type;
}

/// @brief 查看当前是否在监听
/// @return 返回监听状态
tpBool TpBluetoothServer::isListening()
{
	TpBluetoothServerData *data = static_cast<TpBluetoothServerData *>(data_);
	return (data->status==TpSocket::TP_SOCK_LISTEN ? TP_TRUE:TP_FALSE);
}

TpBluetoothSocket *TpBluetoothServer::nextPendingConnection()
{
	TpBluetoothServerData *data = static_cast<TpBluetoothServerData *>(data_);

    if (data->connects.empty()) 
		return nullptr;
    TpBluetoothSocket* client = data->connects.front();
    data->connects.pop_front();
    return client;
}


int TpBluetoothServer::accept(TpString &client_addr, int &client_port)
{
	TpBluetoothServerData *data = static_cast<TpBluetoothServerData *>(data_);
	if(!data)
		return -1;
	char c_addr[64];
	int acceptfd;
	union {
		sockaddr_rc rc;
		sockaddr_l2 l2;
		sockaddr_storage storage; // 保证足够大
	} clieaddr;

	int size_clieaddr=sizeof(clieaddr);
	if((acceptfd=::accept(data->sockfd,(struct sockaddr*)(&clieaddr), (socklen_t *)&size_clieaddr))<0)
	{
		fprintf(stderr,"[Error]: accept error\n");
		return -1;
	}
	
	if (data->type == TpBluetoothService::TP_BLUET_RFCOMM_PROTOCOL) {
		ba2str(&clieaddr.rc.rc_bdaddr, c_addr);
		client_addr = c_addr;
        client_port = clieaddr.rc.rc_channel;   // RFCOMM 通道
	} 
	else if (data->type == TpBluetoothService::TP_BLUET_L2CAP_PROTOCOL) {
		ba2str(&clieaddr.l2.l2_bdaddr, c_addr);
		client_addr = c_addr;
        client_port = clieaddr.l2.l2_psm;       // L2CAP PSM
	}
	else {
        client_addr = "";
        client_port = -1;
    }

	printf("accetp new connect %s\n",c_addr);
	return acceptfd;
}

// 内部处理新连接
void TpBluetoothServer::handleNewConnection() 
{
	TpBluetoothServerData *data = static_cast<TpBluetoothServerData *>(data_);
	// 使用 TpSocket 接受
	TpString addr;
	int port;
	int sock = accept(addr,port);
	if (sock<0) {
		std::cerr << "accept failed" << std::endl;
		return;
	}
	// 创建 TpTcpSocket
	TpBluetoothAddress bt_addr(addr);
	TpBluetoothSocket* bt_c = new TpBluetoothSocket(data->adapter_name,sock,data->type);
	data->connects.push_front(bt_c);

	connect(bt_c, TpBluetoothSocket::disconnected, [=](TpBluetoothSocket *client) {
        data->connects.remove(client);
		std::thread([client]() {
           // 等 epoll 事件处理完再删除
            std::this_thread::sleep_for(std::chrono::milliseconds(1));
            delete client;
        }).detach();
    });
	
	newConnection.emit();
}
