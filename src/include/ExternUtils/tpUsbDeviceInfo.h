#ifndef __TP_USB_DEVICE_INFO_H
#define __TP_USB_DEVICE_INFO_H

#include "tpUtils.h"

TP_DEF_VOID_TYPE_VAR(ItpUsbDeviceInfoData);

class tpUsbDeviceInfo{
public:
	enum UsbDeviceClass{
		TP_USB_CLASS_PER_INTERFACE = 0,
		/// @brief Audio class
		TP_USB_CLASS_AUDIO = 1,
		/// @brief  Communications class
		TP_USB_CLASS_COMM = 2,
		/// @brief Human Interface Device class
		TP_USB_CLASS_HID = 3,
		/// @brief Physical
		TP_USB_CLASS_PHYSICAL = 5,
		/// @brief Printer class
		TP_USB_CLASS_PRINTER = 7,
		/// @brief Image class
		TP_USB_CLASS_PTP = 6, // legacy name from libusb-0.1 usb.h 
		TP_USB_CLASS_IMAGE = 6,
		/// @brief Mass storage class
		TP_USB_CLASS_MASS_STORAGE = 8,
		/// @brief Hub class 
		TP_USB_CLASS_HUB = 9,
		/// @brief Data class 
		TP_USB_CLASS_DATA = 10,
		/// @brief Smart Card 
		TP_USB_CLASS_SMART_CARD = 0x0b,
		/// @brief Content Security 
		TP_USB_CLASS_CONTENT_SECURITY = 0x0d,
		/// @brief Video 
		TP_USB_CLASS_VIDEO = 0x0e,
		/// @brief Personal Healthcare 
		TP_USB_CLASS_PERSONAL_HEALTHCARE = 0x0f,
		/// @brief Diagnostic Device 
		TP_USB_CLASS_DIAGNOSTIC_DEVICE = 0xdc,
		/// @brief Wireless class
		TP_USB_CLASS_WIRELESS = 0xe0,
		/// @brief Application class 
		TP_USB_CLASS_APPLICATION = 0xfe,
		/// @brief Class is vendor-specific 
		TP_USB_CLASS_VENDOR_SPEC = 0xff
	};
public:
	tpUsbDeviceInfo(void *info);
	~tpUsbDeviceInfo();
public:
	/// @brief 获取设备类型
	/// @return 
	tpUsbDeviceInfo::UsbDeviceClass getClass();
	/// @brief 获取设备子类型
	/// @return 
	tpUInt8 getSubClass();
	/// @brief 获取厂商ID
	/// @return 
	tpUInt16 getVendorID();
	/// @brief 获取设备ID
	/// @return 
	tpUInt16 getProductID();
	/// @brief 获取绑定端口，可以用于区分VID和PID相同的设备
	/// @return 
	tpUInt8 getBindPort();
private:
	ItpUsbDeviceInfoData *data_;
};



#endif
	