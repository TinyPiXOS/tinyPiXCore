/*///------------------------------------------------------------------------------------------------------------------------//
		网卡管理(DBUS)Settings接口相关操作
说 明 : org.freedesktop.NetworkManager.Settings接口
		主要用于网络配置的object_path（object_path可以用于创建NmConnection），
日 期 : 2026.1.28

/*///------------------------------------------------------------------------------------------------------------------------//

#include <stdio.h>
#include "Network/nm/NetworkManagerPriv.h"
#include "Network/nm/NetworkManager.h"
#include "Network/nm/NmSettings.h"
#include "Network/nm/NmUtils.h"

static GDBusConnection *system_conn = NULL;

struct NmSettingsPrivate_{
	GDBusProxy *proxy;
};

//告诉glib自己的类型和私有结构体,前缀等
G_DEFINE_TYPE_WITH_PRIVATE(NmSettings, nm_settings, G_TYPE_OBJECT);

static void nm_settings_finalize(GObject *gobject);
static void nm_settings_dispose(GObject *gobject);

static void nm_settings_class_init(NmSettingsClass *klass)
{
    GObjectClass *gobject_class = G_OBJECT_CLASS(klass);
    gobject_class->dispose = nm_settings_dispose;
    gobject_class->finalize = nm_settings_finalize;
}

static void nm_settings_init(NmSettings *self)
{
    self->priv = nm_settings_get_instance_private(self);
    self->priv->proxy = NULL;
}

static void nm_settings_dispose(GObject *gobject)
{
    NmSettings *self = NM_SETTINGS(gobject);
    g_clear_object(&self->priv->proxy);
    G_OBJECT_CLASS(nm_settings_parent_class)->dispose(gobject);
}

static void nm_settings_finalize(GObject *gobject)
{
    // 如果有额外资源释放可以放这里
    G_OBJECT_CLASS(nm_settings_parent_class)->finalize(gobject);
}

//私有成员创建
static GDBusProxy *nm_settings_create_proxy(const gchar *path, GError **error)
{
    GDBusProxy *proxy = g_dbus_proxy_new_sync(
        system_conn,                               // system bus
        G_DBUS_PROXY_FLAGS_NONE,
        NULL,                               // introspection
        NETWORK_MANAGER_DBUS_SERVER,        // service
        path,            // object path
        NM_SETTINGS_INTERFACE,              // interface
        NULL,
        error
    );

    if (!proxy && error && *error) {
        g_warning("Failed to create NmSettings proxy: %s",(*error)->message);
        return NULL;
    }
    return proxy;
}

NmSettings *nm_settings_create(GDBusConnection *conn, GError **error)
{
    g_return_val_if_fail(conn != NULL, NULL);

	system_conn=conn;
    NmSettings *self = g_object_new(NM_SETTINGS_TYPE, NULL);

	self->priv->proxy = nm_settings_create_proxy(NM_SETTINGS_OBJECT_PATH, error);
    if (!self->priv->proxy) {
        g_object_unref(self);
        return NULL;
    }

    return self;
}

int nm_settings_delete(NmSettings *self)
{
	if(!self)
		return 0;

	g_object_unref(self);
	return 0;
}

//列出系统所有配置
GList *nm_settings_list_connections(NmSettings *self, GError **error)
{
    g_return_val_if_fail(self != NULL, NULL);
    g_return_val_if_fail(self->priv->proxy != NULL, NULL);

    GVariant *ret = g_dbus_proxy_call_sync(
        self->priv->proxy,
        "ListConnections",
        NULL,
        G_DBUS_CALL_FLAGS_NONE,
        -1,
        NULL,
        error);

    if (!ret) 
		return NULL;

    GVariantIter *iter;
    const char *path;
    GList *list = NULL;

    g_variant_get(ret, "(ao)", &iter);
    while (g_variant_iter_loop(iter, "o", &path)) {
        list = g_list_prepend(list, g_strdup(path));
    }
    g_variant_iter_free(iter);
    g_variant_unref(ret);

    return g_list_reverse(list);
}

//根据 配置名 查找 配置(connection)的object_path
char *nm_settings_find_connection_object(NmSettings *self, const char *conn_name, GError **error)
{
    g_return_val_if_fail(self != NULL, NULL);
    g_return_val_if_fail(conn_name != NULL, NULL);
    GList *conns = nm_settings_list_connections(self, error);
    if (!conns) 
		return NULL;

    for (GList *l = conns; l != NULL; l = l->next) 
	{
        char *path = l->data;

        GDBusProxy *proxy = g_dbus_proxy_new_sync(
            g_dbus_proxy_get_connection(self->priv->proxy),
            G_DBUS_PROXY_FLAGS_NONE,
            NULL,
            NETWORK_MANAGER_DBUS_SERVER,
            path,
            NM_CONNECTION_INTERFACE,
            NULL,
            error);

        if (!proxy)
            continue;

        /* GetSettings() -> (a{sa{sv}}) */
        GVariant *ret = g_dbus_proxy_call_sync(
            proxy,
            "GetSettings",
            NULL,
            G_DBUS_CALL_FLAGS_NONE,
            -1,
            NULL,
            error);

        if (!ret) {
            g_object_unref(proxy);
            continue;
        }

        /* ① 拆 tuple */
        GVariant *settings = NULL;
        g_variant_get(ret, "(@a{sa{sv}})", &settings);
        g_variant_unref(ret);

        if (!settings) {
            g_object_unref(proxy);
            continue;
        }

        /* ② 取 connection setting */
        GVariant *connection =
            g_variant_lookup_value(settings, "connection",
                                   G_VARIANT_TYPE("a{sv}"));

        if (connection) {
            /* ③ 取 id（名字） */
            GVariant *id =
                g_variant_lookup_value(connection, "id",
                                       G_VARIANT_TYPE_STRING);

            if (id) {
                const char *name = g_variant_get_string(id, NULL);
                if (g_strcmp0(name, conn_name) == 0) {
					char *ret_path = g_strdup(path);  
					g_variant_unref(id);
					g_variant_unref(connection);
					g_variant_unref(settings);
					g_object_unref(proxy);
					g_list_free_full(conns, g_free);
					return ret_path;
                }
                g_variant_unref(id);
            }
            g_variant_unref(connection);
        }

        g_variant_unref(settings);
        g_object_unref(proxy);
    }

    g_list_free_full(conns, g_free);
    return NULL;
}

//添加配置到settings，成功返回配置的connection_path,失败返回NULL，返回的path需要调用者负责 g_free
char *nm_settings_add_connection(NmSettings *self, GVariant *settings, GError **error)
{
    GVariant *ret;
    gchar *path = NULL;
	
    ret = g_dbus_proxy_call_sync(
        self->priv->proxy,
        "AddConnection",
		network_variant_tuple1(settings),
        G_DBUS_CALL_FLAGS_NONE,
        -1,
        NULL,
        error
    );

    if (!ret)
	{
		if (error && *error) {
        	g_printerr("AddConnection failed: %s\n", (*error)->message);
		}
		return NULL;
	}

    g_variant_get(ret, "(o)", &path);
    g_variant_unref(ret);

    return path;  // ⚠️ 调用者负责 g_free
}

