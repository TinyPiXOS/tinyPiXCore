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

#include "tpFile.h"
#include "tpSystemInfo.h"
#include "tpDiskManage.h"
#include "tpCpuManage.h"
//#include "tpGpuManage.h"
#include "tpMemory.h"

struct tpSystemInfoData
{
	tpString MachineName;
	tpString SystemName;
	tpString SystemVersion;

	tpDiskManage *disk_m;
	tpCpuManage *cpu_m;
	tpCpu *cpu;
	tpMemory *memory_m;
	//tpGpuManage *gpu_m;
	tpNetworkManage *net_m;
	tpSystemInfoData()
	{
		disk_m=nullptr;
		cpu_m=nullptr;
		memory_m=nullptr;
		cpu=nullptr;
		net_m=nullptr;
	}
};

tpSystemInfo::tpSystemInfo()
{
	data_ = new tpSystemInfoData();
	tpSystemInfoData *sysData = static_cast<tpSystemInfoData *>(data_);
}

tpSystemInfo::~tpSystemInfo()
{
	tpSystemInfoData *sysData = static_cast<tpSystemInfoData *>(data_);
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
tpString tpSystemInfo::getValueFromeFile(const char *file)
{
	tpString value = "";
	std::ifstream fd(file);
	if (fd)
	{
		std::getline(fd, value); 
		fd.close(); // 关闭文件
	}
	return value;
}

// 直接设置file文件的内容(一般用于只有一行的文件)
int tpSystemInfo::setValueFromeFile(const char *file, const char *value)
{
	std::ofstream fd(file, std::ios::out | std::ios::trunc); // 可写打开并且清空原来内容
	if (!fd)
		return -1;
	fd << value << std::endl;
	fd.close();
	return 0;
}

tpString tpSystemInfo::getMachineName()
{
	return getValueFromeFile("/etc/hostname");
}

int tpSystemInfo::setMachineName(tpString &name)
{
	std::ofstream fd("/etx/hostname", std::ios::out | std::ios::trunc); // 可写打开并且清空原来内容
	if (!fd)
		return -1;
	fd << name << std::endl;
	fd.close();
	return 0;
}

tpString tpSystemInfo::getSystemValue(const char *item)
{
	tpString value = "None";
	std::ifstream fd("/etc/os-release");
	if (!fd)
		return value;
	tpString line;
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

tpString tpSystemInfo::getSystemName()
{
	return getSystemValue("NAME=");
}

tpString tpSystemInfo::getSystemVersion()
{
	return getSystemValue("VERSION=");
}

tpString tpSystemInfo::getBoardVendor() // 主板厂商
{
	return getValueFromeFile("/sys/class/dmi/id/board_vendor");
}
tpString tpSystemInfo::getBoardName() // 主板名字
{
	return getValueFromeFile("/sys/class/dmi/id/board_name");
}
tpString tpSystemInfo::getBoardVersion() // 主板版本
{
	return getValueFromeFile("/sys/class/dmi/id/board_version");
}
tpString tpSystemInfo::getBoardSerial() // 主板序列号
{
	return getValueFromeFile("/sys/class/dmi/id/board_serial");
}
tpString tpSystemInfo::getBiosData() // BIOS日期
{
	return getValueFromeFile("/sys/class/dmi/id/bios_data");
}
tpString tpSystemInfo::getBiosVendor() // BIOS厂商
{
	return getValueFromeFile("/sys/class/dmi/id/bios_vendor");
}
tpString tpSystemInfo::getBiosVersion() // BIOS版本
{
	return getValueFromeFile("/sys/class/dmi/id/bios_version");
}
tpString tpSystemInfo::getProductName() // 产品名称
{
	return getValueFromeFile("/sys/class/dmi/id/product_name");
}
tpString tpSystemInfo::getProductFamily() // 产品
{
	return getValueFromeFile("/sys/class/dmi/id/product_family");
}
tpString tpSystemInfo::getProductSerial() // 产品序列号
{
	return getValueFromeFile("/sys/class/dmi/id/product_serial");
}
tpString tpSystemInfo::getProductSku() // 产品库存单位
{
	return getValueFromeFile("/sys/class/dmi/id/product_sku");
}
tpString tpSystemInfo::getProductUuid() // 产品uuid
{
	return getValueFromeFile("/sys/class/dmi/id/product_uuid");
}
tpString tpSystemInfo::getProductVersion() // 产品版本号
{
	return getValueFromeFile("/sys/class/dmi/id/product_version");
}

tpList<tpCpuCore*> tpSystemInfo::getCpuCoreInfo(const tpString& name)
{
	tpSystemInfoData *sysData = static_cast<tpSystemInfoData *>(data_);
	sysData->cpu=new tpCpu("",TP_FALSE);
	return sysData->cpu->getList();
}

tpList<tpCpu*> tpSystemInfo::getCpuInfo()
{
	tpSystemInfoData *sysData = static_cast<tpSystemInfoData *>(data_);
	sysData->cpu_m=new tpCpuManage(TP_FALSE);
	return sysData->cpu_m->getList();
}


tpList<tpCpuCore*> tpSystemInfo::getCpuCoreState(const tpString& name)
{
	tpSystemInfoData *sysData = static_cast<tpSystemInfoData *>(data_);
	sysData->cpu=new tpCpu("",TP_TRUE);
	return sysData->cpu->getState();
}

tpList<tpGpu *> tpSystemInfo::getGpuInfo()
{
	tpList<tpGpu *> info_list;
	return info_list;
}

// 磁盘列表以及每个磁盘的参数(获取详细使用情况使用DiskInfo接口)
tpList<tpDisk *> tpSystemInfo::getDiskInfo()
{
	tpSystemInfoData *sysData = static_cast<tpSystemInfoData *>(data_);
	
	sysData->disk_m=new tpDiskManage();
	return sysData->disk_m->getList();
}

// 网卡信息不通过系统接口获取

// 内存信息
tpMemory tpSystemInfo::getMemoryInfo()
{
	tpMemory info(false);
	return info.getMemoryInfo();
}

double tpSystemInfo::getMemoryUsage()
{
	tpMemory info(true);
	return info.getUsage();
}

tpList<tpNetworkInfo *> tpSystemInfo::getNetworkInfo()
{
	tpSystemInfoData *sysData = static_cast<tpSystemInfoData *>(data_);
	sysData->net_m=new tpNetworkManage();
	return sysData->net_m->getList();
}
