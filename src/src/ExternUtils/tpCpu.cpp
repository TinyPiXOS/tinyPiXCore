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

#include "tpCpu.h"
#include "tpSystemDataManage.h"


struct tpCpuInfoParam
{
	uint16_t processor;								  // 核心编号,
	tpString name;									  // 名称
	double freq;									  // 频率
	//double usage;
	tpCpuInfoParam(uint16_t proc) : processor(proc) {} //
};

// proc/stat文件中的参数以及计算的利用率（本结构体改为内部使用）
struct tpCpuStatParam
{
	uint16_t processor; // 核心编号
	double usage;	   // 使用率
	//	time_t time_samp;		//当前数据读取的时间
	uint64_t time_used;	  // 使用时间
	uint64_t time_idle;	  // 空闲时间
	uint64_t time_iowait; // 等待时间

	// cpu总运行时间=使用时间used+idle+iowait
	tpCpuStatParam(uint16_t proc, uint64_t used, uint64_t idle, uint64_t iowait) : processor(proc),
																					time_used(used),
																					time_idle(idle),
																					time_iowait(iowait) {} //
	tpCpuStatParam(uint16_t proc) : processor(proc) {}													 //
	tpCpuStatParam() {}
};

struct tpCpuCoreData
{
	tpSystemDataManage data;

	tpCpuInfoParam cpuInfo;
	tpCpuStatParam cpuStat; 					// 总/分核心
	tpCpuCoreData(uint16_t proc): cpuInfo(proc) {} 
	tpCpuCoreData(uint16_t proc, uint64_t used, uint64_t idle, uint64_t iowait) : 
					cpuStat(proc,used,idle,iowait),cpuInfo(proc) {} //
};

struct tpCpuData
{
	tpSystemDataManage data;

//	tpList<tpCpuInfoParam> cpuInfo;
	//	tpCpuStatParam cpu_stat;				//总
//	tpList<tpCpuStatParam> cpusStat; // 总+分核心
	tpList<tpCpuCore *> cpuCore;
	tpCpuData()
	{
	}
};


tpList<tpCpuStatParam> get_cpu_stat()
{
	tpList<tpCpuStatParam> stat;
	std::ifstream statFile("/proc/stat");
	if (!statFile.is_open())
	{
		std::cerr << "Error opening /proc/stat." << std::endl;
		return stat;
	}
	tpString line;
	while (std::getline(statFile, line))
	{
		std::istringstream iss(line);
		tpString cpu;
		uint64_t user, nice, system, idle, iowait, irq, softirq;
		iss >> cpu >> user >> nice >> system >> idle >> iowait >> irq >> softirq;

		if (strncmp(cpu.c_str(), "cpu", 3) != 0)
			break;
		uint16_t proc = 0;
		tpString cpu_num=cpu.substr(cpu.find("cpu") + 3);
		if(cpu_num.size()!=0)
			proc=std::stoi(cpu_num);
		// tpCpuStatParam stat_temp(proc,(user+nice+system+irq+softirq),idle,iowait);
		// stat.emplace_back(proc,(user+nice+system+irq+softirq),idle,iowait);
		stat.push_back(tpCpuStatParam(proc, (user + nice + system + irq + softirq), idle, iowait));
	}
	// 计算总 CPU 时间
	return stat;
}




tpCpuCore::tpCpuCore(uint16_t core)
{
	data_ = new tpCpuCoreData(core);
}
tpCpuCore::tpCpuCore(uint16_t core, uint64_t used, uint64_t idle, uint64_t iowait)
{
	data_ = new tpCpuCoreData(core,used,idle,iowait);
}
tpCpuCore::tpCpuCore()
{
	data_ = new tpCpuCoreData(0);
}

tpCpuCore::~tpCpuCore()
{
	tpCpuCoreData *coreData = static_cast<tpCpuCoreData *>(data_);
	if(coreData)
	{
		delete coreData;
		coreData = nullptr;
	}
}


tpString tpCpuCore::getName()
{
	tpCpuCoreData *coreData = static_cast<tpCpuCoreData *>(data_);
	return coreData->cpuInfo.name;
}

int tpCpuCore::setName(const tpString &name)
{
	tpCpuCoreData *coreData = static_cast<tpCpuCoreData *>(data_);
	coreData->cpuInfo.name=name;
	return 0;
}

int tpCpuCore::getCoreNum()
{
	tpCpuCoreData *coreData = static_cast<tpCpuCoreData *>(data_);
	return coreData->cpuInfo.processor;
}
int tpCpuCore::setCoreNum(uint16_t processor)
{
	return 0;
}

double tpCpuCore::getFrequency()
{
	tpCpuCoreData *coreData = static_cast<tpCpuCoreData *>(data_);
	return coreData->cpuInfo.freq;
}

int tpCpuCore::setFrequency(double frequency)
{
	tpCpuCoreData *coreData = static_cast<tpCpuCoreData *>(data_);
	coreData->cpuInfo.freq=frequency;
	return 0;
}

int tpCpuCore::setStat(const void *stat)
{
	tpCpuCoreData *coreData = static_cast<tpCpuCoreData *>(data_);
	tpCpuStatParam *statParam = (tpCpuStatParam *)stat;
	coreData->cpuStat=*statParam;
	return 0;
}

int tpCpuCore::setUsage(double usage)
{
	tpCpuCoreData *coreData = static_cast<tpCpuCoreData *>(data_);
	coreData->data.dataWriteLock();
	coreData->cpuStat.usage=usage;
	coreData->data.dataUnlock();
	return 0;
}

double tpCpuCore::getUsage()
{
	tpCpuCoreData *coreData = static_cast<tpCpuCoreData *>(data_);
	double usage;
	coreData->data.dataReadLock();
	usage=coreData->cpuStat.usage;
	coreData->data.dataUnlock();
	return usage;
}


// 线程里面自动更新cpu信息
/*tpCpu::tpCpu(bool thread_enable, uint16_t time_samp)
{
	data_ = new tpCpuData();
	tpCpuData *cpuData = static_cast<tpCpuData *>(data_);

	uint16_t num = getCpuInfo("/proc/cpuinfo");

	// 根据核心数量来添加到cpus_stat,第一个是总的cpu情况，process此时不生效
	cpuData->cpusStat.push_back(0);

	for (int i = 0; i < num; i++)
		cpuData->cpusStat.push_back(i);

	//	printf("size=%ld\n",cpuData->cpusStat.size());
	if (thread_enable)
	{
		cpuData->data.running = true;
		cpuData->data.thread_t = std::thread(&tpCpu::threadUpdateStat, this, num, time_samp);
	}
}*/

tpCpu::tpCpu(const tpString &name,tpBool enable, uint16_t samp)
{
	data_ = new tpCpuData();
	tpCpuData *cpuData = static_cast<tpCpuData *>(data_);

	uint16_t num = readCpuInfo("/proc/cpuinfo");
	// 根据核心数量来添加到cpus_stat,第一个是总的cpu情况，process此时不生效
	//cpuData->cpuCore.push_back(0);
//	for (int i = 0; i < num; i++)
//		std::cout<<"name:"<<cpuData->cpuCore[i]->getName()<<std::endl;

	//	printf("size=%ld\n",cpuData->cpusStat.size());
	if (enable)
	{
		cpuData->data.running = true;
		cpuData->data.thread_t = std::thread(&tpCpu::threadUpdateStat, this, num, samp);
	}
}


tpCpu::~tpCpu()
{
	tpCpuData *cpuData = static_cast<tpCpuData *>(data_);

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

int tpCpu::readCpuInfo(const char *path)
{
	tpCpuData *cpuData = static_cast<tpCpuData *>(data_);

	std::ifstream fd("/proc/cpuinfo");
	if (!fd)
		return -1;

	tpString line;
	int processor = -1;
	while (std::getline(fd, line))
	{
		if (line.find("processor") != tpString::npos)
		{
			processor = std::stoi(line.substr(line.find(':') + 2));
			tpCpuCore *core=new tpCpuCore((uint16_t)processor);
			cpuData->cpuCore.emplace_back(core);
		}
		else if (line.find("model name") != tpString::npos)
		{
			if (cpuData->cpuCore.size() > processor && cpuData->cpuCore.size() != 0)
				cpuData->cpuCore[processor]->setName(line.substr(line.find(':') + 2));
		}
		else if (line.find("cpu MHz") != tpString::npos)
		{
			if (cpuData->cpuCore.size() > processor && cpuData->cpuCore.size() != 0)
				cpuData->cpuCore[processor]->setFrequency(std::stof(line.substr(line.find(':') + 2)));
		}
	}
	tpCpuCore *core=new tpCpuCore(0);
	cpuData->cpuCore.insert(cpuData->cpuCore.begin(), core);
	return (processor + 1);
}

//更新cpu状态(使用率)
void tpCpu::threadUpdateStat(uint16_t core_num, uint16_t time_samp)
{
	tpCpuData *cpuData = static_cast<tpCpuData *>(data_);

	double time_s = (double)time_samp * 0.001;
	tpList<tpCpuStatParam> stat_l;
	tpList<tpCpuStatParam> stat_n; // 上次状态和当前状态
	stat_l.resize(core_num +1);
	stat_n.resize(core_num +1);
	stat_l = get_cpu_stat();
	while (cpuData->data.running)
	{
		std::this_thread::sleep_for(std::chrono::milliseconds(time_samp));
		stat_n = get_cpu_stat();
		for (int i = 0; i < (core_num + 1); i++)
		{
			tpCpuStatParam stat;
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
int tpCpu::update()
{
	tpCpuData *cpuData = static_cast<tpCpuData *>(data_);
	if (cpuData->data.running) // 进程运行时候会自动更新
		return 0;
	static tpList<tpCpuStatParam> stat_l;
	tpList<tpCpuStatParam> stat_n; // 上次状态和当前状态
	//

	return 0;
}

void tpCpu::updateState(uint16_t num, void *stat)
{
	tpCpuData *cpuData = static_cast<tpCpuData *>(data_);

	cpuData->data.dataWriteLock();
	if(num<cpuData->cpuCore.size())
	{
		cpuData->cpuCore[num]->setStat(stat);
		//cpuData->cpuCore[num].setUsage(stat.usage);
	}
	cpuData->data.dataUnlock();
}

tpList<tpCpuCore*> tpCpu::getList()
{
	tpCpuData *cpuData = static_cast<tpCpuData *>(data_);
	return cpuData->cpuCore;
}

tpList<tpCpuCore*> tpCpu::getState()
{
	tpCpuData *cpuData = static_cast<tpCpuData *>(data_);
	update();
	return cpuData->cpuCore;
}

tpCpuCore* tpCpu::getCoreState()
{
	return getCpuCoreState(-1);
}
tpCpuCore* tpCpu::getCoreState(uint16_t core)
{
	return getCpuCoreState(core);
}

tpCpuCore* tpCpu::getCpuCoreState(int processor)
{
	tpCpuData *cpuData = static_cast<tpCpuData *>(data_);
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