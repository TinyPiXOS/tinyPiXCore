
/*///------------------------------------------------------------------------------------------------------------------------//
		系统内存信息
说 明 :
日 期 : 2024.11.05

/*/
//------------------------------------------------------------------------------------------------------------------------//

#include <iostream>
#include <fstream>
#include <string>
#include <dirent.h>
#include <string.h>
#include "tpMemory.h"


struct tpMemoryInfoParam
{
	tpString name;
	uint64_t size_total;
	uint64_t size_free;
	uint64_t size_available;
	double usage; // 使用率
	uint64_t speed;
	tpMemoryInfoParam() : size_total(0), size_free(0), size_available(0), usage(0.0) {}
};

struct tpMemoryInfoData
{
	tpMemoryInfoParam param;
	//继续添加其他参数或结构体
	tpMemoryInfoData()
	{
	}
};


tpMemory::tpMemory(bool enable, uint16_t samp)
{
	data_ = new tpMemoryInfoData();
	// update();
}

tpMemory::~tpMemory()
{
	tpMemoryInfoData* memoryData = static_cast<tpMemoryInfoData*>(data_);
	if(memoryData)
		delete memoryData;
}

uint64_t tpMemory::getMemoryValue(const char *key)
{
	uint64_t value_i = 0;
	std::ifstream fd("/proc/meminfo");
	if (!fd)
		return 0;
	std::string line;
	while (std::getline(fd, line))
	{
		if (line.find(key) != std::string::npos)
		{
			value_i = std::stoull(line.substr(line.find(':') + 2));
			break;
		}
	}
	fd.close();
	return value_i;
}

// 可用内存
uint64_t tpMemory::getAvailableSize()
{
	tpMemoryInfoData* memoryData = static_cast<tpMemoryInfoData*>(data_);
	
	memoryData->param.size_available = getMemoryValue("MemAvailable:");
	return memoryData->param.size_available;
}

// 空闲内存
uint64_t tpMemory::getFreeSize()
{
	tpMemoryInfoData* memoryData = static_cast<tpMemoryInfoData*>(data_);

	memoryData->param.size_free = getMemoryValue("MemFree:");
	return memoryData->param.size_free;
}

// 总内存大小
uint64_t tpMemory::getTotalSize()
{
	tpMemoryInfoData* memoryData = static_cast<tpMemoryInfoData*>(data_);

	memoryData->param.size_total = getMemoryValue("MemTotal:");
	return memoryData->param.size_total;
}

// 内存使用率
double tpMemory::getUsage(bool unupdate)
{
	tpMemoryInfoData* memoryData = static_cast<tpMemoryInfoData*>(data_);

	if (!unupdate)
		update();

	memoryData->param.usage = (double)(memoryData->param.size_total - memoryData->param.size_available) / (double)memoryData->param.size_total * 100.0;
	return memoryData->param.usage;
}

void tpMemory::update()
{
	tpMemoryInfoData* memoryData = static_cast<tpMemoryInfoData*>(data_);

	std::ifstream fd("/proc/meminfo");
	if (!fd)
		return;
	std::string line;
	while (std::getline(fd, line))
	{
		if (line.find("MemTotal:") != std::string::npos)
		{
			memoryData->param.size_total = std::stoull(line.substr(line.find(':') + 2));
		}
		else if (line.find("MemFree:") != std::string::npos)
		{
			memoryData->param.size_free = std::stoull(line.substr(line.find(':') + 2));
		}
		else if (line.find("MemAvailable:") != std::string::npos)
		{
			memoryData->param.size_available = std::stoull(line.substr(line.find(':') + 2));
		}
	}
	if (memoryData->param.size_total == 0)
		return;
	memoryData->param.usage = (double)(memoryData->param.size_total - memoryData->param.size_free - memoryData->param.size_available) / (double)memoryData->param.size_total * 100.0;
	fd.close();
}

tpMemory tpMemory::getMemoryInfo(bool unupdate)
{
	if (!unupdate)
		update();
	return *this;
}

