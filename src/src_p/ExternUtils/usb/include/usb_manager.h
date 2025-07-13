#ifndef _USB_MANAGER_H_
#define _USB_MANAGER_H_

#ifdef	__cplusplus
extern "C" {
#endif

#include "libusb/libusb.h"


typedef struct UsbManager_{
	libusb_context *ctx;
}UsbManager;

typedef struct UsbDeviceInfo_{
	uint16_t vid;			//vid
	uint16_t pid;			//pid
	uint8_t bindport;		//usb设备和主机绑定的接口,可以用来区分VID和PID相同的设备
	uint8_t key;
	libusb_device *dev;
	uint8_t  deviceClass;		//设备类型
	uint8_t  deviceSubClass;	//设备类型
	uint16_t deviceUSBVersion;	//usb版本
	uint8_t  deviceProtocol;
	int sockport;	//sock通信的port端口号
	int processId;		//设备进程ID
	/*根据需要增删*/
}UsbDeviceInfo;


extern UsbManager *usb;

typedef void (*UsbDeviceListCallback)(UsbDeviceInfo *device, void *user_data);


UsbManager *usb_manager_create();
int usb_manager_delete(UsbManager *usb);
int usb_manager_get_devices(UsbManager *usb, UsbDeviceListCallback callback, void *user_data);
int usb_manager_get_devices_bus_dev(UsbManager *usb, uint8_t busnum, uint8_t devaddr, UsbDeviceListCallback callback, void *user_data);

#ifdef	__cplusplus
}
#endif

#endif