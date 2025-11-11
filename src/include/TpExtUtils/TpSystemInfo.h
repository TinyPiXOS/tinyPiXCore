#ifndef __TP_SYSTEM_INFO_H
#define __TP_SYSTEM_INFO_H

#include <TpCore.h>
#include "TpString.h"
#include "TpDiskManage.h"
#include "TpCpu.h"
#include "TpGpu.h"
#include "TpMemory.h"
#include "TpDisk.h"
#include "TpNetworkManage.h"
#include "TpNetworkInfo.h"

TP_DEF_VOID_TYPE_VAR(ItpSystemInfoData);
class TpSystemInfo
{

public:
	TpSystemInfo();
	~TpSystemInfo();

public:
	/// @brief 获取机器名称
	/// @return 返回机器名称
	TpString getMachineName();

	/// @brief 获取机器名称
	/// @return 返回机器名称
	int setMachineName(TpString &name);

	/// @brief 获取系统名称
	/// @return 返回系统名称
	TpString getSystemName();

	/// @brief 获取系统版本
	/// @return 返回系统版本
	TpString getSystemVersion();

	/// @brief 获取主板厂商
	/// @return 返回主板厂商
	TpString getBoardVendor();

	/// @brief 获取主板名字
	/// @return 返回主板名字
	TpString getBoardName(); 

	/// @brief 获取主板版本
	/// @return 返回主板版本
	TpString getBoardVersion();


	/// @brief 获取主板序列号
	/// @return 返回主板序列号
	TpString getBoardSerial();

	/// @brief 获取BIOS日期
	/// @return 返回BIOS日期
	TpString getBiosData();

	/// @brief 获取BIOS厂商
	/// @return 返回BIOS厂商
	TpString getBiosVendor();

	/// @brief 获取BIOS版本
	/// @return 返回BIOS版本
	TpString getBiosVersion();

	/// @brief 获取产品名称
	/// @return 返回产品名称
	TpString getProductName();

	/// @brief 获取产品
	/// @return 返回产品
	TpString getProductFamily();

	/// @brief 获取产品序列号
	/// @return 返回产品序列号
	TpString getProductSerial(); 

	/// @brief 获取产品库存单位
	/// @return 返回产品库存单位
	TpString getProductSku();

	/// @brief 获取产品uuid
	/// @return 返回产品uuid
	TpString getProductUuid();

	/// @brief 获取产品版本号
	/// @return 返回产品版本号
	TpString getProductVersion();

	/// @brief 获取单个cpu每个核心信息列表（cpu名称，主频，(）
	/// @param name cpu名称(当前仅支持单物理cpu，此参数不生效)
	/// @return 返回列表
	TpList<TpCpuCore*> getCpuCoreInfo(const TpString &name="Default");

	/// @brief cpu使用率(会返回每个核心的状态，如需要全部的则只取第一项)
	/// @param name cpu名称(当前仅支持单物理cpu，此参数不生效)
	/// @return 返回列表
	TpList<TpCpuCore*> getCpuCoreState(const TpString &name="Default");

	/// @brief 获取cpu信息列表(由于当前仅支持单物理CPU，所以只有一项)
	/// @return 返回列表
	TpList<TpCpu*> getCpuInfo();

	/// @brief gpu(还未实现)
	/// @return 
	TpList<TpGpu*> getGpuInfo();


	/// @brief 获取磁盘信息列表
	/// @return 返回列表
	TpList<TpDisk*> getDiskInfo();	

	/// @brief 获取内存信息(不考虑物理上多内存颗粒的情况)
	/// @return 返回内存信息
	TpMemory getMemoryInfo();

	/// @brief 获取内存使用率
	/// @return 返回内存使用率
	double getMemoryUsage();
	
	/// @brief 获取网卡信息列表
	/// @return 返回列表
	TpList<TpNetworkInfo*> getNetworkInfo();

private:
	TpString getValueFromeFile(const char *file);
	int setValueFromeFile(const char *file, const char *value);
	TpString getSystemValue(const char *item);
	uint64_t getDiskSpace(TpString diskName);

private:
	ItpSystemInfoData *data_;
};

#endif