/*///------------------------------------------------------------------------------------------------------------------------//
		事件循环实现
说 明 : 事件相关抽象基类
日 期 : 2026.1.23

/*///------------------------------------------------------------------------------------------------------------------------//

#include "TpEventSource.h"
#include "TpEventLoop.h"

TpEventSource::TpEventSource() = default;

TpEventSource::~TpEventSource() 
{
    detach();
}

bool TpEventSource::isEnabled() const 
{
    return enabled_;
}

void TpEventSource::setEnabled(bool enable) 
{
	if (enabled_ == enable)
        return;
    enabled_ = enable;
    if (loop_) {
        loop_->update(this);
    }
}

//绑定事件源到事件循环
int TpEventSource::attach(TpEventLoop *loop) 
{
    if (!loop || loop_ == loop)
        return -1;

    if (loop_) {
        loop_->remove(this);
    }

    loop_ = loop;
    loop_->add(this);
	return 0;
}

//解绑事件源
void TpEventSource::detach() 
{
    if (loop_) {
        loop_->remove(this);
        loop_ = nullptr;
    }
}

void TpEventSource::autoAttachIfNeeded() 
{
    if (!loop_) {
        attach(&TpEventLoop::defaultLoop());
    }
}

void TpEventSource::deleteLater() 
{
    if (loop_) {
        loop_->scheduleDelete(this);
    } else {
        TpEventLoop::defaultLoop().scheduleDelete(this);
    }
}