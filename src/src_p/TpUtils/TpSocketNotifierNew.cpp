/*///------------------------------------------------------------------------------------------------------------------------//
		网络套接字监视器（新）
说 明 : 用于监视网络套接字的读写、状态及异常事件
日 期 : 2026.1.23

/*///------------------------------------------------------------------------------------------------------------------------//
#include "TpSocketNotifierNew.h"
#include <iostream>

TpSocketNotifierNew::TpSocketNotifierNew(int fd,
                                         Type type,
                                         std::function<void()> callback,
                                         std::function<void()> hangupCallback)
    : fd_(fd),
      type_(type),
      callback_(std::move(callback)),
      hangupCallback_(std::move(hangupCallback))
{
    // 默认构造不注册
}

TpSocketNotifierNew::~TpSocketNotifierNew() 
{
    detach();
}

uint32_t TpSocketNotifierNew::events() const 
{
    switch (type_) 
	{
        case Read:
            return TpEventSource::Read | (hangupCallback_ ? TpEventSource::Hangup : 0);
        case Write:
            return TpEventSource::Write | TpEventSource::Error;
        case Exception:
            return TpEventSource::Error | TpEventSource::Hangup;
        default:
            return TpEventSource::None;
    }
}

void TpSocketNotifierNew::onEvent(uint32_t revents) 
{
    if (!enabled_)
        return;

    switch (type_) 
	{
        case Read:
            if ((revents & TpEventSource::Hangup) && hangupCallback_)
                hangupCallback_();
            else if (revents & TpEventSource::Read)
                callback_();
            break;
        case Write:
            if ((revents & (TpEventSource::Hangup | TpEventSource::Error)) && hangupCallback_)
                hangupCallback_();
            else if (revents & TpEventSource::Write)
                callback_();
            break;
        case Exception:
            if ((revents & (TpEventSource::Hangup | TpEventSource::Error)) && hangupCallback_)
                hangupCallback_();
            break;
    }
}

int TpSocketNotifierNew::attach(TpEventLoop *loop) 
{
    if (!loop || loop_ == loop)
        return -1;

    loop_ = loop;
    loop_->add(this);
}

void TpSocketNotifierNew::detach() 
{
    if (loop_) {
        loop_->remove(this);
        loop_ = nullptr;
    }
}

void TpSocketNotifierNew::setEnabled(bool enable) 
{
    enabled_ = enable;
    if (loop_) {
        loop_->update(this);
    }
}

bool TpSocketNotifierNew::isEnabled() const 
{
	return enabled_;
}