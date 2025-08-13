

#include <iostream>
#include <stdio.h>
#include <thread>
#include <bluetooth/bluetooth.h>
#include <bluetooth/rfcomm.h>
#include <bluetooth/l2cap.h>
#include "tpSocketNotifier.h"
#include "tpDbusConnectManage.h"
#include "bluetooth/include/bluetooth_inc.h"
#include "tpBluetoothServer.h"

struct tpBluetoothServerData{
	tpInt32 max_connect;
	tpUInt16 port;
	tpBluetoothAddress address;
	tpBluetoothService::Protocol type;
	tpSocketNotifier *notifier;
	Adapter *adapter;
	tpList<tpBluetoothSocket *>connects;	//蓝牙连接列表
	int sockfd;
	tpSocket::tpSocketStatus status;
	tpBluetoothServerData(){
		status=tpSocket::TP_SOCK_DISCONNECT;
		sockfd=-1;
		adapter=NULL;
		max_connect=32;
	};
};

//type：协议类型
//address：返回绑定到的地址
//channel:绑定到的端口
static int bluet_server_socket(tpBluetoothService::Protocol type,const char *address,uint16_t channel)
{
	int sock = -1;
    struct sockaddr* addr_ptr = nullptr;
    socklen_t addr_len = 0;

    // 根据协议创建不同类型的socket和地址结构

    if (type == tpBluetoothService::TP_BLUET_RFCOMM_PROTOCOL) {
        sock = socket(AF_BLUETOOTH, SOCK_STREAM, BTPROTO_RFCOMM);
        if (sock < 0) 
			return -1;

		// 设置地址重用
		if (address) {
			int reuse = 1;
			setsockopt(sock, SOL_SOCKET, SO_REUSEADDR, &reuse, sizeof(reuse));
		}

        sockaddr_rc local_addr = {0};
        local_addr.rc_family = AF_BLUETOOTH;
		//bacpy(&local_addr.rc_bdaddr,BDADDR_ANY);
		local_addr.rc_channel = channel;
		// 绑定地址
		if (bind(sock, (struct sockaddr*)&local_addr, sizeof(local_addr)) < 0) {
			close(sock);
			return -1;
		}

		// 获取绑定地址，需要完善
		/*sockaddr_rc bound_addr;
		socklen_t len = sizeof(bound_addr);
		if (getsockname(sock, (struct sockaddr*)&bound_addr, &len) == 0) {
			memcpy(address, bound_addr.rc_bdaddr.b, 6);
		}*/
    }
    else if (type == tpBluetoothService::TP_BLUET_L2CAP_PROTOCOL) {
        sock = socket(AF_BLUETOOTH, SOCK_SEQPACKET, BTPROTO_L2CAP);
        if (sock < 0) 
			return -1;

        sockaddr_l2 local_addr = {0};
        local_addr.l2_family = AF_BLUETOOTH;
		//bacpy(&local_addr.l2_bdaddr, BDADDR_ANY);
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
        return -1; // 不支持的协议
    }
	return sock;
}



tpBluetoothServer::tpBluetoothServer(const tpString& name, tpBluetoothService::Protocol type)
{
	data_ = new tpBluetoothServerData();
	tpBluetoothServerData *data = static_cast<tpBluetoothServerData *>(data_);
	if(!data)
	{
		fprintf(stderr,"[Error]: tpBluetoothServerData\n");
		return ;
	}
	if(tpDbusConnectManage::instance().connection()!=TP_TRUE)
	{
		fprintf(stderr,"[Error]: connect to dbus error\n");
		return ;
	}
	data->adapter=find_adapter(name.c_str(),NULL);
	if(data->adapter)
	{
		fprintf(stderr,"[Error]: 设备不存在\n");
	}
}

tpBluetoothServer::~tpBluetoothServer()
{
	tpBluetoothServerData *data = static_cast<tpBluetoothServerData *>(data_);
	if(!data)
		return ;
	close();

	if(data->adapter)
		bluet_object_free(data->adapter);
	
	delete(data);
}

/// @brief 关闭服务端
/// @return 返回0
tpInt32 tpBluetoothServer::close()
{
	tpBluetoothServerData *data = static_cast<tpBluetoothServerData *>(data_);
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
void tpBluetoothServer::setMaxPendingConnects(tpInt32 max)
{
	tpBluetoothServerData *data = static_cast<tpBluetoothServerData *>(data_);
	data->max_connect=max;
}

/// @brief 获取最大可连接的数量
/// @return 
tpInt32 tpBluetoothServer::getMaxPendingConnects()
{
	tpBluetoothServerData *data = static_cast<tpBluetoothServerData *>(data_);
	return data->max_connect;
}

/// @brief 开始监听客户端连接
/// @param addr 
/// @param port 
/// @return 
tpBool tpBluetoothServer::listen(const tpBluetoothAddress &address, tpUInt16 port)
{
	tpBluetoothServerData *data = static_cast<tpBluetoothServerData *>(data_);
	if(!data)
		return TP_FALSE;
	
	tpBluetoothAddress addr=address;

	data->sockfd=bluet_server_socket(data->type,addr.toString().c_str(),(uint16_t)port);
	if(data->sockfd<0)
	{
		return TP_FALSE;
	}

	tpInt32 flags = fcntl(data->sockfd, F_GETFL, 0);
	if (flags == -1) {
		std::cerr << "fcntl F_GETFL failed\n";
		return TP_FALSE;
	}

	if (fcntl(data->sockfd, F_SETFL, flags | O_NONBLOCK) == -1) {
		std::cerr << "fcntl F_SETFL failed\n";
		return TP_FALSE;
	}

	if(::listen(data->sockfd,data->max_connect)<0)
		return TP_FALSE;

	data->notifier = new tpSocketNotifier(data->sockfd, tpSocketNotifier::Read, 
		[this]() {handleNewConnection();}
	);
	
	data->status=tpSocket::TP_SOCK_LISTEN;
	return TP_TRUE;
}

tpBool tpBluetoothServer::listen(const tpString &uuid)
{
	tpBluetoothServerData *data = static_cast<tpBluetoothServerData *>(data_);
	return TP_FALSE;
}

tpUInt16 tpBluetoothServer::getServerPort()
{
	tpBluetoothServerData *data = static_cast<tpBluetoothServerData *>(data_);
	return 0;
}

tpBluetoothAddress tpBluetoothServer::getServerAddress()
{
	tpBluetoothServerData *data = static_cast<tpBluetoothServerData *>(data_);
	return data->address;
}

tpBluetoothService::Protocol tpBluetoothServer::getServerType()
{
	tpBluetoothServerData *data = static_cast<tpBluetoothServerData *>(data_);
	return data->type;
}

/// @brief 查看当前是否在监听
/// @return 返回监听状态
tpBool tpBluetoothServer::isListening()
{
	tpBluetoothServerData *data = static_cast<tpBluetoothServerData *>(data_);
	return (data->status==tpSocket::TP_SOCK_LISTEN ? TP_TRUE:TP_FALSE);
}

tpBluetoothSocket *tpBluetoothServer::nextPendingConnection()
{
	tpBluetoothServerData *data = static_cast<tpBluetoothServerData *>(data_);

    if (data->connects.empty()) 
		return nullptr;
    tpBluetoothSocket* client = data->connects.front();
    data->connects.pop_front();
    return client;
}


int tpBluetoothServer::accept()
{
	tpBluetoothServerData *data = static_cast<tpBluetoothServerData *>(data_);
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
		//std::cerr << "accept error\n";
		return -1;
	}
	
	if (data->type == tpBluetoothService::TP_BLUET_RFCOMM_PROTOCOL) {
		/*memcpy(clientAddr.data(), 
				client_addr.rc.rc_bdaddr.b, 
				sizeof(client_addr.rc.rc_bdaddr.b));*/
	} 
	else if (data->type == tpBluetoothService::TP_BLUET_L2CAP_PROTOCOL) {
		/*memcpy(clientAddr.data(), 
				client_addr.l2.l2_bdaddr.b, 
				sizeof(client_addr.l2.l2_bdaddr.b));*/
	}


	printf("accetp new connect %s\n",c_addr);

	return acceptfd;
}

// 内部处理新连接
void tpBluetoothServer::handleNewConnection() 
{
	tpBluetoothServerData *data = static_cast<tpBluetoothServerData *>(data_);
	// 使用 tpSocket 接受
	int sock = accept();

	if (sock<0) {
		std::cerr << "accept failed" << std::endl;
		return;
	}
	// 创建 tpTcpSocket
	tpBluetoothSocket* bt_c = new tpBluetoothSocket(sock,data->type);
	data->connects.push_front(bt_c);

	connect(bt_c, tpBluetoothSocket::disconnected, [=](tpBluetoothSocket *client) {
        data->connects.remove(client);
		std::thread([client]() {
           // 等 epoll 事件处理完再删除
            std::this_thread::sleep_for(std::chrono::milliseconds(1));
            delete client;
        }).detach();
    });
	
	newConnection.emit();
}
