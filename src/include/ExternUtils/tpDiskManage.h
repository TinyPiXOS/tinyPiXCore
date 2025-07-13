#ifndef __TP_DISK_INFO_MANAGE_H
#define __TP_DISK_INFO_MANAGE_H

#include "tpProcessInfo.h"
#include "tpThreadManage.h"
#include "tpDisk.h"
#include "tpSignalSlot.h"

TP_DEF_VOID_TYPE_VAR(ItpDiskManageData);

class tpDiskManage
{
public:

	/// @brief 构造一个磁盘管理实例
	/// @param enabled 是否启用磁盘监测，如果不启动可以获取当前磁盘列表但后续不会发送信号
	/// @param samp 
	tpDiskManage(tpBool enabled = TP_FALSE, tpUInt16 samp = 1000);
	~tpDiskManage();

public:
	/// @brief 获取磁盘列表
	/// @return 返回tpDisk列表
	tpList<tpDisk *> getList();		//
	//tpList<tpDisk *> getDeviceListInfo();
	/// @brief 根据磁盘名称获取指定磁盘
	/// @return 返回tpDisk
	/// tpDisk *getDisk(tpString &device);

public
signals:
    declare_signal(diskRemove, tpString);
	declare_signal(diskAdd, tpDisk *);

private:
	tpDisk *getDisk(const tpString &name);
	void monitorList(tpBool once,uint16_t samp);

private:
    ItpDiskManageData *data_;
};

#endif
