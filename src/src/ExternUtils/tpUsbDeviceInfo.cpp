/*///------------------------------------------------------------------------------------------------------------------------//
		USB设备管理
说 明 :	
日 期 : 

/*///------------------------------------------------------------------------------------------------------------------------//

#include <string.h>
#include "usb_manager.h"
#include "tpUsbDeviceInfo.h"



struct tpUsbDeviceInfoData{
	UsbDeviceInfo *info;
	tpUsbDeviceInfoData(UsbDeviceInfo *info_):info(info_)
	{
	}

	tpUsbDeviceInfoData()
	{
		info=NULL;
	}
};


tpUsbDeviceInfo::tpUsbDeviceInfo(void *info)
{
	data_ = new tpUsbDeviceInfoData();
	tpUsbDeviceInfoData* data = static_cast<tpUsbDeviceInfoData*>(data_);
	data->info=new UsbDeviceInfo();
	memcpy(data->info,info,sizeof(UsbDeviceInfo));
}

tpUsbDeviceInfo::~tpUsbDeviceInfo()
{
	tpUsbDeviceInfoData* data = static_cast<tpUsbDeviceInfoData*>(data_);
	if(!data->info)
		free(data->info);
	delete(data);
}

tpUsbDeviceInfo::UsbDeviceClass tpUsbDeviceInfo::getClass()
{
	tpUsbDeviceInfoData* data = static_cast<tpUsbDeviceInfoData*>(data_);
	tpUsbDeviceInfo::UsbDeviceClass type = static_cast<tpUsbDeviceInfo::UsbDeviceClass>(data->info->deviceClass);
	return type;
}

tpUInt8 tpUsbDeviceInfo::getSubClass()
{
	tpUsbDeviceInfoData* data = static_cast<tpUsbDeviceInfoData*>(data_);
	return data->info->deviceSubClass;
}

tpUInt16 tpUsbDeviceInfo::getVendorID()
{
	tpUsbDeviceInfoData* data = static_cast<tpUsbDeviceInfoData*>(data_);
	return data->info->vid;
}

tpUInt16 tpUsbDeviceInfo::getProductID()
{
	tpUsbDeviceInfoData* data = static_cast<tpUsbDeviceInfoData*>(data_);
	return data->info->pid;
}
tpUInt8 tpUsbDeviceInfo::getBindPort()
{
	tpUsbDeviceInfoData* data = static_cast<tpUsbDeviceInfoData*>(data_);
	return data->info->bindport;
}


