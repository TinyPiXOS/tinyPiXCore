#ifndef _DEV_MANAGER_H_
#define _DEV_MANAGER_H_

#ifdef	__cplusplus
extern "C" {
#endif

#include <libudev.h>
#include "dev_monitor.h"

typedef struct DeviceManager_{
	struct udev *udev;
	DeviceMonitor *monitor;
}DeviceManager;


DeviceManager *device_manage_create();
int device_manage_delete(DeviceManager *dev);
int device_manager_add_monitor(DeviceManager *dev);
int device_manage_delete_monitor(DeviceManager *dev);

#ifdef	__cplusplus
}
#endif

#endif