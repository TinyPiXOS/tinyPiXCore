#ifndef __TP_DISK_INFO_H
#define __TP_DISK_INFO_H

#include "tpString.h"
#include "tpUtils.h"

TP_DEF_VOID_TYPE_VAR(ItpDiskInfoData);

class tpDisk
{
public:
    tpDisk();
    tpDisk(const tpString &name, tpBool enabled = TP_FALSE, uint16_t samp = 1000);
	tpDisk(const tpDisk& other);			//移动构造函数
    ~tpDisk();

public:
	/// @brief 更新信息(当使能自动更新的时候不需要使用)
    void update();
	/// @brief 获取设备名(不包含编号的名字，例如name=sda1,device=sda)
	tpString getDevice();
	/// @brief 获取盘符编号名(即传入的name，例如sda1)
	tpString getName();
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
    tpString getVendor();
	/// @brief 获取磁盘型号
    tpString getModel();
	/// @brief 获取磁盘序列号
    tpString getSerial();
	/// @brief 获取磁盘类型(SSD,HDD等)
    tpString getType();
	/// @brief 获取磁盘文件系统类型
    tpString getFstype();
	/// @brief 获取磁盘挂载路径
    tpString getMount();
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
    tpString getLabel();
	/// @brief 设置磁盘盘符名(需要借助开源库，暂未实现)
	/// @param label 要设置的盘符名称
    int setLabel(tpString &label);
	/// @brief 挂载可移动磁盘
	/// @param path 要挂载的路径
    int mountRabDisk(const char *path);
    int mountRabDisk(tpString &path); 
	/// @brief 卸载可移动磁盘
    int umountRabDisk();                    
	/// @brief 弹出磁盘(需要注意会弹出整个磁盘，不仅仅是当前分区)
    int popupRabDisk();                     

private:
	friend class tpDiskManage;
	int getDiskInfo();
	int tpDiskInit(const tpString &name, bool enabled=false, uint16_t samp=1000);
	int setName(tpString &name);
	int setMount(tpString &path);
	int setFsType(tpString &type);
    tpString updateDeviceName();
    int getMountFstype();
	tpBool isPartition();
    void threadUpdateStat(uint16_t time_samp, uint64_t sector_size);
    void updateInfo(uint16_t num, void *stat);
    void updateThreadStop();
    int umountRabDisk(const char *name); //
	int autoMountRabDisk(tpString path,tpUInt16 timeout);	//自动挂载(检测到新设备后期台启动线程持续进行挂载)
	int autoMountRabDiskThread(tpString path,tpUInt16 timeout);
private:
    ItpDiskInfoData *data_;
};

#endif