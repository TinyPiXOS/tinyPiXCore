#ifndef __TP_GPU_INFO_H
#define __TP_GPU_INFO_H

#include <TpCore.h>
#include "TpString.h"
TP_DEF_VOID_TYPE_VAR(ITpGpuInfoData);

class TpGpu
{
public:
	TpGpu();
	~TpGpu();

public:
	TpString getName();
private:
	ITpGpuInfoData *data_;
};

#endif