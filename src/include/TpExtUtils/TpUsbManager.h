#ifndef __TP_USB_MANAGER_H
#define __TP_USB_MANAGER_H

#include "TpUtils.h"
#include "TpSignalSlot.h"
#include "TpUsbDeviceInfo.h"

TP_DEF_VOID_TYPE_VAR(ItpUsbManagerData);

class TpUsbManager
{
public:
	enum UsbManagerFilter{
		TP_USB_FILTER_VENDOR	=0X01,
		TP_USB_FILTER_PRODUCT	=0X02
	};
public:
	TpUsbManager();
	~TpUsbManager();
public:
	/// @brief 获取设备列表
	/// @return 
	TpList<TpUsbDeviceInfo> getDevices();
	/// @brief 获取按照厂商ID和设备ID筛选后的ID
	/// @param id 
	/// @param id 
	/// @param filter
	/// @return 
	TpList<TpUsbDeviceInfo> getDevices(tpUInt16 vid, tpUInt16 pid, TpUsbManager::UsbManagerFilter filer);
	/// @brief 开始监测设备变化
	/// @return 
	int startMonitor();
public signals:
	declare_signal(usbDeviceAdd, TpUsbDeviceInfo *);
	declare_signal(usbDeviceRemove, TpUsbDeviceInfo *);
	declare_signal(usbDeviceChange, TpUsbDeviceInfo *);
private:
	ItpUsbManagerData *data_;
};



#endif