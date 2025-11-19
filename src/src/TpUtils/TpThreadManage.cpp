/*///------------------------------------------------------------------------------------------------------------------------//
		线程管理
说 明 : 用于系统信息获取时，以线程监视的方式运行时候的线程管理
日 期 : 2024.11.07

/*///------------------------------------------------------------------------------------------------------------------------//

#include <iostream>
#include <thread>
#include "TpThreadManage.h"

struct TpThreadManageData
{
	//	DiskstatsData disk_stat;
	//	TpDiskInfoParam param;
	std::atomic<bool> running;
	std::thread thread_t;
	pthread_rwlock_t rwlock = PTHREAD_RWLOCK_INITIALIZER;

	TpThreadManageData()
	{

	}
};

TpThreadManage::TpThreadManage(std::function<void(int)> callback_get,std::function<void(int)> callback_update,uint16_t time_samp)
{
	data_ = new TpThreadManageData();
	TpThreadManageData* threadData =static_cast<TpThreadManageData*>(data_);
	
	threadData->running=true;
	threadData->thread_t=std::thread(&TpThreadManage::threadUpdateStat, this, callback_get,callback_update,time_samp);

}
TpThreadManage::~TpThreadManage()
{

	
}

//采样和时间
//获取数据的回调
//更新数据的回调
void TpThreadManage::threadUpdateStat(std::function<void(int)> callback_get,std::function<void(int)> callback_update,uint16_t time_samp)
{
	TpThreadManageData* threadData =static_cast<TpThreadManageData*>(data_);

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


