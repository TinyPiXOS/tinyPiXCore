#ifndef __TP_SYSTEM_INFO_DEF
#define __TP_SYSTEM_INFO_DEF

#include "tpString.h"

/// proc/diskstats文件的信息
struct DiskstatsData
{
    struct timeval time_now;
    tpString name;
    unsigned long int reads;
    unsigned long int writes;
    unsigned long int read_time;
    unsigned long int write_time; // 时间，读写的时候手动获取
    unsigned long int read_sector;
    unsigned long int write_sector;
    double read_speed;
    double write_speed;
    unsigned long int weight_time;
    DiskstatsData() : reads(0), writes(0),
                      read_time(0), write_time(0),
                      read_sector(0), write_sector(0), weight_time(0),
                      read_speed(0.0), write_speed(0.0) {}
};

struct DiskstatsDataHandle
{
    struct DiskstatsData data;      // 当前数据
    struct DiskstatsData last_data; // 上次采集的数据
    double occupy;                  // 占用率(根据上次采集时间和本次采集时间计算)
    int count;                      // 使用本磁盘的文件的数量
    int dev_num;                    // 设备号
};

#endif