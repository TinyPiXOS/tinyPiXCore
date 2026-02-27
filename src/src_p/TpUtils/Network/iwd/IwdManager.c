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


//根据网卡名创建IwdStation对象
IwdStation* iwd_manager_get_station_by_name(IwdManager *manager, const char *interface_name, GError **error)
{
    g_return_val_if_fail(IWD_MANAGER_IS(manager), NULL);
    g_return_val_if_fail(interface_name != NULL, NULL);
    
    // 1. 获取所有站点路径
    GPtrArray *station_paths = iwd_manager_get_stations(manager, error);
    if (!station_paths || station_paths->len == 0) {
        if (station_paths) {
            g_ptr_array_free(station_paths, TRUE);
        }
        g_set_error(error, G_IO_ERROR, G_IO_ERROR_NOT_FOUND, 
                   "No Wi-Fi stations found");
        return NULL;
    }
    
    IwdStation *found_station = NULL;
    
    // 2. 遍历所有站点，查找匹配的网卡名
    for (guint i = 0; i < station_paths->len; i++) {
        gchar *station_path = g_ptr_array_index(station_paths, i);
        
        // 创建临时站点对象来获取名称
        IwdStation *temp_station = iwd_station_create(system_conn, station_path, NULL);
        if (!temp_station) {
            g_warning("Failed to create temporary station for path: %s", station_path);
            continue;
        }
        
        // 获取站点名称（网卡名）
        GError *name_error = NULL;
        gchar *station_name = iwd_station_get_name(temp_station, &name_error);
        
        if (!name_error && station_name) {
            // 比较网卡名
            if (g_strcmp0(station_name, interface_name) == 0) {
                // 找到匹配的，创建最终站点对象
                found_station = iwd_station_create(system_conn, station_path, error);
                g_free(station_name);
                iwd_station_delete(temp_station);
                break;
            }
            g_free(station_name);
        } else if (name_error) {
            g_warning("Failed to get name for station %s: %s", 
                     station_path, name_error->message);
            g_error_free(name_error);
        }
        
        iwd_station_delete(temp_station);
    }
    
    // 3. 清理
    g_ptr_array_free(station_paths, TRUE);
    
    // 4. 检查是否找到
    if (!found_station) {
        g_set_error(error, G_IO_ERROR, G_IO_ERROR_NOT_FOUND,
                   "No Wi-Fi station found with interface name: %s", interface_name);
    }
    
    return found_station;
}


// 获取指定索引的站点（索引从0开始）
IwdStation* iwd_manager_get_station_by_index(IwdManager *manager, int index, GError **error)
{
    g_return_val_if_fail(IWD_MANAGER_IS(manager), NULL);
    
    // 1. 获取所有站点路径
    GPtrArray *station_paths = iwd_manager_get_stations(manager, error);
    if (!station_paths || station_paths->len == 0) {
        if (station_paths) {
            g_ptr_array_free(station_paths, TRUE);
        }
        g_set_error(error, G_IO_ERROR, G_IO_ERROR_NOT_FOUND, 
                   "No Wi-Fi stations found");
        return NULL;
    }
    
    // 2. 检查索引是否有效
    if (index < 0 || index >= (int)station_paths->len) {
        g_ptr_array_free(station_paths, TRUE);
        g_set_error(error, G_IO_ERROR, G_IO_ERROR_INVALID_ARGUMENT, "Station index %d out of range (0-%d)", 
                   index, station_paths->len - 1);
        return NULL;
    }
    
    // 3. 获取对应路径
    gchar *station_path = g_ptr_array_index(station_paths, index);
    
    // 4. 创建站点对象
    IwdStation *station = iwd_station_create(system_conn, station_path, error);
    
    // 5. 清理
    g_ptr_array_free(station_paths, TRUE);
    
    return station;
}


//把原始的网络路径列表转换成包含更多信息的网络信息列表
//输出的类型为IwdNetworkInfo
static GPtrArray* convert_networks_to_infos(GPtrArray *networks)
{
	GPtrArray *network_infos = g_ptr_array_new_with_free_func(
		(GDestroyNotify)iwd_network_info_free);

	for (guint i = 0; networks && i < networks->len; i++) 
	{
		gchar *network_path = g_ptr_array_index(networks, i);
		IwdNetwork *network = iwd_network_create(system_conn, network_path, NULL);
		
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

	return network_infos;
}

// 扫描 WiFi 网络
GPtrArray* iwd_manager_scan_networks(IwdManager *manager, int timeout_ms ,GError **error)
{
	g_return_val_if_fail(IWD_MANAGER_IS(manager), NULL);
    
    // 参数验证
    if (timeout_ms <= 0) {
        timeout_ms = 10000;  // 默认10秒
    }
    
    // 获取第一个站点
    IwdStation *station = iwd_manager_get_station_by_index(manager, 0, error);  
    if (!station) {
        return NULL;
    }
    
    // 先检查是否已在扫描中
    GError *scan_status_error = NULL;
    gboolean scanning = iwd_station_is_scanning(station, &scan_status_error);
    
    if (scan_status_error) {
        g_warning("Failed to check scan status: %s", scan_status_error->message);
        g_error_free(scan_status_error);
    }
    
    // 如果已经在扫描中，直接获取当前结果
    if (scanning) {
        GPtrArray *current_networks = iwd_station_get_ordered_networks(station, error);
        if (current_networks) {
            iwd_station_delete(station);
            return convert_networks_to_infos(current_networks);
        }
    }
    
    // 执行扫描
    if (!iwd_station_scan(station, error)) {
        iwd_station_delete(station);
        return NULL;
    }
    
    // 等待扫描完成
    gint64 end_time = g_get_monotonic_time() + timeout_ms * 1000;
    GPtrArray *networks = NULL;
    gboolean scan_success = FALSE;
    
    while (g_get_monotonic_time() < end_time) 
    {
        // 检查扫描状态
        GError *check_error = NULL;
        scanning = iwd_station_is_scanning(station, &check_error);
        
        if (check_error) {
            g_warning("Failed to check scan status: %s", check_error->message);
            g_error_free(check_error);
            g_usleep(100000);  // 100ms
            continue;
        }
        
        if (!scanning) {
            // 扫描完成，尝试获取网络列表
            networks = iwd_station_get_ordered_networks(station, error);
            
            // 即使没有网络，也可能是成功扫描但没有发现网络
            if (networks || (error && *error)) {
                scan_success = TRUE;  // 扫描完成（成功或失败）
                break;
            }
        }
        
        g_usleep(100000);  // 100ms
    }
    
    // 检查结果
    if (!scan_success) {
        // 超时
        if (!(*error)) {
            g_set_error(error, G_IO_ERROR, G_IO_ERROR_TIMED_OUT,
                       "Wi-Fi scan timed out after %d ms", timeout_ms);
        }
        iwd_station_delete(station);
        return NULL;
    }
    
    // 如果没有网络但也没有错误，可能只是没有发现任何网络
    if (!networks && !(*error)) {
        iwd_station_delete(station);
        return g_ptr_array_new_with_free_func((GDestroyNotify)iwd_network_info_free);
    }
    
    // 转换网络列表
    GPtrArray *network_infos = convert_networks_to_infos(networks);
    
    if (networks) {
        g_ptr_array_free(networks, TRUE);
    }
    
    iwd_station_delete(station);
    return network_infos;
}

// 连接到指定 WiFi
// timeout:连接超时时间，单位ms
gboolean iwd_manager_connect_to_network(IwdManager *manager,
                                        const gchar *ssid,
                                        const gchar *password,
										int timeout,
                                        GError **error)
{
    g_return_val_if_fail(IWD_MANAGER_IS(manager), FALSE);
    g_return_val_if_fail(ssid != NULL, FALSE);
    
    // 首先扫描网络
    GPtrArray *networks = iwd_manager_scan_networks(manager, 3000, error);
    if (!networks) {
        return FALSE;
    }
    
    // 查找目标网络
    gchar *target_path = NULL;
    for (guint i = 0; i < networks->len; i++) 
	{
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



static bool wait_scan_complete(IwdStation *station, int timeout_ms)
{
    int waited = 0;
    const int interval = 200; // 200ms 轮询一次

    while (waited < timeout_ms) {

        GError *err = NULL;
        char *state = iwd_station_get_state(station, &err);

        if (state) {
            bool scanning = (strcmp(state, "scanning") == 0);
            free(state);

            if (!scanning) {
                return true;
            }
        }

        if (err) {
            g_error_free(err);
        }

        g_usleep(interval * 1000);
        waited += interval;
    }

    return false;
}

bool iwd_manager_connect_by_ssid(
    IwdManager *manager,
    const char *ssid,
    const char *password,
    int timeout,
    GError **error)
{
    g_return_val_if_fail(IWD_MANAGER_IS(manager), false);
    g_return_val_if_fail(ssid != NULL, false);

    // 1. 获取 station
	IwdStation *station = iwd_manager_get_station_by_index(manager, 0, error);
    if (!station) {
        return false;
    }

	// 2. 触发扫描
	iwd_station_scan(station,  NULL);

	// 3. 等待扫描真正完成（最多等待10秒）然后获取列表
	if (!wait_scan_complete(station, 10000)) {
		iwd_station_delete(station);
		g_set_error(error, G_IO_ERROR, G_IO_ERROR_TIMED_OUT, "iwd scan did not complete in time");
		return false;
	}

	GPtrArray *networks = iwd_station_get_ordered_networks(station, error);
	if (!networks) {
		iwd_station_delete(station);
		return false;
	}

    // 4. 在 iwd 的结果里找目标 SSID
    char *target_path = NULL;

    for (guint i = 0; i < networks->len; i++) 
	{
        gchar *path = g_ptr_array_index(networks, i);

        IwdNetwork *network = iwd_network_create(system_conn, path, NULL);
        if (!network)
            continue;

        char *name = iwd_network_get_name(network, NULL);

        if (name && strcmp(name, ssid) == 0) {
            target_path = g_strdup(path);
            free(name);
            iwd_network_delete(network);
            break;
        }

        if (name)
            free(name);

        iwd_network_delete(network);
    }

    g_ptr_array_free(networks, TRUE);

    if (!target_path) {
        iwd_station_delete(station);
        g_set_error(error, G_IO_ERROR, G_IO_ERROR_NOT_FOUND,
                    "SSID '%s' not found in iwd scan list", ssid);
        return false;
    }

    // 5. 创建 network 对象并连接
    IwdNetwork *network = iwd_network_create(system_conn, target_path, error);
    g_free(target_path);

    if (!network) {
        iwd_station_delete(station);
        return false;
    }

    bool ret = iwd_network_connect(network, password, timeout, error);

    iwd_network_delete(network);
    iwd_station_delete(station);

    return ret;
}

// 简单连接,
bool iwd_connect_simple(
    GDBusConnection *conn,
    const char *ssid,
    const char *password,
    int timeout)
{
    GError *error = NULL;

    IwdManager *manager = iwd_manager_create(conn, &error);
    if (!manager) {
        g_printerr("Create manager failed: %s\n", error->message);
        g_error_free(error);
        return false;
    }

    bool ret = iwd_manager_connect_by_ssid(
        manager,
        ssid,
        password,
        timeout,
        &error);

    if (!ret) {
        g_printerr("Connect failed: %s\n", error->message);
        g_error_free(error);
    }

    iwd_manager_delete(manager);

    return ret;
}