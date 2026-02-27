/*///------------------------------------------------------------------------------------------------------------------------//
		iwd WIFI网络实现
说 明 : 每个Networ对应一个wifi网络（SSID）
日 期 : 2026.2.5

/*///------------------------------------------------------------------------------------------------------------------------//

#include "Network/iwd/Iwd.h"
#include "Network/iwd/IwdNetwork.h"
#include <string.h>

static GDBusConnection *system_conn = NULL;
// 私有结构体
struct IwdNetworkPrivate_ {
    GDBusProxy *proxy;
    gchar *path;
};

G_DEFINE_TYPE_WITH_PRIVATE(IwdNetwork, iwd_network, G_TYPE_OBJECT);

// 私有方法：创建代理
static GDBusProxy* _network_create_proxy(const gchar *path, GError **error)
{
    GDBusProxy *proxy = g_dbus_proxy_new_sync(
        system_conn,
        G_DBUS_PROXY_FLAGS_NONE,
        NULL,  // interface info
        IWD_DBUS_SERVICE,
        path,
        IWD_NETWORK_INTERFACE,
        NULL,  // cancellable
        error);
    
    if (!proxy && *error) {
        g_printerr("Failed to create network proxy for %s: %s\n", 
                  path, (*error)->message);
    }
    
    return proxy;
}

// 对象生命周期管理
static void iwd_network_dispose(GObject *gobject)
{
    IwdNetwork *self = IWD_NETWORK(gobject);
    
    g_clear_object(&self->priv->proxy);
    g_clear_pointer(&self->priv->path, g_free);
    
    G_OBJECT_CLASS(iwd_network_parent_class)->dispose(gobject);
}

static void iwd_network_finalize(GObject *gobject)
{
    G_OBJECT_CLASS(iwd_network_parent_class)->finalize(gobject);
}

static void iwd_network_class_init(IwdNetworkClass *klass)
{
    GObjectClass *gobject_class = G_OBJECT_CLASS(klass);
    gobject_class->dispose = iwd_network_dispose;
    gobject_class->finalize = iwd_network_finalize;
}

static void iwd_network_init(IwdNetwork *self)
{
    self->priv = iwd_network_get_instance_private(self);
    self->priv->proxy = NULL;
    self->priv->path = NULL;
}

// 公共方法实现
IwdNetwork* iwd_network_create(GDBusConnection *conn, const char *path, GError **error)
{
    g_return_val_if_fail(path != NULL, NULL);
    system_conn = conn;

    IwdNetwork *self = g_object_new(IWD_NETWORK_TYPE, NULL);
    self->priv->path = g_strdup(path);
    
    self->priv->proxy = _network_create_proxy(path, error);
    if (!self->priv->proxy) {
        g_object_unref(self);
        return NULL;
    }
    
    return self;
}

int iwd_network_delete(IwdNetwork *network)
{
    if (network) {
        g_object_unref(network);
    }
}

// 连接到网络
// password:密码
// timeout:连接超时时间，单位ms
bool iwd_network_connect(IwdNetwork *network, 
                              const gchar *password, 
							  int timeoutms,
                              GError **error)
{
    g_return_val_if_fail(IWD_NETWORK_IS(network), FALSE);
    
    GVariant *parameters = NULL;
    
    if (password && password[0] != '\0') {
        parameters = g_variant_new("(s)", password);
    } else {
        parameters = g_variant_new("(s)", "");
    }
    
    GVariant *result = g_dbus_proxy_call_sync(
        network->priv->proxy,
        "Connect",
        parameters,
        G_DBUS_CALL_FLAGS_NONE,
        timeoutms,  // 超时时间,单位ms
        NULL,   // cancellable
        error);
    
    if (!result) {
        g_printerr("Failed to connect: %s\n", (*error)->message);
        return false;
    }
    
    g_variant_unref(result);
    return true;
}

// 断开网络
bool iwd_network_disconnect(IwdNetwork *network, int timeout,GError **error)
{
    g_return_val_if_fail(IWD_NETWORK_IS(network), FALSE);
    
    GVariant *result = g_dbus_proxy_call_sync(
        network->priv->proxy,
        "Disconnect",
        NULL,  // parameters
        G_DBUS_CALL_FLAGS_NONE,
        timeout,  // 10秒超时
        NULL,   // cancellable
        error);
    
    if (!result) {
        g_printerr("Failed to disconnect: %s\n", (*error)->message);
        return false;
    }
    
    g_variant_unref(result);
    return true;
}

// 获取网络名称
char* iwd_network_get_name(IwdNetwork *network, GError **error)
{
    g_return_val_if_fail(IWD_NETWORK_IS(network), NULL);
    
    GVariant *value = g_dbus_proxy_get_cached_property(network->priv->proxy, "Name");
    
    if (!value) {
        g_set_error(error, G_IO_ERROR, G_IO_ERROR_FAILED,
                   "Failed to get Name property");
        return NULL;
    }
    
    const gchar *name = g_variant_get_string(value, NULL);
    char *result = strdup(name);
    g_variant_unref(value);
    
    return result;
}

// 获取网络类型
char* iwd_network_get_type_str(IwdNetwork *network, GError **error)
{
    g_return_val_if_fail(IWD_NETWORK_IS(network), NULL);
    
    GVariant *value = g_dbus_proxy_get_cached_property(network->priv->proxy, "Type");
    
    if (!value) {
        g_set_error(error, G_IO_ERROR, G_IO_ERROR_FAILED, "Failed to get Type property");
        return NULL;
    }
    
    const gchar *type = g_variant_get_string(value, NULL);
    char *result = strdup(type);
    g_variant_unref(value);
    
    return result;
}

// 获取连接状态
bool iwd_network_get_connected(IwdNetwork *network, GError **error)
{
    g_return_val_if_fail(IWD_NETWORK_IS(network), FALSE);
    
    GVariant *value = g_dbus_proxy_get_cached_property(
        network->priv->proxy, "Connected");
    
    if (!value) {
        g_set_error(error, G_IO_ERROR, G_IO_ERROR_FAILED,
                   "Failed to get Connected property");
        return FALSE;
    }
    
    bool connected = g_variant_get_boolean(value);
    g_variant_unref(value);
    
    return connected;
}

// 获取信号强度
int iwd_network_get_signal_strength(IwdNetwork *network, GError **error)
{
    g_return_val_if_fail(IWD_NETWORK_IS(network), -1);
    
    GVariant *value = g_dbus_proxy_get_cached_property(network->priv->proxy, "SignalStrength");
    
    if (!value) {
        g_set_error(error, G_IO_ERROR, G_IO_ERROR_FAILED, "Failed to get SignalStrength property");
        return -1;
    }
    
    int strength = g_variant_get_int32(value);
    g_variant_unref(value);
    
    return strength;
}