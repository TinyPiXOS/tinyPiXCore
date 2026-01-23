#ifndef _TP_SOCKET_NOTIFIER_MANAGER_H_
#define _TP_SOCKET_NOTIFIER_MANAGER_H_

#include "TpSocket.h"
#include "TpSocketNotifier.h"
#include <thread>
#include <vector>
#include <mutex>
#include <atomic>
#include <map>
#include <unordered_map>

class TpAbstractEventDispatcher
{
public:
    static TpAbstractEventDispatcher &instance();
    void registerNotifier(TpSocketNotifier *notifier);
    void unregisterNotifier(TpSocketNotifier *notifier);

    void stop();

private:
    TpAbstractEventDispatcher();
    ~TpAbstractEventDispatcher();
    TpAbstractEventDispatcher(const TpAbstractEventDispatcher &) = delete;
    TpAbstractEventDispatcher &operator=(const TpAbstractEventDispatcher &) = delete;

    void eventLoop();

    int epollFd_;
    std::thread loopThread_;
    bool running_;

    std::mutex mutex_;
    std::vector<TpSocketNotifier *> notifiers_;
};

#endif