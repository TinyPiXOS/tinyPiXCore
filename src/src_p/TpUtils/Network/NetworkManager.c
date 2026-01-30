/*///------------------------------------------------------------------------------------------------------------------------//
		系统级DBUS接口相关接口
说 明 : org.freedesktop.NetworkManager接口,后续网卡相关管理逐步切换到此处
日 期 : 2025.6.9
作 者 : Chingan

/*///------------------------------------------------------------------------------------------------------------------------//

#include <stdio.h>
#include <string.h>
#include <arpa/inet.h>
#include "Network/NmDevice.h"
#include "Network/NetworkManagerPriv.h"
#include "Network/NetworkManager.h"

static GDBusConnection *system_conn = NULL;
static GDBusProxy *network_manager_create_gdbus_proxy(NetworkManager *self, const gchar *dbus_interface, GError **error);
static void network_manager_dispose(GObject *gobject);

struct NetworkManagerPrivate_{
	GDBusProxy *proxy;
	gchar *object_path;
//	GDBusConnection *system_conn;
};

//告诉glib自己的类型和私有结构体
G_DEFINE_TYPE_WITH_PRIVATE(NetworkManager, network_manager, G_TYPE_OBJECT);	

//第二步
static void network_manager_class_init (NetworkManagerClass *klass)
{
    GObjectClass *gobject_class = G_OBJECT_CLASS(klass);
    gobject_class->dispose = network_manager_dispose;
}

//初始化，会被自动调用，第三步
static void network_manager_init (NetworkManager *self)
{
	self->priv = network_manager_get_instance_private (self);
	self->priv->proxy = NULL;
//	g_assert(system_conn != NULL);
	GError *error = NULL;

	//也可以放在用户调用的new中创建私有
	self->priv->proxy=network_manager_create_gdbus_proxy(self, NETWORK_MANAGER_DBUS_INTERFACE, &error);

	g_assert(error == NULL);
}

static void network_manager_dispose(GObject *gobject)
{
    NetworkManager *self = NETWORK_MANAGER(gobject);

    g_clear_object(&self->priv->proxy);
    g_clear_pointer(&self->priv->object_path, g_free);

    G_OBJECT_CLASS(network_manager_parent_class)->dispose(gobject);
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
	if (!proxy && *error)
	{
		g_printerr("Failed to create proxy for %s: %s\n",dbus_interface, (*error)->message);
		return NULL;
	}
	return proxy;
}

//glib第一步调用
NetworkManager *network_manager_create(GDBusConnection *conn)
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


//获取NmConnection的object_path
char *network_manager_get_connections(NetworkManager *self,GError *error)
{
	GVariant *ret = g_dbus_proxy_call_sync(
		self->priv->proxy,           // NetworkManager proxy
		"ListConnections",
		NULL,                      // no args
		G_DBUS_CALL_FLAGS_NONE,
		-1,
		NULL,
		&error
	);

	if (!ret) {
		g_printerr("ListConnections failed: %s\n", error->message);
		return NULL;
	}

	GVariantIter *iter;
	GVariant *child;
	g_variant_get(ret, "(ao)", &iter);
	while (g_variant_iter_loop(iter, "o", &child)) {
		const char *connection_path = g_variant_get_string(child, NULL);
		g_print("Found connection: %s\n", connection_path);
	}
	g_variant_iter_free(iter);
	g_variant_unref(ret);
	return NULL;
}

//根据设备名获取object_path
//iface_name：网卡设备名
//返回值:object_path，可以用于创建NmDevice句柄
char *network_manager_get_device_path_by_iface_fallback(NetworkManager *self, const char *iface_name, GError **error)
{
    g_return_val_if_fail(NETWORK_MANAGER_IS(self), NULL);
    g_return_val_if_fail(iface_name != NULL, NULL);

    GVariant *ret = g_dbus_proxy_call_sync(
        self->priv->proxy,
        "GetDevices",
        NULL,
        G_DBUS_CALL_FLAGS_NONE,
        -1,
        NULL,
        error
    );

    if (!ret)
        return NULL;

    GVariantIter *iter;
    g_variant_get(ret, "(ao)", &iter);
    const gchar *path;
    char *found_path = NULL;

    while (g_variant_iter_loop(iter, "o", &path)) {
        GError *err = NULL;
        GDBusProxy *dev_proxy = g_dbus_proxy_new_sync(
            system_conn,
            G_DBUS_PROXY_FLAGS_NONE,
            NULL,                                                        
            NETWORK_MANAGER_DBUS_SERVER,
            path,
            NM_DEVICE_INTERFACE,
            NULL,
            &err
        );

        if (!dev_proxy || err) {
            g_clear_error(&err);
            continue;
        }

        GVariant *iface_var = g_dbus_proxy_get_cached_property(dev_proxy, "Interface");
        if (iface_var) {
            const gchar *dev_iface = g_variant_get_string(iface_var, NULL);
            if (g_strcmp0(dev_iface, (const gchar*)iface_name) == 0) {
                found_path = g_strdup(path);
                g_variant_unref(iface_var);
                g_object_unref(dev_proxy);
                break;
            }
            g_variant_unref(iface_var);
        }

        g_object_unref(dev_proxy);
    }

    g_variant_iter_free(iter);
    g_variant_unref(ret);
    return found_path; // 外部使用完记得 g_free()
}


char *network_manager_get_device_path_by_iface(NetworkManager *self,
    									const char *iface_name,
										GError **error)
{
    g_return_val_if_fail(NETWORK_MANAGER_IS(self), NULL);
    g_return_val_if_fail(iface_name != NULL, NULL);

    GVariant *ret = g_dbus_proxy_call_sync(
        self->priv->proxy,
        "GetDeviceByIpIface",
        g_variant_new("(s)", iface_name),
        G_DBUS_CALL_FLAGS_NONE,
        -1,
        NULL,
        error
    );

    if (!ret)
        return NULL;

    const char *path = NULL;
    g_variant_get(ret, "(&o)", &path);

    char *out = g_strdup(path);  // 调用者释放
    g_variant_unref(ret);
    return out;
}

void network_manager_free_device_path(char *device)
{
	g_free(device);
}


// 将配置应用到指定网卡上，返回 0 成功
//conn：要激活的配置
//dev：要应用的网卡
int network_manager_activate_connection_to_device(NetworkManager *self,
                                        NmConnection *conn,
                                        NmDevice *dev,
                                        GError **error)
{
	g_return_val_if_fail(NETWORK_MANAGER_IS(self), -1);

	GDBusProxy *c_proxy = nm_connection_get_proxy_internal(conn);
	GDBusProxy *d_proxy = nm_device_get_proxy_internal(dev);
	
	if(!c_proxy || d_proxy)
		return -1;
    // 调用 ActivateConnection(Connection, Device, SpecificObjectPath)
	g_dbus_proxy_call_sync(
		self->priv->proxy,
		"ActivateConnection",
		g_variant_new("(ooo)", c_proxy, d_proxy, NULL),
		G_DBUS_CALL_FLAGS_NONE,
		-1,
		NULL,
		error
	);
    return 0;
}

/**
 * network_manager_is_connection_active:
 * @nm: NetworkManager 对象
 * @conn_name: 要检查的配置名
 *
 * 返回值:
 *   1 已激活
 *   0 未激活
 *  -1 出错
 */
int network_manager_is_connection_active(NetworkManager *self, const char *conn_name)
{
    g_return_val_if_fail(NETWORK_MANAGER_IS(self), -1);
    g_return_val_if_fail(conn_name != NULL, -1);

    GError *error = NULL;

    GVariant *ret = g_dbus_proxy_call_sync(
        self->priv->proxy,
        "Get",
        g_variant_new("(ss)",
		"org.freedesktop.NetworkManager",
		"ActiveConnections"),
        G_DBUS_CALL_FLAGS_NONE,
        -1,
        NULL,
        &error);

    if (!ret) {
        g_clear_error(&error);
        return -1;
    }

    GVariant *v;
    g_variant_get(ret, "(@v)", &v);
    GVariant *acs = g_variant_get_variant(v);
    g_variant_unref(v);
    g_variant_unref(ret);

    GVariantIter iter;
    const char *ac_path;

    g_variant_iter_init(&iter, acs);
    while (g_variant_iter_loop(&iter, "o", &ac_path)) {

        GDBusProxy *ac = g_dbus_proxy_new_sync(
			system_conn,
			G_DBUS_PROXY_FLAGS_NONE,
			NULL,
			NETWORK_MANAGER_DBUS_SERVER,
			ac_path,
			"org.freedesktop.NetworkManager.Connection.Active",
			NULL,
			NULL);

        if (!ac)
            continue;

        GVariant *idv = g_dbus_proxy_call_sync(
			ac,
			"Get",
			g_variant_new("(ss)",
			"org.freedesktop.NetworkManager.Connection.Active",
			"Id"),
			G_DBUS_CALL_FLAGS_NONE,
			-1,
			NULL,
			NULL);

        if (idv) {
            GVariant *iv;
            g_variant_get(idv, "(@v)", &iv);
            const char *id = g_variant_get_string(
                g_variant_get_variant(iv), NULL);

            if (g_strcmp0(id, conn_name) == 0) {
                g_variant_unref(iv);
                g_variant_unref(idv);
                g_object_unref(ac);
                g_variant_unref(acs);
                return 1;
            }

            g_variant_unref(iv);
            g_variant_unref(idv);
        }

        g_object_unref(ac);
    }

    g_variant_unref(acs);
    return 0;
}
