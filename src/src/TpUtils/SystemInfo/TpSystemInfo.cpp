/*///------------------------------------------------------------------------------------------------------------------------//
		系统信息
说 明 :
日 期 : 2024.11.04

/*/
//------------------------------------------------------------------------------------------------------------------------//

#include <iostream>
#include <fstream>
#include <string>
#include <dirent.h>
#include <vector>
#include <atomic>
#include <thread>
#include <pthread.h>

#include "TpFile.h"
#include "TpSystemInfo.h"
#include "SystemInfo/TpDiskManage.h"
#include "SystemInfo/TpCpuManage.h"
#include "SystemInfo/TpMemory.h"
//#include "TpGpuManage.h"

struct TpSystemInfoData
{
	TpString MachineName;
	TpString SystemName;
	TpString SystemVersion;

	TpDiskManage *disk_m;
	TpCpuManage *cpu_m;
	TpCpu *cpu;
	TpMemory *memory_m;
	//TpGpuManage *gpu_m;
	TpNetworkManage *net_m;
	TpSystemInfoData()
	{
		disk_m=nullptr;
		cpu_m=nullptr;
		memory_m=nullptr;
		cpu=nullptr;
		net_m=nullptr;
	}
};

TpSystemInfo::TpSystemInfo()
{
	data_ = new TpSystemInfoData();
	TpSystemInfoData *sysData = static_cast<TpSystemInfoData *>(data_);
}

TpSystemInfo::~TpSystemInfo()
{
	TpSystemInfoData *sysData = static_cast<TpSystemInfoData *>(data_);
	if(sysData->disk_m!=nullptr)
	{
		delete sysData->disk_m;
	}
	if(sysData->cpu_m!=nullptr)
	{
		delete sysData->cpu_m;
	}
	if(sysData->memory_m!=nullptr)
	{
		delete sysData->memory_m;
	}
	if(sysData->cpu!=nullptr)
	{
		delete sysData->cpu;
	}
	if(sysData->net_m!=nullptr)
	{
		delete sysData->net_m;
	}
}

// 直接返回file文件的内容到字符串(一般用于读一行)
TpString TpSystemInfo::getValueFromeFile(const char *file)
{
	TpString value = "";
	std::ifstream fd(file);
	if (fd)
	{
		std::getline(fd, value); 
		fd.close(); // 关闭文件
	}
	return value;
}

// 直接设置file文件的内容(一般用于只有一行的文件)
int TpSystemInfo::setValueFromeFile(const char *file, const char *value)
{
	std::ofstream fd(file, std::ios::out | std::ios::trunc); // 可写打开并且清空原来内容
	if (!fd)
		return -1;
	fd << value << std::endl;
	fd.close();
	return 0;
}

TpString TpSystemInfo::getMachineName()
{
	return getValueFromeFile("/etc/hostname");
}

int TpSystemInfo::setMachineName(TpString &name)
{
	std::ofstream fd("/etx/hostname", std::ios::out | std::ios::trunc); // 可写打开并且清空原来内容
	if (!fd)
		return -1;
	fd << name << std::endl;
	fd.close();
	return 0;
}

TpString TpSystemInfo::getSystemValue(const char *item)
{
	TpString value = "None";
	std::ifstream fd("/etc/os-release");
	if (!fd)
		return value;
	TpString line;
	while (std::getline(fd, line))
	{	// 逐行读取文件
		// std::cout << line << std::endl; // 输出读取的行
		if (strncmp(line.c_str(), item, strlen(item)) == 0)
		{
			size_t pos = line.find('=');
			if (pos != std::string::npos)
				value = line.substr(pos + 2, line.length() - pos - 3);
			break;
		}
	}
	fd.close();
	return value;
}

TpString TpSystemInfo::getSystemName()
{
	return getSystemValue("NAME=");
}

TpString TpSystemInfo::getSystemVersion()
{
	return getSystemValue("VERSION=");
}

TpString TpSystemInfo::getBoardVendor() // 主板厂商
{
	return getValueFromeFile("/sys/class/dmi/id/board_vendor");
}
TpString TpSystemInfo::getBoardName() // 主板名字
{
	return getValueFromeFile("/sys/class/dmi/id/board_name");
}
TpString TpSystemInfo::getBoardVersion() // 主板版本
{
	return getValueFromeFile("/sys/class/dmi/id/board_version");
}
TpString TpSystemInfo::getBoardSerial() // 主板序列号
{
	return getValueFromeFile("/sys/class/dmi/id/board_serial");
}
TpString TpSystemInfo::getBiosData() // BIOS日期
{
	return getValueFromeFile("/sys/class/dmi/id/bios_data");
}
TpString TpSystemInfo::getBiosVendor() // BIOS厂商
{
	return getValueFromeFile("/sys/class/dmi/id/bios_vendor");
}
TpString TpSystemInfo::getBiosVersion() // BIOS版本
{
	return getValueFromeFile("/sys/class/dmi/id/bios_version");
}
TpString TpSystemInfo::getProductName() // 产品名称
{
	return getValueFromeFile("/sys/class/dmi/id/product_name");
}
TpString TpSystemInfo::getProductFamily() // 产品
{
	return getValueFromeFile("/sys/class/dmi/id/product_family");
}
TpString TpSystemInfo::getProductSerial() // 产品序列号
{
	return getValueFromeFile("/sys/class/dmi/id/product_serial");
}
TpString TpSystemInfo::getProductSku() // 产品库存单位
{
	return getValueFromeFile("/sys/class/dmi/id/product_sku");
}
TpString TpSystemInfo::getProductUuid() // 产品uuid
{
	return getValueFromeFile("/sys/class/dmi/id/product_uuid");
}
TpString TpSystemInfo::getProductVersion() // 产品版本号
{
	return getValueFromeFile("/sys/class/dmi/id/product_version");
}

TpList<TpCpuCore*> TpSystemInfo::getCpuCoreInfo(const TpString& name)
{
	TpSystemInfoData *sysData = static_cast<TpSystemInfoData *>(data_);
	sysData->cpu=new TpCpu("",TP_FALSE);
	return sysData->cpu->getList();
}

TpList<TpCpu*> TpSystemInfo::getCpuInfo()
{
	TpSystemInfoData *sysData = static_cast<TpSystemInfoData *>(data_);
	sysData->cpu_m=new TpCpuManage(TP_FALSE);
	return sysData->cpu_m->getList();
}


TpList<TpCpuCore*> TpSystemInfo::getCpuCoreState(const TpString& name)
{
	TpSystemInfoData *sysData = static_cast<TpSystemInfoData *>(data_);
	sysData->cpu=new TpCpu("",TP_TRUE);
	return sysData->cpu->getState();
}

TpList<TpGpu *> TpSystemInfo::getGpuInfo()
{
	TpList<TpGpu *> info_list;
	return info_list;
}

// 磁盘列表以及每个磁盘的参数(获取详细使用情况使用DiskInfo接口)
TpList<TpDisk *> TpSystemInfo::getDiskInfo()
{
	TpSystemInfoData *sysData = static_cast<TpSystemInfoData *>(data_);
	
	sysData->disk_m=new TpDiskManage();
	return sysData->disk_m->getList();
}

// 网卡信息不通过系统接口获取

// 内存信息
TpMemory TpSystemInfo::getMemoryInfo()
{
	TpMemory info(false);
	return info.getMemoryInfo();
}

double TpSystemInfo::getMemoryUsage()
{
	TpMemory info(true);
	return info.getUsage();
}

TpList<TpNetworkInfo *> TpSystemInfo::getNetworkInfo()
{
	TpSystemInfoData *sysData = static_cast<TpSystemInfoData *>(data_);
	sysData->net_m=new TpNetworkManage();
	return sysData->net_m->getList();
}
