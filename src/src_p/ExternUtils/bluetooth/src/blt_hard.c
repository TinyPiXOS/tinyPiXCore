/*///------------------------------------------------------------------------------------------------------------------------//
		蓝牙适配器相关控制
说 明 : 主要和蓝牙本地硬件相关，主要依赖于hci协议和linux系统接口
		目前主要用于扫描本地蓝牙适配器和蓝牙适配器扫描设备
日 期 : 2025.3.12

/*///------------------------------------------------------------------------------------------------------------------------//

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <errno.h>
#include <sys/socket.h>
#include <sys/ioctl.h>
#include <bluetooth/bluetooth.h>
#include <bluetooth/hci.h>
#include <bluetooth/hci_lib.h>
#include <dbus/dbus.h>
#include <gio/gio.h>
#include <glib.h>
#include "bluetooth_inc.h"
#include "../include/blt_hard.h"
#include "../include/blt_dbussignal.h"

#define DEBUG_BLUETOOTH_SCAN

#ifdef DEBUG_BLUETOOTH_SCAN
    #define debug_printf(fmt, ...) printf(fmt, ##__VA_ARGS__)
#else
    #define debug_printf(fmt, ...)  // 如果不定义DEBUG，什么也不做
#endif

struct CallbackDataInterfacesAdded;

struct BluetDeviceScanPrivate_{
	GMainLoop *mainloop;
	Adapter *adapter;
	struct CallbackDataInterfacesAdded *callback_remove_data;
	struct CallbackDataInterfacesAdded *callback_add_data;
};



//在链表中创建适配器的回调
static void *callback_creat_adapte(void *data)
{
	struct BluetoothAdapter *adapter=(struct BluetoothAdapter *)data;
	if(!adapter)
		return NULL;
	struct BluetoothAdapter *node=(struct BluetoothAdapter *)malloc(sizeof(struct BluetoothAdapter));
	if(!node)
		return NULL;
	memcpy(node,adapter,sizeof(struct BluetoothAdapter));
	return node;
}

//在链表中删除适配器的回调
static void callback_free_adapter(void *data)
{
	struct BluetoothAdapter *adapter=(struct BluetoothAdapter *)data;
	if(!adapter)
		return ;
	free(adapter);
}



//
void bluet_set_adapter_node_callback(struct LinkedList *list)
{
	list->init_node_callback=callback_creat_adapte;
	list->free_node_callback=callback_free_adapter;
}

//获取适配器的回调（C语言）
void bluet_callback_get_adapter(const struct BluetoothAdapter* adapter, void* user_data) {
    // 将适配器添加到链表中
    struct LinkedList* list_head = (struct LinkedList*)user_data;

    // 将新的适配器节点插入链表
    list_head->push_back(list_head,(void *)adapter);
}
//（C++）
/*void BluetoothManager::adapterListCallback(BluetoothAdapter* adapter, void* user_data) {
    std::list<Adapter>* adapters = static_cast<std::list<Adapter>*>(user_data);
    adapters->push_back({adapter->id, adapter->address, adapter->name});
}*/

//获取蓝牙适配器列表
//callback:适配器列表将会返回到该回调，该函数会被多次调用，发现的每个适配器都会传递给该回调
//user_data：回调函数的用户数据
int bluet_get_adapters(BluetoothAdapterCallback callback, void* user_data)
{
	struct hci_dev_list_req *dl;
	struct hci_dev_req *dr;
	int sock, i, adapter_num=0;


	// 创建 HCI socket
	sock = socket(AF_BLUETOOTH, SOCK_RAW, BTPROTO_HCI);
	if (sock < 0) {
		perror("Socket error");
		return -1;
	}

	// 分配内存存储设备列表
	dl = malloc(HCI_MAX_DEV * sizeof(struct hci_dev_req) + sizeof(uint16_t));
	if (!dl) {
		perror("Memory allocation failed");
		close(sock);
		return -1;
	}

	dl->dev_num = HCI_MAX_DEV;
	dr = dl->dev_req;

	// 获取所有 HCI 设备
	if (ioctl(sock, HCIGETDEVLIST, dl) < 0) {
		perror("HCIGETDEVLIST failed");
		free(dl);
		close(sock);
		return -1;
	}

	// 遍历所有蓝牙适配器
	for (i = 0; i < dl->dev_num; i++) {
		struct hci_dev_info di;
		di.dev_id = dr[i].dev_id;

		// 获取设备详细信息
		if (ioctl(sock, HCIGETDEVINFO, (void *)&di) < 0) {
			perror("HCIGETDEVINFO failed");
			continue;
		}

		// 组织数据
		struct BluetoothAdapter adapter;
		adapter.id = di.dev_id;
		ba2str(&di.bdaddr, adapter.address);
		strncpy(adapter.name, di.name, sizeof(adapter.name) - 1);
		adapter.name[sizeof(adapter.name) - 1] = '\0';
		
		// 调用回调函数
		if (callback) {
			callback((const struct BluetoothAdapter *)(&adapter), user_data);
		}
		adapter_num++;
	}

	free(dl);
	close(sock);
	return adapter_num++;
}


//使用id打开蓝牙适配器
int bluet_device_open(int dev_id)
{
	int sock=0;
	sock = hci_open_dev(dev_id);              //打开设备,Socket与device ID=参数dev_id的Dongle绑定起来
	if (sock < 0)
	{
     		perror("opening socket error");
     		exit(1);
    	}
	return sock; 
}

//关闭兰也适配器
int bluet_device_close(int sock)
{
	hci_close_dev(sock);
	return 0;
}

//获取默认的蓝牙适配器
int bluet_get_device_default()
{
	int dev_id;
	dev_id = hci_get_route(NULL);             //查找Dongle，发现Dongle Bdaddr不等于参数bdaddr的第一个Dongle，则返回此Dongle Device ID
	if (dev_id < 0)
	{
		perror("find dongle error\n");
		return -1;
	}
	return dev_id;
}

//设置蓝牙的设备名
int bluet_set_device_name(int sock, const char *name, int timeout)
{
	if(hci_write_local_name(sock,name,timeout)<0)
	{
		fprintf(stderr,"set bluetooth device name error\n");
		return -1;
	}
	return 0;
}

//获取蓝牙设备名
int bluet_get_device_name(int sock, char *name, int len,int timeout)
{
	if(hci_read_local_name(sock,len,name,timeout)<0)
	{
		fprintf(stderr,"get bluetooth device name error\n");
		return -1;
	}
	return 0;
}




//在链表中创建适配器的回调
static void *callback_creat_remote(void *data)
{
	struct BluetoothRemote *remote=(struct BluetoothRemote *)data;
	if(!remote)
		return NULL;
	struct BluetoothRemote *node=(struct BluetoothRemote *)malloc(sizeof(struct BluetoothRemote));
	if(!node)
		return NULL;
	memcpy(node,remote,sizeof(struct BluetoothRemote));
	return node;
}

//在链表中删除适配器的回调 
static void callback_free_remote(void *data)
{
	struct BluetoothRemote *remote=(struct BluetoothRemote *)data;
	if(!remote)
		return ;
	free(remote);
}


void bluet_set_remote_node_callback(struct LinkedList *list)
{
	list->init_node_callback=callback_creat_remote;
	list->free_node_callback=callback_free_remote;
}

//获取蓝牙信号的回调，如果上层使用C++，此函数重新实现即可
void bluet_callback_get_remote(const struct BluetoothRemote* remote, void* user_data)
{
	struct LinkedList* list_head = (struct LinkedList*)user_data;
	list_head->push_back(list_head,(void *)remote);	
	debug_printf("添加设备到列表\n");
}




int bluet_hci_device_inquiry(int dev_id,int sock,BluetoothRemoteCallback callback, void *user_data)
{
	int i=0;
	int num_inquiry_device=0;
	int len =4 ,max_rsp=255;
	long flags=IREQ_CACHE_FLUSH;
	struct hci_version *ver;
	inquiry_info *ii = NULL;
	uint8_t lap[3] = {0x33, 0x8B, 0x9E};  // 通用查询 LAP


	ii = (inquiry_info*)malloc(max_rsp * sizeof(inquiry_info));
	if(!ii)
	{
		return -1;
	}
	printf("seach bluetooth dev\n");
	num_inquiry_device = hci_inquiry(dev_id, len, max_rsp, lap, &ii, flags);
	//指定的Dongle去搜索周围所有bluetooth device，返回的num_rsp是搜索到的周围device数量
	if(num_inquiry_device < 0) 
		perror("hci_inquiry");
	printf("num%d\n",num_inquiry_device);
	for (i = 0; i < num_inquiry_device; i++) 
	{
		struct BluetoothRemote remote;

		ba2str(&(ii+i)->bdaddr, remote.address);  //将BDADDR转换为字符串
		memset(remote.name, 0, sizeof(remote.name));
		if (hci_read_remote_name(sock, &(ii+i)->bdaddr, sizeof(remote.name),remote.name, 0) < 0)//得到指定BDAddr的reomte device Name
			strcpy(remote.name, "[unknown]");
		hci_read_remote_version(sock,0,ver,flags);
		if (hci_read_rssi(sock, hci_devid(remote.address), &remote.rssi,1000) < 0) {
            remote.rssi = -100; // 无法获取 RSSI 时默认值
        }
		remote.class_type=(ii + i)->dev_class[2] << 16 | (ii + i)->dev_class[1] << 8 | (ii + i)->dev_class[0];
		if(callback)
			callback(&remote,user_data);

		printf("%s %s   %d  %d  %d\n", remote.address, remote.name,ver->manufacturer,ver->lmp_ver,ver->lmp_subver);
   	}
	free(ii);
	return 0;
}


//解析值
static void *analysis_message_iter_get_value(DBusMessageIter *prop_entry_iter,int type)
{
	void *value=NULL;
	
	dbus_message_iter_next(prop_entry_iter);
	DBusMessageIter variant_iter;
	dbus_message_iter_recurse(prop_entry_iter, &variant_iter);
	printf("debug\n");
	//设置为DBUS_TYPE_INVALID的时候不管什么类型都返回
	if(dbus_message_iter_get_arg_type(&variant_iter) == type || type == DBUS_TYPE_INVALID)	
	{
		dbus_message_iter_get_basic(&variant_iter, &value);		//获取基本类型的值
	}
//	if(value)
//		printf("value=%s\n",(char *)value);
	return value;
}

//解析键值对
static int analysis_key_value(DBusMessageIter *props_iter,char *key,void *value)
{
	
}


//蓝牙的属性解析
static int bluet_attr_analysis(DBusMessageIter *props_iter,struct BluetoothRemote *remote)
{
	void *value;
	while (dbus_message_iter_get_arg_type(props_iter) != DBUS_TYPE_INVALID) 
	{
		if (dbus_message_iter_get_arg_type(props_iter) != DBUS_TYPE_DICT_ENTRY) 	//字典()
		{
			dbus_message_iter_next(props_iter);
			continue;
		}
		DBusMessageIter prop_entry_iter;
		dbus_message_iter_recurse(props_iter, &prop_entry_iter);

		// 第一个元素：属性名称 (string)
		char *prop_name = NULL;
		if (dbus_message_iter_get_arg_type(&prop_entry_iter) == DBUS_TYPE_STRING)
			dbus_message_iter_get_basic(&prop_entry_iter, &prop_name);

		// 判断属性名称
		if (!prop_name) 
			continue;
		// 查找设备名称
		if (strcmp(prop_name, "Name") == 0) 
		{
			value=analysis_message_iter_get_value(&prop_entry_iter,DBUS_TYPE_STRING);
			if(value)
			{
				//printf("name=%s\n",(char *)value);
				memcpy(remote->name,value,MIN_VALUE(BLUETOOTH_NAME_MAX_LEN,strlen((char *)value)));
			}
		}
		// 查找设备MAC地址
		else if (strcmp(prop_name, "Address") == 0)
		{
			value=analysis_message_iter_get_value(&prop_entry_iter,DBUS_TYPE_STRING);
			if(value)
			{
				//printf("mac=%s\n",(char *)value);
				memcpy(remote->address,value,BLUETOOTH_ADDR_MAX_LEN);		//BLUETOOTH_ADDR_MAX_LEN
			}
		}
		else if(strcmp(prop_name, "RSSI") == 0)
		{
		
			value=analysis_message_iter_get_value(&prop_entry_iter,DBUS_TYPE_INT16);
			remote->rssi=((int8_t )value);		//此处必须强制转换
			//printf("rssi=%d\n",remote->rssi);
		}
		else
		{
			printf("其他类型\n");
		}

		dbus_message_iter_next(props_iter);
	}
	printf("name=%s,addr=%s,rssi=%d\n",remote->name,remote->address,remote->rssi);
	return 0;
}


int bluet_dbus_device_inquiry()
{

}



DBusHandlerResult signal_handler(DBusConnection *conn, DBusMessage *msg, void *user_data) 
{
//	struct LinkedList *remote_list=(struct LinkedList *)user_data;

	struct CallbackData *cb_data=(struct CallbackData *)user_data;
	BluetoothRemoteCallback callback=(BluetoothRemoteCallback)cb_data->callback;
	struct LinkedList *remote_list=(struct LinkedList *)cb_data->userdata;

    if (!dbus_message_is_signal(msg, "org.freedesktop.DBus.ObjectManager", "InterfacesAdded"))
	{
		printf("不是新增信号？？？\n");
        return DBUS_HANDLER_RESULT_NOT_YET_HANDLED;
	}
	printf("开始解析\n");
    DBusMessageIter msg_iter;
    if (!dbus_message_iter_init(msg, &msg_iter)) {
        fprintf(stderr, "消息无参数\n");
        return DBUS_HANDLER_RESULT_HANDLED;
    }

    // 第一个参数：对象路径
    if (dbus_message_iter_get_arg_type(&msg_iter) == DBUS_TYPE_OBJECT_PATH) {
        char *object_path = NULL;
        dbus_message_iter_get_basic(&msg_iter, &object_path);
        printf("发现新对象: %s\n", object_path);
    }

    // 第二个参数：数组(里面是一堆字典)
    dbus_message_iter_next(&msg_iter);
    if (dbus_message_iter_get_arg_type(&msg_iter) != DBUS_TYPE_ARRAY)	//DBUS_TYPE_ARRAY：数组类型，可以存多个键值对
	{
		printf("此类型不是数组\n");
        return DBUS_HANDLER_RESULT_HANDLED;	
	}

    DBusMessageIter array_iter;
    dbus_message_iter_recurse(&msg_iter, &array_iter);	//获取容器类型的值，返回整个容器，还需要继续解析

    // 遍历字典中的每个键值对
    while (dbus_message_iter_get_arg_type(&array_iter) != DBUS_TYPE_INVALID) 	//DBUS_TYPE_INVALID：终止标志
	{
        if (dbus_message_iter_get_arg_type(&array_iter) == DBUS_TYPE_DICT_ENTRY) //DBUS_TYPE_DICT_ENTRY 是 D-Bus 类型系统中用于表示 ​字典条目（键值对）​ 的特殊容器类型
		{
            DBusMessageIter dict_entry_iter;
            dbus_message_iter_recurse(&array_iter, &dict_entry_iter);

            // 第一个元素：接口名称 (string)
            char *interface_name = NULL;
            if (dbus_message_iter_get_arg_type(&dict_entry_iter) == DBUS_TYPE_STRING)
                dbus_message_iter_get_basic(&dict_entry_iter, &interface_name);		//获取基本类型的值，如string，int等

            // 判断是否为 org.bluez.Device1 接口
            if (interface_name && strcmp(interface_name, "org.bluez.Device1") == 0) 
			{
                // 第二个元素：数组
                dbus_message_iter_next(&dict_entry_iter);
                if (dbus_message_iter_get_arg_type(&dict_entry_iter) != DBUS_TYPE_ARRAY) 
					continue;

				DBusMessageIter props_iter;
				dbus_message_iter_recurse(&dict_entry_iter, &props_iter);
				char *device_name = NULL;
				char *device_addr = NULL;
				int16_t device_rssi;
				struct BluetoothRemote remote;
				bluet_attr_analysis(&props_iter,&remote);
				callback(&remote,cb_data->userdata);
				//remote_list->push_back(remote_list,(void *)&remote);
				// 遍历属性字典
				/*while (dbus_message_iter_get_arg_type(&props_iter) != DBUS_TYPE_INVALID) 
				{
					if (dbus_message_iter_get_arg_type(&props_iter) == DBUS_TYPE_DICT_ENTRY) 	//字典()
					{
						DBusMessageIter prop_entry_iter;
						dbus_message_iter_recurse(&props_iter, &prop_entry_iter);

						// 第一个元素：属性名称 (string)
						char *prop_name = NULL;
						if (dbus_message_iter_get_arg_type(&prop_entry_iter) == DBUS_TYPE_STRING)
							dbus_message_iter_get_basic(&prop_entry_iter, &prop_name);

						// 判断属性名称
						if (!prop_name) 
							continue;
						// 查找设备名称
						if (strcmp(prop_name, "Name") == 0) {
							//dbus_message_iter_next(&prop_entry_iter);
							//DBusMessageIter variant_iter;
							device_name=analysis_message_iter_get_value(&prop_entry_iter,DBUS_TYPE_STRING);
						}
						// 查找设备MAC地址
						else if (strcmp(prop_name, "Address") == 0) {
							//dbus_message_iter_next(&prop_entry_iter);
							//DBusMessageIter variant_iter;
							device_addr=analysis_message_iter_get_value(&prop_entry_iter,DBUS_TYPE_STRING);
						}
						else if(strcmp(prop_name, "RSSI") == 0)
						{
							
							dbus_message_iter_next(&prop_entry_iter);
							DBusMessageIter variant_iter;
							dbus_message_iter_recurse(&prop_entry_iter, &variant_iter);
							if (dbus_message_iter_get_arg_type(&variant_iter) == DBUS_TYPE_INT16) {
								dbus_message_iter_get_basic(&variant_iter, &device_rssi);
							}
						}
						else
						{
							printf("未知类型\n");
						}
					}
					dbus_message_iter_next(&props_iter);
				}*/
            }
        }
		else
		{
			printf("这个子项不是字典\n");
		}
        dbus_message_iter_next(&array_iter);
    }
    return DBUS_HANDLER_RESULT_HANDLED;
}

//开始扫描
void bluet_dbus_start_discovery(DBusConnection *conn,const char *adapter_path) {
    DBusMessage *msg, *reply;
    DBusError err;
    
    dbus_error_init(&err);
    
    // 调用 StartDiscovery 方法，无参数
    msg = dbus_message_new_method_call(BLUEZ_BUS_NAME,       // 目标服务
									adapter_path,         // 对象路径，如 "/org/bluez/hci1"
									ADAPTER_INTERFACE,    // 接口名 "org.bluez.Adapter1"
									"StartDiscovery"      // 方法名，无参数
    );
    
    if (!msg) {
        fprintf(stderr, "无法创建 DBus 消息\n");
        return;
    }
    
    reply = dbus_connection_send_with_reply_and_block(conn, msg, -1, &err);
	dbus_message_unref(msg);

    if (dbus_error_is_set(&err)) {
        fprintf(stderr, "StartDiscovery 请求失败: %s\n", err.message);
        dbus_error_free(&err);
    } else {
        printf("蓝牙扫描已启动 (%s)\n", ADAPTER_PATH);
    }
    
    if (reply)
        dbus_message_unref(reply);
}

// 停止蓝牙扫描
void bluet_dbus_stop_discovery(DBusConnection *conn, const char *adapter_path) {
    DBusMessage *msg, *reply;
    DBusError err;

    dbus_error_init(&err);

    msg = dbus_message_new_method_call(BLUEZ_BUS_NAME,
                                       adapter_path,
                                       ADAPTER_INTERFACE,
                                       "StopDiscovery");

    if (!msg) {
        fprintf(stderr, "无法创建 StopDiscovery D-Bus 消息\n");
        return;
    }

    // 发送请求
    reply = dbus_connection_send_with_reply_and_block(conn, msg, -1, &err);
    dbus_message_unref(msg);

    if (dbus_error_is_set(&err)) {
        fprintf(stderr, "StopDiscovery 请求失败: %s\n", err.message);
        dbus_error_free(&err);
    } else {
        printf("成功停止蓝牙扫描\n");
    }

    if (reply) {
        dbus_message_unref(reply);
    }
}


//////////////////////////////////////////////////////////////////////////////////////////////////////////

struct CallbackDataInterfacesAdded{
	BluetoothRemoteCallback callback;
	void *user_data;
	gchar *adapter_object_path;
};







//Glib接口属性改变信号回调处理函数
static void _adapter_property_changed(GDBusConnection *connection, 
									const gchar *sender_name, 
									const gchar *object_path, 
									const gchar *interface_name, 
									const gchar *signal_name, 
									GVariant *parameters, 
									gpointer user_data)
{
    g_assert(user_data != NULL);
    GMainLoop *mainloop = user_data;
    
    GVariant *changed_properties = g_variant_get_child_value(parameters, 1);
    GVariant *discovering_variant = g_variant_lookup_value(changed_properties, "Discovering", NULL);
    if(discovering_variant)
    {
        const gboolean discovering = g_variant_get_boolean(discovering_variant);
        if(!discovering)
        {
            g_main_loop_quit(mainloop);
        }
        g_variant_unref(discovering_variant);
    }
    g_variant_unref(changed_properties);
}

//Glib接口设备新增信号回调接口
static void _manager_device_found(GDBusConnection *connection, 
								const gchar *sender_name, 
								const gchar *object_path, 
								const gchar *interface_name, 
								const gchar *signal_name, 
								GVariant *parameters, 
								gpointer user_data)
{
    g_assert(user_data != NULL);
	printf("蓝牙信号新增？？？？？？？？？？？？？？？？？\n");
	struct CallbackDataInterfacesAdded *callback_data=(struct CallbackDataInterfacesAdded *)user_data;
    const gchar *adapter_object_path = callback_data->adapter_object_path;
	BluetoothRemoteCallback callback = callback_data->callback;

    GVariant *arg0 = g_variant_get_child_value(parameters, 0);
    const gchar *str_object_path = g_variant_get_string(arg0, NULL);
    g_variant_unref(arg0);

    if (!g_str_has_prefix(str_object_path, adapter_object_path))
        return;

    GVariant *interfaces_and_properties = g_variant_get_child_value(parameters, 1);
    GVariant *properties = NULL;
    if (g_variant_lookup(interfaces_and_properties, DEVICE_DBUS_INTERFACE, "@a{sv}", &properties))
    {
		struct BluetoothRemote remote;
		memset(&remote,0,sizeof(struct BluetoothRemote));
		remote.object_path = g_strdup(str_object_path);
		debug_printf("object_path:%s\n",remote.object_path);
		 // 2. 取 Address
		GVariant *v = g_variant_lookup_value(properties, "Address", NULL);
		if (v) {
			char *address = g_variant_dup_string(v, NULL);
			strncpy(remote.address,address,sizeof(remote.address));
			debug_printf("address=%s\n",remote.address);
			g_free(address);
			g_variant_unref(v);
		}

		// 3. 取 Name
		v = g_variant_lookup_value(properties, "Name", NULL);
		if (v) {
			char *name = g_variant_dup_string(v, NULL);
			strncpy(remote.name,name,sizeof(remote.name));
			debug_printf("name=%s\n",remote.name);
			g_free(name);
			g_variant_unref(v);
		}

		// 4. 取 Alias
		v = g_variant_lookup_value(properties, "Alias", NULL);
		if (v) {
			remote.alias = g_variant_dup_string(v, NULL);
			debug_printf("Alias=%s\n",remote.alias);
			g_variant_unref(v);
		}

		// 5. 取 Icon
		v = g_variant_lookup_value(properties, "Icon", NULL);
		if (v) {
			remote.icon = g_variant_dup_string(v, NULL);
			debug_printf("Icon=%s\n",remote.icon);
			g_variant_unref(v);
		}

		// 6. 取 Class
		v = g_variant_lookup_value(properties, "Class", NULL);
		if (v) {
			remote.class_type = g_variant_get_uint32(v);
			debug_printf("Class=%d\n",remote.class_type);
			g_variant_unref(v);
		}

		// 7. 取 LegacyPairing
		v = g_variant_lookup_value(properties, "LegacyPairing", NULL);
		if (v) {
			remote.legacy_pairing = (uint8_t)g_variant_get_boolean(v);
			debug_printf("LegacyPairing=%d\n",remote.legacy_pairing);
			g_variant_unref(v);
		}

		// 8. 取 Paired
		v = g_variant_lookup_value(properties, "Paired", NULL);
		if (v) {
			remote.paired = (uint8_t)g_variant_get_boolean(v);
			debug_printf("Paired=%d\n",remote.paired);
			g_variant_unref(v);
		}

		// 9. 取 RSSI
		v = g_variant_lookup_value(properties, "RSSI", NULL);
		if (v) {
			remote.rssi = g_variant_get_int16(v);
			debug_printf("RSSI=%d\n",remote.rssi);
			g_variant_unref(v);
		}

		if(callback)
		{
			callback(&remote, callback_data->user_data);
		}
		if(remote.icon)
			g_free(remote.icon);
		if(remote.alias)
			g_free(remote.alias);
		if(remote.object_path)
			g_free(remote.object_path);
		remote.icon=NULL;
		remote.alias=NULL;
		remote.object_path=NULL;
		g_variant_unref(properties);
	}
    g_variant_unref(interfaces_and_properties);
}


static void _manager_device_remove(GDBusConnection *connection, 
								const gchar *sender_name, 
								const gchar *object_path, 
								const gchar *interface_name, 
								const gchar *signal_name, 
								GVariant *parameters, 
								gpointer user_data)
{

	struct CallbackDataInterfacesAdded *callback_data=(struct CallbackDataInterfacesAdded *)user_data;
    const gchar *adapter_object_path = callback_data->adapter_object_path;
	BluetoothRemoteCallback callback = callback_data->callback;



	GVariant *arg0 = g_variant_get_child_value(parameters, 0);
    const gchar *str_object_path = g_variant_get_string(arg0, NULL);
    g_variant_unref(arg0);


	if(g_strcmp0(signal_name, "InterfacesRemoved") == 0)
	{
		const gchar *interface_object_path = NULL;
		GVariant *interfaces = NULL;

		// parameters 是一个元组：(object_path, as)
		g_variant_get(parameters, "(&o@as)", &interface_object_path, &interfaces);

		// 遍历字符串数组
		GVariantIter iter;
		gchar *interface_name;
		g_variant_iter_init(&iter, interfaces);
		while (g_variant_iter_loop(&iter, "s", &interface_name)) 
		{
			if (g_strcmp0(interface_name, DEVICE_DBUS_INTERFACE) != 0) {
				continue;
			}
			printf("蓝牙信号没了？？？？？？？？？？？？？？？？？\n");
			struct BluetoothRemote remote;
			memset(&remote,0,sizeof(struct BluetoothRemote));
			remote.object_path=g_strdup(str_object_path);
			debug_printf("object_path:%s\n",remote.object_path);
			path_to_address(str_object_path,remote.address,sizeof(remote.address));
			
			if(callback)
			{
				callback(&remote, callback_data->user_data);
			}

			if(remote.object_path)
				free(remote.object_path);
			remote.object_path=NULL;
		}

		g_variant_unref(interfaces);	
	}
}


BluetDeviceScan *bluet_device_scan_creat()
{
	BluetDeviceScan *scan=(BluetDeviceScan *)malloc(sizeof(BluetDeviceScan));
	if(!scan)
		return NULL;

	BluetDeviceScanPrivate *priv=(BluetDeviceScanPrivate *)malloc(sizeof(BluetDeviceScanPrivate ));
	if(!priv)
	{	
		free(scan);
		return NULL;
	}

	struct CallbackDataInterfacesAdded *data_add=(struct CallbackDataInterfacesAdded *)malloc(sizeof(struct CallbackDataInterfacesAdded ));
	if(!data_add)
	{
		free(priv);
		free(scan);
		return NULL;
	}

	struct CallbackDataInterfacesAdded *data_remove=(struct CallbackDataInterfacesAdded *)malloc(sizeof(struct CallbackDataInterfacesAdded ));
	if(!data_remove)
	{
		free(data_add);
		free(priv);
		free(scan);
		return NULL;
	}

	scan->priv=priv;
	scan->priv->callback_add_data=data_add;
	scan->priv->callback_remove_data=data_remove;
	scan->priv->mainloop=g_main_loop_new(NULL, FALSE);



	return scan;
}

int bluet_device_scan_delete(BluetDeviceScan *scan)
{
	if(!scan)
		return 0;
	if(scan->priv->mainloop)
		g_main_loop_unref(scan->priv->mainloop);
	if(scan->priv->adapter)
		g_object_unref(scan->priv->adapter);
	if(scan->priv->callback_add_data)
		free(scan->priv->callback_add_data);
	if(scan->priv->callback_remove_data)
		free(scan->priv->callback_remove_data);
	scan->priv->mainloop=NULL;
	scan->priv->adapter=NULL;
	scan->priv->callback_add_data=NULL;
	scan->priv->callback_remove_data=NULL;
	free(scan->priv);
	free(scan);
	return 0;
}

int bluet_adapter_start_discovery(BluetDeviceScan *scan)
{
	GError *error = NULL;

	if(!scan)
		return -1;
	adapter_start_discovery(scan->priv->adapter, &error);
	g_main_loop_run(scan->priv->mainloop);
	return 0;
}


int bluet_adapter_stop_discovery(BluetDeviceScan *scan)
{
	GError *error = NULL;

	if(!scan)
		return -1;
	adapter_stop_discovery(scan->priv->adapter, &error);
	g_main_loop_quit(scan->priv->mainloop);
	g_main_loop_unref(scan->priv->mainloop);
	scan->priv->mainloop=NULL;
	scan=NULL;
	return 0;
}


BluetDeviceScan *bluet_adapter_scan_creat(const char *local)
{
	GError *error = NULL;
	Adapter *adapter=find_adapter(local,&error);
	if(!adapter)
		return NULL;
	
	BluetDeviceScan *scan=bluet_device_scan_creat();
	if(!scan)
	{
		g_object_unref(adapter);
		return NULL;
	}
	scan->priv->adapter=adapter;
	return scan;
}

int bluet_adapter_scan_delete(BluetDeviceScan *scan)
{
	if(!scan)
		return 0;
	if(!scan->priv)
		return 0;
	if(scan->priv->adapter)
		g_object_unref(scan->priv->adapter);
	scan->priv->adapter=NULL;
	bluet_device_scan_delete(scan);
	return 0;
}



BluetDbusSignal *bluet_adapter_interfaces_added(BluetDeviceScan *scan,BluetoothRemoteCallback callback,void *userdata)
{
	scan->priv->callback_add_data->adapter_object_path=(gpointer) adapter_get_dbus_object_path(scan->priv->adapter);
	scan->priv->callback_add_data->callback = callback;		//用户回调
	scan->priv->callback_add_data->user_data = userdata;	//用户回调的数据	
	BluetDbusSignal *object_sig_sub = bluet_dbus_signal_subscribe_interfaces_added(system_conn,_manager_device_found,
								(gpointer) scan->priv->callback_add_data, 
								NULL);
	return object_sig_sub;
}

BluetDbusSignal *bluet_adapter_interfaces_removed(BluetDeviceScan *scan,BluetoothRemoteCallback callback,void *userdata)
{
	scan->priv->callback_remove_data->adapter_object_path=(gpointer) adapter_get_dbus_object_path(scan->priv->adapter);
	scan->priv->callback_remove_data->callback = callback;		//用户回调
	scan->priv->callback_remove_data->user_data = userdata;	//用户回调的数据	
	BluetDbusSignal *object_sig_sub = bluet_dbus_signal_subscribe_interfaces_removed(system_conn,_manager_device_remove,
								(gpointer) scan->priv->callback_remove_data, 
								NULL);
	return object_sig_sub;
}



BluetDbusSignal *bluet_adapter_properties_changed(BluetDeviceScan *scan,BluetoothRemoteCallback callback,void *userdata)
{
	BluetDbusSignal *object_sig_sub = bluet_dbus_signal_subscribe_properties_changed(system_conn,scan->priv->adapter,_adapter_property_changed,scan->priv->mainloop,NULL);
	return object_sig_sub;
}

void bluet_adapter_sbus_signal_delete(BluetDbusSignal *sig_sub)
{
	if(!sig_sub)
		return ;
	bluet_dbus_signal_delete(system_conn,sig_sub);
	sig_sub=NULL;
}



int scan_device_glib() {
    GError *error = NULL;

	dbus_connect_init();

	if (!dbus_system_connect(&error))
    {
        g_printerr("Couldn't connect to DBus system bus: %s\n", error->message);
        exit(EXIT_FAILURE);
    }
	printf("dbus_system_connect ok\n");

	
	BluetDeviceScan *scan=bluet_adapter_scan_creat("hci0");
	printf("creat ok\n");

	BluetDbusSignal *sig_sub_ia=bluet_adapter_interfaces_added(scan ,NULL ,NULL);

	BluetDbusSignal *sig_sub_pc=bluet_adapter_properties_changed(scan,NULL,NULL);

	
	g_print("Searching...\n");
	bluet_adapter_start_discovery(scan);

	/* Discovering process here... */
	bluet_adapter_stop_discovery(scan);

	bluet_adapter_sbus_signal_delete(sig_sub_ia);
	exit_if_error(error);
	bluet_adapter_sbus_signal_delete(sig_sub_pc);
	exit_if_error(error);
	
	g_print("Done\n");
	bluet_adapter_scan_delete(scan);
    return 0;
}


int bluet_adapter_get_powered(Adapter *adapter)
{
	return adapter_get_powered(adapter, NULL);
}

int bluet_adapter_set_powered(Adapter *adapter,uint8_t state)
{
	adapter_set_powered(adapter, state== 0 ? FALSE : TRUE ,NULL);
	return 0;
}



typedef enum{
	GET_DEVICE_FILTER_CONNECTED,
	GET_DEVICE_FILTER_PAIRED,
	GET_DEVICE_FILTER_TRUSTED
}GetDeviceFilterType;

//获取设备列表
static int bluet_adapter_get_device_list(Adapter *adapter,GetDeviceFilterType filter,BluetoothRemoteCallback callback, void *user_data)
{
	GError *error = NULL;
	Manager *manager = manager_new(system_conn);
	const gchar **devices_list = manager_get_devices(manager, adapter_get_dbus_object_path(adapter));

	if (devices_list == NULL)
	{
		fprintf(stderr,"No devices found\n");
		return -1;
	}

	const gchar **devices = NULL;
	for (devices = devices_list; *devices != NULL; devices++)
	{
		const gchar *device_path = *devices;
		Device *device = device_new(device_path);
		g_print("%s (%s)\n", device_get_alias(device, &error), device_get_address(device, &error));
		switch(filter)
		{
			case GET_DEVICE_FILTER_CONNECTED:
				if(device_get_connected(device,&error)!=TRUE)
				continue;
			case GET_DEVICE_FILTER_PAIRED:
				if(device_get_paired(device,&error)!=TRUE)
				continue;
			case GET_DEVICE_FILTER_TRUSTED:
				if(device_get_trusted(device,&error)!=TRUE)
				continue;
			default:
				break;
		}
		

		BluetoothRemote remote;
		memset(&remote,0,sizeof(remote));
		const gchar *address=device_get_address(device, &error);
		const gchar *alias=device_get_alias(device, &error);

		strncpy(remote.address,address,BLUETOOTH_ADDR_MAX_LEN-1);
		strncpy(remote.name,alias,BLUETOOTH_NAME_MAX_LEN-1);

		if(callback)
			callback(&remote,user_data);

		g_object_unref(device);
	}
}

//获取已连接设备列表
int bluet_adapter_get_connected_device_list(Adapter *adapter,BluetoothRemoteCallback callback, void *user_data)
{
	return bluet_adapter_get_device_list(adapter,GET_DEVICE_FILTER_CONNECTED,callback,user_data);
}

//获取以配对的设备列表
int bluet_adapter_get_paired_device_list(Adapter *adapter,BluetoothRemoteCallback callback, void *user_data)
{
	return bluet_adapter_get_device_list(adapter,GET_DEVICE_FILTER_PAIRED,callback,user_data);
}

//获取已信任的设备列表
int bluet_adapter_get_trusted_device_list(Adapter *adapter,BluetoothRemoteCallback callback, void *user_data)
{
	return bluet_adapter_get_device_list(adapter,GET_DEVICE_FILTER_TRUSTED,callback,user_data);
}


//设置可被扫描(可见性)
//discoverable,0或1
int bluet_adapter_set_discoverable(Adapter *adapter,uint8_t discoverable)
{
	adapter_set_discoverable(adapter, discoverable==0?FALSE:TRUE, NULL);
}

//设置可见性超时
int bluet_adapter_set_discoverable_timeout(Adapter *adapter,uint32_t timeout)
{
	adapter_set_discoverable_timeout(adapter, timeout, NULL);
}
            


//获取蓝牙设备ID
//bt_addr:地址，设置为NULL的时候返回默认的蓝牙适配器的ID
BltID bluet_get_device_id(bdaddr_t *bdaddr)
{
	return (BltID)hci_get_route(bdaddr);
}

//以HCI的方式打开蓝牙
BltSock bluet_open_device_hci(BltID dev_id)
{
	return hci_open_dev((int)dev_id);
}

int bluet_close_device(BltSock sock)
{
	return hci_close_dev(sock);
}





