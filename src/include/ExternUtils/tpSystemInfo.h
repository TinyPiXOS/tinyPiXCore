#ifndef __TP_SYSTEM_INFO_H
#define __TP_SYSTEM_INFO_H

#include "tpUtils.h"
#include "tpString.h"
#include "tpDiskManage.h"
#include "tpCpu.h"
#include "tpGpu.h"
#include "tpMemory.h"
#include "tpDisk.h"
#include "tpNetworkManage.h"
#include "tpNetworkInfo.h"

TP_DEF_VOID_TYPE_VAR(ItpSystemInfoData);
class tpSystemInfo
{

public:
	tpSystemInfo();
	~tpSystemInfo();

public:
	/// @brief 获取机器名称
	/// @return 返回机器名称
	tpString getMachineName();

	/// @brief 获取机器名称
	/// @return 返回机器名称
	int setMachineName(tpString &name);

	/// @brief 获取系统名称
	/// @return 返回系统名称
	tpString getSystemName();

	/// @brief 获取系统版本
	/// @return 返回系统版本
	tpString getSystemVersion();

	/// @brief 获取主板厂商
	/// @return 返回主板厂商
	tpString getBoardVendor();

	/// @brief 获取主板名字
	/// @return 返回主板名字
	tpString getBoardName(); 

	/// @brief 获取主板版本
	/// @return 返回主板版本
	tpString getBoardVersion();


	/// @brief 获取主板序列号
	/// @return 返回主板序列号
	tpString getBoardSerial();

	/// @brief 获取BIOS日期
	/// @return 返回BIOS日期
	tpString getBiosData();

	/// @brief 获取BIOS厂商
	/// @return 返回BIOS厂商
	tpString getBiosVendor();

	/// @brief 获取BIOS版本
	/// @return 返回BIOS版本
	tpString getBiosVersion();

	/// @brief 获取产品名称
	/// @return 返回产品名称
	tpString getProductName();

	/// @brief 获取产品
	/// @return 返回产品
	tpString getProductFamily();

	/// @brief 获取产品序列号
	/// @return 返回产品序列号
	tpString getProductSerial(); 

	/// @brief 获取产品库存单位
	/// @return 返回产品库存单位
	tpString getProductSku();

	/// @brief 获取产品uuid
	/// @return 返回产品uuid
	tpString getProductUuid();

	/// @brief 获取产品版本号
	/// @return 返回产品版本号
	tpString getProductVersion();

	/// @brief 获取单个cpu每个核心信息列表（cpu名称，主频，(）
	/// @param name cpu名称(当前仅支持单物理cpu，此参数不生效)
	/// @return 返回列表
	tpList<tpCpuCore*> getCpuCoreInfo(const tpString &name="Default");

	/// @brief cpu使用率(会返回每个核心的状态，如需要全部的则只取第一项)
	/// @param name cpu名称(当前仅支持单物理cpu，此参数不生效)
	/// @return 返回列表
	tpList<tpCpuCore*> getCpuCoreState(const tpString &name="Default");

	/// @brief 获取cpu信息列表(由于当前仅支持单物理CPU，所以只有一项)
	/// @return 返回列表
	tpList<tpCpu*> getCpuInfo();

	/// @brief gpu(还未实现)
	/// @return 
	tpList<tpGpu*> getGpuInfo();


	/// @brief 获取磁盘信息列表
	/// @return 返回列表
	tpList<tpDisk*> getDiskInfo();	

	/// @brief 获取内存信息(不考虑物理上多内存颗粒的情况)
	/// @return 返回内存信息
	tpMemory getMemoryInfo();

	/// @brief 获取内存使用率
	/// @return 返回内存使用率
	double getMemoryUsage();
	
	/// @brief 获取网卡信息列表
	/// @return 返回列表
	tpList<tpNetworkInfo*> getNetworkInfo();

private:
	tpString getValueFromeFile(const char *file);
	int setValueFromeFile(const char *file, const char *value);
	tpString getSystemValue(const char *item);
	uint64_t getDiskSpace(tpString diskName);

private:
	ItpSystemInfoData *data_;
};

#endif