#ifndef __TP_CPU_INFO_H
#define __TP_CPU_INFO_H

#include "tpThreadManage.h"
#include "tpUtils.h"
#include "tpList.h"

TP_DEF_VOID_TYPE_VAR(ItpCpuData);
TP_DEF_VOID_TYPE_VAR(ItpCpuCoreData);

class tpCpu;

class tpCpuCore
{
public:
	tpCpuCore(uint16_t core);
	tpCpuCore();
	tpCpuCore(uint16_t core, uint64_t used, uint64_t idle, uint64_t iowait);
	~tpCpuCore();
public:
	/// @brief 获取CPU单核心名字
	/// @return 返回CPU单核心名字
	tpString getName();
	/// @brief 获取cpu单核心的编号
	/// @return 返回cpu单核心的编号
	int getCoreNum();
	/// @brief 获取cpu单个核心主频
	/// @return 返回cpu单个核心主频
	double getFrequency();
	/// @brief 获取cpu单个核心使用率
	/// @return 返回cpu单个核心主频
	double getUsage();

private:
	friend class tpCpu;
	int setName(const tpString &name);
	int setCoreNum(uint16_t processor);
	int setFrequency(double frequency);
	int setUsage(double usage);
	int setStat(const void *stat);

private:
	ItpCpuCoreData *data_;
};




class tpCpu
{
public:
	/// @brief 构造函数
	/// @param name CPU名字，用于多CPU情况，暂未考虑多CPU情况
	/// @param enable 自动更新cpu状态使能，设置为true会自动更新CPU状态
	/// @param samp CPU状态更新频率
	tpCpu(const tpString &name=tpString(""),tpBool enable = TP_TRUE, uint16_t samp = 1000);
	~tpCpu();

public:
	/// @brief 获取CPU所有核心的信息(静态信息)
	/// @return CPU核心列表
	tpList<tpCpuCore*> getList();
	/// @brief 获取cpu状态(使用率)列表(动态信息)
	/// @return cpu核心列表
	tpList<tpCpuCore*> getState();
	/// @brief 获取CPU核心状态(总核心)
	/// @return 返回对应的核心的状态信息
	tpCpuCore* getCoreState();
	/// @brief 获取指定的单个核心的状态
	/// @param core 核心编号
	/// @return 返回对应核心的状态信息
	tpCpuCore* getCoreState(uint16_t core);

private:
	int update(); // 只会更新CPU的状态参数，不会更新cpu信息
	int readCpuInfo(const char *path);//内部使用，从文件中读取cpu信息
	void updateState(uint16_t num, void *stat);
	int threadUpdateCreat(uint16_t time_samp);
	void threadUpdateStat(uint16_t core_num, uint16_t time_samp);
	tpCpuCore* getCpuCoreState(int processor);

private:
	ItpCpuData *data_;
};

#endif
