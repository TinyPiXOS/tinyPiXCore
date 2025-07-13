#include <iostream>
#include <vector>
#include <algorithm>
#include "tpSocketNotifierManager.h"

tpSocketNotifierManager& tpSocketNotifierManager::instance() {
    static tpSocketNotifierManager inst;
    return inst;
}

tpSocketNotifierManager::tpSocketNotifierManager()
    : running_(true) {
    epollFd_ = epoll_create1(0);
    if (epollFd_ < 0) {
        perror("epoll_create1 failed");
        exit(EXIT_FAILURE);
    }
	printf("tpSocketNotifierManager构造\n");
    loopThread_ = std::thread(&tpSocketNotifierManager::eventLoop, this);
}

tpSocketNotifierManager::~tpSocketNotifierManager() {
    stop();
}

void tpSocketNotifierManager::stop() {
    running_ = false;
    if (loopThread_.joinable()) {
        // 触发 epoll_wait 返回
        uint64_t u = 1;
        write(epollFd_, &u, sizeof(uint64_t));
        loopThread_.join();
    }
    close(epollFd_);
}

void tpSocketNotifierManager::registerNotifier(tpSocketNotifier* notifier) {
    std::lock_guard<std::mutex> lock(mutex_);

    epoll_event ev{};
    ev.data.ptr = notifier;
    ev.events = EPOLLIN | EPOLLET | EPOLLRDHUP; // 边缘触发，关注可读事件和对端挂断

    if (epoll_ctl(epollFd_, EPOLL_CTL_ADD, notifier->socket(), &ev) < 0) {
        perror("epoll_ctl ADD failed");
        return;
    }
    notifiers_.push_back(notifier);
}

void tpSocketNotifierManager::unregisterNotifier(tpSocketNotifier* notifier) {
    std::lock_guard<std::mutex> lock(mutex_);
    epoll_ctl(epollFd_, EPOLL_CTL_DEL, notifier->socket(), nullptr);
    notifiers_.erase(std::remove(notifiers_.begin(), notifiers_.end(), notifier), notifiers_.end());
}

void tpSocketNotifierManager::eventLoop() {
    epoll_event events[64];
	printf("debug:eventLoop\n");
    while (running_) {
        int nfds = epoll_wait(epollFd_, events, 64, -1);
        if (nfds < 0) {
            if (errno == EINTR) continue;
            perror("epoll_wait error");
            break;
        }
        /*for (int i = 0; i < nfds; i++) {
            tpSocketNotifier* notifier = static_cast<tpSocketNotifier*>(events[i].data.ptr);
            if (notifier && notifier->isEnabled()) {
                notifier->callback_(); // 调用回调通知事件
            }
        }*/

		for (int i = 0; i < nfds; i++) {
			tpSocketNotifier* notifier = static_cast<tpSocketNotifier*>(events[i].data.ptr);
			if (!notifier || !notifier->isEnabled()) 
				continue;

			uint32_t evs = events[i].events;
			// 对端挂断
			if (evs & EPOLLRDHUP) {
				notifier->hangupCallback_();  // 新：触发 hang-up 回调
				continue;
			}
			// 可读事件
			if (evs & EPOLLIN) {
				notifier->callback_();
			}
		}

    }
}