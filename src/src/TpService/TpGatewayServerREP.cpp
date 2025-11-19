#include "TpGatewayServerREP.h"
#include <thread>
#include <unordered_map>
#include <functional>
#include <mutex>
#include <condition_variable>
#include <queue>
#include <atomic>
#include <iostream>
#include "nanomsg/nn.h"
#include "nanomsg/reqrep.h"

// 平台特定的IPC地址
#ifdef _WIN32
#define REP_IPC_ADDRESS "tcp://127.0.0.1"
#else
#define REP_IPC_ADDRESS "ipc:///tmp/gateway_rep.ipc"
#endif

class GatewayServerREPImpl : public TpGatewayServerREP
{
    int32_t repSockets_;
    std::atomic<bool> running_{false};
    std::atomic<uint32_t> messageCount_{0};
    std::atomic<uint32_t> requestCount_{0};

    std::thread workerThread_;
    // std::vector<std::thread> workerThreads_;

    // 请求处理回调函数类型
    using RequestHandler = std::function<std::string(const std::string &)>;
    std::unordered_map<std::string, RequestHandler> requestHandlers_;
    std::mutex handlerMutex_;

    void workerThread()
    {
        while (running_)
        {
            char *msg = nullptr;
            int32_t bytes = nn_recv(repSockets_, &msg, NN_MSG, 0);

            if (bytes > 0)
            {
                requestCount_++;
                std::string requestData(msg, bytes);
                // std::string clientId = generateClientId(threadId);
                std::cout << " 收到请求！！！！" << std::endl;
                std::string responseData = processRequest(requestData, "");

                // 在同一线程中发送响应
                int32_t sendBytes = nn_send(repSockets_, responseData.c_str(), responseData.size(), 0);

                if (sendBytes < 0)
                {
                    std::cout << " 响应发送失败: " << nn_strerror(nn_errno()) << std::endl;
                }
                else
                {
                    std::cout << " 响应发送失败: " << nn_strerror(nn_errno()) << std::endl;

                }

                nn_freemsg(msg);
                messageCount_++;
            }
            else if (bytes < 0 && nn_errno() != EAGAIN && running_)
            {
                std::this_thread::sleep_for(std::chrono::milliseconds(10));
            }
        }
    }

    std::string generateClientId(int workerId)
    {
        static std::atomic<uint64_t> counter{0};
        return "worker_" + std::to_string(workerId) + "_client_" +
               std::to_string(++counter);
    }

    std::string processRequest(const std::string &requestData, const std::string &clientId)
    {
        std::cout << "处理请求来自 " << "ECHO: " + requestData
                  << ", 数据大小: " << requestData.size()
                  << " 字节" << std::endl;

        return "ECHO: " + requestData;
    }

public:
    GatewayServerREPImpl() = default;

    ~GatewayServerREPImpl() override
    {
        stop();
    }

    bool start(uint16_t tcpPort) override
    {
        if (running_)
            return true;

        int workerCount = 4; // 4个工作线程

        // for (int i = 0; i < workerCount; ++i)
        {
            // 创建REP套接字
            repSockets_ = nn_socket(AF_SP_RAW, NN_REP);
            if (repSockets_ < 0)
            {
                std::cout << "创建REP套接字失败: " << nn_strerror(nn_errno()) << std::endl;
                return false;
            }

            // 设置套接字选项
            int32_t recv_timeout = 1000;                 // 1秒接收超时
            int32_t send_timeout = 1000;                 // 1秒发送超时
            int32_t recv_buffer_size = 10 * 1024 * 1024; // 10MB

            nn_setsockopt(repSockets_, NN_SOL_SOCKET, NN_RCVTIMEO,
                          &recv_timeout, sizeof(recv_timeout));
            nn_setsockopt(repSockets_, NN_SOL_SOCKET, NN_SNDTIMEO,
                          &send_timeout, sizeof(send_timeout));
            nn_setsockopt(repSockets_, NN_SOL_SOCKET, NN_RCVBUF,
                          &recv_buffer_size, sizeof(recv_buffer_size));
            nn_setsockopt(repSockets_, NN_SOL_SOCKET, NN_SNDBUF,
                          &recv_buffer_size, sizeof(recv_buffer_size));

            // 绑定地址
            std::string tcp_addr = "tcp://*:" + std::to_string(tcpPort);
            std::string ipc_addr = REP_IPC_ADDRESS;

            // 优先尝试IPC绑定（更高效）
            if (nn_bind(repSockets_, ipc_addr.c_str()) < 0)
            {
                std::cout << "IPC绑定失败，使用TCP: " << nn_strerror(nn_errno()) << std::endl;
                // IPC失败时使用TCP
                if (nn_bind(repSockets_, tcp_addr.c_str()) < 0)
                {
                    std::cout << "TCP绑定失败: " << nn_strerror(nn_errno()) << std::endl;
                    stop();
                    return false;
                }
            }
        }

        running_ = true;

        // 启动请求处理线程
        workerThread_ = std::thread(&GatewayServerREPImpl::workerThread, this);

        // 启动工作线程
        // for (size_t i = 0; i < repSockets_.size(); ++i)
        // {
        //     workerThreads_.emplace_back(&GatewayServerREPImpl::workerThread, this, i, repSockets_[i]);
        // }

        std::cout << "启动REP工作线程" << std::endl;
        return true;
    }

    void stop() override
    {
        running_ = false;

        if (workerThread_.joinable())
        {
            workerThread_.join();
            // }
            // for (auto &thread : workerThreads_)
            // {
            //     if (thread.joinable())
            //         thread.join();
            // }
            // workerThreads_.clear();

            // for (auto socket : repSockets_)
            // {
            //     nn_close(socket);
            // }
            // repSockets_.clear();
        }
        nn_close(repSockets_);
    }

    void registerRequestHandler(const std::string &command, RequestHandler handler)
    {
        std::lock_guard<std::mutex> lock(handlerMutex_);
        requestHandlers_[command] = handler;
    }

    int32_t getRequestCount() const override
    {
        return static_cast<int32_t>(requestCount_.load());
    }
};

std::shared_ptr<TpGatewayServerREP> createGatewayREPServer()
{
    return std::make_shared<GatewayServerREPImpl>();
}