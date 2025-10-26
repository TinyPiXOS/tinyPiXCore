/*///------------------------------------------------------------------------------------------------------------------------//
		网卡数据的封装
说 明 : 
日 期 : 2025.6.29

/*///------------------------------------------------------------------------------------------------------------------------//

#include "TpNetworkDatagram.h"


struct TpNetworkDatagramData{

	tpUInt8* buffer;    // 数据负载（动态分配）
    tpUInt64 bufferSize;            // 数据长度
    TpString senderAddr;      // 发送方IP（字符串格式）
    TpString destAddr;        // 目标IP（字符串格式）
    tpUInt16 senderPort;     // 发送方端口
    tpUInt16 destPort ;       // 目标端口
    int hopLimit;           // TTL（-1=系统默认）
    unsigned interfaceIndex; // 网络接口索引
	TpNetworkDatagramData()
	{
		buffer = NULL;
		bufferSize = 0; 
		senderAddr = ""; 
		destAddr = "";
		destPort = 0; 
		hopLimit = -1;
		interfaceIndex = 0;
	}
};


TpNetworkDatagram::TpNetworkDatagram()
{
	data_ = new TpNetworkDatagramData();
}

// 构造函数
TpNetworkDatagram::TpNetworkDatagram(
    const tpUInt8* buff, 
    tpUInt64 size, 
    const TpString& destAddr, 
    tpUInt16 destPort
) {
    data_ = new TpNetworkDatagramData();
	TpNetworkDatagramData *data=static_cast<TpNetworkDatagramData*>(data_);
	if(!data)
	{
		fprintf(stderr,"[Error]: TpNetworkDatagram creat error\n");
		return ;
	}
    data->destAddr = destAddr; // 直接赋值（自动内存管理）
    data->destPort = destPort;
    // 深拷贝数据
    if (buff && size > 0) {
        data->buffer = new tpUInt8[size];
        memcpy(data->buffer, buff, size);
        data->bufferSize = size;
    }
}

// 析构函数
TpNetworkDatagram::~TpNetworkDatagram() {
   TpNetworkDatagramData *data=static_cast<TpNetworkDatagramData*>(data_);
    delete[] data->buffer;  // 释放数据缓冲区
    delete data;            // 释放结构体本身
}

// 拷贝构造（深拷贝）
TpNetworkDatagram::TpNetworkDatagram(const TpNetworkDatagram& other) 
{
	TpNetworkDatagramData *otherData=static_cast<TpNetworkDatagramData*>(other.data_);
    data_ = new TpNetworkDatagramData(*otherData); // 拷贝基础类型
    
    // 深拷贝数据缓冲区
    TpNetworkDatagramData* d = static_cast<TpNetworkDatagramData*>(data_);
    if (otherData->buffer && otherData->bufferSize > 0) {
        d->buffer = new tpUInt8[otherData->bufferSize];
        memcpy(d->buffer, otherData->buffer, otherData->bufferSize);
    }
}

// 赋值运算符（深拷贝）
TpNetworkDatagram& TpNetworkDatagram::operator=(const TpNetworkDatagram& other) {
    if (this == &other) return *this;
    
    TpNetworkDatagramData* d = static_cast<TpNetworkDatagramData*>(data_);
    TpNetworkDatagramData* otherData = static_cast<TpNetworkDatagramData*>(other.data_);
    
    // 释放旧数据
    delete[] d->buffer;
    
    // 拷贝基础类型
    *d = *otherData;
    d->buffer = nullptr; // 防止悬空指针
    
    // 深拷贝新数据
    if (otherData->buffer && otherData->bufferSize > 0) {
        d->buffer = new tpUInt8[otherData->bufferSize];
        memcpy(d->buffer, otherData->buffer, otherData->bufferSize);
    }
    return *this;
}



tpBool TpNetworkDatagram::isNull() const {
    const TpNetworkDatagramData* d = static_cast<TpNetworkDatagramData*>(data_);
    return ((!d->buffer && d->senderAddr.empty() && d->destAddr.empty())?TP_TRUE:TP_FALSE);
}

tpBool TpNetworkDatagram::isValid() const {
    const TpNetworkDatagramData* d = static_cast<TpNetworkDatagramData*>(data_);
    return ((d->buffer || !d->senderAddr.empty() || !d->destAddr.empty())?TP_TRUE:TP_FALSE);
}

const tpUInt8* TpNetworkDatagram::data() const {
	TpNetworkDatagramData* data = static_cast<TpNetworkDatagramData*>(data_);
    return data->buffer;
}

tpUInt64 TpNetworkDatagram::size() const {
	TpNetworkDatagramData* data = static_cast<TpNetworkDatagramData*>(data_);
    return data->bufferSize;
}

void TpNetworkDatagram::setData(const tpUInt8* data, tpUInt64 size) {
    TpNetworkDatagramData* d = static_cast<TpNetworkDatagramData*>(data_);
    delete[] d->buffer; // 释放旧数据
    d->buffer = nullptr;
    d->bufferSize = 0;

    if (data && size > 0) {
        d->buffer = new tpUInt8[size];
        memcpy(d->buffer, data, size);
        d->bufferSize = size;
    }
}

// 地址相关函数
TpString TpNetworkDatagram::senderAddress() const {
	TpNetworkDatagramData *data=static_cast<TpNetworkDatagramData*>(data_);
    return data->senderAddr;
}

tpUInt16 TpNetworkDatagram::senderPort() const {
	TpNetworkDatagramData *data=static_cast<TpNetworkDatagramData*>(data_);
    return data->senderPort;
}

void TpNetworkDatagram::setSender(const TpString& addr, tpUInt16 port) {
    TpNetworkDatagramData *data=static_cast<TpNetworkDatagramData*>(data_);
    data->senderAddr = addr; // std::string赋值自动释放旧内存[1](@ref)
    data->senderPort = port;
}

TpString TpNetworkDatagram::destinationAddress() const {
	TpNetworkDatagramData* data = static_cast<TpNetworkDatagramData*>(data_);
    return data->destAddr;
}

tpUInt16 TpNetworkDatagram::destinationPort() const {
	TpNetworkDatagramData* data = static_cast<TpNetworkDatagramData*>(data_);
    return data->destPort;
}

void TpNetworkDatagram::setDestination(const TpString& addr, tpUInt16 port) {
    TpNetworkDatagramData* data = static_cast<TpNetworkDatagramData*>(data_);
    data->destAddr = addr;
    data->destPort = port;
}

// 元数据函数
int TpNetworkDatagram::hopLimit() const {
	TpNetworkDatagramData* data = static_cast<TpNetworkDatagramData*>(data_);
    return data->hopLimit;
}

void TpNetworkDatagram::setHopLimit(int count) {
	TpNetworkDatagramData* data = static_cast<TpNetworkDatagramData*>(data_);
	data->hopLimit = (count >= -1) ? count : -1;
}

unsigned TpNetworkDatagram::interfaceIndex() const {
	TpNetworkDatagramData* data = static_cast<TpNetworkDatagramData*>(data_);
    return data->interfaceIndex;

}

void TpNetworkDatagram::setInterfaceIndex(unsigned index) {
	TpNetworkDatagramData* data = static_cast<TpNetworkDatagramData*>(data_);
	data->interfaceIndex = index;
}