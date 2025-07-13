#ifndef _DEV_MONITOR_H_
#define _DEV_MONITOR_H_

#ifdef	__cplusplus
extern "C" {
#endif

#include <stdint.h>
#include <pthread.h>
#include <libudev.h>
#include "utils/utthread.h"
#include "usb_manager.h"
/*
#define DEV_MONITOR_FILTER_USB			"usb"
#define DEV_MONITOR_FILTER_BLOCK		"block"
#define DEV_MONITOR_FILTER_SOUND		"sound"
#define DEV_MONITOR_FILTER_NET			"net"
#define DEV_MONITOR_FILTER_VIDEO		"video4linux"
#define DEV_MONITOR_FILTER_POWER		"power_supply"
#define DEV_MONITOR_FILTER_BATRARY		"power_supply"
#define DEV_MONITOR_FILTER_BLUETOOTH	"bluetooth"
#define DEV_MONITOR_FILTER_TTY			"tty"
#define DEV_MONITOR_FILTER_SERIAL		"usb-serial"
#define DEV_MONITOR_FILTER_INPUT		"input"
#define DEV_MONITOR_FILTER_HIDRAW		"hidraw"
#define DEV_MONITOR_FILTER_PRINTER		"printer"
*/

typedef enum{
	DEV_MONITOR_FILTER_USB		= 0x0001,
	DEV_MONITOR_FILTER_BLOCK	= 0x0002,
	DEV_MONITOR_FILTER_SOUND	= 0x0004,
	DEV_MONITOR_FILTER_NET		= 0x0008,
	DEV_MONITOR_FILTER_VIDEO	= 0x0010,
	DEV_MONITOR_FILTER_POWER	= 0x0020,
//	DEV_MONITOR_FILTER_BATRARY	= 0x0020,
	DEV_MONITOR_FILTER_BLUETOOTH= 0x0040,
	DEV_MONITOR_FILTER_TTY		= 0x0080,
	DEV_MONITOR_FILTER_SERIAL	= 0x0100,
	DEV_MONITOR_FILTER_INPUT	= 0x0200,
	DEV_MONITOR_FILTER_HIDRAW	= 0x0400,
	DEV_MONITOR_FILTER_PRINTER	= 0x0800,
}DeviceMonitorFilterType;

typedef enum{
	DEV_MONITOR_ACTION_ADD,		//新增设备
	DEV_MONITOR_ACTION_REMOVE,	//设备移除
	DEV_MONITOR_ACTION_CHANGE	//设备改变
}DeviceMonitorActionType;



typedef struct DeviceMonitor_{
	struct udev_monitor *monitor;
	void *device_handle;		//某个设备的句柄，例如libusb
	UtilsThread *thread;
	void *thread_data;
	int pipe_fd_w;
}DeviceMonitor;

typedef struct DeviceMonitorAction_{
	char *action;		// 动作：add/remove/change
	uint8_t busnum;		//设备所在的 USB 总线编号（1,2,3...）busnum和devaddr可以共同用于标志唯一的USB设备
	uint8_t devaddr;	//该设备在所属总线上的唯一地址（1,2,3...）
	char *devnode;		// 设备节点，如 /dev/ttyUSB0
	char *subsystem;	// 设备所属的子系统名称
	char *syspath;		// "sysfs 路径
	char *devtype;		// 设备的“类型”,比如 "partition"（分区）、"disk"（磁盘）
}DeviceMonitorAction;


//设备插拔要调用的回调函数
//action:udev监测到的发生变动的设备
//device_data：根据设备类型获取的详细设备数据
//user_data：用户回调数据
typedef void (*DeviceMonitorCallback)(DeviceMonitorAction *action, void *device_data,void *user_data);


int device_manager_start_monitor(DeviceMonitor *monitor,DeviceMonitorCallback callback,void *user_data);
int device_manager_stop_monitor(DeviceMonitor *monitor);
int device_manager_add_filter(DeviceMonitor *monitor, const char *filter);

#ifdef	__cplusplus
}
#endif

#endif