#ifndef __TP_APP_DATA_DISK_H
#define __TP_APP_DATA_DISK_H

#include "tpUtils.h"
#include "tpString.h"

class tpAppDataDisk
{
public:
	tpAppDataDisk();
	~tpAppDataDisk();
	
public:
	long int getAppDiskSpace(const tpString &uuid);
	long int getAppDataDiskSpace(const tpString &uuid);
	long int getAllAppDiskSpace();
};

#endif