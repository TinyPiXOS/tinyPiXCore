#ifndef __TP_NETWORK_DATAGRAM_H
#define __TP_NETWORK_DATAGRAM_H

#include "tpUtils.h"
#include "tpString.h"

TP_DEF_VOID_TYPE_VAR(ItpNetworkDatagramData);


class tpNetworkDatagram{
public:
	tpNetworkDatagram();
	tpNetworkDatagram(const tpUInt8* data =nullptr, tpUInt64 size=0, 
                      const tpString& destAddr = "", 
                      tpUInt16 destPort = 0);
	~tpNetworkDatagram();

	tpNetworkDatagram(const tpNetworkDatagram& other);
	tpNetworkDatagram &operator=(const tpNetworkDatagram &other);

public:
	tpBool isNull() const;               // 是否为空
    tpBool isValid() const;              // 是否有效（含数据或地址）
    const tpUInt8* data() const;       // 获取数据指针
    tpUInt64 size() const;               // 获取数据长度
    void setData(const tpUInt8* data, tpUInt64 size); // 深拷贝设置数据

    // 地址/端口访问
    tpString senderAddress() const;
    tpUInt16 senderPort() const;
    void setSender(const tpString& addr, tpUInt16 port = 0);

    tpString destinationAddress() const;
    tpUInt16 destinationPort() const;
    void setDestination(const tpString& addr, tpUInt16 port = 0);

    // 元数据（TTL、接口索引）
    int hopLimit() const;
    void setHopLimit(int count);

    unsigned interfaceIndex() const;
    void setInterfaceIndex(unsigned index);


private:
	ItpNetworkDatagramData *data_;
};



#endif
