#ifndef __TP_DISK_INFO_H
#define __TP_DISK_INFO_H

#include "TpString.h"
#include <TpCore.h>

TP_DEF_VOID_TYPE_VAR(ITpDiskInfoData);

class TpDisk
{
public:
    TpDisk();
    TpDisk(const TpString &name, tpBool enabled = TP_FALSE, uint16_t samp = 1000);
    TpDisk(const TpDisk &other); // 移动构造函数
    ~TpDisk();

public:
    /// @brief 更新信息(当使能自动更新的时候不需要使用)
    void update();
    /// @brief 获取设备名(不包含编号的名字，例如name=sda1,device=sda)
    TpString getDevice();
    /// @brief 获取盘符编号名(即传入的name，例如sda1)
    TpString getName();
    /// @brief 获取磁盘的扇区大小
    /// @return 扇区大小，单位Byte
    uint64_t getSectorSize();
    /// @brief 获取磁盘的扇区数量
    /// @return 扇区数量
    uint64_t getSectorNum();
    /// @brief 获取磁盘空间大小
    /// @return 返回字节数，单位Byte
    uint64_t getSpace();
    /// @brief 获取磁盘分区号
    int16_t getPartition();
    /// @brief 获取磁盘是否是可移动磁盘
    tpBool getRemovable();
    /// @brief 获取磁盘是否只读
    tpBool getReadonly();
    /// @brief 获取磁盘厂商
    TpString getVendor();
    /// @brief 获取磁盘型号
    TpString getModel();
    /// @brief 获取磁盘序列号
    TpString getSerial();
    /// @brief 获取磁盘类型(SSD,HDD等)
    TpString getType();
    /// @brief 获取磁盘文件系统类型
    TpString getFstype();
    /// @brief 获取磁盘挂载路径
    TpString getMount();
    /// @brief 获取磁盘是否已挂载
    /// @return
    tpBool isMount();
    /// @brief 获取磁盘已用空间
    /// @return 已使用空间，Byte
    uint64_t getUsedSize();
    /// @brief 获取磁盘当前读取速度
    /// @return 读取速度，Byte/s
    double getReadSpeed();
    /// @brief 获取磁盘当前写入速度
    /// @return 写入速度，Byte/S
    double getWriteSpeed();
    /// @brief 获取磁盘盘符名(需要借助开源库，暂未实现)
    TpString getLabel();
    /// @brief 设置磁盘盘符名(需要借助开源库，暂未实现)
    /// @param label 要设置的盘符名称
    int setLabel(TpString &label);
    /// @brief 挂载可移动磁盘
    /// @param path 要挂载的路径
    int mountRabDisk(const char *path);
    int mountRabDisk(TpString &path);
    /// @brief 卸载可移动磁盘
    int umountRabDisk();
    /// @brief 弹出磁盘(需要注意会弹出整个磁盘，不仅仅是当前分区)
    int popupRabDisk();

private:
    friend class TpDiskManage;
    int getDiskInfo();
    int TpDiskInit(const TpString &name, bool enabled = false, uint16_t samp = 1000);
    int setName(TpString &name);
    int setMount(TpString &path);
    int setFsType(TpString &type);
    TpString updateDeviceName();
    int getMountFstype();
    tpBool isPartition();
    void threadUpdateStat(uint16_t time_samp, uint64_t sector_size);
    void updateInfo(uint16_t num, void *stat);
    void updateThreadStop();
    int umountRabDisk(const char *name);                   //
    int autoMountRabDisk(TpString path, tpUInt16 timeout); // 自动挂载(检测到新设备后期台启动线程持续进行挂载)
    int autoMountRabDiskThread(TpString path, tpUInt16 timeout);

private:
    ITpDiskInfoData *data_;
};

#endif