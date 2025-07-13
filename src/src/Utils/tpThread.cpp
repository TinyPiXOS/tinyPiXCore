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
#include "tpThread.h"
#include <signal.h>
#include <stdlib.h>
#include <string.h>
#include <pthread.h>
#include <stdio.h>


typedef struct{
	int32_t (*fn)(void*);
	void *data;
}PiThreadArgs;

typedef struct{
	pthread_t handle;
	PiThreadArgs args;
}PiThread;

typedef struct{
	PiThread *thread;
	bool running;
	bool finished;
	bool runOnce;
}tpThreadSet;

static int32_t sig_list[] = {
	SIGHUP, SIGINT, SIGQUIT, SIGPIPE, SIGALRM, SIGTERM, SIGCHLD, SIGWINCH,
	SIGVTALRM, SIGPROF, SIGKILL, 0
};

static inline void priv_setup_thread(void)
{
	int32_t i;
	sigset_t mask;

	sigemptyset(&mask);

	for ( i=0; sig_list[i]; ++i ) {
		sigaddset(&mask, sig_list[i]);
	}

	pthread_sigmask(SIG_BLOCK, &mask, 0);

#ifdef PTHREAD_CANCEL_ASYNCHRONOUS
	{ int32_t oldstate;
		pthread_setcanceltype(PTHREAD_CANCEL_ASYNCHRONOUS, &oldstate);
	}
#endif
}

static inline void priv_running_thread(void *data)
{
	PiThreadArgs *args = (PiThreadArgs*)data;

	if(args){
		args->fn(args->data);
	}
}

static inline void *priv_run_thread(void *data)
{
	priv_setup_thread();
	priv_running_thread(data);
	pthread_exit((void*)0);

	return ((void *)0);
}

static inline int32_t priv_create_thread(PiThread *thread, void *args)
{
	pthread_attr_t type;

	if (pthread_attr_init(&type) != 0 ) {
		return TP_INVALIDATE_VALUE;
	}

	pthread_attr_setdetachstate(&type, PTHREAD_CREATE_JOINABLE);

	if (pthread_create(&thread->handle, &type, priv_run_thread, args) != 0 ) {
		return TP_INVALIDATE_VALUE;
	}

	return true;
}

static inline PiThread *thread_create(int32_t (STDCALL *fn)(void*), void *data)
{
	PiThread *thread = (PiThread*)malloc(sizeof(PiThread));

	if(thread == nullptr ||
		fn == nullptr){
		return nullptr;
	}

	thread->args.fn = fn;
	thread->args.data = data;

	if(priv_create_thread(thread, &thread->args) == TP_INVALIDATE_VALUE){
		free(thread);
		return nullptr;
	}

	return thread;
}

static inline int32_t thread_id(void)
{
	return((int32_t)((size_t)pthread_self()));
}

static inline void thread_wait(PiThread *thread, void *status)
{
	PiThread *t_thread = (PiThread*)thread;

	if(t_thread){
		pthread_join(t_thread->handle, &status);
		free(t_thread);
	}
}

static inline void thread_kill(PiThread *thread)
{
	PiThread *t_thread = (PiThread*)thread;

	if(t_thread){
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
   sigemptyset (&sig_mask);
   sigaddset (&sig_mask, signal_mask);
   
   return (pthread_sigmask (thread_mask, &sig_mask, nullptr) != 0);
}

TP_DEF_VOID_TYPE_VAR(IPiThreadSet);

static inline int32_t thead_function(IPiThreadSet *args)
{
	tpThread *thread = (tpThread*)args;

	if(thread){
		while(thread->isRunning()){
			thread->run();

			if(thread->getRunOnce()){
				break;
			}
		}
	}

	return 0;
}

tpThread::tpThread()
{
	tpThreadSet *set = new tpThreadSet();
	
	if(set){
		memset(set, 0, sizeof(tpThread));
		this->threadSet = set;
	}
}

tpThread::~tpThread()
{
	tpThreadSet *set = (tpThreadSet *)this->threadSet;
	
	if(set){
		this->terminate();
		delete set;
	}
}

bool tpThread::setMask(int32_t signalMask, int32_t threadMask)
{
	return thread_set_sigmask(signalMask, threadMask);
}

bool tpThread::start()
{
	tpThreadSet *set = (tpThreadSet *)this->threadSet;
	
	if(set){
		set->running = true;
		set->thread = thread_create(thead_function, this);

		if(set->thread == nullptr){
			set->running = false;
			set->finished = true;
			return false;
		}

		set->finished = false;
	}
	
	return true;
}

void tpThread::terminate()
{
	tpThreadSet *set = (tpThreadSet *)this->threadSet;
	
	if(set){
		if(set->thread){
			thread_kill(set->thread);

			set->running = false;
			set->finished = true;
			set->runOnce = false;

			set->thread = nullptr;
		}
	}
}

void tpThread::setRunOnce(bool runOnce)
{
	tpThreadSet *set = (tpThreadSet *)this->threadSet;
	
	if(set){
		set->runOnce = runOnce;
	}
}

bool tpThread::getRunOnce()
{
	tpThreadSet *set = (tpThreadSet *)this->threadSet;
	
	if(set){
		return set->runOnce;
	}
	
	return false;
}

void tpThread::stop()
{
	tpThreadSet *set = (tpThreadSet *)this->threadSet;
	
	if(set){
		set->running = false;
		thread_wait(set->thread, nullptr);
		set->thread = nullptr;
		set->finished = true;
	}
}

void tpThread::run()
{

}

int32_t tpThread::getThreadID()
{
	return (int32_t)thread_id();
}

bool tpThread::isFinished()
{
	tpThreadSet *set = (tpThreadSet *)this->threadSet;
	
	if(set){
		return set->finished;
	}
	
	return true;
}

bool tpThread::isRunning()
{
	tpThreadSet *set = (tpThreadSet *)this->threadSet;
	
	if(set){
		return set->running;
	}
	
	return false;
}
