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


class TpSocketNotifierManager {
public:
	static TpSocketNotifierManager& instance();
	void registerNotifier(TpSocketNotifier* notifier);
	void unregisterNotifier(TpSocketNotifier* notifier);

    void stop();

private:
    TpSocketNotifierManager();
    ~TpSocketNotifierManager();
    TpSocketNotifierManager(const TpSocketNotifierManager&) = delete;
    TpSocketNotifierManager& operator=(const TpSocketNotifierManager&) = delete;

    void eventLoop();

    int epollFd_;
    std::thread loopThread_;
    bool running_;

    std::mutex mutex_;
    std::vector<TpSocketNotifier*> notifiers_;
};

#endif