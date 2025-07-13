/*///------------------------------------------------------------------------------------------------------------------------//
		系统级DBUS接口相关接口
说 明 : org.freedesktop.NetworkManager接口,后续网卡相关管理逐步切换到此处
日 期 : 2025.6.9
作 者 : Chingan

/*///------------------------------------------------------------------------------------------------------------------------//

#include <stdio.h>
#include <string.h>
#include "network_manager.h"

static GDBusConnection *system_conn = NULL;
static GDBusProxy *network_manager_create_gdbus_proxy(NetworkManager *self, const gchar *dbus_interface, GError **error);

struct NetworkManagerPrivate_{
	GDBusProxy *proxy;
	
};

G_DEFINE_TYPE_WITH_PRIVATE(NetworkManager, network_manager, G_TYPE_OBJECT);	


static void network_manager_class_init (NetworkManagerClass *klass)
{

}

//初始化，会被自动调用
static void network_manager_init (NetworkManager *self)
{
	self->priv = network_manager_get_instance_private (self);
	self->priv->proxy = NULL;
//	g_assert(system_conn != NULL);
	GError *error = NULL;

	self->priv->proxy=network_manager_create_gdbus_proxy(self, NETWORK_MANAGER_DBUS_INTERFACE, &error);

	g_assert(error == NULL);
}


static GDBusProxy *network_manager_create_gdbus_proxy(NetworkManager *self, const gchar *dbus_interface, GError **error)
{
	GDBusProxy *proxy=g_dbus_proxy_new_sync(
		system_conn,                            // 已获取的连接
		G_DBUS_PROXY_FLAGS_NONE,         // 默认标志
		NULL,                            // 自动加载 introspection
		NETWORK_MANAGER_DBUS_SERVER,			// D-Bus 服务名 
		NETWORK_MANAGER_DBUS_PATH,				// 对象路径
		dbus_interface,         				// 接口名
		NULL, 								// GCancellable 
		error);								// 错误返回
	if (!proxy)
	{
		fprintf(stderr,"org.freedesktop.login1 creat error\n");
		return NULL;
	}
	return proxy;
}


NetworkManager *network_manager_creat(GDBusConnection *conn)
{
	system_conn=conn;
	NetworkManager *self = g_object_new(NETWORK_MANAGER_TYPE,NULL);
	return self;
}

int network_manager_delete(NetworkManager *self)
{
	if(!self)
		return 0;

	g_object_unref(self);
	return 0;
}

// D-Bus 接口常量
#define NM_IFACE      "org.freedesktop.NetworkManager"
#define DEV_IFACE     "org.freedesktop.NetworkManager.Device"
#define AC_IFACE      "org.freedesktop.NetworkManager.Connection.Active"
#define CONN_IFACE    "org.freedesktop.NetworkManager.Settings.Connection"

// 创建一个 GDBusProxy 的简化函数
static GDBusProxy *
proxy_new(GDBusConnection *bus,
          const char      *path,
          const char      *interface,
          GError         **error)
{
    return g_dbus_proxy_new_sync(bus,
                                 G_DBUS_PROXY_FLAGS_NONE,
                                 NULL,
                                 NETWORK_MANAGER_DBUS_SERVER,
                                 path,
                                 interface,
                                 NULL,
                                 error);
}

// 查询 DHCP 状态：1=auto/DHCP, 0=manual/static, -1=error/no-active
int nm_dhcp_status(GDBusConnection *bus,
                   const char     *ifname,
                   GError        **error)
{
    GVariant     *reply;
    char         *dev_path = NULL;
    char         *ac_path  = NULL;
    char         *conn_path = NULL;
    GDBusProxy   *nm_proxy = NULL;
    GDBusProxy   *dev_proxy = NULL;
    GDBusProxy   *ac_proxy  = NULL;
    GDBusProxy   *conn_proxy = NULL;
    int           status = -1;

    nm_proxy = proxy_new(bus, NETWORK_MANAGER_DBUS_PATH, NETWORK_MANAGER_DBUS_INTERFACE, error);
    if (!nm_proxy) goto done;

    reply = g_dbus_proxy_call_sync(nm_proxy,
                                   "GetDeviceByIpIface",
                                   g_variant_new("(s)", ifname),
                                   G_DBUS_CALL_FLAGS_NONE,
                                   -1, NULL, error);
    g_object_unref(nm_proxy);
    if (!reply) goto done;
    g_variant_get(reply, "(o)", &dev_path);
    g_variant_unref(reply);

    dev_proxy = proxy_new(bus, dev_path, DEV_IFACE, error);
    if (!dev_proxy) goto done;
    GVariant *act = g_dbus_proxy_get_cached_property(dev_proxy, "ActiveConnection");
    g_object_unref(dev_proxy);
    if (!act || g_variant_is_of_type(act, G_VARIANT_TYPE("()"))) {
        g_clear_error(error);
        status = -1;
        goto done;
    }
    g_variant_get(act, "o", &ac_path);
    g_variant_unref(act);

    ac_proxy = proxy_new(bus, ac_path, AC_IFACE, error);
    g_free(ac_path);
    if (!ac_proxy) goto done;
    GVariant *conn = g_dbus_proxy_get_cached_property(ac_proxy, "Connection");
    g_object_unref(ac_proxy);
    if (!conn || g_variant_is_of_type(conn, G_VARIANT_TYPE("()"))) {
        g_clear_error(error);
        status = -1;
        goto done;
    }
    g_variant_get(conn, "o", &conn_path);
    g_variant_unref(conn);

    printf("conn_path:%s\n", conn_path);

    conn_proxy = proxy_new(bus, conn_path, CONN_IFACE, error);
    g_free(conn_path);
    if (!conn_proxy) goto done;
    reply = g_dbus_proxy_call_sync(conn_proxy,
                                   "GetSettings",
                                   NULL,
                                   G_DBUS_CALL_FLAGS_NONE,
                                   -1, NULL, error);
    g_object_unref(conn_proxy);
    if (!reply) goto done;

    GVariant *settings_dict = NULL;
    g_variant_get(reply, "(@a{sa{sv}})", &settings_dict);
    g_variant_unref(reply);
    if (!settings_dict) goto done;

    GVariant *ip4_dict = g_variant_lookup_value(settings_dict,
                                                "ipv4",
                                                G_VARIANT_TYPE("a{sv}"));
    g_variant_unref(settings_dict);
    if (!ip4_dict) {
        status = 0;
        goto done;
    }

    GVariant *method = g_variant_lookup_value(ip4_dict,
                                              "method",
                                              G_VARIANT_TYPE_STRING);
    g_variant_unref(ip4_dict);
    if (!method) {
        status = 0;
        goto done;
    }

    const char *m = g_variant_get_string(method, NULL);
    status = (g_strcmp0(m, "auto") == 0) ? 1 : 0;
    g_variant_unref(method);

done:
    g_free(dev_path);
    return status;
}

//获取DHCP状态
int network_manager_get_dhcp_status(const char *ifname)
{
	GError *error = NULL;
	GDBusConnection *bus = g_bus_get_sync(G_BUS_TYPE_SYSTEM, NULL, &error);
	if (!bus) {
		g_printerr("Bus error: %s\n", error->message);
		return -1;
	}
	int ret= nm_dhcp_status(bus,ifname,&error);
	g_clear_error(&error);
	g_object_unref(bus);
	return ret;
}


