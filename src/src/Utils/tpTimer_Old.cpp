/* liucy has been here，but nothing to see and nothing left ^_^!*/

/*
** Copyright (c) 2007-2021 By Alexander.King.
**
** Permission is hereby granted, free of charge, to any person obtaining a
** copy of this software and/or associated documentation files (the
** "Materials"), to deal in the Materials without restriction, including
** without limitation the rights to use, copy, modify, merge, publish,
** distribute, sublicense, and/or sell copies of the Materials, and to
** permit persons to whom the Materials are furnished to do so, subject to
** the following conditions:
**
** The above copyright notice and this permission notice shall be included
** in all copies or substantial portions of the Materials.
**
** THE MATERIALS ARE PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
** EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
** MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.
** IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY
** CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT,
** TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE
** MATERIALS OR THE USE OR OTHER DEALINGS IN THE MATERIALS.
*/

/*
#include "tpTimer.h"
#include "pthread.h"
#include <sys/epoll.h>
#include <sys/timerfd.h>
#include <sys/time.h>
#include <time.h>
#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include <errno.h>
#include <signal.h>
#include "tpTimer.h"

#define EPOLL_LISTEN_CNT 1
#define EPOLL_LISTEN_TIMEOUT 500

typedef struct
{
	int32_t (*fn)(void *);
	void *data;
} PiThreadArgs;

typedef struct
{
	pthread_t handle;
	PiThreadArgs args;
} PiThread;

static int32_t sig_list[] = {
	SIGHUP, SIGINT, SIGQUIT, SIGPIPE, SIGALRM, SIGTERM, SIGCHLD, SIGWINCH,
	SIGVTALRM, SIGPROF, SIGKILL, 0};

static inline void priv_setup_thread(void)
{
	int32_t i;
	sigset_t mask;

	sigemptyset(&mask);

	for (i = 0; sig_list[i]; ++i)
	{
		sigaddset(&mask, sig_list[i]);
	}

	pthread_sigmask(SIG_BLOCK, &mask, 0);

#ifdef PTHREAD_CANCEL_ASYNCHRONOUS
	{
		int32_t oldstate;
		pthread_setcanceltype(PTHREAD_CANCEL_ASYNCHRONOUS, &oldstate);
	}
#endif
}

static inline void priv_running_thread(void *data)
{
	PiThreadArgs *args = (PiThreadArgs *)data;

	if (args)
	{
		args->fn(args->data);
	}
}

static inline void *priv_run_thread(void *data)
{
	priv_setup_thread();
	priv_running_thread(data);
	pthread_exit((void *)0);

	return ((void *)0);
}

static inline int32_t priv_create_thread(PiThread *thread, void *args)
{
	pthread_attr_t type;

	if (pthread_attr_init(&type) != 0)
	{
		return TP_INVALIDATE_VALUE;
	}

	pthread_attr_setdetachstate(&type, PTHREAD_CREATE_JOINABLE);

	if (pthread_create(&thread->handle, &type, priv_run_thread, args) != 0)
	{
		return TP_INVALIDATE_VALUE;
	}

	return true;
}


static inline PiThread *thread_create(int32_t(STDCALL *fn)(void *), void *data)
{
	PiThread *thread = (PiThread *)malloc(sizeof(PiThread));

	if (thread == nullptr ||
		fn == nullptr)
	{
		return nullptr;
	}

	thread->args.fn = fn;
	thread->args.data = data;

	if (priv_create_thread(thread, &thread->args) == TP_INVALIDATE_VALUE)
	{
		free(thread);
		return nullptr;
	}

	return thread;
}

static inline int32_t thread_id(void)
{
	return ((int32_t)((size_t)pthread_self()));
}

static inline void thread_wait(PiThread *thread, void *status)
{
	PiThread *t_thread = (PiThread *)thread;

	if (t_thread)
	{
		pthread_join(t_thread->handle, &status);
		free(t_thread);
	}
}

static inline void thread_kill(PiThread *thread)
{
	PiThread *t_thread = (PiThread *)thread;

	if (t_thread)
	{
#ifdef PTHREAD_CANCEL_ASYNCHRONOUS
		pthread_cancel(t_thread->handle);
#endif
		pthread_kill(t_thread->handle, 0);
		free(t_thread);
	}
}

static inline bool thread_set_sigmask(int32_t signal_mask, int32_t thread_mask)
{
	sigset_t sig_mask;
	sigemptyset(&sig_mask);
	sigaddset(&sig_mask, signal_mask);

	return (pthread_sigmask(thread_mask, &sig_mask, nullptr) != 0);
}

typedef struct
{
	int32_t timer_id;
	PiThread *thread_id;
	int32_t(STDCALL *fn)(void *);
	void *userdata;
	volatile bool stop;
	int32_t epfd;
} PiTimer;

TP_DEF_VOID_TYPE_VAR(IPiTimer);

static inline int32_t priv_timer_thread(void *args)
{
	PiTimer *t_timer = (PiTimer *)args;

	int32_t i = 0, fd_cnt = 0, sfd = 0;
	unsigned long long exp = 0;
	struct epoll_event events[EPOLL_LISTEN_CNT];

	memset(events, 0, sizeof(events));

	while (!t_timer->stop)
	{
		fd_cnt = epoll_wait(t_timer->epfd, events, EPOLL_LISTEN_CNT, -1);

		for (i = 0; i < fd_cnt; i++)
		{
			sfd = events[i].data.fd;
			if (events[i].events & EPOLLIN)
			{
				if (sfd == t_timer->timer_id)
				{
					read(sfd, &exp, sizeof(unsigned long long));

					if (t_timer->fn)
					{
						t_timer->fn(t_timer->userdata);
					}
				}
			}
		}
	}

	return 0;
}

IPiTimer *timer_create(int32_t millsecond, int32_t(STDCALL *fn)(void *), void *args, bool uflag)
{
	PiTimer *timer = (PiTimer *)malloc(sizeof(PiTimer));

	struct itimerspec stTimeSpec;
	struct timespec stTimernow;
	struct epoll_event event;

	time_t tv_sec;
	long long tv_nsec;

	if (timer == nullptr ||
		fn == nullptr ||
		millsecond <= 0)
	{
		return nullptr;
	}

	timer->timer_id = timerfd_create(CLOCK_REALTIME, TFD_NONBLOCK);

	if (timer->timer_id == -1)
	{
		free(timer);
		return nullptr;
	}

	if (clock_gettime(CLOCK_REALTIME, &stTimernow) == -1)
	{
		free(timer);
		return nullptr;
	}

	timer->fn = fn;

	tv_sec = (millsecond * 1000000) / 1000000000;
	tv_nsec = (millsecond * 1000000) % 1000000000;

	stTimeSpec.it_value.tv_sec = stTimernow.tv_sec + tv_sec;
	stTimeSpec.it_value.tv_nsec = stTimernow.tv_nsec + tv_nsec;

	if (stTimeSpec.it_value.tv_nsec > 1000000000)
	{
		stTimeSpec.it_value.tv_sec += 1;
		stTimeSpec.it_value.tv_nsec %= 1000000000;
	}

	stTimeSpec.it_interval.tv_sec = uflag ? tv_sec : 0;
	stTimeSpec.it_interval.tv_nsec = uflag ? tv_nsec : 0;

	if (timerfd_settime(timer->timer_id, TFD_TIMER_ABSTIME, &stTimeSpec, nullptr) == -1)
	{
		free(timer);
		return nullptr;
	}

	timer->epfd = epoll_create(EPOLL_LISTEN_CNT);

	if (timer->epfd < 0)
	{
		free(timer);
		return nullptr;
	}

	memset(&event, 0, sizeof(event));

	event.data.fd = timer->timer_id;
	event.events = EPOLLIN | EPOLLET;

	int32_t nret = epoll_ctl(timer->epfd, EPOLL_CTL_ADD, timer->timer_id, &event);

	if (nret < 0)
	{
		free(timer);
		return nullptr;
	}

	timer->thread_id = nullptr;
	timer->userdata = args;
	timer->stop = true;

	return (PiTimer *)timer;
}

bool timer_start(IPiTimer *timer)
{
	PiTimer *t_timer = (PiTimer *)timer;

	if (t_timer == nullptr ||
		t_timer->stop == false)
	{
		return false;
	}

	t_timer->thread_id = thread_create(&priv_timer_thread, timer);

	if (t_timer->thread_id == nullptr)
	{
		return false;
	}

	return !(t_timer->stop = false);
}

bool timer_stop(IPiTimer *timer)
{
	PiTimer *t_timer = (PiTimer *)timer;

	if (t_timer == nullptr ||
		t_timer->stop == true ||
		t_timer->thread_id == nullptr)
	{
		return false;
	}

	t_timer->stop = true;
	thread_wait(t_timer->thread_id, nullptr);
	t_timer->thread_id = nullptr;

	return t_timer->stop;
}

bool timer_restart(IPiTimer *timer)
{
	PiTimer *t_timer = (PiTimer *)timer;

	if (t_timer == nullptr)
	{
		return false;
	}

	if (t_timer->stop == false)
	{
		timer_stop(timer);
	}

	return timer_start(timer);
}

void timer_free(IPiTimer *timer)
{
	PiTimer *t_timer = (PiTimer *)timer;

	if (t_timer)
	{
		if (t_timer->stop == false)
		{
			timer_stop(timer);
		}

		close(t_timer->timer_id);
		free(t_timer);
	}
}

void timer_delay(unsigned long long usec)
{
#if 1
	struct timeval tv;
	tv.tv_sec = usec / 1000000;
	tv.tv_usec = usec % 1000000;
	int32_t err;
	do
	{
		err = select(0, nullptr, nullptr, nullptr, &tv);
	} while (err < 0 && errno == EINTR);
#endif
}

int32_t timer_ticks()
{
	int32_t ticks;
	struct timeval now;
	gettimeofday(&now, nullptr);
	ticks = (now.tv_sec) * 1000 + (now.tv_usec) / 1000;
	return (ticks);
}

typedef struct
{
	IPiTimer *timer;
	unsigned long long usec;
	int32_t id;
} tpTimerSet;

static int32_t timerID = 0;

static inline int32_t triggerTimer(void *args)
{
	tpTimer *timerObject = (tpTimer *)args;

	if (timerObject)
	{
		timerObject->onTimer.emit(timerObject->getTimerID(), timerObject->getTimerInterval());
	}

	return 0;
}

tpTimer::tpTimer(int32_t millsecond, bool uflag)
{
	tpTimerSet *set = new tpTimerSet();

	if (set)
	{
		set->timer = timer_create(millsecond, triggerTimer, (void *)this, uflag);
		set->usec = millsecond;
		set->id = timerID++;

		this->timerSet = set;
	}
}

tpTimer::~tpTimer()
{
	tpTimerSet *set = (tpTimerSet *)this->timerSet;

	if (set)
	{
		timer_free(set->timer);
		delete set;
	}
}

bool tpTimer::startTimer()
{
	tpTimerSet *set = (tpTimerSet *)this->timerSet;

	if (set)
	{
		return timer_start(set->timer);
	}

	return false;
}

bool tpTimer::stopTimer()
{
	tpTimerSet *set = (tpTimerSet *)this->timerSet;

	if (set)
	{
		return timer_stop(set->timer);
	}

	return false;
}

bool tpTimer::restartTimer()
{
	tpTimerSet *set = (tpTimerSet *)this->timerSet;

	if (set)
	{
		return timer_restart(set->timer);
	}

	return false;
}

unsigned long long tpTimer::getTimerInterval()
{
	tpTimerSet *set = (tpTimerSet *)this->timerSet;

	if (set)
	{
		return set->usec;
	}

	return 0;
}

int32_t tpTimer::getTimerID()
{
	tpTimerSet *set = (tpTimerSet *)this->timerSet;

	if (set)
	{
		return set->id;
	}

	return 0;
}

void tpTimer::delay(unsigned long long usec)
{
	timer_delay(usec);
}

int32_t tpTimer::ticks()
{
	return timer_ticks();
}

*/