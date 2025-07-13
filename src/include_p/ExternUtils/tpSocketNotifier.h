#ifndef _TP_SOCKET_NOTIFIER_H_
#define _TP_SOCKET_NOTIFIER_H_

#include "tpSocket.h"

#include <map>
#include <functional>
#include <sys/epoll.h>

class tpSocketNotifierManager;

class tpSocketNotifier {
public:
    enum Type { Read, Write, Exception };

    //tpSocketNotifier(int fd, Type type, std::function<void()> callback);
	tpSocketNotifier(int fd, Type type, std::function<void()> readCb, std::function<void()> hangupCb = std::function<void()>());
    ~tpSocketNotifier();

    void setEnabled(bool enable);
    bool isEnabled() const;
    int socket() const;
    Type type() const;

    std::function<void()> callback_;
	std::function<void()> hangupCallback_;

private:
    int fd_;
    Type type_;
    bool enabled_;
};

#endif
