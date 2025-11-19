#include "TpThread.h"
#include <signal.h>
#include <stdlib.h>
#include <string.h>
#include <pthread.h>
#include <stdio.h>

struct PiThreadArgs
{
	int32_t (*fn)(void *);
	void *data;
};

struct PiThread
{
	pthread_t handle;
	PiThreadArgs args;
};

struct TpThreadData
{
	PiThread *thread;
	bool running;
	bool finished;
	bool runOnce;
};

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

static inline int32_t thead_function(ItpThreadData *args)
{
	TpThread *thread = (TpThread *)args;

	if (thread)
	{
		while (thread->isRunning())
		{
			thread->run();

			if (thread->getRunOnce())
			{
				break;
			}
		}
	}

	return 0;
}

TpThread::TpThread()
{
	TpThreadData *set = new TpThreadData();

	if (set)
	{
		memset(set, 0, sizeof(TpThread));
		this->threadSet = set;
	}
}

TpThread::~TpThread()
{
	TpThreadData *set = (TpThreadData *)this->threadSet;

	if (set)
	{
		this->terminate();
		delete set;
	}
}

bool TpThread::start()
{
	TpThreadData *set = (TpThreadData *)this->threadSet;

	if (set)
	{
		set->running = true;
		set->thread = thread_create(thead_function, this);

		if (set->thread == nullptr)
		{
			set->running = false;
			set->finished = true;
			return false;
		}

		set->finished = false;
	}

	return true;
}

void TpThread::terminate()
{
	TpThreadData *set = (TpThreadData *)this->threadSet;

	if (set)
	{
		if (set->thread)
		{
			thread_kill(set->thread);

			set->running = false;
			set->finished = true;
			set->runOnce = false;

			set->thread = nullptr;
		}
	}
}

void TpThread::stop()
{
	TpThreadData *set = (TpThreadData *)this->threadSet;

	if (set)
	{
		set->running = false;
		thread_wait(set->thread, nullptr);
		set->thread = nullptr;
		set->finished = true;
	}
}

void TpThread::setRunOnce(bool runOnce)
{
	TpThreadData *set = (TpThreadData *)this->threadSet;

	if (set)
	{
		set->runOnce = runOnce;
	}
}

bool TpThread::getRunOnce()
{
	TpThreadData *set = (TpThreadData *)this->threadSet;

	if (set)
	{
		return set->runOnce;
	}

	return false;
}

int32_t TpThread::getThreadID()
{
	return (int32_t)thread_id();
}

bool TpThread::isFinished()
{
	TpThreadData *set = (TpThreadData *)this->threadSet;

	if (set)
	{
		return set->finished;
	}

	return true;
}

bool TpThread::isRunning()
{
	TpThreadData *set = (TpThreadData *)this->threadSet;

	if (set)
	{
		return set->running;
	}

	return false;
}
