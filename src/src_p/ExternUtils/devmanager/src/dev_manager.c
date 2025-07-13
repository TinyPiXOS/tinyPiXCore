/*///------------------------------------------------------------------------------------------------------------------------//
		设备管理程序
说 明 : 
日 期 : 2025.5.29

/*///------------------------------------------------------------------------------------------------------------------------//


#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <libudev.h>
#include "utils/utlist.h"
#include "dev_monitor.h"
#include "dev_manager.h"



DeviceManager *device_manage_create()
{
	DeviceManager *dev=(DeviceManager *)malloc(sizeof(DeviceManager));
	if(!dev)
		return NULL;
	dev->monitor=NULL;
	dev->udev = udev_new();
    if (!dev->udev) {
        fprintf(stderr, "cannot create udev\n");
		free(dev);
        return NULL;
    }

	return dev;
}

int device_manage_delete(DeviceManager *dev)
{
	if(!dev)
		return 0;
	if(dev->monitor)
	{
		device_manage_delete_monitor(dev);
	}

	udev_unref(dev->udev);
	free(dev);
}





//添加监视器
int device_manager_add_monitor(DeviceManager *dev)
{
	if(!dev)
		return -1;
	DeviceMonitor *monitor=(DeviceMonitor *)malloc(sizeof(DeviceMonitor));
	if(!monitor)
		return -1;
	
	monitor->monitor = udev_monitor_new_from_netlink(dev->udev, "udev");		//创建监视器
    if (!monitor->monitor) {
        fprintf(stderr, "Cannot create udev monitor\n");
		free(monitor);
        return -1;
    }
	dev->monitor=monitor;
	return 0;
}

//删除监视器
int device_manage_delete_monitor(DeviceManager *dev)
{
	if(!dev)
		return -1;
	device_manager_stop_monitor(dev->monitor);
	free(dev->monitor);
}