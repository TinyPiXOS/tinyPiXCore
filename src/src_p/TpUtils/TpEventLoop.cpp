/*///------------------------------------------------------------------------------------------------------------------------//
		事件循环实现
说 明 : 基于 epoll 实现的事件循环
日 期 : 2026.1.23

/*///------------------------------------------------------------------------------------------------------------------------//
#include <unistd.h>
#include <errno.h>
#include <cstdio>
#include <cstring>
#include <queue>
#include <sys/epoll.h>
#include <sys/eventfd.h>
#include "TpEventLoop.h"
#include "TpEventSource.h"


static std::queue<std::function<void()>> g_tasks;
static std::mutex g_taskMutex;

TpEventLoop::TpEventLoop() 
{
	epollFd_ = epoll_create1(0);
	if (epollFd_ < 0) {
		perror("epoll_create1");
	}

	wakeFd_ = eventfd(0, EFD_NONBLOCK);
	if (wakeFd_ >= 0) {
		epoll_event ev{};
		ev.events = EPOLLIN;
		ev.data.fd = wakeFd_;
		epoll_ctl(epollFd_, EPOLL_CTL_ADD, wakeFd_, &ev);
	}
}

TpEventLoop::~TpEventLoop() 
{
	stop();
	if (wakeFd_ >= 0) close(wakeFd_);
	if (epollFd_ >= 0) close(epollFd_);
}

//阻塞模式运行
void TpEventLoop::run() 
{
	running_ = true;
	loop();
}

//异步模式运行
void TpEventLoop::runAsync() 
{
	running_ = true;
	loopThread_ = std::thread(&TpEventLoop::loop, this);
}

void TpEventLoop::stop() 
{
	running_ = false;
	wakeup();
	if (loopThread_.joinable())
		loopThread_.join();
}

void TpEventLoop::add(TpEventSource *src) 
{
	if (!src) 
		return;

	std::lock_guard<std::mutex> lock(mutex_);		//会自动解锁

	epoll_event ev{};
	ev.data.ptr = src;
	ev.events = toEpoll(src->events());

	epoll_ctl(epollFd_, EPOLL_CTL_ADD, src->fd(), &ev);
	sources_[src->fd()] = src;
}

void TpEventLoop::update(TpEventSource *src) 
{
	if (!src) 
		return;

	std::lock_guard<std::mutex> lock(mutex_);		//会自动解锁
	epoll_event ev{};
	ev.data.ptr = src;
	ev.events = src->isEnabled() ? toEpoll(src->events()) : 0;

	epoll_ctl(epollFd_, EPOLL_CTL_MOD, src->fd(), &ev);
}

void TpEventLoop::remove(TpEventSource *src) 
{
	if (!src) 
		return;
	
	std::lock_guard<std::mutex> lock(mutex_);		//会自动解锁
	epoll_ctl(epollFd_, EPOLL_CTL_DEL, src->fd(), nullptr);
	sources_.erase(src->fd());
}

void TpEventLoop::wakeup() 
{
	uint64_t one = 1;
	write(wakeFd_, &one, sizeof(one));
}

void TpEventLoop::postTask(std::function<void()> task) 
{
	{
		std::lock_guard<std::mutex> lock(g_taskMutex);
		g_tasks.push(std::move(task));
	}
	wakeup();
}

void TpEventLoop::loop() 
{
	epoll_event events[64];

	while (running_) {
		int n = epoll_wait(epollFd_, events, 64, -1);
		if (n < 0)
			continue;

		for (int i = 0; i < n; ++i) {
			if (events[i].data.fd == wakeFd_) {
				handleWakeup();
				continue;
			}

			auto *src = static_cast<TpEventSource*>(events[i].data.ptr);
			if (!src || !src->isEnabled())
				continue;

			src->onEvent(fromEpoll(events[i].events));
		}
	}
}

void TpEventLoop::handleWakeup() 
{
	uint64_t v;
	while (read(wakeFd_, &v, sizeof(v)) > 0) {}

	std::queue<std::function<void()>> tasks;
	{
		std::lock_guard<std::mutex> lock(g_taskMutex);
		std::swap(tasks, g_tasks);
	}

	while (!tasks.empty()) {
		tasks.front()();
		tasks.pop();
	}
}

TpEventLoop& TpEventLoop::defaultLoop() {
    static TpEventLoop loop;
    static std::once_flag flag;

    std::call_once(flag, [&]() {
        loop.runAsync();   // 默认 loop 永远在后台跑
    });

    return loop;
}

uint32_t TpEventLoop::toEpoll(uint32_t ev) const 
{
	uint32_t e = 0;
	if (ev & TpEventSource::Read)   e |= EPOLLIN;
	if (ev & TpEventSource::Write)  e |= EPOLLOUT;
	if (ev & TpEventSource::Error)  e |= EPOLLERR;
	if (ev & TpEventSource::Hangup) e |= EPOLLHUP | EPOLLRDHUP;
	return e;
}

uint32_t TpEventLoop::fromEpoll(uint32_t ev) const 
{
	uint32_t e = TpEventSource::None;
	if (ev & EPOLLIN) e |= TpEventSource::Read;
	if (ev & EPOLLOUT) e |= TpEventSource::Write;
	if (ev & EPOLLERR) e |= TpEventSource::Error;
	if (ev & (EPOLLHUP | EPOLLRDHUP)) e |= TpEventSource::Hangup;
	return e;
}