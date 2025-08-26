#ifndef __TP_GPU_INFO_H
#define __TP_GPU_INFO_H

#include "TpUtils.h"
#include "TpString.h"
TP_DEF_VOID_TYPE_VAR(ItpGpuInfoData);

class TpGpu
{
public:
	TpGpu();
	~TpGpu();

public:
	TpString getName();
private:
	ItpGpuInfoData *data_;
};

#endif