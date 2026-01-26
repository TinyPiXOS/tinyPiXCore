/*///------------------------------------------------------------------------------------------------------------------------//
		网络套接字监视器（新）
说 明 : 用于监视网络套接字的读写、状态及异常事件
日 期 : 2026.1.23

/*///------------------------------------------------------------------------------------------------------------------------//
#include "TpSocketNotifier.h"
#include <iostream>

TpSocketNotifier::TpSocketNotifier(int fd,
                                         Type type,
                                         std::function<void()> callback,
                                         std::function<void()> hangupCallback)
    : fd_(fd),
      type_(type),
      callback_(std::move(callback)),
      hangupCallback_(std::move(hangupCallback))
{
    autoAttachIfNeeded();   // 自动 attach defaultLoop
}

TpSocketNotifier::TpSocketNotifier(TpEventLoop *loop,
										 int fd,
                                         Type type,
                                         std::function<void()> callback,
                                         std::function<void()> hangupCallback)
    : fd_(fd),
      type_(type),
      callback_(std::move(callback)),
      hangupCallback_(std::move(hangupCallback))
{
    attach(loop);           // 显式 attach

}

TpSocketNotifier::~TpSocketNotifier() 
{
    detach();
}

uint32_t TpSocketNotifier::events() const 
{
	if (!isEnabled()) 
		return TpEventSource::None;
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
	/*uint32_t ev = 0;
    switch (type_) {
        case Type::Read:
            ev = TpEventSource::Read;
            break;
        case Type::Write:
            ev = TpEventSource::Write;
            break;
        case Type::Exception:
            ev = TpEventSource::Error | TpEventSource::Hangup;
            break;
    }
    // 挂断和错误总是关心
    ev |= TpEventSource::Error | TpEventSource::Hangup;
    return ev;*/
}

void TpSocketNotifier::onEvent(uint32_t revents) 
{
    /*switch (type_) 
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
    }*/
	
    if (revents & (TpEventSource::Hangup | TpEventSource::Error)) {
        if (hangupCallback_)
            hangupCallback_();
        detach();
        return;
    }

    if (type_ == Read && (revents & TpEventSource::Read)) {
        callback_();
    }
}
