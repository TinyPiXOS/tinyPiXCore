#ifndef _TP_SOCKET_NOTIFIER_MANAGER_H_
#define _TP_SOCKET_NOTIFIER_MANAGER_H_

#include "tpSocket.h"
#include "tpSocketNotifier.h"
#include <thread>
#include <vector>
#include <mutex>
#include <atomic>
#include <map>
#include <unordered_map>


class tpSocketNotifierManager {
public:
	static tpSocketNotifierManager& instance();
	void registerNotifier(tpSocketNotifier* notifier);
	void unregisterNotifier(tpSocketNotifier* notifier);

    void stop();

private:
    tpSocketNotifierManager();
    ~tpSocketNotifierManager();
    tpSocketNotifierManager(const tpSocketNotifierManager&) = delete;
    tpSocketNotifierManager& operator=(const tpSocketNotifierManager&) = delete;

    void eventLoop();

    int epollFd_;
    std::thread loopThread_;
    bool running_;

    std::mutex mutex_;
    std::vector<tpSocketNotifier*> notifiers_;
};

#endif