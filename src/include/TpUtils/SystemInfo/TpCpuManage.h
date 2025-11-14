#ifndef __TP_CPU_MANAGE_H
#define __TP_CPU_MANAGE_H

#include "TpCpu.h"
TP_DEF_VOID_TYPE_VAR(ItpCpuManageData);

class TpCpuManage
{
public:
	TpCpuManage(tpBool enabled = TP_FALSE, tpUInt16 samp = 1000);
	~TpCpuManage();

public:
	/// @brief 获取CPU列表(不支持)
	/// @return 
	TpList<TpCpu*> getList();
	/// @brief 获取默认CPU
	/// @return 
	TpCpu* getCpu();

private:
	TpList<TpCpu*> readList();

private:
	ItpCpuManageData *data_;
};










#endif