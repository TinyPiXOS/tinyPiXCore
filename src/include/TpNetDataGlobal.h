#ifndef __TP_NET_DATA_GLOBAL_H
#define __TP_NET_DATA_GLOBAL_H

#include "TpVector.h"
#include "TpString.h"
#include "TpCore/StructPackage/DataPackagerMacro.h"
#include "TpCore/StructPackage/DataPackager.hpp"

/// @brief 与服务通信相关数据定义；

#ifndef StructSerializeFunc
#define StructSerializeFunc                                                          \
    virtual bool StructSerialize(PStructPackager &p) override                        \
    {                                                                                \
        p << *this;                                                                  \
        if (p.data() && p.size() > 0)                                                \
        {                                                                            \
            return true;                                                             \
        }                                                                            \
        return false;                                                                \
    }                                                                                \
    virtual bool StructDeserialize(const void *_data, const uint32_t &_len) override \
    {                                                                                \
        if (_data && _len > 0)                                                       \
        {                                                                            \
            PStructUnpackager up(_data, _len);                                       \
            up >> *this;                                                             \
            return true;                                                             \
        }                                                                            \
        return false;                                                                \
    }
#endif

struct NetDataHeader
{
    std::string type_;

    NetDataHeader(const std::string &_type = "")
        : type_(_type)
    {
    }

    virtual ~NetDataHeader() {}
};
STRUCT_PACKAGE(NetDataHeader, type_);

struct NetDataBase
{
    NetDataBase(const std::string &_type)
        : dataHead_(_type)
    {
    }

    virtual ~NetDataBase() {}

    virtual bool StructSerialize(PStructPackager &p) = 0;
    virtual bool StructDeserialize(const void *_data, const uint32_t &_len) = 0;

    NetDataHeader dataHead_;
};
STRUCT_PACKAGE(NetDataBase, dataHead_);

/// @brief 接收数据接口；用于反序列化出结构体Topic
struct RecvTopicDataImpl : NetDataBase
{
    RecvTopicDataImpl() : NetDataBase("")
    {
    }

    StructSerializeFunc;
};
STRUCT_PACKAGE(RecvTopicDataImpl, dataHead_);

#endif
