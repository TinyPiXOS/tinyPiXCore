/*///------------------------------------------------------------------------------------------------------------------------//
		USB设备管理程序
说 明 : 
日 期 : 2025.5.29

/*///------------------------------------------------------------------------------------------------------------------------//

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <libudev.h>
#include "utils/utlist.h"
#include "usb_manager.h"

//libusb_init在一个进程中只能调用一次，故使用全局，单后续需要优化避免用户滥用usb_manager_delete类释放导致usb_ctx_init_num提前清0的问题
UsbManager *usb_ctx=NULL;
int usb_ctx_init_num=0;


UsbManager *usb_manager_create()
{
	int err;
	if(usb_ctx)
	{
		usb_ctx_init_num++;
		return usb_ctx;
	}
	else
	{	
		usb_ctx_init_num++;
		usb_ctx=(UsbManager *)malloc(sizeof(UsbManager));
	}	
	
	if(!usb_ctx)
		return NULL;

	err=libusb_init(&usb_ctx->ctx);
	if(err<0)
	{
		fprintf(stderr,"libusb init error\n");
		free(usb_ctx);
		return NULL;
	}
	
	return usb_ctx;
}


int usb_manager_delete(UsbManager *usb)
{
	if(!usb)
		return 0;
	usb_ctx_init_num--;
	if(usb_ctx_init_num!=0)
		return 0;
	if(usb->ctx)
		libusb_exit(usb->ctx);
	free(usb);
}


static void get_device_list_callback(UsbDeviceInfo *dev_info, void *user_data)
{
	struct LinkedList *list=(struct LinkedList *)user_data;
	UsbDeviceInfo *info=(UsbDeviceInfo *)malloc(sizeof(UsbDeviceInfo));
	memcpy(info,dev_info,sizeof(UsbDeviceInfo));
	list->push_back(list,info);
}

//扫描usb设备

//过滤器的回调，不同的过滤器使用不同的函数，
//返回1表示继续执行(非唯一)，返回0表示不继续执行(唯一或只要第一个设备)，返回-1表示不匹配
typedef int(*GetDevicesCallback)(struct libusb_device_descriptor *dev,void *filter_data);

//描述符解析以及用户回调
static int usb_manage_descriptor_analysis(libusb_device *dev,GetDevicesCallback filter_cb, void *filter_data,UsbDeviceListCallback user_cb, void *user_data)
{
	struct libusb_device_descriptor desc;
	int r = libusb_get_device_descriptor(dev, &desc);
	if (r < 0) {
		fprintf(stderr, "Failed to get device descriptor\n");
		return -1;
	}
	if(filter_cb)
		filter_cb(&desc,filter_data);

	printf("Device : Vendor ID: %04x, Product ID: %04x, Class: %02x, SubClass: %02x\n", 
			desc.idVendor, desc.idProduct, desc.bDeviceClass, desc.bDeviceSubClass);
	UsbDeviceInfo dev_info;
	memset(&dev_info,0,sizeof(UsbDeviceInfo));
	dev_info.pid=desc.idProduct;
	dev_info.vid=desc.idVendor;
	dev_info.deviceClass=desc.bDeviceClass;
	dev_info.deviceSubClass=desc.bDeviceSubClass;
	dev_info.bindport=libusb_get_port_number(dev);
	if(user_cb)
		user_cb(&dev_info,user_data);
}

//使用bus和dev获取唯一设备
static int usb_manager_get_only_device(UsbManager *usb, uint8_t busnum, uint8_t devaddr,UsbDeviceListCallback user_cb, void *user_data)
{
	libusb_device **list;
	int num_dev=0;
	num_dev = libusb_get_device_list(usb->ctx, &list);
	if(num_dev<0)
	{
		fprintf(stderr, "Failed to get device list: %s\n", libusb_error_name((int)num_dev));
		return -1;
	}

	for (ssize_t i = 0; i < num_dev; i++) 
	{
		if (libusb_get_bus_number(list[i]) == busnum && libusb_get_device_address(list[i]) == devaddr)
		{
			printf("usb_manage_descriptor_analysis\n");
			usb_manage_descriptor_analysis(list[i],NULL,NULL,user_cb,user_data);
			printf("已拿到唯一标志对应的USB设备\n");
			break;
		}
	}
	libusb_free_device_list(list, 1);
	return 0;
}

//带有过滤的扫描，也可以不设置过滤而获取所有设备
static int usb_manager_get_devices_filter(UsbManager *usb, GetDevicesCallback filter_cb, void *filter_data, UsbDeviceListCallback user_cb, void *user_data)
{
	libusb_device **list;
	int num_dev=0;
	num_dev = libusb_get_device_list(usb->ctx, &list);
	if(num_dev<0)
	{
		fprintf(stderr, "Failed to get device list: %s\n", libusb_error_name((int)num_dev));
		return -1;
	}

	for (ssize_t i = 0; i < num_dev; i++) 
	{
		usb_manage_descriptor_analysis(list[i],filter_cb,filter_data,user_cb,user_data);
	}
	libusb_free_device_list(list, 1);
	return 0;
}



//扫描所有usb设备
int usb_manager_get_devices(UsbManager *usb, UsbDeviceListCallback callback, void *user_data)
{
	usb_manager_get_devices_filter(usb,NULL,NULL,callback,user_data);
}

//根据bus很dev来筛选唯一的设备
int usb_manager_get_devices_bus_dev(UsbManager *usb, uint8_t busnum, uint8_t devaddr, UsbDeviceListCallback callback, void *user_data)
{
	usb_manager_get_only_device(usb, busnum, devaddr, callback, user_data);
}



//获取usb设备列表
struct LinkedList *usb_manager_get_device_list(UsbManager *usb)
{
	struct LinkedList *list=creat_linked_list();
	if(usb_manager_get_devices(usb,get_device_list_callback,(void *)list)<0)
	{
		delete_linked_list(list);
		return NULL;
	}
	return list;
}

