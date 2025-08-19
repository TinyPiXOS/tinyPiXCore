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
        return;
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


//events：
//	EPOLLIN：可读
//	EPOLLOUT：可写
//	EPOLLERR：错误发生
//	EPOLLHUP：挂断
//	EPOLLET：边缘触发模式
//V1版本
void tpSocketNotifierManager::registerNotifier(tpSocketNotifier* notifier) {
    std::lock_guard<std::mutex> lock(mutex_);

    epoll_event ev{};
    ev.data.ptr = notifier;
	switch (notifier->type()) {
		case tpSocketNotifier::Read:
			ev.events = EPOLLIN | EPOLLET;
			// 如果注册了 hangupCallback，就同时关注对端断开
			if (notifier->hangupCallback_) {
				ev.events |= EPOLLRDHUP;
			}
			break;

		case tpSocketNotifier::Write:
			ev.events = EPOLLOUT | EPOLLERR;
			break;

		case tpSocketNotifier::Exception:
			ev.events = EPOLLERR | EPOLLHUP | EPOLLET;
			break;

		default:
			ev.events = EPOLLIN | EPOLLET;
	}

	if (epoll_ctl(epollFd_, EPOLL_CTL_ADD, notifier->socket(), &ev) < 0) {
        if (errno == EEXIST) {
            // 已存在，改用 MOD 更新事件
            if (epoll_ctl(epollFd_, EPOLL_CTL_MOD, notifier->socket(), &ev) < 0) {
                perror("epoll_ctl MOD failed");
                return;
            }
        } else {
            perror("epoll_ctl ADD failed");
            return;
        }
    }
    notifiers_.push_back(notifier);
}

//V2版本
/*void tpSocketNotifierManager::registerNotifier(tpSocketNotifier* notifier) {
    std::lock_guard<std::mutex> lock(mutex_);
    
    // 查找相同socket的已有通知器
    std::vector<tpSocketNotifier*> sameSocketNotifiers;
    for (auto* n : notifiers_) {
        if (n->socket() == notifier->socket()) {
            sameSocketNotifiers.push_back(n);
        }
    }
    
    // 创建合并事件
    uint32_t events = 0;
    for (auto* n : sameSocketNotifiers) {
        switch (n->type()) {
            case tpSocketNotifier::Read:
                events |= (EPOLLIN | EPOLLET);
                if (n->hangupCallback_) events |= EPOLLRDHUP;
                break;
                
            case tpSocketNotifier::Write:
                events |= (EPOLLOUT | EPOLLERR | EPOLLHUP);
                break;
                
            case tpSocketNotifier::Exception:
                // 修复：添加 Exception 类型处理
                events |= (EPOLLERR | EPOLLHUP | EPOLLET);
                break;
        }
    }
    
    // 添加新通知器的事件
    switch (notifier->type()) {
        case tpSocketNotifier::Read:
            events |= (EPOLLIN | EPOLLET);
            if (notifier->hangupCallback_) events |= EPOLLRDHUP;
            break;
            
        case tpSocketNotifier::Write:
            events |= (EPOLLOUT | EPOLLERR | EPOLLHUP);
            break;
            
        case tpSocketNotifier::Exception:
            // 修复：添加 Exception 类型处理
            events |= (EPOLLERR | EPOLLHUP | EPOLLET);
            break;
    }
    
    epoll_event ev{};
    ev.events = events;
    ev.data.ptr = notifier;  // 注意：指向当前通知器
    
    if (sameSocketNotifiers.empty()) {
        // 新注册
        if (epoll_ctl(epollFd_, EPOLL_CTL_ADD, notifier->socket(), &ev) < 0) {
            perror("epoll_ctl ADD failed");
        }
    } else {
        // 更新事件
        if (epoll_ctl(epollFd_, EPOLL_CTL_MOD, notifier->socket(), &ev) < 0) {
            perror("epoll_ctl MOD failed");
        }
    }
    
    notifiers_.push_back(notifier);
}*/

void tpSocketNotifierManager::unregisterNotifier(tpSocketNotifier* notifier) {
    std::lock_guard<std::mutex> lock(mutex_);
    epoll_ctl(epollFd_, EPOLL_CTL_DEL, notifier->socket(), nullptr);
    notifiers_.erase(std::remove(notifiers_.begin(), notifiers_.end(), notifier), notifiers_.end());
}

//V1版本
void tpSocketNotifierManager::eventLoop() {
    epoll_event events[64];
	printf("debug:eventLoop\n");
    while (running_) 
	{
        int nfds = epoll_wait(epollFd_, events, 64, -1);
        if (nfds < 0) {
            if (errno == EINTR) continue;
            perror("epoll_wait error");
            break;
        }

		for (int i = 0; i < nfds; i++) 
		{
			tpSocketNotifier* notifier = static_cast<tpSocketNotifier*>(events[i].data.ptr);
			if (!notifier || !notifier->isEnabled()) 
				continue;

			uint32_t evs = events[i].events;
            switch (notifier->type()) 
			{
                case tpSocketNotifier::Read:
                    if (evs & (EPOLLRDHUP | EPOLLHUP | EPOLLERR)) {		//EPOLLRDHUP
                        // 挂断回调（TCP 专用）
                        notifier->hangupCallback_();
                    } else if (evs & EPOLLIN) {
                        // 可读回调
                        notifier->callback_();
                    }
                    break;

                case tpSocketNotifier::Write:
                    if (evs & (EPOLLERR | EPOLLHUP)) {
        				//notifier->hangupCallback_();
    				}
    				else if (evs & EPOLLOUT) {
						notifier->callback_();
					}
                    break;

                case tpSocketNotifier::Exception:
                    if (evs & (EPOLLERR|EPOLLHUP)) {
                        // 异常/挂断回调
                        notifier->hangupCallback_();
                    }
                    break;
            }
		}

    }
}

//V2版本
/*void tpSocketNotifierManager::eventLoop() {
    epoll_event events[64];
    
    while (running_) {
        int nfds = epoll_wait(epollFd_, events, 64, -1);
        if (nfds < 0) {
            if (errno == EINTR) continue;
            perror("epoll_wait error");
            break;
        }

        for (int i = 0; i < nfds; i++) {
            tpSocketNotifier* notifier = static_cast<tpSocketNotifier*>(events[i].data.ptr);
            
            // 添加有效性检查
            {
                std::lock_guard<std::mutex> lock(mutex_);
                auto it = std::find(notifiers_.begin(), notifiers_.end(), notifier);
                if (it == notifiers_.end()) continue; // 通知器已注销
            }
            
            if (!notifier || !notifier->isEnabled()) continue;
            
            uint32_t evs = events[i].events;
            
            // 根据通知器类型处理事件
            switch (notifier->type()) {
                case tpSocketNotifier::Read:
                    if (evs & EPOLLRDHUP) {
                        // TCP 连接挂断
                        if (notifier->hangupCallback_) notifier->hangupCallback_();
                    } else if (evs & EPOLLIN) {
                        // 可读事件
                        if (notifier->callback_) notifier->callback_();
                    }
                    break;
                    
                case tpSocketNotifier::Write:
                    if (evs & (EPOLLERR | EPOLLHUP)) {
                        // 错误处理
                        if (notifier->hangupCallback_) notifier->hangupCallback_();
                    } else if (evs & EPOLLOUT) {
                        // 可写事件
                        if (notifier->callback_) notifier->callback_();
                    }
                    break;
                    
                case tpSocketNotifier::Exception:  // 修复：添加 Exception 处理
                    if (evs & (EPOLLERR | EPOLLHUP)) {
                        // 异常/挂断回调
                        if (notifier->hangupCallback_) notifier->hangupCallback_();
                    }
                    break;
            }
        }
    }
}*/