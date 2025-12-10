#ifndef __TP_SYSTEM_MANAGE_H
#define __TP_SYSTEM_MANAGE_H

#include <TpCore.h>
#include "TpProcessInfo.h"

TP_DEF_VOID_TYPE_VAR(ITpProcessManageData);
struct TpAppData;

class TpProcessManage
{
public:
	TpProcessManage(bool enabled=true,uint16_t samp=1000);
	~TpProcessManage();

public:
	/// @brief 获取进程以及子进程的CPU总使用率
	/// @param pid 进程号
	double getCpuUsage(int pid);
	/// @brief 获取进程以及子进程的CPU总使用率
	/// @param app 进程对象,可以使用tpProcessInfo *findProcess(int pid)获取
	double getCpuUsage(TpProcessInfo *app);
	/// @brief 获取进程以及子进程的GPU总使用率
	/// @param pid 进程号
	double getGpuUsage(int pid);
	double getGpuUsage(TpProcessInfo *app);
	/// @brief 获取进程以及子进程的内存总使用率
	/// @param pid 进程号
	double getMemoryUsage(int pid);
	/// @brief 获取进程以及子进程的内存总使用率
	/// @param app 进程对象,可以使用tpProcessInfo *findProcess(int pid)获取
	double getMemoryUsage(TpProcessInfo *app);
	/// @brief 获取进程以及子进程的磁盘总读速度
	/// @param pid 进程号
	double getDiskReadSpeed(int pid);
	/// @brief 获取进程以及子进程的磁盘总读速度
	/// @param app 进程对象,可以使用tpProcessInfo *findProcess(int pid)获取
	double getDiskReadSpeed(TpProcessInfo *app);
	/// @brief 获取进程以及子进程的磁盘总写速度
	double getDiskWriteSpeed(int pid);
	/// @brief 获取进程以及子进程的磁盘总写速度
	/// @param app 进程对象,可以使用tpProcessInfo *findProcess(int pid)获取
	double getDiskWriteSpeed(TpProcessInfo *app);
	/// @brief 获取进程以及子进程的网络总上传速度
	/// @param pid 进程号
	double getNetUpSpeed(int pid);
	/// @brief 获取进程以及子进程的网络总上传速度
	/// @param app 进程对象,可以使用tpProcessInfo *findProcess(int pid)获取
	double getNetUpSpeed(TpProcessInfo *app);
	/// @brief 获取进程以及子进程的网络总下载速度
	/// @param pid 进程号
	double getNetDownSpeed(int pid);
	/// @brief 获取进程以及子进程的网络总下载速度
	/// @param app 进程对象,可以使用tpProcessInfo *findProcess(int pid)获取
	double getNetDownSpeed(TpProcessInfo *app);
	/// @brief 更新进程树及信息(使能自动更新时无需调用)
	void update();
	/// @brief 使用进程名查找进程(暂时不支持)
	TpProcessInfo *findProcess(TpString& name);
	/// @brief 使用进程ID查找进程
	/// @param pid 进程号
	TpProcessInfo *findProcess(int pid);
	/// @brief 打印某个进程的进程树(调试使用)
	/// @param app 进程对象,可以使用tpProcessInfo *findProcess(int pid)获取
	void printProcessTree(TpProcessInfo *app, int level = 0) const;
	
	TpList<TpProcessInfo *> getChildren(int pid);
	TpProcessInfo *getParent(int pid);

public:
	void threadUpdateStat(uint16_t time_samp);		//自动更新信息的线程
	int updateInfo();			//更新进程树所有进程的信息
	int updateNetLocalAddr();
	int updateConnectInfo();
	int initNetworkMonitor();						//网络抓包监测初始化
	int deinitNetworkMonitor();						//取消抓包监测初始化
	int countProcessInfo(TpProcessInfo *process,double samp);	
	int getProcessInfo(int pid);						//获取进程信息
	int getProcessInfo(TpProcessInfo *process); 		//获取进程信息
	int updateProcessTree();					//更新进程树
	int updateProcessTree(uint16_t samp_time);	//更新进程树

	int getAllProcessMap(std::map<int, TpProcessInfo *> &processMap);
	std::map<int, TpProcessInfo *> *getProcessInfoMap();
	void *getPcapHandle();
	void *thread_pcap_cpature(void *param);
	static void packet_handler(unsigned char *args, const struct pcap_pkthdr *header, const unsigned char *packet);
private:
	ITpProcessManageData *data_;
};




#endif
