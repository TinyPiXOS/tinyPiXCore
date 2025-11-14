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
#include "TpUsbDeviceInfo.h"
#include "TpUsbManager.h"



struct TpUsbManagerData{
	UsbManager *usb_m;
	DeviceManager *dev_m;
	TpUsbManagerData()
	{
		usb_m=NULL;
		dev_m=NULL;
	}
};

//获取设备列表的回调
static void getUsbDeviceListCallback(UsbDeviceInfo *device, void *user_data)
{
	TpList<TpUsbDeviceInfo> *list= static_cast<TpList<TpUsbDeviceInfo>*>(user_data);

	list->emplace_back(device);
}

static void UsbDeviceMonitorCallback(DeviceMonitorAction *action, void *device_data,void *user_data)
{
	UsbDeviceInfo *usb_info=(UsbDeviceInfo *)device_data;

	printf("设备发生改变,%02x,%02x---%02x,\n",usb_info->vid,usb_info->pid,usb_info->deviceClass);

	if(strcmp(action->action,"add")==0)
	{

		//TpUsbDeviceInfo TpUsb(usb_info);
		//usbDeviceAdd.emit(&TpUsb);
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

	/*declare_signal(usbDeviceAdd, TpUsbDeviceInfo *);
	declare_signal(usbDeviceRemove, TpUsbDeviceInfo *);
	declare_signal(usbDeviceChange, TpUsbDeviceInfo *);*/
}

TpUsbManager::TpUsbManager()
{
	data_ = new TpUsbManagerData();
	TpUsbManagerData* data = static_cast<TpUsbManagerData*>(data_);

	data->usb_m=usb_manager_create();
	if(!data->usb_m)
	{
		fprintf(stderr,"usb init error\n");
		return ;
	}
}

TpUsbManager::~TpUsbManager()
{
	TpUsbManagerData* data = static_cast<TpUsbManagerData*>(data_);
	if(!data)
		return ;
	usb_manager_delete(data->usb_m);
	device_manage_delete(data->dev_m);
	delete(data);
}


TpList<TpUsbDeviceInfo> TpUsbManager::getDevices()
{
	TpUsbManagerData* data = static_cast<TpUsbManagerData*>(data_);
	TpList<TpUsbDeviceInfo> list;
	if(usb_manager_get_devices(data->usb_m,getUsbDeviceListCallback,&list)<0)
		fprintf(stderr,"get device list error\n");
	return list;
}

TpList<TpUsbDeviceInfo> TpUsbManager::getDevices(tpUInt16 vid, tpUInt16 pid, TpUsbManager::UsbManagerFilter filer)
{
	TpUsbManagerData* data = static_cast<TpUsbManagerData*>(data_);
	TpList<TpUsbDeviceInfo> list;

	return list;
}

int TpUsbManager::startMonitor()
{
	TpUsbManagerData* data = static_cast<TpUsbManagerData*>(data_);
	
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



