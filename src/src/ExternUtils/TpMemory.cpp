
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
#include "TpMemory.h"


struct TpMemoryInfoParam
{
	TpString name;
	uint64_t size_total;
	uint64_t size_free;
	uint64_t size_available;
	double usage; // 使用率
	uint64_t speed;
	TpMemoryInfoParam() : size_total(0), size_free(0), size_available(0), usage(0.0) {}
};

struct TpMemoryInfoData
{
	TpMemoryInfoParam param;
	//继续添加其他参数或结构体
	TpMemoryInfoData()
	{
	}
};


TpMemory::TpMemory(bool enable, uint16_t samp)
{
	data_ = new TpMemoryInfoData();
	// update();
}

TpMemory::~TpMemory()
{
	TpMemoryInfoData* memoryData = static_cast<TpMemoryInfoData*>(data_);
	if(memoryData)
		delete memoryData;
}

uint64_t TpMemory::getMemoryValue(const char *key)
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
uint64_t TpMemory::getAvailableSize()
{
	TpMemoryInfoData* memoryData = static_cast<TpMemoryInfoData*>(data_);
	
	memoryData->param.size_available = getMemoryValue("MemAvailable:");
	return memoryData->param.size_available;
}

// 空闲内存
uint64_t TpMemory::getFreeSize()
{
	TpMemoryInfoData* memoryData = static_cast<TpMemoryInfoData*>(data_);

	memoryData->param.size_free = getMemoryValue("MemFree:");
	return memoryData->param.size_free;
}

// 总内存大小
uint64_t TpMemory::getTotalSize()
{
	TpMemoryInfoData* memoryData = static_cast<TpMemoryInfoData*>(data_);

	memoryData->param.size_total = getMemoryValue("MemTotal:");
	return memoryData->param.size_total;
}

// 内存使用率
double TpMemory::getUsage(bool unupdate)
{
	TpMemoryInfoData* memoryData = static_cast<TpMemoryInfoData*>(data_);

	if (!unupdate)
		update();

	memoryData->param.usage = (double)(memoryData->param.size_total - memoryData->param.size_available) / (double)memoryData->param.size_total * 100.0;
	return memoryData->param.usage;
}

void TpMemory::update()
{
	TpMemoryInfoData* memoryData = static_cast<TpMemoryInfoData*>(data_);

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

TpMemory TpMemory::getMemoryInfo(bool unupdate)
{
	if (!unupdate)
		update();
	return *this;
}

