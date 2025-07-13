#include "tpGateway.h"
#include <atomic>
#include <thread>
#include <mutex>
#include <condition_variable>

#include "nanomsg/nn.h"
#include "nanomsg/pubsub.h"
#include "nanomsg/pipeline.h"
#include "nanomsg/pair.h"

#include "tpVector.h"
#include "tpHash.h"
#include "tpString.h"

#include "tpGateway.h"
#include "nanomsg/nn.h"
#include "nanomsg/pubsub.h"
#include <mutex>
#include <thread>
#include <unordered_map>
#include <vector>
#include <memory>
#include <string>
#include <atomic>

#define DEFAULT_SERVER "tcp://127.0.0.1"

const uint16_t DEFAULT_PORT = 5555;
const uint32_t MAX_MSG_SIZE = 10 * 1024 * 1024; // 10MB

class GatewayClientImpl
{
public:
    GatewayClientImpl()
    {
        pubSocket = nn_socket(AF_SP, NN_PUB);
        subSocket = nn_socket(AF_SP, NN_SUB);
    }

    ~GatewayClientImpl()
    {
        disconnect();
    }

    bool connect(const tpString &server_address = "", uint16_t port = DEFAULT_PORT)
    {
        if (connected)
            return true;

        // 设置服务器地址
        if (server_address.empty())
        {
            this->serverAddress = DEFAULT_SERVER;
        }
        else
        {
            this->serverAddress = server_address;
        }
        this->port = port;

        // 构建连接地址
        tpString pub_addr = this->serverAddress + ":" + std::to_string(port);
        tpString sub_addr = this->serverAddress + ":" + std::to_string(port + 1);

        // 连接到服务器
        if (nn_connect(pubSocket, pub_addr.c_str()) < 0)
        {
            return false;
        }

        if (nn_connect(subSocket, sub_addr.c_str()) < 0)
        {
            return false;
        }

        // 设置接收缓冲区大小
        int recv_size = 10 * 1024 * 1024; // 10MB
        nn_setsockopt(subSocket, NN_SOL_SOCKET, NN_RCVBUF, &recv_size, sizeof(recv_size));

        // 订阅所有主题
        if (nn_setsockopt(subSocket, NN_SUB, NN_SUB_SUBSCRIBE, "", 0) < 0)
        {
            return false;
        }

        // 启动接收线程
        connected = true;
        running = true;
        receiverThread = std::thread(&GatewayClientImpl::receiverThreadFunc, this);

        return true;
    }

    void disconnect()
    {
        connected = false;
        running = false;

        if (receiverThread.joinable())
        {
            receiverThread.join();
        }

        if (pubSocket >= 0)
        {
            nn_close(pubSocket);
            pubSocket = -1;
        }

        if (subSocket >= 0)
        {
            nn_close(subSocket);
            subSocket = -1;
        }

        // 清空订阅
        std::lock_guard<std::mutex> lock(subMutex);
        subscriptions.clear();
    }

    bool publish(const char *topic, const void *data, uint32_t size)
    {
        if (!connected)
            return false;

        if (!data)
            return false;

        if (size == 0)
            return false;

        // 消息格式: [主题长度(4字节)][主题][数据]
        uint32_t topic_len = static_cast<uint32_t>(strlen(topic));
        uint32_t total_size = sizeof(uint32_t) + topic_len + size;

        if (total_size > MAX_MSG_SIZE)
        {
            return false;
        }

        void *msg = nn_allocmsg(total_size, 0);
        if (!msg)
            return false;

        char *ptr = static_cast<char *>(msg);
        *reinterpret_cast<uint32_t *>(ptr) = topic_len;
        memcpy(ptr + sizeof(uint32_t), topic, topic_len);
        memcpy(ptr + sizeof(uint32_t) + topic_len, data, size);

        int rc = nn_send(pubSocket, &msg, NN_MSG, NN_DONTWAIT);
        if (rc < 0)
        {
            nn_freemsg(msg);
            return false;
        }

        return true;
    }

    bool subscribe(const char *topic, tpGateway *receiver)
    {
        std::lock_guard<std::mutex> lock(subMutex);
        subscriptions[topic].emplace_back(Subscription(receiver, nullptr));
        return true;
    }

    bool subscribe(const char *topic, RecvDataFunc callback)
    {
        std::lock_guard<std::mutex> lock(subMutex);
        subscriptions[topic].emplace_back(Subscription(nullptr, callback));
        return true;
    }

    bool unsubscribe(const char *topic, tpGateway *receiver)
    {
        std::lock_guard<std::mutex> lock(subMutex);
        auto it = subscriptions.find(topic);
        if (it != subscriptions.end())
        {
            auto &subs = it->second;
            auto new_end = std::remove_if(subs.begin(), subs.end(),
                                          [receiver](const Subscription &sub)
                                          {
                                              return sub.receiver == receiver;
                                          });
            subs.erase(new_end, subs.end());

            if (subs.empty())
            {
                subscriptions.erase(it);
            }
        }
        return true;
    }

    bool unsubscribe(const char *topic, RecvDataFunc callback)
    {
        std::lock_guard<std::mutex> lock(subMutex);
        auto it = subscriptions.find(topic);
        if (it != subscriptions.end())
        {
            auto &subs = it->second;
            auto new_end = std::remove_if(subs.begin(), subs.end(),
                                          [&callback](const Subscription &sub)
                                          {
                                              return sub.callback &&
                                                     sub.callback.target_type() == callback.target_type();
                                          });
            subs.erase(new_end, subs.end());

            if (subs.empty())
            {
                subscriptions.erase(it);
            }
        }
        return true;
    }

private:
    struct Subscription
    {
        tpGateway *receiver = nullptr;
        RecvDataFunc callback;

        Subscription()
        {
        }
        Subscription(tpGateway *recv, RecvDataFunc callback)
            : receiver(recv), callback(callback)
        {
        }
    };

    int pubSocket = -1;
    int subSocket = -1;
    std::atomic<bool> running{false};
    std::atomic<bool> connected{false};

    std::thread receiverThread;
    tpString serverAddress;
    uint16_t port = DEFAULT_PORT;

    tpHash<tpString, tpVector<Subscription>> subscriptions;
    std::mutex subMutex;

    void receiverThreadFunc()
    {
        while (running)
        {
            char *msg = nullptr;
            int bytes = nn_recv(subSocket, &msg, NN_MSG, 0);

            if (bytes > 0)
            {
                // 消息格式: [主题长度(4字节)][主题][数据]
                if (bytes > 4)
                {
                    uint32_t topic_len = *reinterpret_cast<uint32_t *>(msg);
                    if (topic_len > 0 && static_cast<uint32_t>(bytes) > sizeof(uint32_t) + topic_len)
                    {
                        const char *topic = msg + sizeof(uint32_t);
                        const void *data = topic + topic_len;
                        uint32_t data_size = bytes - sizeof(uint32_t) - topic_len;

                        // 通知订阅者
                        notifySubscribers(topic, data, data_size);
                    }
                }
                nn_freemsg(msg);
            }
        }
    }

    void notifySubscribers(const char *topic, const void *data, uint32_t size)
    {
        tpVector<Subscription> subs;

        tpString recvTopic(topic);
        recvTopic.erase(std::remove_if(recvTopic.begin(),
                                       recvTopic.end(),
                                       [](char c)
                                       {
                                           return c < 32 || c > 126;
                                       }),
                        recvTopic.end());

        {
            std::lock_guard<std::mutex> lock(subMutex);
            auto it = subscriptions.find(recvTopic);
            if (it != subscriptions.end())
            {
                subs = it->second;
            }
        }

        for (auto &sub : subs)
        {
            if (sub.receiver)
            {
                sub.receiver->recvData(recvTopic.c_str(), data, size);
            }
            else if (sub.callback)
            {
                sub.callback(recvTopic.c_str(), data, size);
            }
        }
    }
};

// 全局客户端单例
static GatewayClientImpl gClient;
static std::mutex gClientMutex;

// 在全局变量区域添加
static std::atomic<bool> gInitialized{false};

// 公共接口实现
bool initializeGateway(const char *serverAddress)
{
    // 初始化检查
    if (gInitialized)
    {
        return true;
    }

    std::lock_guard<std::mutex> lock(gClientMutex);
    bool connectRes = false;
    // if (server_address)
    // {
    //     connectRes = g_client.connect(server_address);
    // }
    // else
    // {
    //      connectRes = g_client.connect();
    // }

    connectRes = gClient.connect();

    // 成功连接后设置标志
    if (connectRes)
    {
        gInitialized = true;
    }

    return connectRes;
}

bool publishGatewayData(const char *topic, const void *data, const uint32_t &size)
{
    std::lock_guard<std::mutex> lock(gClientMutex);
    return gClient.publish(topic, data, size);
}

bool subscribeGatewayData(const char *topic, tpGateway *obj)
{
    std::lock_guard<std::mutex> lock(gClientMutex);
    return gClient.subscribe(topic, obj);
}

bool subscribeGatewayData(const char *topic, RecvDataFunc func)
{
    std::lock_guard<std::mutex> lock(gClientMutex);
    return gClient.subscribe(topic, func);
}

bool unsubscribeGatewayData(const char *topic, tpGateway *obj)
{
    std::lock_guard<std::mutex> lock(gClientMutex);
    return gClient.unsubscribe(topic, obj);
}

bool unsubscribeGatewayData(const char *topic, RecvDataFunc func)
{
    std::lock_guard<std::mutex> lock(gClientMutex);
    return gClient.unsubscribe(topic, func);
}

void shutdownGateway()
{
    std::lock_guard<std::mutex> lock(gClientMutex);
    gClient.disconnect();
    gInitialized = false; // 重置初始化状态
}