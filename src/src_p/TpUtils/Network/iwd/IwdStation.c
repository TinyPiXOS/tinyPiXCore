/*///------------------------------------------------------------------------------------------------------------------------//
		iwd网络站点实现
说 明 : 每个站点对应一个无线网卡
日 期 : 2026.2.5

/*///------------------------------------------------------------------------------------------------------------------------//

#include "Network/iwd/Iwd.h"
#include "Network/iwd/IwdStation.h"
#include <string.h>

static GDBusConnection *system_conn = NULL;

// 私有结构体
struct IwdStationPrivate_ {
    GDBusProxy *proxy;
    gchar *path;
};

G_DEFINE_TYPE_WITH_PRIVATE(IwdStation, iwd_station, G_TYPE_OBJECT);

// 私有方法：创建代理
static GDBusProxy* _station_create_proxy(const gchar *path, GError **error)
{
    GDBusProxy *proxy = g_dbus_proxy_new_sync(
        system_conn,
        G_DBUS_PROXY_FLAGS_NONE,
        NULL,  // interface info
        IWD_DBUS_SERVICE,
        path,
        IWD_STATION_INTERFACE,
        NULL,  // cancellable
        error);
    
    if (!proxy && *error) {
        g_printerr("Failed to create station proxy for %s: %s\n", 
                  path, (*error)->message);
    }
    
    return proxy;
}

// 对象生命周期管理
static void iwd_station_dispose(GObject *gobject)
{
    IwdStation *self = IWD_STATION(gobject);
    
    g_clear_object(&self->priv->proxy);
    g_clear_pointer(&self->priv->path, g_free);
    
    G_OBJECT_CLASS(iwd_station_parent_class)->dispose(gobject);
}

static void iwd_station_finalize(GObject *gobject)
{
    G_OBJECT_CLASS(iwd_station_parent_class)->finalize(gobject);
}

static void iwd_station_class_init(IwdStationClass *klass)
{
    GObjectClass *gobject_class = G_OBJECT_CLASS(klass);
    gobject_class->dispose = iwd_station_dispose;
    gobject_class->finalize = iwd_station_finalize;
}

static void iwd_station_init(IwdStation *self)
{
    self->priv = iwd_station_get_instance_private(self);
    self->priv->proxy = NULL;
    self->priv->path = NULL;
}

// 公共方法实现
IwdStation* iwd_station_create(GDBusConnection *conn, const char *path, GError **error)
{
    g_return_val_if_fail(path != NULL, NULL);
    system_conn = conn;

    IwdStation *self = g_object_new(IWD_STATION_TYPE, NULL);
    self->priv->path = g_strdup(path);
    
    self->priv->proxy = _station_create_proxy(path, error);
    if (!self->priv->proxy) {
        g_object_unref(self);
        return NULL;
    }
    
    return self;
}

int iwd_station_delete(IwdStation *station)
{
    if (station) {
        g_object_unref(station);
    }
}

// 开始扫描网络（此接口暂时不用，无法实时获取，直接使用ioctl的方式）
int iwd_station_scan(IwdStation *station, GError **error)
{
	g_return_val_if_fail(IWD_STATION_IS(station), FALSE);

	GVariant *result = g_dbus_proxy_call_sync(
		station->priv->proxy,
		"Scan",
		NULL,  // parameters
		G_DBUS_CALL_FLAGS_NONE,
		5000,  // timeout in milliseconds
		NULL,
		error);
	if (result) {
        g_variant_unref(result);
    }
	return 0;
}

// 获取有序网络列表,不管是同步还是异步，都要等完成才能获取列表
GPtrArray* iwd_station_get_ordered_networks(IwdStation *station, GError **error)
{
    g_return_val_if_fail(IWD_STATION_IS(station), NULL);
    
    GVariant *result = g_dbus_proxy_call_sync(
        station->priv->proxy,
        "GetOrderedNetworks",
        NULL,  // parameters
        G_DBUS_CALL_FLAGS_NONE,
        1000,  // 1秒超时
        NULL,   // cancellable
        error);
    
    if (!result) {
        g_printerr("Failed to get ordered networks: %s\n", (*error)->message);
        return NULL;
    }
    
    GPtrArray *networks = g_ptr_array_new_with_free_func(g_free);
    GVariantIter iter;
    gchar *path;
    
    g_variant_get(result, "(ao)", &iter);
    while (g_variant_iter_next(&iter, "o", &path)) {
        g_ptr_array_add(networks, g_strdup(path));
    }
    
    g_variant_unref(result);
    return networks;
}

// 连接到指定网络
int iwd_station_connect(IwdStation *station, 
                        const gchar *network_path, 
                        GError **error)
{
    g_return_val_if_fail(IWD_STATION_IS(station), FALSE);
    g_return_val_if_fail(network_path != NULL, FALSE);
    
    GVariant *parameters = g_variant_new("(o)", network_path);
    GVariant *result = g_dbus_proxy_call_sync(
        station->priv->proxy,
        "Connect",
        parameters,
        G_DBUS_CALL_FLAGS_NONE,
        30000,  // 30秒超时
        NULL,   // cancellable
        error);
    
    if (!result) {
        g_printerr("Failed to connect to network: %s\n", (*error)->message);
        return -1;
    }
    
    g_variant_unref(result);
    return 0;
}

// 断开连接
int iwd_station_disconnect(IwdStation *station, GError **error)
{
    g_return_val_if_fail(IWD_STATION_IS(station), FALSE);
    
    GVariant *result = g_dbus_proxy_call_sync(
        station->priv->proxy,
        "Disconnect",
        NULL,  // parameters
        G_DBUS_CALL_FLAGS_NONE,
        10000,  // 10秒超时
        NULL,   // cancellable
        error);
    
    if (!result) {
        g_printerr("Failed to disconnect: %s\n", (*error)->message);
        return -1;
    }
    
    g_variant_unref(result);
    return 0;
}

// 获取连接状态
//返回值如下：
//"disconnected"​	未连接任何网络（空闲状态）
//"connected"​		已成功连接并获取了IP地址
//"connecting"​		正在连接过程中（认证、获取IP等）
//"disconnecting"​	正在断开连接
//"roaming"​		在同一ESSID内的不同AP间漫游
char* iwd_station_get_state(IwdStation *station, GError **error)
{
    g_return_val_if_fail(IWD_STATION_IS(station), NULL);
    
    GVariant *value = g_dbus_proxy_get_cached_property(station->priv->proxy, "State");
    
    if (!value) {
        g_set_error(error, G_IO_ERROR, G_IO_ERROR_FAILED, "Failed to get State property");
        return NULL;
    }
    
    const gchar *state = g_variant_get_string(value, NULL);
    char *result = strdup(state);
    g_variant_unref(value);
    
    return result;
}

// 获取已连接网络
char* iwd_station_get_connected_network(IwdStation *station, GError **error)
{
    g_return_val_if_fail(IWD_STATION_IS(station), NULL);
    
    GVariant *value = g_dbus_proxy_get_cached_property(station->priv->proxy, "ConnectedNetwork");
    
    if (!value) {
        g_set_error(error, G_IO_ERROR, G_IO_ERROR_FAILED, "Failed to get ConnectedNetwork property");
        return NULL;
    }
    
    const gchar *path = g_variant_get_string(value, NULL);
    char *result = strdup(path);
    g_variant_unref(value);
    
    return result;
}

// 获取站点名称
char* iwd_station_get_name(IwdStation *station, GError **error)
{
    g_return_val_if_fail(IWD_STATION_IS(station), NULL);
    
    GVariant *value = g_dbus_proxy_get_cached_property(station->priv->proxy, "Name");
    
    if (!value) {
        g_set_error(error, G_IO_ERROR, G_IO_ERROR_FAILED, "Failed to get Name property");
        return NULL;
    }
    
    const gchar *name = g_variant_get_string(value, NULL);
    char *result = strdup(name);
    g_variant_unref(value);
    
    return result;
}

// 获取扫描状态
gboolean iwd_station_is_scanning(IwdStation *station, GError **error)
{
	g_return_val_if_fail(IWD_STATION_IS(station), FALSE);

	GVariant *value = g_dbus_proxy_get_cached_property(
		station->priv->proxy, "Scanning");

	if (!value) {
		g_set_error(error, G_IO_ERROR, G_IO_ERROR_FAILED, "Failed to get Scanning property");
		return FALSE;
	}

	gboolean is_scanning = g_variant_get_boolean(value);
	g_variant_unref(value);

	return is_scanning;
}