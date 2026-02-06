/*///------------------------------------------------------------------------------------------------------------------------//
		IWD管理(DBUS)Manager接口相关操作
说 明 : 
日 期 : 2026.2.5

/*///------------------------------------------------------------------------------------------------------------------------//


#include <string.h>
#include "Network/iwd/Iwd.h"
#include "Network/iwd/IwdManager.h"
#include "Network/iwd/IwdNetwork.h"
#include "Network/iwd/IwdStation.h"

// 私有结构体
struct IwdManagerPrivate_ {
    GDBusConnection *connection;
    GDBusProxy *manager_proxy;
    GHashTable *stations;  // 路径 -> IwdStation*
    GHashTable *networks;  // 路径 -> IwdNetwork*
};

// 告诉 GLib 自己的类型和私有结构体
G_DEFINE_TYPE_WITH_PRIVATE(IwdManager, iwd_manager, G_TYPE_OBJECT);

// 静态全局连接
static GDBusConnection *system_conn = NULL;

// 工具函数：获取字符串属性
static gchar* _get_string_property(GDBusProxy *proxy, 
                                   const gchar *interface_name,
                                   const gchar *property_name,
                                   GError **error)
{
    GVariant *value = g_dbus_proxy_get_cached_property(proxy, property_name);
    if (!value) {
        g_set_error(error, G_IO_ERROR, G_IO_ERROR_FAILED, "Failed to get property %s", property_name);
        return NULL;
    }
    
    const gchar *str = g_variant_get_string(value, NULL);
    gchar *result = g_strdup(str);
    g_variant_unref(value);
    
    return result;
}

// 工具函数：获取布尔属性
static gboolean _get_boolean_property(GDBusProxy *proxy,
                                       const gchar *interface_name,
                                       const gchar *property_name,
                                       GError **error)
{
    GVariant *value = g_dbus_proxy_get_cached_property(proxy, property_name);
    if (!value) {
        g_set_error(error, G_IO_ERROR, G_IO_ERROR_FAILED, "Failed to get property %s", property_name);
        return FALSE;
    }
    
    gboolean result = g_variant_get_boolean(value);
    g_variant_unref(value);
    
    return result;
}

// 工具函数：获取整型属性
static gint _get_int32_property(GDBusProxy *proxy,
                                const gchar *interface_name,
                                const gchar *property_name,
                                GError **error)
{
    GVariant *value = g_dbus_proxy_get_cached_property(proxy, property_name);
    if (!value) {
        g_set_error(error, G_IO_ERROR, G_IO_ERROR_FAILED, "Failed to get property %s", property_name);
        return -1;
    }
    
    gint result = g_variant_get_int32(value);
    g_variant_unref(value);
    
    return result;
}

// 工具函数：调用 D-Bus 方法
static GVariant* _call_method(GDBusProxy *proxy,
                              const gchar *method_name,
                              GVariant *parameters,
                              GError **error)
{
    return g_dbus_proxy_call_sync(proxy,
                                  method_name,
                                  parameters,
                                  G_DBUS_CALL_FLAGS_NONE,
                                  30000,  // 30秒超时
                                  NULL,   // cancellable
                                  error);
}

// 工具函数：创建管理器代理
static GDBusProxy* _create_manager_proxy(GError **error)
{
    GDBusProxy *proxy = g_dbus_proxy_new_sync(
        system_conn,
        G_DBUS_PROXY_FLAGS_NONE,
        NULL,  // GDBusInterfaceInfo
        IWD_DBUS_SERVICE,
        IWD_MANAGER_PATH,
        IWD_MANAGER_INTERFACE,
        NULL,  // GCancellable
        error);
    
    if (!proxy && *error) {
        g_printerr("Failed to create iwd manager proxy: %s\n", (*error)->message);
    }
    
    return proxy;
}

// 对象析构函数
static void iwd_manager_dispose(GObject *gobject)
{
    IwdManager *self = IWD_MANAGER(gobject);
    IwdManagerPrivate *priv = self->priv;
    
    g_clear_object(&priv->manager_proxy);
    
    if (priv->stations) {
        g_hash_table_destroy(priv->stations);
        priv->stations = NULL;
    }
    
    if (priv->networks) {
        g_hash_table_destroy(priv->networks);
        priv->networks = NULL;
    }
    
    G_OBJECT_CLASS(iwd_manager_parent_class)->dispose(gobject);
}

static void iwd_manager_finalize(GObject *gobject)
{
    // 清理额外资源
    G_OBJECT_CLASS(iwd_manager_parent_class)->finalize(gobject);
}

static void iwd_manager_class_init(IwdManagerClass *klass)
{
    GObjectClass *gobject_class = G_OBJECT_CLASS(klass);
    gobject_class->dispose = iwd_manager_dispose;
    gobject_class->finalize = iwd_manager_finalize;
}

static void iwd_manager_init(IwdManager *self)
{
    self->priv = iwd_manager_get_instance_private(self);
    self->priv->connection = NULL;
    self->priv->manager_proxy = NULL;
    self->priv->stations = g_hash_table_new_full(g_str_hash, g_str_equal, g_free, g_object_unref);
    self->priv->networks = g_hash_table_new_full(g_str_hash, g_str_equal, g_free, g_object_unref);
}

// 公共方法实现
IwdManager* iwd_manager_create(GDBusConnection *conn, GError **error)
{
    // 确保 D-Bus 连接存在
	system_conn = conn;
    
    // 创建管理器对象
    IwdManager *self = g_object_new(IWD_MANAGER_TYPE, NULL);
    self->priv->connection = g_object_ref(system_conn);
    
    // 创建管理器代理
    self->priv->manager_proxy = _create_manager_proxy(error);
    if (!self->priv->manager_proxy) {
        g_object_unref(self);
        return NULL;
    }
    
    return self;
}

int iwd_manager_delete(IwdManager *manager)
{
    if (manager) {
        g_object_unref(manager);
    }
}

// 获取所有适配器
GPtrArray* iwd_manager_get_adapters(IwdManager *manager, GError **error)
{
    g_return_val_if_fail(IWD_MANAGER_IS(manager), NULL);
    
    GVariant *result = _call_method(manager->priv->manager_proxy,
                                    "GetAdapters",
                                    NULL,
                                    error);
    if (!result) {
        return NULL;
    }
    
    GPtrArray *adapters = g_ptr_array_new_with_free_func(g_free);
    GVariantIter iter;
    gchar *path;
    
    g_variant_get(result, "(ao)", &iter);
    while (g_variant_iter_next(&iter, "o", &path)) {
        g_ptr_array_add(adapters, g_strdup(path));
    }
    
    g_variant_unref(result);
    return adapters;
}

// 获取所有设备
GPtrArray* iwd_manager_get_devices(IwdManager *manager, GError **error)
{
    g_return_val_if_fail(IWD_MANAGER_IS(manager), NULL);
    
    GVariant *result = _call_method(manager->priv->manager_proxy,
                                    "GetDevices",
                                    NULL,
                                    error);
    if (!result) {
        return NULL;
    }
    
    GPtrArray *devices = g_ptr_array_new_with_free_func(g_free);
    GVariantIter iter;
    gchar *path;
    
    g_variant_get(result, "(ao)", &iter);
    while (g_variant_iter_next(&iter, "o", &path)) {
        g_ptr_array_add(devices, g_strdup(path));
    }
    
    g_variant_unref(result);
    return devices;
}

// 获取所有站点
GPtrArray* iwd_manager_get_stations(IwdManager *manager, GError **error)
{
    g_return_val_if_fail(IWD_MANAGER_IS(manager), NULL);
    
    GVariant *result = _call_method(manager->priv->manager_proxy,
                                    "GetStations",
                                    NULL,
                                    error);
    if (!result) {
        return NULL;
    }
    
    GPtrArray *stations = g_ptr_array_new_with_free_func(g_free);
    GVariantIter iter;
    gchar *path;
    
    g_variant_get(result, "(ao)", &iter);
    while (g_variant_iter_next(&iter, "o", &path)) {
        g_ptr_array_add(stations, g_strdup(path));
    }
    
    g_variant_unref(result);
    return stations;
}

// 扫描 WiFi 网络
GPtrArray* iwd_manager_scan_networks(IwdManager *manager, GError **error)
{
    g_return_val_if_fail(IWD_MANAGER_IS(manager), NULL);
    
    // 获取第一个站点
    GPtrArray *station_paths = iwd_manager_get_stations(manager, error);
    if (!station_paths || station_paths->len == 0) {
        g_set_error(error, G_IO_ERROR, G_IO_ERROR_NOT_FOUND, "No station found");
        if (station_paths) 
			g_ptr_array_free(station_paths, TRUE);
        return NULL;
    }
    
    gchar *station_path = g_ptr_array_index(station_paths, 0);
    IwdStation *station = iwd_station_create(system_conn, station_path, error);
    g_ptr_array_free(station_paths, TRUE);
    
    if (!station) {
        return NULL;
    }
    
    // 执行扫描
    if (!iwd_station_scan(station, NULL, NULL, error)) {
        iwd_station_delete(station);
        return NULL;
    }
    
    // 等待扫描完成
    g_usleep(3000000);  // 3秒
    
    // 获取网络列表
    GPtrArray *networks = iwd_station_get_ordered_networks(station, error);
    iwd_station_delete(station);
    
    if (!networks) {
        return NULL;
    }
    
    // 转换为 IwdNetworkInfo 数组
    GPtrArray *network_infos = g_ptr_array_new_with_free_func(
        (GDestroyNotify)iwd_network_info_free);
    
    for (guint i = 0; i < networks->len; i++) 
	{
        gchar *network_path = g_ptr_array_index(networks, i);
        IwdNetwork *network = iwd_network_create(system_conn,network_path, NULL);
        
        if (network) 
		{
            IwdNetworkInfo *info = g_new0(IwdNetworkInfo, 1);
            info->path = g_strdup(network_path);
            info->ssid = iwd_network_get_name(network, NULL);
            info->signal_strength = iwd_network_get_signal_strength(network, NULL);
            info->is_connected = iwd_network_get_connected(network, NULL);
            
            g_ptr_array_add(network_infos, info);
            iwd_network_delete(network);
        }
    }
    
    g_ptr_array_free(networks, TRUE);
    return network_infos;
}

// 连接到指定 WiFi
gboolean iwd_manager_connect_to_network(IwdManager *manager,
                                        const gchar *ssid,
                                        const gchar *password,
										int timeout,
                                        GError **error)
{
    g_return_val_if_fail(IWD_MANAGER_IS(manager), FALSE);
    g_return_val_if_fail(ssid != NULL, FALSE);
    
    // 首先扫描网络
    GPtrArray *networks = iwd_manager_scan_networks(manager, error);
    if (!networks) {
        return FALSE;
    }
    
    // 查找目标网络
    gchar *target_path = NULL;
    for (guint i = 0; i < networks->len; i++) {
        IwdNetworkInfo *info = g_ptr_array_index(networks, i);
        if (g_strcmp0(info->ssid, ssid) == 0) {
            target_path = g_strdup(info->path);
            break;
        }
    }
    
    g_ptr_array_free(networks, TRUE);
    
    if (!target_path) {
        g_set_error(error, G_IO_ERROR, G_IO_ERROR_NOT_FOUND, "Network '%s' not found", ssid);
        return FALSE;
    }
    
    // 创建网络对象
    IwdNetwork *network = iwd_network_create(system_conn,target_path, error);
    g_free(target_path);
    
    if (!network) {
        return FALSE;
    }
    
    // 连接到网络
    gboolean success = iwd_network_connect(network, password, timeout, error);
    iwd_network_delete(network);
    
    return success;
}

// 断开当前连接
gboolean iwd_manager_disconnect(IwdManager *manager, GError **error)
{
    g_return_val_if_fail(IWD_MANAGER_IS(manager), FALSE);
    
    // 获取站点
    GPtrArray *station_paths = iwd_manager_get_stations(manager, error);
    if (!station_paths || station_paths->len == 0) {
        if (station_paths) 
			g_ptr_array_free(station_paths, TRUE);
        g_set_error(error, G_IO_ERROR, G_IO_ERROR_NOT_FOUND, "No station found");
        return FALSE;
    }
    
    gchar *station_path = g_ptr_array_index(station_paths, 0);
    IwdStation *station = iwd_station_create(system_conn, station_path, error);
    g_ptr_array_free(station_paths, TRUE);
    
    if (!station) {
        return FALSE;
    }
    
    // 断开连接
    gboolean success = iwd_station_disconnect(station, error);
    iwd_station_delete(station);
    
    return success;
}

// 获取当前连接的 SSID
char* iwd_manager_get_connected_ssid(IwdManager *manager, GError **error)
{
    g_return_val_if_fail(IWD_MANAGER_IS(manager), NULL);
    
    // 获取站点
    GPtrArray *station_paths = iwd_manager_get_stations(manager, error);
    if (!station_paths || station_paths->len == 0) {
        if (station_paths) g_ptr_array_free(station_paths, TRUE);
        g_set_error(error, G_IO_ERROR, G_IO_ERROR_NOT_FOUND, "No station found");
        return NULL;
    }
    
    gchar *station_path = g_ptr_array_index(station_paths, 0);
    IwdStation *station = iwd_station_create(system_conn, station_path, error);
    g_ptr_array_free(station_paths, TRUE);
    
    if (!station) {
        return NULL;
    }
    
    // 获取连接的网络路径
    gchar *network_path = iwd_station_get_connected_network(station, error);
    if (!network_path) {
        iwd_station_delete(station);
        return NULL;
    }
    
    // 创建网络对象获取名称
    IwdNetwork *network = iwd_network_create(system_conn,network_path, error);
    g_free(network_path);
    iwd_station_delete(station);
    
    if (!network) {
        return NULL;
    }
    
    char *ssid = iwd_network_get_name(network, error);
    iwd_network_delete(network);
    
    return ssid;
}

// 释放网络信息结构
void iwd_network_info_free(IwdNetworkInfo *info)
{
    if (info) {
        g_free(info->ssid);
        g_free(info->path);
        g_free(info);
    }
}

// 释放设备信息结构
void iwd_device_info_free(IwdDeviceInfo *info)
{
    if (info) {
        g_free(info->path);
        g_free(info->name);
        g_free(info->model);
        g_free(info->address);
        g_free(info);
    }
}