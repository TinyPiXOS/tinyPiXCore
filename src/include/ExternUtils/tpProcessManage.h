#ifndef __TP_SYSTEM_MANAGE_H
#define __TP_SYSTEM_MANAGE_H

#include "tpUtils.h"
#include "tpProcessInfo.h"

TP_DEF_VOID_TYPE_VAR(ItpProcessManageData);
struct tpAppData;

class tpProcessManage
{
public:
	tpProcessManage(bool enabled=true,uint16_t samp=1000);
	~tpProcessManage();

public:
	/// @brief 获取进程以及子进程的CPU总使用率
	/// @param pid 进程号
	double getCpuUsage(int pid);
	/// @brief 获取进程以及子进程的CPU总使用率
	/// @param app 进程对象,可以使用tpProcessInfo *findProcess(int pid)获取
	double getCpuUsage(tpProcessInfo *app);
	/// @brief 获取进程以及子进程的GPU总使用率
	/// @param pid 进程号
	double getGpuUsage(int pid);
	double getGpuUsage(tpProcessInfo *app);
	/// @brief 获取进程以及子进程的内存总使用率
	/// @param pid 进程号
	double getMemoryUsage(int pid);
	/// @brief 获取进程以及子进程的内存总使用率
	/// @param app 进程对象,可以使用tpProcessInfo *findProcess(int pid)获取
	double getMemoryUsage(tpProcessInfo *app);
	/// @brief 获取进程以及子进程的磁盘总读速度
	/// @param pid 进程号
	double getDiskReadSpeed(int pid);
	/// @brief 获取进程以及子进程的磁盘总读速度
	/// @param app 进程对象,可以使用tpProcessInfo *findProcess(int pid)获取
	double getDiskReadSpeed(tpProcessInfo *app);
	/// @brief 获取进程以及子进程的磁盘总写速度
	double getDiskWriteSpeed(int pid);
	/// @brief 获取进程以及子进程的磁盘总写速度
	/// @param app 进程对象,可以使用tpProcessInfo *findProcess(int pid)获取
	double getDiskWriteSpeed(tpProcessInfo *app);
	/// @brief 获取进程以及子进程的网络总上传速度
	/// @param pid 进程号
	double getNetUpSpeed(int pid);
	/// @brief 获取进程以及子进程的网络总上传速度
	/// @param app 进程对象,可以使用tpProcessInfo *findProcess(int pid)获取
	double getNetUpSpeed(tpProcessInfo *app);
	/// @brief 获取进程以及子进程的网络总下载速度
	/// @param pid 进程号
	double getNetDownSpeed(int pid);
	/// @brief 获取进程以及子进程的网络总下载速度
	/// @param app 进程对象,可以使用tpProcessInfo *findProcess(int pid)获取
	double getNetDownSpeed(tpProcessInfo *app);
	/// @brief 更新进程树及信息(使能自动更新时无需调用)
	void update();
	/// @brief 使用进程名查找进程(暂时不支持)
	tpProcessInfo *findProcess(tpString& name);
	/// @brief 使用进程ID查找进程
	/// @param pid 进程号
	tpProcessInfo *findProcess(int pid);
	/// @brief 打印某个进程的进程树(调试使用)
	/// @param app 进程对象,可以使用tpProcessInfo *findProcess(int pid)获取
	void printProcessTree(tpProcessInfo *app, int level = 0) const;
	
	tpList<tpProcessInfo *> getChildren(int pid);
	tpProcessInfo *getParent(int pid);

public:
	void threadUpdateStat(uint16_t time_samp);		//自动更新信息的线程
	int updateInfo();			//更新进程树所有进程的信息
	int updateNetLocalAddr();
	int updateConnectInfo();
	int initNetworkMonitor();						//网络抓包监测初始化
	int deinitNetworkMonitor();						//取消抓包监测初始化
	int countProcessInfo(tpProcessInfo *process,double samp);	
	int getProcessInfo(int pid);						//获取进程信息
	int getProcessInfo(tpProcessInfo *process); 		//获取进程信息
	int updateProcessTree();					//更新进程树
	int updateProcessTree(uint16_t samp_time);	//更新进程树

	int getAllProcessMap(std::map<int, tpProcessInfo *> &processMap);
	std::map<int, tpProcessInfo *> *getProcessInfoMap();
	void *getPcapHandle();
	void *thread_pcap_cpature(void *param);
	static void packet_handler(unsigned char *args, const struct pcap_pkthdr *header, const unsigned char *packet);
private:
	ItpProcessManageData *data_;
};




#endif
