/*///------------------------------------------------------------------------------------------------------------------------//
		系统CPU信息
说 明 :
日 期 : 2024.11.05

/*/
//------------------------------------------------------------------------------------------------------------------------//

#include <iostream>
#include <fstream>
#include <string>
#include <sstream>
#include <string.h>
#include <vector>
#include <ctime>
#include <atomic>
#include <thread>
#include <pthread.h>

#include "TpCpu.h"
#include "TpSystemDataManage.h"


struct TpCpuInfoParam
{
	uint16_t processor;								  // 核心编号,
	TpString name;									  // 名称
	double freq;									  // 频率
	//double usage;
	TpCpuInfoParam(uint16_t proc) : processor(proc) {} //
};

// proc/stat文件中的参数以及计算的利用率（本结构体改为内部使用）
struct TpCpuStatParam
{
	uint16_t processor; // 核心编号
	double usage;	   // 使用率
	//	time_t time_samp;		//当前数据读取的时间
	uint64_t time_used;	  // 使用时间
	uint64_t time_idle;	  // 空闲时间
	uint64_t time_iowait; // 等待时间

	// cpu总运行时间=使用时间used+idle+iowait
	TpCpuStatParam(uint16_t proc, uint64_t used, uint64_t idle, uint64_t iowait) : processor(proc),
																					time_used(used),
																					time_idle(idle),
																					time_iowait(iowait) {} //
	TpCpuStatParam(uint16_t proc) : processor(proc) {}													 //
	TpCpuStatParam() {}
};

struct TpCpuCoreData
{
	TpSystemDataManage data;

	TpCpuInfoParam cpuInfo;
	TpCpuStatParam cpuStat; 					// 总/分核心
	TpCpuCoreData(uint16_t proc): cpuInfo(proc) {} 
	TpCpuCoreData(uint16_t proc, uint64_t used, uint64_t idle, uint64_t iowait) : 
					cpuStat(proc,used,idle,iowait),cpuInfo(proc) {} //
};

struct TpCpuData
{
	TpSystemDataManage data;

//	TpList<TpCpuInfoParam> cpuInfo;
	//	TpCpuStatParam cpu_stat;				//总
//	TpList<TpCpuStatParam> cpusStat; // 总+分核心
	TpList<TpCpuCore *> cpuCore;
	TpCpuData()
	{
	}
};


TpList<TpCpuStatParam> get_cpu_stat()
{
	TpList<TpCpuStatParam> stat;
	std::ifstream statFile("/proc/stat");
	if (!statFile.is_open())
	{
		std::cerr << "Error opening /proc/stat." << std::endl;
		return stat;
	}
	TpString line;
	while (std::getline(statFile, line))
	{
		std::istringstream iss(line);
		TpString cpu;
		uint64_t user, nice, system, idle, iowait, irq, softirq;
		iss >> cpu >> user >> nice >> system >> idle >> iowait >> irq >> softirq;

		if (strncmp(cpu.c_str(), "cpu", 3) != 0)
			break;
		uint16_t proc = 0;
		TpString cpu_num=cpu.substr(cpu.find("cpu") + 3);
		if(cpu_num.size()!=0)
			proc=std::stoi(cpu_num);
		// TpCpuStatParam stat_temp(proc,(user+nice+system+irq+softirq),idle,iowait);
		// stat.emplace_back(proc,(user+nice+system+irq+softirq),idle,iowait);
		stat.push_back(TpCpuStatParam(proc, (user + nice + system + irq + softirq), idle, iowait));
	}
	// 计算总 CPU 时间
	return stat;
}




TpCpuCore::TpCpuCore(uint16_t core)
{
	data_ = new TpCpuCoreData(core);
}
TpCpuCore::TpCpuCore(uint16_t core, uint64_t used, uint64_t idle, uint64_t iowait)
{
	data_ = new TpCpuCoreData(core,used,idle,iowait);
}
TpCpuCore::TpCpuCore()
{
	data_ = new TpCpuCoreData(0);
}

TpCpuCore::~TpCpuCore()
{
	TpCpuCoreData *coreData = static_cast<TpCpuCoreData *>(data_);
	if(coreData)
	{
		delete coreData;
		coreData = nullptr;
	}
}


TpString TpCpuCore::getName()
{
	TpCpuCoreData *coreData = static_cast<TpCpuCoreData *>(data_);
	return coreData->cpuInfo.name;
}

int TpCpuCore::setName(const TpString &name)
{
	TpCpuCoreData *coreData = static_cast<TpCpuCoreData *>(data_);
	coreData->cpuInfo.name=name;
	return 0;
}

int TpCpuCore::getCoreNum()
{
	TpCpuCoreData *coreData = static_cast<TpCpuCoreData *>(data_);
	return coreData->cpuInfo.processor;
}
int TpCpuCore::setCoreNum(uint16_t processor)
{
	return 0;
}

double TpCpuCore::getFrequency()
{
	TpCpuCoreData *coreData = static_cast<TpCpuCoreData *>(data_);
	return coreData->cpuInfo.freq;
}

int TpCpuCore::setFrequency(double frequency)
{
	TpCpuCoreData *coreData = static_cast<TpCpuCoreData *>(data_);
	coreData->cpuInfo.freq=frequency;
	return 0;
}

int TpCpuCore::setStat(const void *stat)
{
	TpCpuCoreData *coreData = static_cast<TpCpuCoreData *>(data_);
	TpCpuStatParam *statParam = (TpCpuStatParam *)stat;
	coreData->cpuStat=*statParam;
	return 0;
}

int TpCpuCore::setUsage(double usage)
{
	TpCpuCoreData *coreData = static_cast<TpCpuCoreData *>(data_);
	coreData->data.dataWriteLock();
	coreData->cpuStat.usage=usage;
	coreData->data.dataUnlock();
	return 0;
}

double TpCpuCore::getUsage()
{
	TpCpuCoreData *coreData = static_cast<TpCpuCoreData *>(data_);
	double usage;
	coreData->data.dataReadLock();
	usage=coreData->cpuStat.usage;
	coreData->data.dataUnlock();
	return usage;
}


// 线程里面自动更新cpu信息
/*TpCpu::TpCpu(bool thread_enable, uint16_t time_samp)
{
	data_ = new TpCpuData();
	TpCpuData *cpuData = static_cast<TpCpuData *>(data_);

	uint16_t num = getCpuInfo("/proc/cpuinfo");

	// 根据核心数量来添加到cpus_stat,第一个是总的cpu情况，process此时不生效
	cpuData->cpusStat.push_back(0);

	for (int i = 0; i < num; i++)
		cpuData->cpusStat.push_back(i);

	//	printf("size=%ld\n",cpuData->cpusStat.size());
	if (thread_enable)
	{
		cpuData->data.running = true;
		cpuData->data.thread_t = std::thread(&TpCpu::threadUpdateStat, this, num, time_samp);
	}
}*/

TpCpu::TpCpu(const TpString &name,tpBool enable, uint16_t samp)
{
	data_ = new TpCpuData();
	TpCpuData *cpuData = static_cast<TpCpuData *>(data_);

	uint16_t num = readCpuInfo("/proc/cpuinfo");
	// 根据核心数量来添加到cpus_stat,第一个是总的cpu情况，process此时不生效
	//cpuData->cpuCore.push_back(0);
//	for (int i = 0; i < num; i++)
//		std::cout<<"name:"<<cpuData->cpuCore[i]->getName()<<std::endl;

	//	printf("size=%ld\n",cpuData->cpusStat.size());
	if (enable)
	{
		cpuData->data.running = true;
		cpuData->data.thread_t = std::thread(&TpCpu::threadUpdateStat, this, num, samp);
	}
}


TpCpu::~TpCpu()
{
	TpCpuData *cpuData = static_cast<TpCpuData *>(data_);

	cpuData->data.running = false;
	if (cpuData->data.thread_t.joinable())
		cpuData->data.thread_t.join(); // 等待线程完成

	if (cpuData)
	{
		for (auto &core : cpuData->cpuCore)
		{
			if (core)
			{
				delete core;
				core=nullptr;
			}
		}
		delete cpuData;
		cpuData = nullptr;
		data_ = nullptr;
	}
}

int TpCpu::readCpuInfo(const char *path)
{
	TpCpuData *cpuData = static_cast<TpCpuData *>(data_);

	std::ifstream fd("/proc/cpuinfo");
	if (!fd)
		return -1;

	TpString line;
	int processor = -1;
	while (std::getline(fd, line))
	{
		if (line.find("processor") != TpString::npos)
		{
			processor = std::stoi(line.substr(line.find(':') + 2));
			TpCpuCore *core=new TpCpuCore((uint16_t)processor);
			cpuData->cpuCore.emplace_back(core);
		}
		else if (line.find("model name") != TpString::npos)
		{
			if (cpuData->cpuCore.size() > processor && cpuData->cpuCore.size() != 0)
				cpuData->cpuCore[processor]->setName(line.substr(line.find(':') + 2));
		}
		else if (line.find("cpu MHz") != TpString::npos)
		{
			if (cpuData->cpuCore.size() > processor && cpuData->cpuCore.size() != 0)
				cpuData->cpuCore[processor]->setFrequency(std::stof(line.substr(line.find(':') + 2)));
		}
	}
	TpCpuCore *core=new TpCpuCore(0);
	cpuData->cpuCore.insert(cpuData->cpuCore.begin(), core);
	return (processor + 1);
}

//更新cpu状态(使用率)
void TpCpu::threadUpdateStat(uint16_t core_num, uint16_t time_samp)
{
	TpCpuData *cpuData = static_cast<TpCpuData *>(data_);

	double time_s = (double)time_samp * 0.001;
	TpList<TpCpuStatParam> stat_l;
	TpList<TpCpuStatParam> stat_n; // 上次状态和当前状态
	stat_l.resize(core_num +1);
	stat_n.resize(core_num +1);
	stat_l = get_cpu_stat();
	while (cpuData->data.running)
	{
		std::this_thread::sleep_for(std::chrono::milliseconds(time_samp));
		stat_n = get_cpu_stat();
		for (int i = 0; i < (core_num + 1); i++)
		{
			TpCpuStatParam stat;
			stat.time_idle = stat_n[i].time_idle - stat_l[i].time_idle;
			stat.time_iowait = stat_n[i].time_iowait - stat_l[i].time_iowait;
			stat.time_used = stat_n[i].time_used - stat_l[i].time_used;
			stat.usage = (double)stat.time_used / (double)(stat.time_idle + stat.time_iowait + stat.time_used) * 100;
			stat.processor = stat_n[i].processor;
			updateState(i, (void *)(&stat));
		}
		stat_l = stat_n;
	}
}

//暂时不支持手动更新
int TpCpu::update()
{
	TpCpuData *cpuData = static_cast<TpCpuData *>(data_);
	if (cpuData->data.running) // 进程运行时候会自动更新
		return 0;
	static TpList<TpCpuStatParam> stat_l;
	TpList<TpCpuStatParam> stat_n; // 上次状态和当前状态
	//

	return 0;
}

void TpCpu::updateState(uint16_t num, void *stat)
{
	TpCpuData *cpuData = static_cast<TpCpuData *>(data_);

	cpuData->data.dataWriteLock();
	if(num<cpuData->cpuCore.size())
	{
		cpuData->cpuCore[num]->setStat(stat);
		//cpuData->cpuCore[num].setUsage(stat.usage);
	}
	cpuData->data.dataUnlock();
}

TpList<TpCpuCore*> TpCpu::getList()
{
	TpCpuData *cpuData = static_cast<TpCpuData *>(data_);
	return cpuData->cpuCore;
}

TpList<TpCpuCore*> TpCpu::getState()
{
	TpCpuData *cpuData = static_cast<TpCpuData *>(data_);
	update();
	return cpuData->cpuCore;
}

TpCpuCore* TpCpu::getCoreState()
{
	return getCpuCoreState(-1);
}
TpCpuCore* TpCpu::getCoreState(uint16_t core)
{
	return getCpuCoreState(core);
}

TpCpuCore* TpCpu::getCpuCoreState(int processor)
{
	TpCpuData *cpuData = static_cast<TpCpuData *>(data_);
	update();
	if(processor<0)
		return cpuData->cpuCore[0];
	for (auto &it : cpuData->cpuCore)
	{
		if(it->getCoreNum()==processor)
        {
            return it;
        }
	}
	return cpuData->cpuCore[0];
}