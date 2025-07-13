/*///------------------------------------------------------------------------------------------------------------------------//
		蓝牙作为设备相关的功能
说 明 : 配对，连接，一个BluetDevicePrivate_仅允许处理一个蓝牙设备
日 期 : 2025.4.8

/*///------------------------------------------------------------------------------------------------------------------------//

#include <stdio.h>
#include <stdint.h>
#include "obex_agent.h"
#include "obex_agent_manager.h"
#include "../include/blt_device.h"
#include "lib/agent-helper.h"
#include "bluetooth_inc.h"
#include "device.h"
#include "adapter.h"
#include "g_utils.h"
#include "blt_device.h"
#include "blt_dbussignal.h"
#include "bluetooth_inc.h"

struct BluetDeviceState{
	uint8_t connected;		//是否已连接
	uint8_t pairwed;		//是否已配对

};

struct BluetDevicePrivate_{
	Device *device;
	DbusMainThread *mainthread;
	GHashTable *user_data_hash;		//用户数据，此参数后续考虑删除
	struct BluetDeviceState *state;
	VariableArray *signal_array;		//BluetDbusSignal类型
};

//BluetDevice结构体创建
BluetDevice *bluet_device_creat(Adapter *adapter,const char *name)
{
	BluetDevice *self=(BluetDevice *)malloc(sizeof(BluetDevice));
	if(!self)
		return NULL;

	BluetDevicePrivate *priv=(BluetDevicePrivate *)malloc(sizeof(BluetDevicePrivate));
	if(!priv)
	{	
		free(self);
		return NULL;
	}
	
	GError *error = NULL;
	Device *device = find_device(adapter, name, &error);
	if(error)
	{
		g_printerr("Error: Device not found.\n");
		g_clear_error(&error);
		free(priv);
		free(self);
		return NULL;
	}

	VariableArray *signal_array=creat_variable_array(sizeof(BluetDbusSignal),4);
	if(signal_array==NULL)
	{
		g_object_unref(self->priv->device);
		free(priv);
		free(self);
		return NULL;
	}
	DbusMainThread *mainthread=dbus_main_thread_creat_once(DBUS_MAIN_THREAD_TYPE_LOOP);
	if(!mainthread)
	{
		delete_variable_array(self->priv->signal_array);
		g_object_unref(self->priv->device);
		free(priv);
		free(self);
	}

	self->priv=priv;
	self->priv->mainthread=mainthread;
	self->priv->device=device;
	self->priv->signal_array=signal_array;
	self->priv->user_data_hash=NULL;

	return self;
}

int bluet_device_delete(BluetDevice *self)
{
	if(!self)
		return 0;
	dbus_main_thread_delete_once(self->priv->mainthread);

	if(self->priv->device)
		g_object_unref(self->priv->device);
	
	if(self->priv->signal_array)
	{
		VariableArray *signal_array=self->priv->signal_array;
		size_t len=signal_array->get_size(signal_array);
		for(int i=0;i<len;i++) 
		{
			char **signal=signal_array->get(signal_array,i);
			bluet_dbus_signal_delete(system_conn,(BluetDbusSignal*)(*signal));
		}
		delete_variable_array(self->priv->signal_array);
	}
	if(self->priv->user_data_hash)
		g_hash_table_unref(self->priv->user_data_hash);

	free(self->priv);
	free(self);
	return 0;
}

Device *bluet_device_get_device(BluetDevice *device)
{
	return device->priv->device;
}

//
static void bluet_device_properties_changed_handle(GDBusConnection *connection,
										const gchar     *sender_name,
										const gchar     *object_path,
										const gchar     *interface_name,
										const gchar     *signal_name,
										GVariant        *parameters,
										gpointer         user_data)
{

	printf("interface_name:%s\nsignal_name:%s\n",interface_name,signal_name);
	// 只处理 PropertiesChanged 信号
	if (g_strcmp0(interface_name, "org.freedesktop.DBus.Properties") != 0 ||
        g_strcmp0(signal_name,     "PropertiesChanged")     != 0)
        return;

    /* 解包参数： tuple (sa{sv}as) → iface_name, dict, invalidated */
	const gchar *arg0 = g_variant_get_string(g_variant_get_child_value(parameters, 0), NULL); //第一个参数，通常是接口名
	GVariant *changed_dict = g_variant_get_child_value(parameters, 1);

	/*打印所有属性改变的信息
	GVariantIter iter;
	gchar *key;
    GVariant *value;
	g_variant_iter_init(&iter, changed_dict);
	while (g_variant_iter_next(&iter, "{sv}", &key, &value))
    {
        // 打印键名和对应值的类型
        printf("属性名: %s，类型: %s\n", key, g_variant_get_type_string(value));
	}
 	g_free(key);
	g_variant_unref(value);*/
    //仅关注 org.bluez.Device1 接口 
    if (g_strcmp0(arg0, "org.bluez.Device1") == 0)
    {
        gboolean connected;
        if (g_variant_lookup(changed_dict, "Connected", "b", &connected))
        {
            g_print("[%s] Connected = %s\n",
                    object_path,
                    connected ? "TRUE" : "FALSE");
        }

        // 5. 查找 Paired 属性
        gboolean paired;
        if (g_variant_lookup(changed_dict, "Paired", "b", &paired))
        {
            g_print("[%s] Paired = %s\n",
                    object_path,
                    paired ? "TRUE" : "FALSE");
        }

    }

    /* 释放由 g_variant_get 分配的资源 */
    g_variant_unref(changed_dict);
}


static void bluet_device_connect_callback()
{

}


static void bluet_device_pair_callback(GObject *source_object, GAsyncResult *res, gpointer user_data)
{
	printf("配对完成\n");
    g_assert(user_data != NULL);
    GHashTable *dict = (GHashTable *) user_data;
    Device *device = (Device *) g_hash_table_lookup(dict, "device");
    GError *error = NULL;
    device_pair_finish(device, res, &error);
    exit_if_error(error);
	uint8_t trused = *(uint8_t *) g_hash_table_lookup(dict, "trused");
	if(trused==1)
	{
		bluet_device_set_trusted(device,trused);
		printf("设置信任\n");
	}

    GMainLoop *mainloop = (GMainLoop *) g_hash_table_lookup(dict, "mainloop");
    g_main_loop_quit(mainloop);
}

//配对
//trused：是否信任
int bluet_device_pair_with_remote(BluetDevice *self,uint8_t trused)
{
	GError *error = NULL;
	self->priv->user_data_hash=g_hash_table_new(g_str_hash, g_str_equal);
	g_hash_table_insert(self->priv->user_data_hash, "device", self->priv->device);
	g_hash_table_insert(self->priv->user_data_hash, "mainloop", dbus_main_thread_get_main_thread(self->priv->mainthread));
	g_hash_table_insert(self->priv->user_data_hash, "trused", (gpointer)&trused);
	device_pair_async(self->priv->device, (GAsyncReadyCallback) bluet_device_pair_callback, (gpointer) self->priv->user_data_hash);
	return 0;
}


//取消配对
int bluet_device_cancel_paie_with_remote(Device *device)
{
	GError *error = NULL;
	device_cancel_pair(device,&error);
	if(error)
	{
		g_clear_error(&error);
		return -1;
	}
	return 0;
}

//取消配对
int bluet_cancel_paie_with_remote(BluetDevice *self)
{
	return bluet_device_cancel_paie_with_remote(self->priv->device);
}


//使用Device连接到远程设备
int bluet_connect_remote_device(BluetDevice *self,const char *uuid)
{
	GError *error = NULL;

	BluetDbusSignal *signal_p=bluet_dbus_signal_subscribe(system_conn,
									"org.bluez",
									"org.freedesktop.DBus.Properties",
									"PropertiesChanged",
									NULL,
									bluet_device_properties_changed_handle,
									NULL,
									NULL);
	self->priv->signal_array->append_shallow(self->priv->signal_array,signal_p);
	if(uuid==NULL)
		device_connect(self->priv->device,&error);
	else
		device_connect_profile(self->priv->device,(const gchar *)uuid,&error);
	if(error)
	{
		g_printerr("Connect failed: %s\n", error->message);
    	g_clear_error(&error);
		return -1;
	}
//	g_main_loop_run(self->priv->mainloop);
	return 0;
}

//使用Device和远程设备断开连接
int bluet_disconnect_remote_device(BluetDevice *self,const char *uuid)
{
	GError *error = NULL;

	BluetDbusSignal *signal_p=bluet_dbus_signal_subscribe(system_conn,
									"org.bluez",
									"org.freedesktop.DBus.Properties",
									"PropertiesChanged",
									NULL,
									bluet_device_properties_changed_handle,
									NULL,
									NULL);
	self->priv->signal_array->append_shallow(self->priv->signal_array,signal_p);
	if(uuid==NULL)
		device_disconnect(self->priv->device, &error);
	else
		device_disconnect_profile(self->priv->device,(const gchar *)uuid,&error);
	if(error)
	{
		g_printerr("Disconnect failed: %s\n", error->message);
    	g_clear_error(&error);
		return -1;
	}
	return 0;
}




//移除设备
int bluet_remove_remote(Adapter *adapter,const char *name)
{
	GError *error = NULL;
	printf("remove_arg\n");
	Device *device = find_device(adapter, name, &error);
	if(!device)
	{
		g_printerr("Error: Device not found.\n");
		return -1;
	}

	adapter_remove_device(adapter, device_get_dbus_object_path(device), &error);

	g_object_unref(device);
	return 0;
}


const char *bluet_device_get_address(BluetDevice *self)
{
	return device_get_address(self->priv->device,NULL);
}

const char *bluet_device_get_name(BluetDevice *self)
{
	return device_get_name(self->priv->device,NULL);
}



//获取蓝牙设备信息暂时不用
int bluet_get_remote_info(Adapter *adapter,const char *name)
{
	GError *error = NULL;
	printf("info_arg\n");
	Device *device = find_device(adapter, name, &error);
	if(!device)
	{
		g_printerr("Error: Device not found.\n");
		return -1;
	}

	g_print("[%s]\n", device_get_address(device, NULL));
	g_print("  Name: %s\n", device_get_name(device, NULL));
	g_print("  Alias: %s [rw]\n", device_get_alias(device, NULL));
	g_print("  Address: %s\n", device_get_address(device, NULL));
	g_print("  Icon: %s\n", device_get_icon(device, NULL));
	g_print("  Class: 0x%x\n", device_get_class(device, NULL));
	g_print("  Paired: %d\n", device_get_paired(device, NULL));
	g_print("  Trusted: %d [rw]\n", device_get_trusted(device, NULL));
	g_print("  Blocked: %d [rw]\n", device_get_blocked(device, NULL));
	g_print("  Connected: %d\n", device_get_connected(device, NULL));
	g_print("  UUIDs: [");
	const gchar **uuids = device_get_uuids(device, NULL);
	for (int j = 0; uuids[j] != NULL; j++)
	{
		if (j > 0) g_print(", ");
		g_print("%s", uuid_to_name(uuids[j]));
	}
	g_print("]\n");

	g_object_unref(device);
	return 0;
}


//-----------------------------------------------------------------------------------------------------------------
//属性设置/获取，不需要BluetDevice
//------------------------------------------------------------------------------------------------------------------


int bluet_device_get_trusted(Device *device)
{
	GError *error = NULL;
	device_get_trusted(device,&error);
	if(error)
	{
		g_clear_error(&error);
		return -1;
	}
	return 0;
}

//获取远程设备信任状态
int bluet_adapter_get_trusted(Adapter *adapter,const char *name)
{
	GError *error = NULL;
	Device *device = find_device(adapter, name, &error);
	if(!device)
	{
		g_clear_error(&error);
		return -1;
	}
	int value=bluet_device_get_trusted(device);
	bluet_object_free(device);
	return value;
}


int bluet_device_set_trusted(Device *device,uint8_t value)
{
	GError *error = NULL;
	device_set_trusted(device,value==0? 0 :1,&error);
	if(error)
	{
		g_clear_error(&error);
		return -1;
	}
	return 0;
}

//把远程设备标记为信任
int bluet_adapter_set_trusted(Adapter *adapter,const char *name,uint8_t value)
{
	GError *error = NULL;
	printf("remove_arg\n");
	Device *device = find_device(adapter, name, &error);
	if(!device)
	{
		g_clear_error(&error);
		return -1;
	}
	bluet_device_set_trusted(device,value);
	bluet_object_free(device);
	return 0;
}

//获取远程设备配对状态
int bluet_device_get_paired(Device *device)
{
	GError *error = NULL;
	device_get_paired(device,&error);
	if(error)
	{
		g_clear_error(&error);
		return -1;
	}
	return 0;
}

//获取远程设备配对状态
int bluet_adapter_get_paired(Adapter *adapter,const char *name)
{
	GError *error = NULL;
	Device *device = find_device(adapter, name, &error);
	if(!device)
	{
		g_clear_error(&error);
		return -1;
	}
	int value=bluet_device_get_paired(device);
	bluet_object_free(device);
	return value;
}


