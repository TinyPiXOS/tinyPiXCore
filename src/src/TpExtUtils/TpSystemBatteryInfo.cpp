/*///------------------------------------------------------------------------------------------------------------------------//
		电池信息相关接口
说 明 : 
日 期 : 2025.5.15

/*///------------------------------------------------------------------------------------------------------------------------//


#include <iostream>
#include "TpSystemBatteryInfo.h"
#include "TpDbusConnectManage.h"
#include "TpDbus/properties.h"
#include "TpDbus/connect.h"
#include "TpDbus/freedesktop_upower.h"

#define UPOWER_SERVICE        "org.freedesktop.UPower"
#define UPOWER_DISPLAY_DEVICE "/org/freedesktop/UPower/devices/line_power_ACAD"//"/org/freedesktop/UPower/devices/DisplayDevice"
#define UPOWER_DEVICE_IFACE   "org.freedesktop.UPower.Device"

struct TpSystemBatteryInfoData{
	Properties *prop_online;
	Properties *prop_battery;
	DesktopUPower *power;
	TpSystemBatteryInfoData()
	{
		prop_online=NULL;
		prop_battery=NULL;
		power=NULL;
	}
};


static void callback_upower_list(const char *device_path, void *user_data)
{
	TpList<char *> *list = static_cast<TpList<char *> *>(user_data);
	char *path=strdup(device_path);
    list->emplace_back(path);
}


TpSystemBatteryInfo::TpSystemBatteryInfo()
{
	data_=new TpSystemBatteryInfoData();
	TpSystemBatteryInfoData *data=static_cast<TpSystemBatteryInfoData *>(data_);
	if(TpDbusConnectManage::instance().connection()!=TP_TRUE)
	{
		throw std::runtime_error("Error: Connect to dbus error");
		delete(data);
		return ;
	}

	if(getPath()<0)
	{
		throw std::runtime_error("Error: Get power devices error");
		delete(data);
		return ;
	}
}

TpSystemBatteryInfo::~TpSystemBatteryInfo()
{
	TpSystemBatteryInfoData *data=static_cast<TpSystemBatteryInfoData *>(data_);
	if(!data)
		return ;
	properties_free(data->prop_battery);
	properties_free(data->prop_online);
	desktop_upower_delete(data->power);
	delete(data);
	data = nullptr;
}

TpSystemBatteryInfo::SystemPowerState TpSystemBatteryInfo::getState()
{
	TpSystemBatteryInfoData *data=static_cast<TpSystemBatteryInfoData *>(data_);
	if(!data)
		return TpSystemBatteryInfo::TP_SYSTEM_POWER_UNKNOWN;
	if(!data->prop_battery)	//没有电池的情况下
	{
		if(!data->prop_online)
			return TpSystemBatteryInfo::TP_SYSTEM_POWER_UNKNOWN;
		return TpSystemBatteryInfo::TP_SYSTEM_POWER_CHARGE;
	}

	uint32_t value=properties_get_uint(data->prop_battery, UPOWER_DEVICE_IFACE, "State");	//1 = 正在充电，2 = 放电中，4 = 已充满
	switch(value)
	{
		case UPOWER_STATE_CHARGE:
			return TpSystemBatteryInfo::TP_SYSTEM_POWER_CHARGE;
		case UPOWER_STATE_DISCHARGE:
			return TpSystemBatteryInfo::TP_SYSTEM_POWER_DISCHARGE;
		case UPOWER_STATE_FULL:
			return TpSystemBatteryInfo::TP_SYSTEM_POWER_FULL;
		default:
			break;
	}
	return TpSystemBatteryInfo::TP_SYSTEM_POWER_UNKNOWN;
}

double TpSystemBatteryInfo::getVoltage()
{
	TpSystemBatteryInfoData *data=static_cast<TpSystemBatteryInfoData *>(data_);
	if(!data)
		return -1;
	if(data->prop_battery)
		return properties_get_double(data->prop_battery, UPOWER_DEVICE_IFACE, "Voltage");
	else
		return properties_get_double(data->prop_online, UPOWER_DEVICE_IFACE, "Voltage");
}

double TpSystemBatteryInfo::getCurrent()
{
	TpSystemBatteryInfoData *data=static_cast<TpSystemBatteryInfoData *>(data_);
	if(!data)
		return -1;
	return 0.0;
}

int TpSystemBatteryInfo::getBatteryLevel()
{
	TpSystemBatteryInfoData *data=static_cast<TpSystemBatteryInfoData *>(data_);
	if(!data || !data->prop_battery)
		return 0;
	double leval=properties_get_double(data->prop_battery,UPOWER_DEVICE_IFACE,"Percentage");
	return ((int)leval);
}

tpBool TpSystemBatteryInfo::isCharging()
{
	TpSystemBatteryInfoData *data=static_cast<TpSystemBatteryInfoData *>(data_);
	if(!data)
		return TP_FALSE;
	SystemPowerState state=getState();
	if(state==TpSystemBatteryInfo::TP_SYSTEM_POWER_CHARGE || state==TpSystemBatteryInfo::TP_SYSTEM_POWER_FULL)
		return TP_TRUE;
	else
		TP_FALSE;
	return TP_FALSE;
}

int TpSystemBatteryInfo::getType(void *prop)
{
	TpSystemBatteryInfoData *data=static_cast<TpSystemBatteryInfoData *>(data_);
	if(!data)
		return 0;
	Properties *properties=(Properties *)prop;
	return properties_get_uint(properties, UPOWER_DEVICE_IFACE, "Type");
}

int TpSystemBatteryInfo::getPath()
{
	TpSystemBatteryInfoData *data=static_cast<TpSystemBatteryInfoData *>(data_);
	if(!data)
		return 0;
	data->power=desktop_upower_creat(system_conn);
	if(!data->power)
	{
		fprintf(stderr,"desktop upower create error\n");
		return -1;
	}

	TpList<char *> list;
	upower_get_path_list(data->power,callback_upower_list,&list);

	for(auto &it : list)
	{
		Properties *properties=properties_new(system_conn,"system",UPOWER_SERVICE,it);
		if(!properties)
		{
			fprintf(stderr,"properties create error\n");
			return -1;
		}

		uint32_t type=properties_get_uint(properties, UPOWER_DEVICE_IFACE, "Type");
		switch (type) {
			case 1:
				printf("这是一个交流电源（Line Power）设备。\n");
				data->prop_online=properties;
				break;
			case 2:
				printf("这是一个电池设备。\n");
				data->prop_battery=properties;
				break;
			default:
				printf("其他类型设备(暂不考虑)，Type = %u\n", type);
				properties_free(properties);
				break;
    	}

		char *path=it;
		free(path);
	}
	return 0;
}
