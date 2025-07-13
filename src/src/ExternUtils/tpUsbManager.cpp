/*///------------------------------------------------------------------------------------------------------------------------//
		USB设备管理
说 明 :	
日 期 : 

/*///------------------------------------------------------------------------------------------------------------------------//

#include <iostream>
#include <string.h>
#include "usb_manager.h"
#include "dev_manager.h"
#include "dev_monitor.h"
#include "tpUsbDeviceInfo.h"
#include "tpUsbManager.h"



struct tpUsbManagerData{
	UsbManager *usb_m;
	DeviceManager *dev_m;
	tpUsbManagerData()
	{
		usb_m=NULL;
		dev_m=NULL;
	}
};

//获取设备列表的回调
static void getUsbDeviceListCallback(UsbDeviceInfo *device, void *user_data)
{
	tpList<tpUsbDeviceInfo> *list= static_cast<tpList<tpUsbDeviceInfo>*>(user_data);

	list->emplace_back(device);
}

static void UsbDeviceMonitorCallback(DeviceMonitorAction *action, void *device_data,void *user_data)
{
	UsbDeviceInfo *usb_info=(UsbDeviceInfo *)device_data;

	printf("设备发生改变,%02x,%02x---%02x,\n",usb_info->vid,usb_info->pid,usb_info->deviceClass);

	if(strcmp(action->action,"add")==0)
	{

		//tpUsbDeviceInfo tpusb(usb_info);
		//usbDeviceAdd.emit(&tpusb);
	}
	else if(strcmp(action->action,"remove")==0)
	{

	}
	else if(strcmp(action->action,"change")==0)
	{

	}
	else if(strcmp(action->action,"bind")==0)
	{

	}
	else if(strcmp(action->action,"unbind")==0)
	{

	}

//	usbDeviceAdd.emit();

	/*declare_signal(usbDeviceAdd, tpUsbDeviceInfo *);
	declare_signal(usbDeviceRemove, tpUsbDeviceInfo *);
	declare_signal(usbDeviceChange, tpUsbDeviceInfo *);*/
}

tpUsbManager::tpUsbManager()
{
	data_ = new tpUsbManagerData();
	tpUsbManagerData* data = static_cast<tpUsbManagerData*>(data_);

	data->usb_m=usb_manager_create();
	if(!data->usb_m)
	{
		fprintf(stderr,"usb init error\n");
		return ;
	}
}

tpUsbManager::~tpUsbManager()
{
	tpUsbManagerData* data = static_cast<tpUsbManagerData*>(data_);
	if(!data)
		return ;
	usb_manager_delete(data->usb_m);
	device_manage_delete(data->dev_m);
	delete(data);
}


tpList<tpUsbDeviceInfo> tpUsbManager::getDevices()
{
	tpUsbManagerData* data = static_cast<tpUsbManagerData*>(data_);
	tpList<tpUsbDeviceInfo> list;
	if(usb_manager_get_devices(data->usb_m,getUsbDeviceListCallback,&list)<0)
		fprintf(stderr,"get device list error\n");
	return list;
}

tpList<tpUsbDeviceInfo> tpUsbManager::getDevices(tpUInt16 vid, tpUInt16 pid, tpUsbManager::UsbManagerFilter filer)
{
	tpUsbManagerData* data = static_cast<tpUsbManagerData*>(data_);
	tpList<tpUsbDeviceInfo> list;

	return list;
}

int tpUsbManager::startMonitor()
{
	tpUsbManagerData* data = static_cast<tpUsbManagerData*>(data_);
	
	data->dev_m=device_manage_create();
	if(!data->dev_m)
	{
		fprintf(stderr,"creat device manager error\n");
		return -1;
	}
	if(device_manager_add_monitor(data->dev_m)<0)
	{
		fprintf(stderr,"add monitor error\n");
		return -1;
	}

	device_manager_add_filter(data->dev_m->monitor, "usb");

	if(device_manager_start_monitor(data->dev_m->monitor,UsbDeviceMonitorCallback,NULL)<0)
	{
		fprintf(stderr,"monitor thread start error\n");
		return -1;
	}

	return 0;
}



