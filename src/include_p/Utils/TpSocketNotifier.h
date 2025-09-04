#ifndef _TP_SOCKET_NOTIFIER_H_
#define _TP_SOCKET_NOTIFIER_H_

#include "TpSocket.h"

#include <map>
#include <functional>
#include <sys/epoll.h>

class TpSocketNotifierManager;

class TpSocketNotifier {
public:
    enum Type { Read, Write, Exception };

    //TpSocketNotifier(int fd, Type type, std::function<void()> callback);
	TpSocketNotifier(int fd, Type type, std::function<void()> readCb, std::function<void()> hangupCb = std::function<void()>());
    ~TpSocketNotifier();

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
