#ifndef _PROCESS_INFO_H_
#define _PROCESS_INFO_H_

#include <array>
#include <atomic>
#include <deque>
#include <chrono>
#include <unistd.h>
#include <sys/time.h>
#include <sys/stat.h> //stat
#include <pthread.h>
#include <fstream>
#include <sstream>
#include <algorithm>
#include "tpHash.h"
#include "tpMap.h"
#include "tpUtils.h"
#include "tpString.h"



#define MAX_PATH_LENGTH 1024



TP_DEF_VOID_TYPE_VAR(ItpProcessInfoData);

class tpProcessInfo
{
public:
	tpProcessInfo();
	tpProcessInfo(int pid, int ppid, const tpString &name);
	~tpProcessInfo();

public:
	/// @brief 获取进程ID
	int getPid();
	/// @brief 获取进程的父进程的ID
	int getPpid();
	/// @brief 获取进程的名称
	tpString getName();
	/// @brief 获取进程状态
	int getState();
	/// @brief 获取当前进程的CPU使用率
	double getCpuUsage();
	/// @brief 获取当前进程的GPU使用率
	double getGpuUsage();
	/// @brief 获取当前进程的内存使用率
	double getMemoryUsage();
	/// @brief 获取当前进程的磁盘读取速率
	double getDiskReadSpeed();
	/// @brief 获取当前进程的磁盘写入速率
	double getDiskWriteSpeed();
	/// @brief 获取当前进程的网络上传速率
	double getNetUpSpeed();
	/// @brief 获取当前进程的网络下载速率
	double getNetDownSpeed();
	/// @brief 获取当前进程的子进程列表
	tpList<tpProcessInfo *> getChildren();
	/// @brief 获取当前进程的父进程对象
	tpProcessInfo *getParent();



	

private:
    friend class tpProcessManage;
	tpProcessInfo(tpProcessInfo *proc);		//私有构造，仅允许class tpProcessManage使用
	//tpProcessInfo(int pid, int ppid, const tpString &name);
	void updateBasicInfo(int new_ppid, int new_pid, const std::string &new_name, double cpu_time, double new_memory,double cpu_total);
	void updateBasicInfo(tpProcessInfo *data,double cputime);
	void updateOtherInfo();
	void updateOtherInfo(double samp);	//更新并传入采样间隔的参数

	int readBasicInfo();		//读取进程基本信息(改为私有)
	int readDiskInfo();		//从文件读磁盘读写信息

	void addChild(tpProcessInfo *child);
	void removeChild(tpProcessInfo *child);
	void setParent(tpProcessInfo *parent);

	void sumUsage(double cpu, double gpu, double memory,double disk_r,double disk_w,double net_up,double net_down);
	void clearAppUsage();

	int initGpuUsage();
	int deinitGpuUsage();
	int readGpuUsage();
	int setGpuUsage(double usage);
	
	int getDiskReadByte();		//还未实现	//读取磁盘单位采样时间内读的字节数
	int getDiskWriteByte();		//还未实现	//读取磁盘单位采样时间内写的字节数
	int initNetworkSpeed();			//启动网络监测
	int deinitNetworkSpeed();		//关闭网络监测
	int getNetUpByte();
	int getNetDownByte();
	int readNetworkSpeed(double samp=0);		//读网速
	int readMemoryInfo();		//读取进程内存信息(改为私有)

	double getAppCpuUsage();		//进程及子进程总参数
	double getAppGpuUsage();
	double getAppMemoryUsage();
	double getAppDiskReadSpeed();
	double getAppDiskWriteSpeed();
	double getAppNetUpSpeed();
	double getAppNetDownSpeed();

	void *getNetHandle();
private:
	ItpProcessInfoData *data_;
};

#endif