#ifndef _TP_SOCKET_NOTIFIER_H_
#define _TP_SOCKET_NOTIFIER_H_

#include <map>
#include <functional>
#include <sys/epoll.h>
#include "TpEventLoop.h"
#include "TpEventSource.h"

class TpAbstractEventDispatcher;

class TpSocketNotifierNew : public TpEventSource 
{
public:
    enum Type { Read, Write, Exception };

	/// @brief 
	/// @param fd 文件接口描述符
	/// @param type 监测类型
	/// @param rwCb 回调
	/// @param hangupCb 异常回调
	TpSocketNotifierNew(int fd, Type type, std::function<void()> rwCb, std::function<void()> hangupCb = std::function<void()>());
	~TpSocketNotifierNew();


    int fd() const override { return fd_; }
    uint32_t events() const override;
    void onEvent(uint32_t revents) override;

    // 控制接口
    int attach(TpEventLoop *loop);
    void detach();

    void setEnabled(bool enable);
    bool isEnabled() const;
    int socket() const {return fd_;}
    Type type() const;

private:
	int fd_;
	Type type_;
	bool enabled_;

	std::function<void()> callback_;
	std::function<void()> hangupCallback_;
	TpEventLoop *loop_{nullptr};
};

#endif
