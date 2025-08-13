/*///------------------------------------------------------------------------------------------------------------------------//
		蓝牙通信相关接口
说 明 : 一个类只允许连接到一个蓝牙设备 
日 期 : 2025.5.9

/*///------------------------------------------------------------------------------------------------------------------------//

#include <iostream>
#include <stdio.h>
#include <bluetooth/bluetooth.h>
#include <bluetooth/rfcomm.h>
#include <bluetooth/l2cap.h>
#include "tpSocket.h"
#include "tpDbusConnectManage.h"
#include "tpSocketNotifier.h"
#include "bluetooth/include/bluetooth_inc.h"
#include "bluetooth/include/blt_device.h"
#include "tpBluetoothSocket.h"

struct tpBluetoothSocketData{
	Adapter *adapter;	//当前网络连接使用的蓝牙适配器
	tpString uuid;
	int sockfd;		
	tpBluetoothService::Protocol type;
	tpSocket::tpSocketStatus status;
	tpSocketNotifier *notifier_read;		//监听断开，读写
	tpSocketNotifier *notifier_write;		//监听连接
	BluetDevice *device;
	tpBluetoothSocketData(){
		adapter=NULL;
		type=tpBluetoothService::TP_BLUET_UNKNOWN_PROTOCOL;
		notifier_read=NULL;
		notifier_write=NULL;
		device=NULL;
	};
};


bool setBlocking(int sock_fd, bool blocking) {
	int flags = fcntl(sock_fd, F_GETFL, 0);
	if (flags == -1) return false;
	
	if (blocking) {
		flags &= ~O_NONBLOCK;
	} else {
		flags |= O_NONBLOCK;
	}
	
	if (fcntl(sock_fd, F_SETFL, flags) == -1) 
		return false;
	
	return true;
}

static int bluet_socket(tpBluetoothService::Protocol type,const char *address,uint16_t channel)
{
	int sock = -1;
    struct sockaddr* addr_ptr = nullptr;
    socklen_t addr_len = 0;

    // 根据协议创建不同类型的socket和地址结构

    if (type == tpBluetoothService::TP_BLUET_RFCOMM_PROTOCOL) {
        sock = socket(AF_BLUETOOTH, SOCK_STREAM, BTPROTO_RFCOMM);
        if (sock < 0) 
			return -1;

        sockaddr_rc remote_addr = {0};
        remote_addr.rc_family = AF_BLUETOOTH;
        remote_addr.rc_channel = static_cast<uint8_t>(channel);
        // 复制地址
        bdaddr_t bt_addr;
        memcpy(&bt_addr.b, address, 6);
        bacpy(&remote_addr.rc_bdaddr, &bt_addr);

        addr_ptr = (struct sockaddr*)&remote_addr;
        addr_len = sizeof(remote_addr);
    }
    else if (type == tpBluetoothService::TP_BLUET_L2CAP_PROTOCOL) {
        sock = socket(AF_BLUETOOTH, SOCK_SEQPACKET, BTPROTO_L2CAP);
        if (sock < 0) 
			return -1;

        sockaddr_l2 remote_addr = {0};
        remote_addr.l2_family = AF_BLUETOOTH;
        remote_addr.l2_psm = htobs(channel); // PSM需要转换字节序
        // 复制地址
        bdaddr_t bt_addr;
        memcpy(&bt_addr.b, address, 6);
        bacpy(&remote_addr.l2_bdaddr, &bt_addr);

        addr_ptr = (struct sockaddr*)&remote_addr;
        addr_len = sizeof(remote_addr);
    }
    else {
        return -1; // 不支持的协议
    }

	setBlocking(sock,false);
    // 建立连接
    int connect_ret = (::connect)(sock, addr_ptr, addr_len);
	if (connect_ret == 0) {
        // 立即连接成功（罕见情况）
        //return new tpBluetoothSocket(sock, type);
    } 
    else if (errno != EINPROGRESS) {
        // 立即失败
		printf("立即失败\n");
        close(sock);
        return -1;
    }
	return sock;
}




tpBluetoothSocket::tpBluetoothSocket(const tpString& name,tpBluetoothService::Protocol type)
{
	data_ = new tpBluetoothSocketData();
	tpBluetoothSocketData *data = static_cast<tpBluetoothSocketData *>(data_);
	if(tpDbusConnectManage::instance().connection()!=TP_TRUE)
	{
		fprintf(stderr,"[Error]: connect to dbus error\n");
		return ;
	}
	printf("查找设备：%s\n",name.c_str());
	data->adapter=find_adapter(name.c_str(),NULL);
	if(!data->adapter)
	{
		fprintf(stderr,"[Error]: 设备不存在\n");
	}
	printf("tpBluetoothSocket\n");
	data->type=type;
	data->notifier_read = new tpSocketNotifier(data->sockfd, tpSocketNotifier::Read, 
		[this]() { handleRead(); },
		[this]() { handleDisconnected(); }
	);
}

tpBluetoothSocket::tpBluetoothSocket(int sockfd,tpBluetoothService::Protocol type)
{
	data_ = new tpBluetoothSocketData();
	tpBluetoothSocketData *data = static_cast<tpBluetoothSocketData *>(data_);
	if(tpDbusConnectManage::instance().connection()!=TP_TRUE)
	{
		fprintf(stderr,"[Error]: connect to dbus error\n");
		return ;
	}
	data->type=type;
	data->notifier_read = new tpSocketNotifier(data->sockfd, tpSocketNotifier::Read, 
		[this]() { handleRead(); },
		[this]() { handleDisconnected(); }
	);
}


tpBluetoothSocket::~tpBluetoothSocket()
{
	tpBluetoothSocketData *data = static_cast<tpBluetoothSocketData *>(data_);
	if(!data)
		return ;
	if(data->adapter)
		bluet_object_free(data->adapter);
	disconnectFromService();
	if (data->notifier_read) {
		delete data->notifier_read; 
		data->notifier_read = nullptr;
	}
	delete(data);
}

int tpBluetoothSocket::connectToService(const tpBluetoothService& service)
{
	tpBluetoothSocketData *data = static_cast<tpBluetoothSocketData *>(data_);
	if(!data || !data->adapter)
		return -1;
	if(data->status == tpSocket::TP_SOCK_CONNECT)
	{
		fprintf(stderr,"[Error]: Repeatedly establish connection\n");
		return -1;
	}

	return 0;
}


int tpBluetoothSocket::connectToService(const tpBluetoothAddress& address,tpUInt16 port)
{
	tpBluetoothSocketData *data = static_cast<tpBluetoothSocketData *>(data_);
	if(!data || !data->adapter)
		return -1;
	if(data->status == tpSocket::TP_SOCK_CONNECT)
	{
		fprintf(stderr,"[Error]: Repeatedly establish connection\n");
		return -1;
	}

	tpBluetoothAddress addr(address);
	data->device=bluet_device_creat(data->adapter,addr.toString().c_str());
	if(!data->device)
	{
		fprintf(stderr,"[Error]: can't find device\n");
		return -1;
	}
	printf("find device:%s\n",bluet_device_get_address(data->device));
	

	data->sockfd=bluet_socket(data->type,addr.toString().c_str(),(uint16_t)port);
	if(data->sockfd<0)
	{
		printf("[Debug]: bluet socket connect error\n");
		return -1;
	}		

	data->notifier_write = new tpSocketNotifier(
			data->sockfd, tpSocketNotifier::Write,
			[this](){ handleWrite(); }
		);

	return 0;
}

int tpBluetoothSocket::connectToService(const tpBluetoothAddress& address,const tpString &uuid)
{
	tpBluetoothSocketData *data = static_cast<tpBluetoothSocketData *>(data_);
	if(!data || !data->adapter)
		return -1;
	if(data->status == tpSocket::TP_SOCK_CONNECT)
	{
		fprintf(stderr,"[Error]: Repeatedly establish connection\n");
		return -1;
	}
	data->uuid=uuid;
	tpBluetoothAddress addr(address);
	data->device=bluet_device_creat(data->adapter,addr.toString().c_str());
	return bluet_connect_remote_device(data->device,uuid.empty()?NULL:uuid.c_str());
}

int tpBluetoothSocket::disconnectFromService()
{
	tpBluetoothSocketData *data = static_cast<tpBluetoothSocketData *>(data_);
	if(!data || !data->adapter)
		return -1;
	if(data->sockfd>=0)
		::close(data->sockfd);
	::close(data->sockfd);
	data->status=tpSocket::TP_SOCK_DISCONNECT;
	disconnected.emit(this);		//发送断开连接的信号

	return 0;
}


tpUInt64 tpBluetoothSocket::send(const tpUInt8 *buff, tpUInt64 size)
{
	tpBluetoothSocketData *data = static_cast<tpBluetoothSocketData *>(data_);
	if(!data || !data->adapter)
		return -1;
	if(data->status!=tpSocket::TP_SOCK_CONNECT)
		return -1;
	return ::send(data->sockfd,buff,size,0);
}

tpUInt64 tpBluetoothSocket::recv(tpUInt8 *buff, tpUInt64 size)
{
	tpBluetoothSocketData *data = static_cast<tpBluetoothSocketData *>(data_);
	if(!data || !data->adapter)
		return -1;
	if(data->status!=tpSocket::TP_SOCK_CONNECT)
		return -1;
	return ::recv(data->sockfd,buff,size,0);
}






void tpBluetoothSocket::handleWrite() {
    tpBluetoothSocketData *data = static_cast<tpBluetoothSocketData *>(data_);
	printf("handlewrite触发\n");
    // 检查连接结果
    int err = 0;
    socklen_t len = sizeof(err);
    getsockopt(data->sockfd, SOL_SOCKET, SO_ERROR, &err, &len);

    if (err == 0) 
	{
		// 停掉写事件监听
		if (data->notifier_write) {
			delete data->notifier_write;
			data->notifier_write = nullptr;
		}
        data->status = tpSocket::TP_SOCK_CONNECT;
        connected.emit();
    }
	else
	{
		printf("debug 未知错误\n");
	}
    // 之后就继续依赖 notifier_read 处理数据和断开
}


void tpBluetoothSocket::handleRead()
{
	tpBluetoothSocketData *data = static_cast<tpBluetoothSocketData *>(data_);
	readyRead.emit(this);	
}

void tpBluetoothSocket::handleDisconnected()
{
	tpBluetoothSocketData *data = static_cast<tpBluetoothSocketData *>(data_);
	data->status = tpSocket::TP_SOCK_DISCONNECT;
	disconnected.emit(this);
}	


tpBool tpBluetoothSocket::checkDisconnected() 
{
	tpBluetoothSocketData *data = static_cast<tpBluetoothSocketData *>(data_);
	char c;
	int ret = ::recv(data->sockfd,(tpUInt8*)&c, 1, MSG_PEEK);
	if (ret == 0) {
		data->status = tpSocket::TP_SOCK_DISCONNECT;
		disconnected.emit(this);
		return TP_TRUE;
	}
	return TP_FALSE;
}
