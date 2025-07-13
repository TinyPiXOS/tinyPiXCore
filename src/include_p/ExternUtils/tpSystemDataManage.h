#ifndef __TP_SYSTEN_DATA_MANAGE_H
#define __TP_SYSTEN_DATA_MANAGE_H

#include <thread>
#include <atomic>
#include <chrono>

class tpSystemDataManage
{
public:
	tpSystemDataManage();
	~tpSystemDataManage();

public:
	void update();
	void dataWriteLock();
	void dataReadLock();
	void dataUnlock();

public:
	std::thread thread_t;
	std::atomic<bool> running;

private:
	pthread_rwlock_t rwlock = PTHREAD_RWLOCK_INITIALIZER;
};

#endif