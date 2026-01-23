#ifndef __TP_EVENT_LOOP_H
#define __TP_EVENT_LOOP_H

#include <thread>
#include <vector>
#include <mutex>
#include <atomic>
#include <map>
#include <functional>
#include <unordered_map>

class TpEventSource;
class TpEventLoop {
public:
    TpEventLoop();
    ~TpEventLoop();

public:
	/// @brief 阻塞模式运行事件循环，直到 stop 被调用
    void run();
	/// @brief 异步模式运行事件循环，在新线程中执行
    void runAsync();
	/// @brief 停止事件循环
    void stop();

    // ===== 供 TpEventSource 调用（不是给用户）=====
    void add(TpEventSource *src);
    void update(TpEventSource *src);
    void remove(TpEventSource *src);

    // ===== 高级能力（Qt Core 等价）=====
    void wakeup();               // eventfd
    void postTask(std::function<void()> task);

	static TpEventLoop& defaultLoop();

private:
    void loop();
    void handleWakeup();

    uint32_t toEpoll(uint32_t ev) const;
    uint32_t fromEpoll(uint32_t ev) const;

private:
    int epollFd_{-1};
    int wakeFd_{-1};

    std::atomic<bool> running_{false};
    std::thread loopThread_;
    std::mutex mutex_;

    std::unordered_map<int, TpEventSource*> sources_;
};




#endif