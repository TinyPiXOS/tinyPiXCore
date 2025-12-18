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
    int64_t appDiskSpace(const TpString &uuid);
    int64_t appDataDiskSpace(const TpString &uuid);
    int64_t allAppDiskSpace();
};

#endif