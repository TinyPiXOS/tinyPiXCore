#ifndef __TP_APP_DATA_DISK_H
#define __TP_APP_DATA_DISK_H

#include <TpCore.h>
#include "TpString.h"

class TpAppDataDisk
{
public:
	TpAppDataDisk();
	~TpAppDataDisk();
	
public:
	long int getAppDiskSpace(const TpString &uuid);
	long int getAppDataDiskSpace(const TpString &uuid);
	long int getAllAppDiskSpace();
};

#endif