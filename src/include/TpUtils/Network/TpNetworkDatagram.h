#ifndef __TP_NETWORK_DATAGRAM_H
#define __TP_NETWORK_DATAGRAM_H

#include <TpCore.h>
#include "TpString.h"

TP_DEF_VOID_TYPE_VAR(ItpNetworkDatagramData);

class TpNetworkDatagram
{
public:
    TpNetworkDatagram();
    TpNetworkDatagram(const tpUInt8 *data = nullptr, tpUInt64 size = 0,
                      const TpString &destAddr = "",
                      tpUInt16 destPort = 0);
    ~TpNetworkDatagram();

    TpNetworkDatagram(const TpNetworkDatagram &other);
    TpNetworkDatagram &operator=(const TpNetworkDatagram &other);

public:
    tpBool isNull() const;                            // 是否为空
    tpBool isValid() const;                           // 是否有效（含数据或地址）
    const tpUInt8 *data() const;                      // 获取数据指针
    tpUInt64 size() const;                            // 获取数据长度
    void setData(const tpUInt8 *data, tpUInt64 size); // 深拷贝设置数据

    // 地址/端口访问
    TpString senderAddress() const;
    tpUInt16 senderPort() const;
    void setSender(const TpString &addr, tpUInt16 port = 0);

    TpString destinationAddress() const;
    tpUInt16 destinationPort() const;
    void setDestination(const TpString &addr, tpUInt16 port = 0);

    // 元数据（TTL、接口索引）
    int hopLimit() const;
    void setHopLimit(int count);

    unsigned interfaceIndex() const;
    void setInterfaceIndex(unsigned index);

private:
    ItpNetworkDatagramData *data_;
};

#endif
