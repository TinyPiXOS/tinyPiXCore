#ifndef __TP_DESKTOP_GLOBAL_H
#define __TP_DESKTOP_GLOBAL_H

#include "tpVector.h"
#include "tpString.h"
#include "Utils/StructPackage/DataPackagerMacro.h"
#include "Utils/StructPackage/DataPackager.hpp"

/// @brief 与桌面通信相关数据定义；若无桌面则无需使用

#ifndef StructSerializeFunc
#define StructSerializeFunc                                                             \
    virtual bool StructSerialize(PStructPackager &p) override                           \
    {                                                                                   \
        p << *this;                                                                     \
        if (p.data() && p.size() > 0)                                                   \
        {                                                                               \
            return true;                                                                \
        }                                                                               \
        return false;                                                                   \
    }                                                                                   \
    virtual bool StructDeserialize(const void *_data, const uint32_t &_len) override    \
    {                                                                                   \
        if (_data && _len > 0)                                                          \
        {                                                                               \
            PStructUnpackager up(_data, _len);                                          \
            up >> *this;                                                                \
            return true;                                                                \
        }                                                                               \
        return false;                                                                   \
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

// namespace tinyPiXDesktop
// {
/// @brief 应用通知桌面启动指定应用
static const char *RunAppTopic = "RunAppStructTopic";
struct RunApp : NetDataBase
{
    /// @brief 应用UUID
    std::string appUuid;
    /// @brief 启动参数列表
    std::vector<std::string> argList;

    RunApp() : NetDataBase(RunAppTopic)
    {
    }

    StructSerializeFunc;
};
STRUCT_PACKAGE(RunApp, dataHead_, appUuid, argList);

#endif
