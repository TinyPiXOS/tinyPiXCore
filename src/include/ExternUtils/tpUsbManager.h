#ifndef __TP_USB_MANAGER_H
#define __TP_USB_MANAGER_H

#include "tpUtils.h"
#include "tpSignalSlot.h"
#include "tpUsbDeviceInfo.h"

TP_DEF_VOID_TYPE_VAR(ItpUsbManagerData);

class tpUsbManager
{
public:
	enum UsbManagerFilter{
		TP_USB_FILTER_VENDOR	=0X01,
		TP_USB_FILTER_PRODUCT	=0X02
	};
public:
	tpUsbManager();
	~tpUsbManager();
public:
	/// @brief 获取设备列表
	/// @return 
	tpList<tpUsbDeviceInfo> getDevices();
	/// @brief 获取按照厂商ID和设备ID筛选后的ID
	/// @param id 
	/// @param id 
	/// @param filter
	/// @return 
	tpList<tpUsbDeviceInfo> getDevices(tpUInt16 vid, tpUInt16 pid, tpUsbManager::UsbManagerFilter filer);
	/// @brief 开始监测设备变化
	/// @return 
	int startMonitor();
public signals:
	declare_signal(usbDeviceAdd, tpUsbDeviceInfo *);
	declare_signal(usbDeviceRemove, tpUsbDeviceInfo *);
	declare_signal(usbDeviceChange, tpUsbDeviceInfo *);
private:
	ItpUsbManagerData *data_;
};



#endif