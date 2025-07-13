//多线程的数据保护
#include <iostream>
#include "tpSystemDataManage.h"


int safe_rwlock_destroy(pthread_rwlock_t* rwlock) {
	if (!rwlock) {
		std::cerr << "Error: Lock is NULL.\n";
		return -1;
	}

	// 检查锁是否正在被使用
	int try_lock_status = pthread_rwlock_trywrlock(rwlock);
	if (try_lock_status == 0) {
		// 成功获取写锁，表示锁未被占用
		pthread_rwlock_unlock(rwlock);

		// 安全销毁锁
		int destroy_status = pthread_rwlock_destroy(rwlock);
		if (destroy_status == 0) {
			std::cerr << "Lock successfully destroyed.\n";
			return 0;
		} else {
			std::cerr << "Error destroying lock:\n";
			return destroy_status;
		}
	} else if (try_lock_status == EBUSY) {
		// 锁正在被使用
		std::cerr << "Error: Lock is currently in use.\n";
		return -1;
	} else {
		// 其他错误
		std::cerr << "Error: Unable to check lock status\n";
		return -1;
	}
}

tpSystemDataManage::tpSystemDataManage()
{
}

tpSystemDataManage::~tpSystemDataManage()
{
	safe_rwlock_destroy(&rwlock);
	//std::cout<<"析构tpSystemDataManage\n";
}

void tpSystemDataManage::dataReadLock()
{
    if (!running)
        return;
    pthread_rwlock_rdlock(&rwlock);
}

void tpSystemDataManage::dataWriteLock()
{
    if (!running)
        return;
    pthread_rwlock_wrlock(&rwlock);
}

void tpSystemDataManage::dataUnlock()
{
    if (!running)
        return;
    pthread_rwlock_unlock(&rwlock);
}

void tpSystemDataManage::update()
{
    if (running)
        return;
    // 需要添加不使用线程的处理
}