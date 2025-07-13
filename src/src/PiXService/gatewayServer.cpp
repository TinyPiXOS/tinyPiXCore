#include "gatewayServer.h"
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

// 平台特定的IPC地址
#ifdef _WIN32
#define IPC_ADDRESS "tcp://127.0.0.1"
#else
#define IPC_ADDRESS "ipc:///tmp/gateway.ipc"
#endif

// 消息最大尺寸
const uint32_t MAX_MSG_SIZE = 10 * 1024 * 1024; // 10MB

class GatewayServerImpl : public GatewayServer
{
    int32_t pub_socket_ = -1;
    int32_t sub_socket_ = -1;
    std::atomic<bool> running_{false};
    std::atomic<uint32_t> message_count_{0};

    std::thread receiver_thread_;
    std::thread publisher_thread_;

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

    std::queue<Message> message_queue_;
    std::mutex queue_mutex_;
    std::condition_variable queue_cv_;

    void receiverThread()
    {
        while (running_)
        {
            char *msg = nullptr;
            int32_t bytes = nn_recv(sub_socket_, &msg, NN_MSG, 0);
            if (bytes > 0)
            {
                {
                    std::lock_guard<std::mutex> lock(queue_mutex_);
                    message_queue_.push(Message(msg, bytes));
                }
                queue_cv_.notify_one();
                message_count_++;
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
                std::unique_lock<std::mutex> lock(queue_mutex_);
                queue_cv_.wait(lock, [this]
                               { return !message_queue_.empty() || !running_; });

                if (!running_)
                    break;

                if (!message_queue_.empty())
                {
                    msg = message_queue_.front();
                    message_queue_.pop();
                }
                else
                {
                    continue;
                }
            }

            if (msg.data && msg.size > 0)
            {
                // 处理消息并转发
                if (msg.size > 4)
                {
                    uint32_t topic_len = *reinterpret_cast<uint32_t *>(msg.data);
                    if (topic_len > 0 && topic_len < (msg.size - sizeof(uint32_t)))
                    {
                        const char *topic = msg.data + sizeof(uint32_t);
                        const char *payload = topic + topic_len;
                        uint32_t payload_size = msg.size - sizeof(uint32_t) - topic_len;

                        // 广播消息
                        nn_send(pub_socket_, msg.data, msg.size, 0);
                    }
                }
                nn_freemsg(msg.data);
            }
        }

        // 清理剩余消息
        std::lock_guard<std::mutex> lock(queue_mutex_);
        while (!message_queue_.empty())
        {
            auto &msg = message_queue_.front();
            if (msg.data)
                nn_freemsg(msg.data);
            message_queue_.pop();
        }
    }

public:
    GatewayServerImpl() = default;

    ~GatewayServerImpl() override
    {
        stop();
    }

    bool start(uint16_t tcp_port) override
    {
        if (running_)
            return true;

        // 创建套接字
        pub_socket_ = nn_socket(AF_SP, NN_PUB);
        sub_socket_ = nn_socket(AF_SP, NN_SUB);

        if (pub_socket_ < 0 || sub_socket_ < 0)
        {
            stop();
            return false;
        }

        // 设置接收缓冲区大小
        int32_t recv_size = 10 * 1024 * 1024; // 10MB
        nn_setsockopt(sub_socket_, NN_SOL_SOCKET, NN_RCVBUF, &recv_size, sizeof(recv_size));
        nn_setsockopt(pub_socket_, NN_SOL_SOCKET, NN_SNDBUF, &recv_size, sizeof(recv_size));

        // 设置IPC连接
        // if (nn_bind(sub_socket_, IPC_ADDRESS) < 0)
        // {
            // 允许IPC绑定失败
        // }

        // 绑定TCP端口
        // std::string pub_addr = "tcp://*:" + std::to_string(tcp_port);
        // std::string sub_addr = "tcp://*:" + std::to_string(tcp_port + 1);

        std::string sub_addr = "tcp://*:" + std::to_string(tcp_port);
        std::string pub_addr = "tcp://*:" + std::to_string(tcp_port + 1);

        if (nn_bind(pub_socket_, pub_addr.c_str()) < 0)
        {
            stop();
            return false;
        }

        if (nn_bind(sub_socket_, sub_addr.c_str()) < 0)
        {
            stop();
            return false;
        }

        // 设置订阅所有主题
        if (nn_setsockopt(sub_socket_, NN_SUB, NN_SUB_SUBSCRIBE, "", 0) < 0)
        {
            stop();
            return false;
        }

        // 设置接收超时
        int32_t timeout = 100; // 100ms
        nn_setsockopt(sub_socket_, NN_SOL_SOCKET, NN_RCVTIMEO, &timeout, sizeof(timeout));

        // 启动工作线程
        running_ = true;
        receiver_thread_ = std::thread(&GatewayServerImpl::receiverThread, this);
        publisher_thread_ = std::thread(&GatewayServerImpl::publisherThread, this);

        return true;
    }

    void stop() override
    {
        running_ = false;
        queue_cv_.notify_one();

        if (receiver_thread_.joinable())
        {
            receiver_thread_.join();
        }

        if (publisher_thread_.joinable())
        {
            publisher_thread_.join();
        }

        if (pub_socket_ >= 0)
        {
            nn_close(pub_socket_);
            pub_socket_ = -1;
        }

        if (sub_socket_ >= 0)
        {
            nn_close(sub_socket_);
            sub_socket_ = -1;
        }
    }

    size_t getMessageRate() override
    {
        auto count = message_count_.exchange(0);
        return static_cast<size_t>(count);
    }
};

// 创建GatewayServer实例
std::shared_ptr<GatewayServer> createGatewayServer()
{
    return std::make_shared<GatewayServerImpl>();
}