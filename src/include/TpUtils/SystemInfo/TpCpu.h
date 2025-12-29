#ifndef __TP_CPU_INFO_H
#define __TP_CPU_INFO_H

#include <TpCore.h>
#include "TpList.h"

TP_DEF_VOID_TYPE_VAR(ITpCpuData);
TP_DEF_VOID_TYPE_VAR(ITpCpuCoreData);

class TpCpu;

class TpCpuCore
{
public:
    TpCpuCore(uint16_t core);
    TpCpuCore();
    TpCpuCore(uint16_t core, uint64_t used, uint64_t idle, uint64_t iowait);
    ~TpCpuCore();

public:
    /// @brief 获取CPU单核心名字
    /// @return 返回CPU单核心名字
    TpString getName();
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
    friend class TpCpu;
    int setName(const TpString &name);
    int setCoreNum(uint16_t processor);
    int setFrequency(double frequency);
    int setUsage(double usage);
    int setStat(const void *stat);

private:
    ITpCpuCoreData *data_;
};

class TpCpu
{
public:
    /// @brief 构造函数
    /// @param name CPU名字，用于多CPU情况，暂未考虑多CPU情况
    /// @param enable 自动更新cpu状态使能，设置为true会自动更新CPU状态
    /// @param samp CPU状态更新频率
    TpCpu(const TpString &name = TpString(""), tpBool enable = TP_TRUE, uint16_t samp = 1000);
    ~TpCpu();

public:
    /// @brief 获取CPU所有核心的信息(静态信息)
    /// @return CPU核心列表
    TpList<TpCpuCore *> getList();
    /// @brief 获取cpu状态(使用率)列表(动态信息)
    /// @return cpu核心列表
    TpList<TpCpuCore *> getState();
    /// @brief 获取CPU核心状态(总核心)
    /// @return 返回对应的核心的状态信息
    TpCpuCore *getCoreState();
    /// @brief 获取指定的单个核心的状态
    /// @param core 核心编号
    /// @return 返回对应核心的状态信息
    TpCpuCore *getCoreState(uint16_t core);

private:
    int update();                      // 只会更新CPU的状态参数，不会更新cpu信息
    int readCpuInfo(const char *path); // 内部使用，从文件中读取cpu信息
    void updateState(uint16_t num, void *stat);
    int threadUpdateCreat(uint16_t time_samp);
    void threadUpdateStat(uint16_t core_num, uint16_t time_samp);
    TpCpuCore *getCpuCoreState(int processor);

private:
    ITpCpuData *data_;
};

#endif
