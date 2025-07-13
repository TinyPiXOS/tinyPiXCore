//USB设备测试
#include <iostream>
#include "tpUsbManager.h"
#include "tpUsbDeviceInfo.h"

int main()
{
	tpUsbManager usb_manager;
	tpList<tpUsbDeviceInfo> usb_list=usb_manager.getDevices();
	//获取设备列表
	for(auto &it : usb_list)
	{
		tpUsbDeviceInfo::UsbDeviceClass type = it.getClass();
		printf("USB Class:0x%02x\n",type);
		printf("USB SubClass:0x%02x\n",(uint8_t)(it.getSubClass()));
		printf("USB 厂商ID:0x%04x\n",(uint16_t)(it.getVendorID()));
		printf("USB 设备ID:0x%04x\n\n",(uint16_t)(it.getProductID()));
	}

	//设备监测
	usb_manager.startMonitor();
	sleep(10);

}