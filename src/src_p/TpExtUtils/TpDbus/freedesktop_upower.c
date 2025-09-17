/*///------------------------------------------------------------------------------------------------------------------------//
		系统级DBUS接口相关接口
说 明 : org.freedesktop.UPower接口
日 期 : 2025.5.24
作 者 : Chingan

/*///------------------------------------------------------------------------------------------------------------------------//

#include <stdio.h>
#include <string.h>
#include "freedesktop_upower.h"


static GDBusConnection *system_conn = NULL;
static void desktop_upower_create_gdbus_proxy(DesktopUPower *system, const gchar *dbus_server, const gchar *dbus_path,GError **error);

struct DesktopUPowerPrivate_{
	GDBusProxy *proxy;
	
};



G_DEFINE_TYPE_WITH_PRIVATE(DesktopUPower, desktop_upower, G_TYPE_OBJECT);	


static void desktop_upower_class_init (DesktopUPowerClass *klass)
{

}

//初始化，会被自动调用
static void desktop_upower_init (DesktopUPower *self)
{
	self->priv = desktop_upower_get_instance_private (self);
	self->priv->proxy = NULL;
//	g_assert(system_conn != NULL);
	GError *error = NULL;

	desktop_upower_create_gdbus_proxy(self, FREEDESKTOP_UPOWER_DBUS_SERVER, FREEDESKTOP_UPOWER_DBUS_PATH, &error);

	g_assert(error == NULL);
}


static void desktop_upower_create_gdbus_proxy(DesktopUPower *self, const gchar *dbus_server, const gchar *dbus_path,GError **error)
{
	self->priv->proxy=g_dbus_proxy_new_sync(
		system_conn,                            // 已获取的连接
		G_DBUS_PROXY_FLAGS_NONE,         // 默认标志
		NULL,                            // 自动加载 introspection
		dbus_server,                  // D-Bus 服务名 
		dbus_path,                 // 对象路径
		"org.freedesktop.UPower",         // 接口名
		NULL,                            // GCancellable 
		error);                           // 错误返回
	if (!self->priv->proxy)
	{
		fprintf(stderr,"org.freedesktop.UPower.Device creat error\n");
	}
}


DesktopUPower *desktop_upower_creat(GDBusConnection *conn)
{
	system_conn=conn;
	DesktopUPower *self = g_object_new(FREEDESKTOP_UPOWER_TYPE,NULL);
	return self;
}

int desktop_upower_delete(DesktopUPower *self)
{
	if(!self)
		return 0;

	g_object_unref(self);
	return 0;
}

int upower_get_path_list(DesktopUPower *self, UpowerDeviceListCallback callback, void *user)
{
	GError *error = NULL;
	GVariant *devices = g_dbus_proxy_call_sync(self->priv->proxy,
											"EnumerateDevices",
											NULL,
											G_DBUS_CALL_FLAGS_NONE,
											-1,
											NULL,
											&error);
	if (!devices) {
        fprintf(stderr, "Failed to enumerate devices: %s\n", error->message);
        g_error_free(error);
        return -1;
    }

    GVariantIter *iter;
    gchar *device_path;
    g_variant_get(devices, "(ao)", &iter);
    printf("Found devices:\n");
    while (g_variant_iter_loop(iter, "o", &device_path)) {
        printf("device path:%s\n",device_path);
		callback(device_path,user);
    }
	g_variant_iter_free(iter);
    g_variant_unref(devices);
	return 0;
}



