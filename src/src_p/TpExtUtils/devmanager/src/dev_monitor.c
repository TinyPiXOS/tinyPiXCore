/*///------------------------------------------------------------------------------------------------------------------------//
		设备管理监视器相关程序
说 明 : 
日 期 : 2025.5.29

/*///------------------------------------------------------------------------------------------------------------------------//

#include <stdio.h>
#include <poll.h>
#include <fcntl.h>
#include <stdlib.h>
#include <string.h>
#include "dev_monitor.h"
#include "dev_manager.h"

//线程参数传递使用，私有
struct ThreadDeviceMonitorData{
	DeviceMonitorCallback callback;
	void *userdata;
	struct udev_monitor *monitor;
	int pipe_fd_r;
};

static void *pthread_device_monitor(void *arg);


//filter:过滤器
int device_manager_add_filter(DeviceMonitor *monitor, const char *filter)
{		
	udev_monitor_filter_add_match_subsystem_devtype(monitor->monitor, filter, NULL);
}


//启动监测
int device_manager_start_monitor(DeviceMonitor *monitor,DeviceMonitorCallback callback,void *user_data)
{
	struct ThreadDeviceMonitorData *data=(struct ThreadDeviceMonitorData *)malloc(sizeof(struct ThreadDeviceMonitorData));
	data->monitor=monitor->monitor;
	data->callback=callback;
	data->userdata=user_data;

	int pipe_fd[2];
    if (pipe(pipe_fd) < 0) {
        fprintf(stderr,"pipe error\n");
		free(data);
        return -1;
    }
	data->pipe_fd_r=pipe_fd[0];

	monitor->pipe_fd_w=pipe_fd[1];
	monitor->thread_data=data;
	monitor->thread = utils_thread_create("MonitorThread", pthread_device_monitor, data);
	if(utils_thread_start(monitor->thread)!=true)
	{
		close(pipe_fd[0]);
    	close(pipe_fd[1]);
		free(data);
		return -1;
	}
	return 0;
}

//停止监测
int device_manager_stop_monitor(DeviceMonitor *monitor)
{
	utils_thread_stop(monitor->thread);	//停止线程
	if(write(monitor->pipe_fd_w, "quit", 5)<0) // 触发退出
	{
		printf("退出失败\n");
	}
    utils_thread_join(monitor->thread);	//等待线程结束

	udev_monitor_unref(monitor->monitor);

	utils_thread_destroy(monitor->thread);

	close(monitor->pipe_fd_w);
	struct ThreadDeviceMonitorData *data=(struct ThreadDeviceMonitorData *)monitor->thread_data;
	free(data);
}

//磁盘类监视处理
static int device_monitor_block(struct udev_device *dev,void *usb_info)
{

}

//usb类监视处理，返回UsbDeviceInfo
static void device_monitor_usb_callback(UsbDeviceInfo *device, void *user_data)
{
	memcpy(user_data,device,sizeof(UsbDeviceInfo));
}

static int device_monitor_usb(struct udev_device *dev,void *usb_info)
{
	const char *vendor = udev_device_get_sysattr_value(dev, "idVendor");   // USB 厂商 ID
	const char *product = udev_device_get_sysattr_value(dev, "idProduct"); // USB 产品 ID
	const char *dev_class = udev_device_get_sysattr_value(dev, "bDeviceClass");	
	const char *usb_version = udev_device_get_sysattr_value(dev, "bcdUSB");
	const char *busnum = udev_device_get_sysattr_value(dev, "busnum");
    const char *devnum = udev_device_get_sysattr_value(dev, "devnum");

	printf("厂商 ID  : %s\n", vendor ? vendor : "(未知)");
	printf("产品 ID  : %s\n", product ? product : "(未知)");
	printf("设备bus：%s,devnum:%s\n",busnum,devnum);
	printf("USB版本：%s\n",usb_version);
	printf("usb类型：%s\n",dev_class);

	uint8_t bus_num=atoi(busnum);
	uint8_t dev_num=atoi(devnum);
	if(bus_num==0 || dev_num==0)
		return 0;
	
	UsbManager *usb=usb_manager_create();
	UsbDeviceInfo *device=(UsbDeviceInfo *)usb_info;
	usb_manager_get_devices_bus_dev(usb, bus_num, dev_num, device_monitor_usb_callback, device);
	usb_manager_delete(usb);
	
	return 0;
}

//设备监视线程
static void *pthread_device_monitor(void *arg)
{
	UtilsThread *self=(UtilsThread *)arg;
	struct ThreadDeviceMonitorData *data=(struct ThreadDeviceMonitorData *)self->task_arg;
	DeviceMonitorCallback callback=data->callback;
	void *userdata=data->userdata;
	struct udev_monitor *monitor=data->monitor;
	int pipe_fd_r=data->pipe_fd_r;

	udev_monitor_enable_receiving(monitor);		//启动监听
	int fd = udev_monitor_get_fd(monitor);		//获取监听的文件描述符

	// poll 等待设备事件，有事件时返回 > 0
	struct pollfd fds[2];
	fds[0].fd = fd;
	fds[0].events = POLLIN;
	fds[1].fd = pipe_fd_r;
	fds[1].events = POLLIN;

	while (!utils_thread_should_stop(self)) 
	{

		int ret = poll(fds, 2, -1);  // -1 表示一直等直到有事件
		if (ret > 0 && (fds[0].revents & POLLIN)) 
		{
			// 获取事件对应的设备信息
			struct udev_device *dev = udev_monitor_receive_device(monitor);
			if (dev) 
			{
				const char *devnode = udev_device_get_devnode(dev);       // 设备节点，如 /dev/ttyUSB0
				if(!devnode)
				{
					udev_device_unref(dev);
					continue;
				}
				const char *action = udev_device_get_action(dev);         // 动作：add/remove/change/bind/unbind

				const char *subsystem = udev_device_get_subsystem(dev);		//获取设备所属的子系统名称
				const char *syspath = udev_device_get_syspath(dev);
				const char *devtype = udev_device_get_devtype(dev);			//获取设备的“类型”，比如 "partition"（分区）、"disk"（磁盘）、"usb_device"、"usb_interface"。
				const char *vendor = udev_device_get_sysattr_value(dev, "idVendor");	//获取 sysfs 中的属性值
				const char *product = udev_device_get_sysattr_value(dev, "idProduct");
				const char *model = udev_device_get_property_value(dev, "ID_MODEL");	//获取 udev 自动生成的设备属性（来自规则匹配的）
				const char *name = udev_device_get_sysattr_value(dev, "product");
				// 打印事件信息
				printf("\n[事件] %s\n", action);
				printf("设备节点: %s\n", devnode ? devnode : "(无)");
				printf("子系统%s\n",subsystem);
				
				DeviceMonitorAction mon_action;
				mon_action.action=(char *)action;
				mon_action.devnode=(char *)devnode;
				mon_action.devtype=(char *)devtype;
				mon_action.subsystem=(char *)subsystem;
				mon_action.syspath=(char *)syspath;
				
				typedef union {
					UsbDeviceInfo device;
					//添加其他种类的设备信息的结构体
				} AnonymousUnion;
				AnonymousUnion device_data;
				memset(&device_data,0,sizeof(AnonymousUnion));
				if(strcmp(action,"add")==0)
				{
					if(strcmp(subsystem,"usb")==0)
						device_monitor_usb(dev,&device_data);
					else if(strcmp(subsystem,"block"))
						device_monitor_block(dev,&device_data);
				}
				if(callback)
					callback(&mon_action,&device_data,userdata);

				// 释放设备对象
				udev_device_unref(dev);
			}
		}

		if (ret > 0 && (fds[1].revents & POLLIN)) 
		{
			char buf[8];
			read(pipe_fd_r, buf, sizeof(buf));  // 清除数据
			//printf("收到退出指令，退出主循环。\n");
		}

	}

	close(pipe_fd_r);
	printf("线程结束\n");
}
