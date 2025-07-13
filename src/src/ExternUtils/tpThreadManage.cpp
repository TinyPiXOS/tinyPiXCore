/*///------------------------------------------------------------------------------------------------------------------------//
		线程管理
说 明 : 用于系统信息获取时，以线程监视的方式运行时候的线程管理
日 期 : 2024.11.07

/*///------------------------------------------------------------------------------------------------------------------------//

#include <iostream>
#include <thread>
#include "tpThreadManage.h"

struct tpThreadManageData
{
	//	DiskstatsData disk_stat;
	//	tpDiskInfoParam param;
	std::atomic<bool> running;
	std::thread thread_t;
	pthread_rwlock_t rwlock = PTHREAD_RWLOCK_INITIALIZER;

	tpThreadManageData()
	{

	}
};

tpThreadManage::tpThreadManage(std::function<void(int)> callback_get,std::function<void(int)> callback_update,uint16_t time_samp)
{
	data_ = new tpThreadManageData();
	tpThreadManageData* threadData =static_cast<tpThreadManageData*>(data_);
	
	threadData->running=true;
	threadData->thread_t=std::thread(&tpThreadManage::threadUpdateStat, this, callback_get,callback_update,time_samp);

}
tpThreadManage::~tpThreadManage()
{

	
}

//采样和时间
//获取数据的回调
//更新数据的回调
void tpThreadManage::threadUpdateStat(std::function<void(int)> callback_get,std::function<void(int)> callback_update,uint16_t time_samp)
{
	tpThreadManageData* threadData =static_cast<tpThreadManageData*>(data_);

	//获取数据回调
	callback_get(0);
	while(threadData->running)
	{
		std::this_thread::sleep_for(std::chrono::milliseconds(time_samp)); 
		callback_get(0);
		//更新数据回调
		callback_update(0);
	}

}


