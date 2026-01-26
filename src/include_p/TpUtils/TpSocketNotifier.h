#ifndef _TP_SOCKET_NOTIFIER_H_
#define _TP_SOCKET_NOTIFIER_H_

#include <map>
#include <functional>
#include <sys/epoll.h>
#include "TpEventLoop.h"
#include "TpEventSource.h"

class TpAbstractEventDispatcher;

class TpSocketNotifier : public TpEventSource 
{
public:
    enum Type { Read, Write, Exception };

	/// @brief 构造，会自动绑定到默认事件循环
	/// @param fd 文件接口描述符
	/// @param type 监测类型
	/// @param rwCb 回调
	/// @param hangupCb 异常回调
	TpSocketNotifier(int fd, Type type, std::function<void()> rwCb, std::function<void()> hangupCb = std::function<void()>());
	/// @brief 
	/// @param loop 绑定用户指定的事件循环
	/// @param fd 文件接口描述符
	/// @param type 监测类型
	/// @param rwCb 回调
	/// @param hangupCb 异常回调
	TpSocketNotifier(TpEventLoop *loop, int fd, Type type, std::function<void()> rwCb, std::function<void()> hangupCb = std::function<void()>());
	~TpSocketNotifier();

	/// 返回文件描述符
    int fd() const override { return fd_; }
	/// 返回套接字(也是文件描述符)
	int socket() const {return fd_;}
	/// 返回监测类型
    Type type() const {return type_;}

	/// 返回监测事件
    uint32_t events() const override;
	///
    void onEvent(uint32_t revents) override;

private:
	int fd_{-1};
	Type type_;

	std::function<void()> callback_;
	std::function<void()> hangupCallback_;
};

#endif
