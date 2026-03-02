/*///------------------------------------------------------------------------------------------------------------------------//
		IWD管理(DBUS)Manager接口相关操作
说 明 : 
日 期 : 2026.2.5

/*///------------------------------------------------------------------------------------------------------------------------//

#include <stdio.h>
#include <string.h>
#include "Dbus/manager.h"
#include "Network/iwd/Iwd.h"
#include "Network/iwd/IwdManager.h"
#include "Network/iwd/IwdNetwork.h"
#include "Network/iwd/IwdStation.h"

// 私有结构体
struct IwdManagerPrivate_ {
    Manager *object_manager;      // 通用的对象管理器
    GDBusConnection *connection;  // DBus连接
    GPtrArray *stations_cache;    // 缓存的站点路径
    GPtrArray *networks_cache;    // 缓存的网络路径
    gboolean cache_valid;         // 缓存是否有效
};

G_DEFINE_TYPE_WITH_PRIVATE(IwdManager, iwd_manager, G_TYPE_OBJECT);

// 静态全局连接
static GDBusConnection *system_conn = NULL;

// 对象生命周期管理
static void iwd_manager_dispose(GObject *gobject)
{
    IwdManager *self = IWD_MANAGER(gobject);
    IwdManagerPrivate *priv = self->priv;
    
    g_clear_object(&priv->object_manager);
    g_clear_object(&priv->connection);
    
    if (priv->stations_cache) {
        g_ptr_array_free(priv->stations_cache, TRUE);
        priv->stations_cache = NULL;
    }
    
    if (priv->networks_cache) {
        g_ptr_array_free(priv->networks_cache, TRUE);
        priv->networks_cache = NULL;
    }
    
    G_OBJECT_CLASS(iwd_manager_parent_class)->dispose(gobject);
}

static void iwd_manager_finalize(GObject *gobject)
{
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
    self->priv->object_manager = NULL;
    self->priv->connection = NULL;
    self->priv->stations_cache = NULL;
    self->priv->networks_cache = NULL;
    self->priv->cache_valid = FALSE;
}

// 刷新缓存
static gboolean refresh_cache(IwdManager *manager, GError **error)
{
    g_return_val_if_fail(IWD_MANAGER_IS(manager), FALSE);
    
    printf("刷新iwd对象缓存...\n");
    
    // 清空旧缓存
    if (manager->priv->stations_cache) {
        g_ptr_array_free(manager->priv->stations_cache, TRUE);
        manager->priv->stations_cache = NULL;
    }
    
    if (manager->priv->networks_cache) {
        g_ptr_array_free(manager->priv->networks_cache, TRUE);
        manager->priv->networks_cache = NULL;
    }
    
    // 获取所有对象
    GVariant *objects = manager_get_managed_objects(manager->priv->object_manager, error);
    if (!objects) {
        return FALSE;
    }
    
    manager->priv->stations_cache = g_ptr_array_new_with_free_func(g_free);
    manager->priv->networks_cache = g_ptr_array_new_with_free_func(g_free);
    
    const gchar *object_path;
    GVariant *ifaces_and_properties;
    GVariantIter i;
    
    g_variant_iter_init(&i, objects);
    while (g_variant_iter_next(&i, "{&o@a{sa{sv}}}", 
                               &object_path, &ifaces_and_properties)) {
        const gchar *interface_name;
        GVariant *properties;
        GVariantIter ii;
        
        g_variant_iter_init(&ii, ifaces_and_properties);
        while (g_variant_iter_next(&ii, "{&s@a{sv}}", 
                                   &interface_name, &properties)) {
            // 处理站点
            if (g_strcmp0(interface_name, IWD_STATION_INTERFACE) == 0) {
                g_ptr_array_add(manager->priv->stations_cache, g_strdup(object_path));
            }
            // 处理网络
            else if (g_strcmp0(interface_name, IWD_NETWORK_INTERFACE) == 0) {
                g_ptr_array_add(manager->priv->networks_cache, g_strdup(object_path));
            }
            
            g_variant_unref(properties);
        }
        g_variant_unref(ifaces_and_properties);
    }
    
    g_variant_unref(objects);
    
    manager->priv->cache_valid = TRUE;
    
    printf("刷新完成: %d个站点, %d个网络\n",
           manager->priv->stations_cache->len,
           manager->priv->networks_cache->len);
    
    return TRUE;
}

// 公共方法实现
IwdManager* iwd_manager_create(GDBusConnection *conn, GError **error)
{
    g_return_val_if_fail(conn != NULL, NULL);
    
    // 保存全局连接
    system_conn = conn;
    
    // 创建IwdManager对象
    IwdManager *self = g_object_new(IWD_MANAGER_TYPE, NULL);
    self->priv->connection = g_object_ref(conn);
    
    // 创建通用的对象管理器
    self->priv->object_manager = manager_new_with_service(
        conn,
        IWD_DBUS_SERVICE,      // "net.connman.iwd"
        IWD_MANAGER_PATH,      // "/"
        error);
    
    if (!self->priv->object_manager) {
        g_object_unref(self);
        return NULL;
    }
    
    printf("IwdManager创建成功\n");
    
    return self;
}

void iwd_manager_delete(IwdManager *manager)
{
    if (manager) {
        g_object_unref(manager);
    }
}

// 获取所有站点路径
GPtrArray* iwd_manager_get_stations(IwdManager *manager, GError **error)
{
    g_return_val_if_fail(IWD_MANAGER_IS(manager), NULL);
    
    // 如果缓存无效，刷新缓存
    if (!manager->priv->cache_valid) {
        GError *refresh_error = NULL;
        if (!refresh_cache(manager, &refresh_error)) {
            g_warning("刷新缓存失败: %s", refresh_error->message);
            g_error_free(refresh_error);
        }
    }
    
    // 如果没有缓存，直接通过Manager获取
    if (!manager->priv->stations_cache || manager->priv->stations_cache->len == 0) {
        GPtrArray *stations = manager_find_objects_by_interface(
            manager->priv->object_manager,
            IWD_STATION_INTERFACE,
            error);
        
        if (!stations) {
            return NULL;
        }
        
        // 缓存结果
        if (!manager->priv->stations_cache) {
            manager->priv->stations_cache = stations;
        } else {
            // 合并结果
            for (guint i = 0; i < stations->len; i++) {
                gchar *path = g_ptr_array_index(stations, i);
                g_ptr_array_add(manager->priv->stations_cache, g_strdup(path));
            }
            g_ptr_array_free(stations, TRUE);
        }
    }
    
    // 返回副本
    GPtrArray *result = g_ptr_array_new_with_free_func(g_free);
    for (guint i = 0; i < manager->priv->stations_cache->len; i++) {
        gchar *path = g_ptr_array_index(manager->priv->stations_cache, i);
        g_ptr_array_add(result, g_strdup(path));
    }
    
    return result;
}

// 根据网卡名获取站点
IwdStation* iwd_manager_get_station_by_name(IwdManager *manager, 
                                           const char *interface_name, 
                                           GError **error)
{
    g_return_val_if_fail(IWD_MANAGER_IS(manager), NULL);
    g_return_val_if_fail(interface_name != NULL, NULL);
    
    printf("查找网卡名为 '%s' 的站点\n", interface_name);
    
    // 获取所有站点路径
    GPtrArray *station_paths = iwd_manager_get_stations(manager, error);
    if (!station_paths || station_paths->len == 0) {
        if (station_paths) {
            g_ptr_array_free(station_paths, TRUE);
        }
        g_set_error(error, G_IO_ERROR, G_IO_ERROR_NOT_FOUND, 
                   "没有找到Wi-Fi站点");
        return NULL;
    }
    
    IwdStation *found_station = NULL;
    
    // 遍历查找匹配的网卡名
    for (guint i = 0; i < station_paths->len; i++) {
        gchar *station_path = g_ptr_array_index(station_paths, i);
        
        // 创建临时站点对象来获取名称
        IwdStation *temp_station = iwd_station_create(system_conn, station_path, NULL);
        if (!temp_station) {
            g_warning("无法创建临时站点对象: %s", station_path);
            continue;
        }
        
        // 获取站点名称
        GError *name_error = NULL;
        gchar *station_name = iwd_station_get_name(temp_station, &name_error);
        
        if (!name_error && station_name) {
            printf("检查站点: %s (路径: %s)\n", station_name, station_path);
            
            if (g_strcmp0(station_name, interface_name) == 0) {
                printf("✓ 找到匹配的站点: %s\n", interface_name);
                
                // 创建最终站点对象
                found_station = iwd_station_create(system_conn, station_path, error);
                g_free(station_name);
                iwd_station_delete(temp_station);
                break;
            }
            g_free(station_name);
        } else if (name_error) {
            g_warning("获取站点名称失败: %s", name_error->message);
            g_error_free(name_error);
        }
        
        iwd_station_delete(temp_station);
    }
    
    // 清理
    g_ptr_array_free(station_paths, TRUE);
    
    if (!found_station) {
        g_set_error(error, G_IO_ERROR, G_IO_ERROR_NOT_FOUND,
                   "未找到网卡名为 '%s' 的站点", interface_name);
    }
    
    return found_station;
}

// 获取指定索引的站点
IwdStation* iwd_manager_get_station_by_index(IwdManager *manager, 
                                            int index, 
                                            GError **error)
{
    g_return_val_if_fail(IWD_MANAGER_IS(manager), NULL);
    
    // 获取所有站点路径
    GPtrArray *station_paths = iwd_manager_get_stations(manager, error);
    if (!station_paths || station_paths->len == 0) {
        if (station_paths) {
            g_ptr_array_free(station_paths, TRUE);
        }
        g_set_error(error, G_IO_ERROR, G_IO_ERROR_NOT_FOUND, 
                   "没有找到Wi-Fi站点");
        return NULL;
    }
    
    // 检查索引是否有效
    if (index < 0 || index >= (int)station_paths->len) {
        g_ptr_array_free(station_paths, TRUE);
        g_set_error(error, G_IO_ERROR, G_IO_ERROR_INVALID_ARGUMENT,
                   "站点索引 %d 超出范围 (0-%d)", 
                   index, station_paths->len - 1);
        return NULL;
    }
    
    // 获取对应路径
    gchar *station_path = g_ptr_array_index(station_paths, index);
    
    // 创建站点对象
    IwdStation *station = iwd_station_create(system_conn, station_path, error);
    
    // 清理
    g_ptr_array_free(station_paths, TRUE);
    
    return station;
}

// 把原始的网络路径列表转换成包含更多信息的网络信息列表
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

// 扫描网络
GPtrArray* iwd_manager_scan_networks(IwdManager *manager, int timeout_ms, GError **error)
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
        g_warning("检查扫描状态失败: %s", scan_status_error->message);
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
            g_warning("检查扫描状态失败: %s", check_error->message);
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
                       "Wi-Fi扫描超时 (%d ms)", timeout_ms);
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


void iwd_network_info_free(IwdNetworkInfo *info)
{
    if (info) {
        g_free(info->path);
        g_free(info->ssid);
        g_free(info);
    }
}