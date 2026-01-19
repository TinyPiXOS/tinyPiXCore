#ifndef __TP_DISK_INFO_MANAGE_H
#define __TP_DISK_INFO_MANAGE_H

#include "TpDisk.h"
#include "TpSignalSlot.h"

TP_DEF_VOID_TYPE_VAR(ITpDiskManageData);

class TpDiskManage
{
public:
    /// @brief 构造一个磁盘管理实例
    /// @param enabled 是否启用磁盘监测，如果不启动可以获取当前磁盘列表但后续不会发送信号
    /// @param samp
    TpDiskManage(tpBool enabled = TP_FALSE, tpUInt16 samp = 1000);
    ~TpDiskManage();

public:
    /// @brief 获取磁盘列表
    /// @return 返回tpDisk列表
    TpList<TpDisk *> getList(); //
                                // TpList<TpDisk *> getDeviceListInfo();
    /// @brief 根据磁盘名称获取指定磁盘
    /// @return 返回tpDisk
    /// TpDisk *getDisk(TpString &device);

public
signals:
    declare_signal(diskRemove, TpString);
    declare_signal(diskAdd, TpDisk *);

private:
    TpDisk *getDisk(const TpString &name);
    void monitorList(tpBool once, uint16_t samp);

private:
    ITpDiskManageData *data_;
};

#endif
