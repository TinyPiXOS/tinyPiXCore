#ifndef __TP_GATEWAY_H
#define __TP_GATEWAY_H

#include "tpString.h"
#include <functional>

/// @brief 网关数据接收接口类
class tpGateway
{
public:
    virtual void recvData(const char *topic, const void *data, const uint32_t &size) = 0;
    ~tpGateway() {}
};
using RecvDataFunc = std::function<void(const char *, const void *, uint32_t)>;

/// @brief 初始化网关；目前只支持本机通信
/// @param serverAddress 服务端IP；目前给入无效；仅支持本机通信
/// @return 初始化结果
bool initializeGateway(const char *serverAddress = nullptr);

/// @brief 发布消息;不支持发送空数据
bool publishGatewayData(const char *topic, const void *data, const uint32_t &size);

/// @brief 订阅接口
bool subscribeGatewayData(const char *topic, tpGateway *obj);
bool subscribeGatewayData(const char *topic, RecvDataFunc func);

/// @brief 取消订阅
bool unsubscribeGatewayData(const char *topic, tpGateway *obj);
bool unsubscribeGatewayData(const char *topic, RecvDataFunc func);

/// @brief 关闭网关连接
void shutdownGateway();

#endif