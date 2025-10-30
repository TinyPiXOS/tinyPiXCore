#include "TpGatewayServer.h"
#include "nanomsg/nn.h"
#include "nanomsg/pubsub.h"
#include <unordered_map>
#include <mutex>
#include <thread>
#include <queue>
#include <condition_variable>
#include <chrono>
#include <atomic>
#include <cstring>
#include "TpString.h"

// 平台特定的IPC地址
#ifdef _WIN32
#define IPC_ADDRESS "tcp://127.0.0.1"
#else
#define IPC_ADDRESS "ipc:///tmp/gateway.ipc"
#endif

// 消息最大尺寸
const uint32_t MAX_MSG_SIZE = 10 * 1024 * 1024; // 10MB

class GatewayServerImpl : public TpGatewayServer
{
    int32_t pubSocket_ = -1;
    int32_t subSocket_ = -1;
    std::atomic<bool> running_{false};
    std::atomic<uint32_t> messageCount_{0};

    std::thread receiverThread_;
    std::thread publisherThread_;

    struct Message
    {
        char *data = nullptr;
        int32_t size = 0;

        Message()
        {
        }
        Message(char *msg, int32_t size)
            : data(msg), size(size)
        {
        }
    };

    std::queue<Message> messagequeue_;
    std::mutex queuemutex_;
    std::condition_variable queuecv_;

    void receiverThread()
    {
        while (running_)
        {
            char *msg = nullptr;
            int32_t bytes = nn_recv(subSocket_, &msg, NN_MSG, 0);
            if (bytes > 0)
            {
                {
                    std::lock_guard<std::mutex> lock(queuemutex_);
                    messagequeue_.push(Message(msg, bytes));
                }
                queuecv_.notify_one();
                messageCount_++;
            }
            else
            {
                // 短暂休眠避免忙等待
                std::this_thread::sleep_for(std::chrono::milliseconds(10));
            }
        }
    }

    void publisherThread()
    {
        while (running_)
        {
            Message msg;
            {
                std::unique_lock<std::mutex> lock(queuemutex_);
                queuecv_.wait(lock, [this]
                              { return !messagequeue_.empty() || !running_; });

                if (!running_)
                    break;

                if (!messagequeue_.empty())
                {
                    msg = messagequeue_.front();
                    messagequeue_.pop();
                }
                else
                {
                    continue;
                }
            }

            if (msg.data && msg.size > 0)
            {
                std::cout << "收到发布数据，数据长度：" << msg.size << std::endl;
                // 处理消息并转发
                if (msg.size > 4)
                {
                    uint32_t topicLen = *reinterpret_cast<uint32_t *>(msg.data);
                    std::cout << "收到发布数据，主题长度：" << topicLen << std::endl;

                    if (topicLen > 0 && topicLen < (msg.size - sizeof(uint32_t)))
                    {
                        const char *topic = msg.data + sizeof(uint32_t);
                        TpString topicString(topic);

                        std::cout << "收到发布数据，主题为：" << topicString << std::endl;

                        // 广播消息
                        nn_send(pubSocket_, msg.data, msg.size, 0);
                    }
                }
                nn_freemsg(msg.data);
            }
        }

        // 清理剩余消息
        std::lock_guard<std::mutex> lock(queuemutex_);
        while (!messagequeue_.empty())
        {
            auto &msg = messagequeue_.front();
            if (msg.data)
                nn_freemsg(msg.data);
            messagequeue_.pop();
        }
    }

public:
    GatewayServerImpl() = default;

    ~GatewayServerImpl() override
    {
        stop();
    }

    bool start(uint16_t tcpPort) override
    {
        if (running_)
            return true;

        // 创建套接字
        pubSocket_ = nn_socket(AF_SP, NN_PUB);
        subSocket_ = nn_socket(AF_SP, NN_SUB);

        if (pubSocket_ < 0 || subSocket_ < 0)
        {
            stop();
            return false;
        }

        // 设置接收缓冲区大小
        int32_t recv_size = 10 * 1024 * 1024; // 10MB
        nn_setsockopt(subSocket_, NN_SOL_SOCKET, NN_RCVBUF, &recv_size, sizeof(recv_size));
        nn_setsockopt(pubSocket_, NN_SOL_SOCKET, NN_SNDBUF, &recv_size, sizeof(recv_size));

        // 设置IPC连接
        // if (nn_bind(subSocket_, IPC_ADDRESS) < 0)
        // {
        // 允许IPC绑定失败
        // }

        // 绑定TCP端口
        // std::string pub_addr = "tcp://*:" + std::to_string(tcpPort);
        // std::string sub_addr = "tcp://*:" + std::to_string(tcpPort + 1);

        std::string sub_addr = "tcp://*:" + std::to_string(tcpPort);
        std::string pub_addr = "tcp://*:" + std::to_string(tcpPort + 1);

        if (nn_bind(pubSocket_, pub_addr.c_str()) < 0)
        {
            stop();
            return false;
        }

        if (nn_bind(subSocket_, sub_addr.c_str()) < 0)
        {
            stop();
            return false;
        }

        // 设置订阅所有主题
        if (nn_setsockopt(subSocket_, NN_SUB, NN_SUB_SUBSCRIBE, "", 0) < 0)
        {
            stop();
            return false;
        }

        // 设置接收超时
        int32_t timeout = 100; // 100ms
        nn_setsockopt(subSocket_, NN_SOL_SOCKET, NN_RCVTIMEO, &timeout, sizeof(timeout));

        // 启动工作线程
        running_ = true;
        receiverThread_ = std::thread(&GatewayServerImpl::receiverThread, this);
        publisherThread_ = std::thread(&GatewayServerImpl::publisherThread, this);

        return true;
    }

    void stop() override
    {
        running_ = false;
        queuecv_.notify_one();

        if (receiverThread_.joinable())
        {
            receiverThread_.join();
        }

        if (publisherThread_.joinable())
        {
            publisherThread_.join();
        }

        if (pubSocket_ >= 0)
        {
            nn_close(pubSocket_);
            pubSocket_ = -1;
        }

        if (subSocket_ >= 0)
        {
            nn_close(subSocket_);
            subSocket_ = -1;
        }
    }

    size_t getMessageRate() override
    {
        auto count = messageCount_.exchange(0);
        return static_cast<size_t>(count);
    }
};

// 创建GatewayServer实例
std::shared_ptr<TpGatewayServer> createGatewayServer()
{
    return std::make_shared<GatewayServerImpl>();
}