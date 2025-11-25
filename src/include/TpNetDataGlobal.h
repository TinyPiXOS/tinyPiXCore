#ifndef __TP_DESKTOP_GLOBAL_H
#define __TP_DESKTOP_GLOBAL_H

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

/// @brief 应用通知桌面启动指定应用
static const char *TpRunAppKey = "TinyPiX.TpRunAppKey";
struct TpRunApp : NetDataBase
{
    /// @brief 应用UUID
    std::string appUuid;
    /// @brief 启动参数列表
    std::vector<std::string> argList;

    TpRunApp() : NetDataBase(TpRunAppKey)
    {
    }

    StructSerializeFunc;
};
STRUCT_PACKAGE(TpRunApp, dataHead_, appUuid, argList);

static const char *TpRPCRunAppProcessInfoKey = "TinyPiX.TpRPCRunAppProcessInfoKey";
struct TpRPCRunAppProcessInfo : NetDataBase
{
    std::vector<std::string> uuidList;
    std::vector<int32_t> pidList;

    TpRPCRunAppProcessInfo() : NetDataBase(TpRPCRunAppProcessInfoKey)
    {
    }

    StructSerializeFunc;
};
STRUCT_PACKAGE(TpRPCRunAppProcessInfo, dataHead_, uuidList, pidList);

/// @brief 小组件初始化消息；用于上层应用通知小组件渲染区域
static const char *TpInitWidgetsKey = "TinyPiX.TpInitWidgetsKey";
struct TpInitWidgets : NetDataBase
{
    int32_t width = 0;
    int32_t height = 0;

    int32_t depth = 0;
    int32_t Rmask = 0;
    int32_t Gmask = 0;
    int32_t Bmask = 0;
    int32_t Amask = 0;

    TpInitWidgets() : NetDataBase(TpInitWidgetsKey)
    {
    }

    StructSerializeFunc;
};
STRUCT_PACKAGE(TpInitWidgets, dataHead_, width, height);

/// @brief 上层应用通知小组件需要重新绘制并发送渲染指令
static const char *TpPaintWidgetsKey = "TinyPiX.TpPaintWidgetsKey";
struct TpPaintWidgets : NetDataBase
{
    TpPaintWidgets() : NetDataBase(TpPaintWidgetsKey)
    {
    }

    StructSerializeFunc;
};
STRUCT_PACKAGE(TpPaintWidgets, dataHead_);

/// @brief 小组件通知上层应用组件已绘制完毕；开始渲染
static const char *TpUpdateWidgetsKey = "TinyPiX.TpUpdateWidgetsKey";
struct TpUpdateWidgets : NetDataBase
{
    uint64_t sharedId = 0;

    TpUpdateWidgets() : NetDataBase(TpUpdateWidgetsKey)
    {
    }

    StructSerializeFunc;
};
STRUCT_PACKAGE(TpUpdateWidgets, dataHead_, sharedId);

#endif
