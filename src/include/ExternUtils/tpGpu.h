#ifndef __TP_GPU_INFO_H
#define __TP_GPU_INFO_H

#include "tpUtils.h"
#include "tpString.h"
TP_DEF_VOID_TYPE_VAR(ItpGpuInfoData);

class tpGpu
{
public:
	tpGpu();
	~tpGpu();

public:
	tpString getName();
private:
	ItpGpuInfoData *data_;
};

#endif