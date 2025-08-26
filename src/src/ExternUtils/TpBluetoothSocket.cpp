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
#include "TpSocket.h"
#include "TpDbusConnectManage.h"
#include "TpSocketNotifier.h"
#include "bluetooth/include/bluetooth_inc.h"
#include "bluetooth/include/blt_device.h"
#include "TpBluetoothSocket.h"

struct TpBluetoothSocketData{
	Adapter *adapter;	//当前网络连接使用的蓝牙适配器,客户端必须不为空，服务端接收的socket的此参数通常为空
	BluetDevice *device;		//设备
	TpString uuid;
	int sockfd;			//连接的文件描述符
	TpString adapter_name;				//本地适配器名字(可以是名字或地址)
	TpBluetoothService::Protocol type;	//连接的类型
	TpSocket::TpSocketStatus status;	//连接状态
	TpSocketNotifier *notifier_read;	//监听断开，读写
	TpSocketNotifier *notifier_write;	//监听连接
	TpBluetoothAddress address;			//远端地址
	TpBluetoothSocketData(){
		adapter=NULL;
		type=TpBluetoothService::TP_BLUET_UNKNOWN_PROTOCOL;
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



static int bluet_socket(TpBluetoothService::Protocol type)
{
	int sock = -1;
	if (type == TpBluetoothService::TP_BLUET_RFCOMM_PROTOCOL) {
        sock = socket(AF_BLUETOOTH, SOCK_STREAM, BTPROTO_RFCOMM);
        if (sock < 0) 
			return -1;
    }
    else if (type == TpBluetoothService::TP_BLUET_L2CAP_PROTOCOL) {
        sock = socket(AF_BLUETOOTH, SOCK_SEQPACKET, BTPROTO_L2CAP);
        if (sock < 0) 
			return -1;
    }
    else {
        return -1; // 不支持的协议
    }
	return sock;
}


static int bluet_connect(int sock,TpBluetoothService::Protocol type,const char *address,uint16_t channel)
{
    struct sockaddr* addr_ptr = nullptr;
    socklen_t addr_len = 0;

    // 根据协议创建不同类型的socket和地址结构
    if (type == TpBluetoothService::TP_BLUET_RFCOMM_PROTOCOL) {

        sockaddr_rc remote_addr = {0};
        remote_addr.rc_family = AF_BLUETOOTH;
        remote_addr.rc_channel = static_cast<uint8_t>(channel);
        // 复制地址
        bdaddr_t bt_addr;
        memcpy(&bt_addr.b, address, 6);
		str2ba(address, &bt_addr);  // address 必须是 "XX:XX:XX:XX:XX:XX"
		bacpy(&remote_addr.rc_bdaddr, &bt_addr);
        //bacpy(&remote_addr.rc_bdaddr, &bt_addr);

        addr_ptr = (struct sockaddr*)&remote_addr;
        addr_len = sizeof(remote_addr);
    }
    else if (type == TpBluetoothService::TP_BLUET_L2CAP_PROTOCOL) {

        sockaddr_l2 remote_addr = {0};
        remote_addr.l2_family = AF_BLUETOOTH;
        remote_addr.l2_psm = htobs(channel); // PSM需要转换字节序
        // 复制地址
        bdaddr_t bt_addr;
        memcpy(&bt_addr.b, address, 6);
		str2ba(address, &bt_addr); 
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
        //return new TpBluetoothSocket(sock, type);
    } 
    else if (errno != EINPROGRESS) {
        // 立即失败
		printf("立即失败\n");
		perror("connect:");
        close(sock);
        return -1;
    }
	return connect_ret;
}



TpBluetoothSocket::TpBluetoothSocket(const TpString& name,TpBluetoothService::Protocol type)
{
	data_ = new TpBluetoothSocketData();
	TpBluetoothSocketData *data = static_cast<TpBluetoothSocketData *>(data_);
	if(TpDbusConnectManage::instance().connection()!=TP_TRUE)
	{
		fprintf(stderr,"[Error]: connect to dbus error\n");
		return ;
	}
	printf("查找设备：%s\n",name.c_str());
	data->adapter=find_adapter(name.c_str(),NULL);
	if(!data->adapter)
	{
		fprintf(stderr,"[Error]: 设备不存在\n");
		return ;
	}
	printf("TpBluetoothSocket\n");
	data->type=type;
	data->sockfd=bluet_socket(data->type);
	if(data->sockfd<0)
	{
		fprintf(stderr,"[Error]: 蓝牙socket无法创建\n");
		return ;
	}
	data->notifier_read = new TpSocketNotifier(data->sockfd, TpSocketNotifier::Read, 
		[this]() { handleRead(); },
		[this]() { handleDisconnected(); }
	);
}

TpBluetoothSocket::TpBluetoothSocket(const TpString& name,int sockfd,TpBluetoothService::Protocol type)
{
	data_ = new TpBluetoothSocketData();
	TpBluetoothSocketData *data = static_cast<TpBluetoothSocketData *>(data_);
	if(TpDbusConnectManage::instance().connection()!=TP_TRUE)
	{
		fprintf(stderr,"[Error]: connect to dbus error\n");
		return ;
	}
	data->type=type;
	data->sockfd=sockfd;
	data->adapter_name=name;
	data->notifier_read = new TpSocketNotifier(data->sockfd, TpSocketNotifier::Read, 
		[this]() { handleRead(); },
		[this]() { handleDisconnected(); }
	);
	data->status=TpSocket::TP_SOCK_CONNECT;
}


TpBluetoothSocket::~TpBluetoothSocket()
{
	TpBluetoothSocketData *data = static_cast<TpBluetoothSocketData *>(data_);
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

int TpBluetoothSocket::connectToService(const TpBluetoothService& service)
{
	TpBluetoothSocketData *data = static_cast<TpBluetoothSocketData *>(data_);
	if(!data || !data->adapter)
		return -1;
	if(data->status == TpSocket::TP_SOCK_CONNECT)
	{
		fprintf(stderr,"[Error]: Repeatedly establish connection\n");
		return -1;
	}
	return 0;
}


int TpBluetoothSocket::connectToService(const TpBluetoothAddress& address,tpUInt16 port)
{
	TpBluetoothSocketData *data = static_cast<TpBluetoothSocketData *>(data_);
	if(!data || !data->adapter)
		return -1;
	if(data->status == TpSocket::TP_SOCK_CONNECT)
	{
		fprintf(stderr,"[Error]: Repeatedly establish connection\n");
		return -1;
	}

	TpBluetoothAddress addr(address);
	data->device=bluet_device_creat(data->adapter,addr.toString().c_str());
	if(!data->device)
	{
		fprintf(stderr,"[Error]: can't find device\n");
		return -1;
	}
	printf("find device:%sok\n",bluet_device_get_address(data->device));
	
	int ret=bluet_connect(data->sockfd,data->type,addr.toString().c_str(),(uint16_t)port);
	if(ret<0)
	{
		printf("[Debug]: bluet socket connect error\n");
		return -1;
	}		

	data->address=address;
	data->notifier_write = new TpSocketNotifier(
			data->sockfd, TpSocketNotifier::Write,
			[this](){ handleWrite(); }
		);

	return 0;
}

int TpBluetoothSocket::connectToService(const TpBluetoothAddress& address,const TpString &uuid)
{
	TpBluetoothSocketData *data = static_cast<TpBluetoothSocketData *>(data_);
	if(!data || !data->adapter)
		return -1;
	if(data->status == TpSocket::TP_SOCK_CONNECT)
	{
		fprintf(stderr,"[Error]: Repeatedly establish connection\n");
		return -1;
	}
	data->uuid=uuid;
	TpBluetoothAddress addr(address);
	data->device=bluet_device_creat(data->adapter,addr.toString().c_str());
	return bluet_connect_remote_device(data->device,uuid.empty()?NULL:uuid.c_str());
}

int TpBluetoothSocket::disconnectFromService()
{
	TpBluetoothSocketData *data = static_cast<TpBluetoothSocketData *>(data_);
	if(!data || !data->adapter)
		return -1;
	if(data->sockfd>=0)
		::close(data->sockfd);
	::close(data->sockfd);
	setDisconnectedInfo();
	disconnected.emit(this);		//发送断开连接的信号

	return 0;
}

TpBluetoothAddress TpBluetoothSocket::getPeerAddress()
{
	TpBluetoothSocketData *data = static_cast<TpBluetoothSocketData *>(data_);
	if(!data)
		return TpBluetoothAddress();
	if(data->status!=TpSocket::TP_SOCK_CONNECT)
		return TpBluetoothAddress();

	struct sockaddr_rc addr;
    socklen_t len = sizeof(addr);
    
    if (::getpeername(data->sockfd, (struct sockaddr*)&addr, &len) == 0) {
        char addrStr[18];
        ba2str(&addr.rc_bdaddr, addrStr); // 返回 "00:11:22:33:44:55"
        return TpBluetoothAddress(TpString(addrStr));
    }
    return TpBluetoothAddress();
}

TpString TpBluetoothSocket::getPeerName()
{
	TpBluetoothSocketData *data = static_cast<TpBluetoothSocketData *>(data_);
	if(!data)
		return nullptr;
	if(data->status!=TpSocket::TP_SOCK_CONNECT)
		return nullptr;

	TpBluetoothAddress address=getPeerAddress();
	data->adapter=find_adapter(data->adapter_name.c_str(),NULL);
	if(!data->adapter)
	{
		fprintf(stderr,"[Error]: 无法获取远程设备名称\n");
		return nullptr;
	}
	BluetDevice *device=bluet_device_creat(data->adapter,address.toString().c_str());
	if(!device)
	{
		fprintf(stderr,"[Error]: 无法获取远程设备名称\n");
		return nullptr;
	}

	return bluet_device_get_name(device);
}

tpUInt16 TpBluetoothSocket::getPeerPort()
{
	TpBluetoothSocketData *data = static_cast<TpBluetoothSocketData *>(data_);
	if(!data)
		return 0;
	if(data->status!=TpSocket::TP_SOCK_CONNECT)
		return 0;
	struct sockaddr_rc addr;
    socklen_t len = sizeof(addr);
    
    if (::getpeername(data->sockfd, (struct sockaddr*)&addr, &len) == 0) {
        return addr.rc_channel; // 直接返回整数端口
    }
    return 0;
}


tpUInt64 TpBluetoothSocket::send(const tpUInt8 *buff, tpUInt64 size)
{
	TpBluetoothSocketData *data = static_cast<TpBluetoothSocketData *>(data_);
	if(!data)
		return -1;
	if(data->status!=TpSocket::TP_SOCK_CONNECT)
		return -1;
	return ::send(data->sockfd,buff,size,0);
}

tpUInt64 TpBluetoothSocket::recv(tpUInt8 *buff, tpUInt64 size)
{
	TpBluetoothSocketData *data = static_cast<TpBluetoothSocketData *>(data_);
	if(!data)
		return -1;
	if(data->status!=TpSocket::TP_SOCK_CONNECT)
		return -1;
	int ret= ::recv(data->sockfd,buff,size,0);
	if(ret==0)
	{
		setDisconnectedInfo();
		disconnected.emit(this);
	}
	return ret;
}



void TpBluetoothSocket::handleWrite() {
    TpBluetoothSocketData *data = static_cast<TpBluetoothSocketData *>(data_);
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
        data->status = TpSocket::TP_SOCK_CONNECT;
        connected.emit();
    }
	else
	{
		printf("debug 未知错误\n");
	}
    // 之后就继续依赖 notifier_read 处理数据和断开
}


void TpBluetoothSocket::handleRead()
{
	TpBluetoothSocketData *data = static_cast<TpBluetoothSocketData *>(data_);
	readyRead.emit(this);	
}

void TpBluetoothSocket::handleDisconnected()
{
	TpBluetoothSocketData *data = static_cast<TpBluetoothSocketData *>(data_);
	setDisconnectedInfo();
	disconnected.emit(this);
}	


tpBool TpBluetoothSocket::checkDisconnected() 
{
	TpBluetoothSocketData *data = static_cast<TpBluetoothSocketData *>(data_);
	char c;
	int ret = ::recv(data->sockfd,(tpUInt8*)&c, 1, MSG_PEEK);
	if (ret == 0) {
		setDisconnectedInfo();
		disconnected.emit(this);
		return TP_TRUE;
	}
	return TP_FALSE;
}

void TpBluetoothSocket::setDisconnectedInfo()
{
	TpBluetoothSocketData *data = static_cast<TpBluetoothSocketData *>(data_);
	data->status = TpSocket::TP_SOCK_DISCONNECT;
	data->sockfd = -1;
	data->address = TpBluetoothAddress();
}
