/*///------------------------------------------------------------------------------------------------------------------------//
		网卡数据的封装
说 明 : 
日 期 : 2025.6.29

/*///------------------------------------------------------------------------------------------------------------------------//

#include "tpNetworkDatagram.h"


struct tpNetworkDatagramData{

	tpUInt8* buffer;    // 数据负载（动态分配）
    tpUInt64 bufferSize;            // 数据长度
    tpString senderAddr;      // 发送方IP（字符串格式）
    tpString destAddr;        // 目标IP（字符串格式）
    tpUInt16 senderPort;     // 发送方端口
    tpUInt16 destPort ;       // 目标端口
    int hopLimit;           // TTL（-1=系统默认）
    unsigned interfaceIndex; // 网络接口索引
	tpNetworkDatagramData()
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


tpNetworkDatagram::tpNetworkDatagram()
{
	data_ = new tpNetworkDatagramData();
}

// 构造函数
tpNetworkDatagram::tpNetworkDatagram(
    const tpUInt8* buff, 
    tpUInt64 size, 
    const tpString& destAddr, 
    tpUInt16 destPort
) {
    data_ = new tpNetworkDatagramData();
	tpNetworkDatagramData *data=static_cast<tpNetworkDatagramData*>(data_);
	if(!data)
	{
		fprintf(stderr,"[Error]: tpNetworkDatagram creat error\n");
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
tpNetworkDatagram::~tpNetworkDatagram() {
   tpNetworkDatagramData *data=static_cast<tpNetworkDatagramData*>(data_);
    delete[] data->buffer;  // 释放数据缓冲区
    delete data;            // 释放结构体本身
}

// 拷贝构造（深拷贝）
tpNetworkDatagram::tpNetworkDatagram(const tpNetworkDatagram& other) 
{
	tpNetworkDatagramData *otherData=static_cast<tpNetworkDatagramData*>(other.data_);
    data_ = new tpNetworkDatagramData(*otherData); // 拷贝基础类型
    
    // 深拷贝数据缓冲区
    tpNetworkDatagramData* d = static_cast<tpNetworkDatagramData*>(data_);
    if (otherData->buffer && otherData->bufferSize > 0) {
        d->buffer = new tpUInt8[otherData->bufferSize];
        memcpy(d->buffer, otherData->buffer, otherData->bufferSize);
    }
}

// 赋值运算符（深拷贝）
tpNetworkDatagram& tpNetworkDatagram::operator=(const tpNetworkDatagram& other) {
    if (this == &other) return *this;
    
    tpNetworkDatagramData* d = static_cast<tpNetworkDatagramData*>(data_);
    tpNetworkDatagramData* otherData = static_cast<tpNetworkDatagramData*>(other.data_);
    
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



tpBool tpNetworkDatagram::isNull() const {
    const tpNetworkDatagramData* d = static_cast<tpNetworkDatagramData*>(data_);
    return ((!d->buffer && d->senderAddr.empty() && d->destAddr.empty())?TP_TRUE:TP_FALSE);
}

tpBool tpNetworkDatagram::isValid() const {
    const tpNetworkDatagramData* d = static_cast<tpNetworkDatagramData*>(data_);
    return ((d->buffer || !d->senderAddr.empty() || !d->destAddr.empty())?TP_TRUE:TP_FALSE);
}

const tpUInt8* tpNetworkDatagram::data() const {
	tpNetworkDatagramData* data = static_cast<tpNetworkDatagramData*>(data_);
    return data->buffer;
}

tpUInt64 tpNetworkDatagram::size() const {
	tpNetworkDatagramData* data = static_cast<tpNetworkDatagramData*>(data_);
    return data->bufferSize;
}

void tpNetworkDatagram::setData(const tpUInt8* data, tpUInt64 size) {
    tpNetworkDatagramData* d = static_cast<tpNetworkDatagramData*>(data_);
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
tpString tpNetworkDatagram::senderAddress() const {
	tpNetworkDatagramData *data=static_cast<tpNetworkDatagramData*>(data_);
    return data->senderAddr;
}

tpUInt16 tpNetworkDatagram::senderPort() const {
	tpNetworkDatagramData *data=static_cast<tpNetworkDatagramData*>(data_);
    return data->senderPort;
}

void tpNetworkDatagram::setSender(const tpString& addr, tpUInt16 port) {
    tpNetworkDatagramData *data=static_cast<tpNetworkDatagramData*>(data_);
    data->senderAddr = addr; // std::string赋值自动释放旧内存[1](@ref)
    data->senderPort = port;
}

tpString tpNetworkDatagram::destinationAddress() const {
	tpNetworkDatagramData* data = static_cast<tpNetworkDatagramData*>(data_);
    return data->destAddr;
}

tpUInt16 tpNetworkDatagram::destinationPort() const {
	tpNetworkDatagramData* data = static_cast<tpNetworkDatagramData*>(data_);
    return data->destPort;
}

void tpNetworkDatagram::setDestination(const tpString& addr, tpUInt16 port) {
    tpNetworkDatagramData* data = static_cast<tpNetworkDatagramData*>(data_);
    data->destAddr = addr;
    data->destPort = port;
}

// 元数据函数
int tpNetworkDatagram::hopLimit() const {
	tpNetworkDatagramData* data = static_cast<tpNetworkDatagramData*>(data_);
    return data->hopLimit;
}

void tpNetworkDatagram::setHopLimit(int count) {
	tpNetworkDatagramData* data = static_cast<tpNetworkDatagramData*>(data_);
	data->hopLimit = (count >= -1) ? count : -1;
}

unsigned tpNetworkDatagram::interfaceIndex() const {
	tpNetworkDatagramData* data = static_cast<tpNetworkDatagramData*>(data_);
    return data->interfaceIndex;

}

void tpNetworkDatagram::setInterfaceIndex(unsigned index) {
	tpNetworkDatagramData* data = static_cast<tpNetworkDatagramData*>(data_);
	data->interfaceIndex = index;
}