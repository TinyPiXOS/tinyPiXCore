#ifndef __GATEWAY_CLIENT_REQ_H
#define __GATEWAY_CLIENT_REQ_H

#include <string>
#include <atomic>
#include <memory>
#include <cstdint>

class TpGatewayClientREQ
{
public:
    virtual ~TpGatewayClientREQ() = default;

    // 连接到服务器
    virtual bool connect(const std::string &address = "") = 0;

    // 断开连接
    virtual void disconnect() = 0;

    // 发送请求并接收响应（同步方式）
    virtual std::string sendRequest(const std::string &request, int timeout_ms = 5000) = 0;

    // 获取连接状态
    virtual bool isConnected() const = 0;

    // 获取发送的请求数量
    virtual int32_t getRequestCount() const = 0;

    // 获取接收的响应数量
    virtual int32_t getResponseCount() const = 0;
};

// 创建客户端实例的工厂函数
std::shared_ptr<TpGatewayClientREQ> createGatewayREQClient();

#endif // __GATEWAY_CLIENT_REQ_H