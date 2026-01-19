#include "TpGatewayClientREQ.h"
#include <thread>
#include <mutex>
#include <atomic>
#include <iostream>
#include <chrono>
#include "nanomsg/nn.h"
#include "nanomsg/reqrep.h"

// 平台特定的默认地址
#ifdef _WIN32
#define DEFAULT_REQ_ADDRESS "tcp://127.0.0.1:6555"
#else
#define DEFAULT_REQ_ADDRESS "ipc:///tmp/gateway_rep.ipc"
#endif

class GatewayClientREQImpl : public TpGatewayClientREQ
{
private:
    int32_t reqSocket_;
    std::atomic<bool> connected_{false};
    std::atomic<uint32_t> requestCount_{0};
    std::atomic<uint32_t> responseCount_{0};
    std::string serverAddress_;

    std::mutex socketMutex_; // 保护socket操作（REQ协议需要严格的请求-响应顺序）

public:
    GatewayClientREQImpl() : reqSocket_(-1) {}

    ~GatewayClientREQImpl() override
    {
        disconnect();
    }

    bool connect(const std::string &address) override
    {
        std::lock_guard<std::mutex> lock(socketMutex_);

        if (connected_)
        {
            return true;
        }

        // 使用提供的地址或默认地址
        serverAddress_ = address.empty() ? DEFAULT_REQ_ADDRESS : address;

        // 创建REQ套接字
        reqSocket_ = nn_socket(AF_SP, NN_REQ);
        if (reqSocket_ < 0)
        {
            std::cout << "创建REQ套接字失败: " << nn_strerror(nn_errno()) << std::endl;
            return false;
        }

        // 设置套接字选项
        int32_t recv_timeout = 5000;                 // 5秒接收超时
        int32_t send_timeout = 5000;                 // 5秒发送超时
        int32_t recv_buffer_size = 10 * 1024 * 1024; // 10MB
        int32_t send_buffer_size = 10 * 1024 * 1024; // 10MB

        nn_setsockopt(reqSocket_, NN_SOL_SOCKET, NN_RCVTIMEO,
                      &recv_timeout, sizeof(recv_timeout));
        nn_setsockopt(reqSocket_, NN_SOL_SOCKET, NN_SNDTIMEO,
                      &send_timeout, sizeof(send_timeout));
        nn_setsockopt(reqSocket_, NN_SOL_SOCKET, NN_RCVBUF,
                      &recv_buffer_size, sizeof(recv_buffer_size));
        nn_setsockopt(reqSocket_, NN_SOL_SOCKET, NN_SNDBUF,
                      &send_buffer_size, sizeof(send_buffer_size));

        // 启用重连机制
        int32_t reconnect_interval = 100; // 100ms重连间隔
        nn_setsockopt(reqSocket_, NN_SOL_SOCKET, NN_RECONNECT_IVL,
                      &reconnect_interval, sizeof(reconnect_interval));

        // 连接服务器
        if (nn_connect(reqSocket_, serverAddress_.c_str()) < 0)
        {
            std::cout << "连接服务器失败 " << serverAddress_
                      << ": " << nn_strerror(nn_errno()) << std::endl;
            nn_close(reqSocket_);
            reqSocket_ = -1;
            return false;
        }

        connected_ = true;
        std::cout << "成功连接到服务器: " << serverAddress_ << std::endl;
        return true;
    }

    void disconnect() override
    {
        std::lock_guard<std::mutex> lock(socketMutex_);

        if (reqSocket_ >= 0)
        {
            nn_close(reqSocket_);
            reqSocket_ = -1;
        }
        connected_ = false;
        std::cout << "已断开服务器连接" << std::endl;
    }

    std::string sendRequest(const std::string &request, int timeout_ms) override
    {
        std::lock_guard<std::mutex> lock(socketMutex_);

        if (!connected_ || reqSocket_ < 0)
        {
            std::cout << "客户端未连接，无法发送请求" << std::endl;
            return "";
        }

        // 设置临时超时
        if (timeout_ms > 0)
        {
            int32_t temp_timeout = timeout_ms;
            nn_setsockopt(reqSocket_, NN_SOL_SOCKET, NN_RCVTIMEO,
                          &temp_timeout, sizeof(temp_timeout));
        }

        // 发送请求
        int32_t sendBytes = nn_send(reqSocket_, request.c_str(), request.size(), 0);
        if (sendBytes < 0)
        {
            std::cout << "请求发送失败: " << nn_strerror(nn_errno()) << std::endl;
            return "";
        }

        requestCount_++;
        std::cout << "请求已发送，数据大小: " << request.size() << " 字节" << std::endl;

        // 接收响应
        char *responseMsg = nullptr;
        int32_t recvBytes = nn_recv(reqSocket_, &responseMsg, NN_MSG, 0);

        if (recvBytes < 0)
        {
            std::cout << "响应接收失败: " << nn_strerror(nn_errno()) << std::endl;
            return "";
        }

        responseCount_++;
        std::string response(responseMsg, recvBytes);
        nn_freemsg(responseMsg);

        std::cout << "响应已接收，数据大小: " << response.size() << " 字节" << std::endl;
        return response;
    }

    bool isConnected() const override
    {
        return connected_.load();
    }

    int32_t getRequestCount() const override
    {
        return static_cast<int32_t>(requestCount_.load());
    }

    int32_t getResponseCount() const override
    {
        return static_cast<int32_t>(responseCount_.load());
    }
};

std::shared_ptr<TpGatewayClientREQ> createGatewayREQClient()
{
    return std::make_shared<GatewayClientREQImpl>();
}