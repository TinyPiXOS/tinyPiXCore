/*///------------------------------------------------------------------------------------------------------------------------//
		USB设备管理
说 明 :	
日 期 : 

/*///------------------------------------------------------------------------------------------------------------------------//

#include <string.h>
#include "usb_manager.h"
#include "TpUsbDeviceInfo.h"



struct TpUsbDeviceInfoData{
	UsbDeviceInfo *info;
	TpUsbDeviceInfoData(UsbDeviceInfo *info_):info(info_)
	{
	}

	TpUsbDeviceInfoData()
	{
		info=NULL;
	}
};


TpUsbDeviceInfo::TpUsbDeviceInfo(void *info)
{
	data_ = new TpUsbDeviceInfoData();
	TpUsbDeviceInfoData* data = static_cast<TpUsbDeviceInfoData*>(data_);
	data->info=new UsbDeviceInfo();
	memcpy(data->info,info,sizeof(UsbDeviceInfo));
}

TpUsbDeviceInfo::~TpUsbDeviceInfo()
{
	TpUsbDeviceInfoData* data = static_cast<TpUsbDeviceInfoData*>(data_);
	if(!data->info)
		free(data->info);
	delete(data);
}

TpUsbDeviceInfo::UsbDeviceClass TpUsbDeviceInfo::getClass()
{
	TpUsbDeviceInfoData* data = static_cast<TpUsbDeviceInfoData*>(data_);
	TpUsbDeviceInfo::UsbDeviceClass type = static_cast<TpUsbDeviceInfo::UsbDeviceClass>(data->info->deviceClass);
	return type;
}

tpUInt8 TpUsbDeviceInfo::getSubClass()
{
	TpUsbDeviceInfoData* data = static_cast<TpUsbDeviceInfoData*>(data_);
	return data->info->deviceSubClass;
}

tpUInt16 TpUsbDeviceInfo::getVendorID()
{
	TpUsbDeviceInfoData* data = static_cast<TpUsbDeviceInfoData*>(data_);
	return data->info->vid;
}

tpUInt16 TpUsbDeviceInfo::getProductID()
{
	TpUsbDeviceInfoData* data = static_cast<TpUsbDeviceInfoData*>(data_);
	return data->info->pid;
}
tpUInt8 TpUsbDeviceInfo::getBindPort()
{
	TpUsbDeviceInfoData* data = static_cast<TpUsbDeviceInfoData*>(data_);
	return data->info->bindport;
}


