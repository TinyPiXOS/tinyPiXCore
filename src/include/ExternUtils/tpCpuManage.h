#ifndef __TP_CPU_MANAGE_H
#define __TP_CPU_MANAGE_H

#include "tpCpu.h"
TP_DEF_VOID_TYPE_VAR(ItpCpuManageData);

class tpCpuManage
{
public:
	tpCpuManage(tpBool enabled = TP_FALSE, tpUInt16 samp = 1000);
	~tpCpuManage();

public:
	/// @brief 获取CPU列表(不支持)
	/// @return 
	tpList<tpCpu*> getList();
	/// @brief 获取默认CPU
	/// @return 
	tpCpu* getCpu();

private:
	tpList<tpCpu*> readList();

private:
	ItpCpuManageData *data_;
};










#endif