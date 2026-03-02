/*///------------------------------------------------------------------------------------------------------------------------//
		iwd网络站点实现
说 明 : 每个站点对应一个无线网卡
日 期 : 2026.2.5

/*///------------------------------------------------------------------------------------------------------------------------//

#include <stdio.h>
#include <string.h>
#include <stdlib.h> 
#include "Network/iwd/Iwd.h"
#include "Network/iwd/IwdStation.h"
#include "Network/iwd/IwdNetwork.h"


static GDBusConnection *dbus_conn = NULL;

// 私有结构体
struct IwdStationPrivate_ {
    GDBusProxy *proxy;
    gchar *path;
	GDBusConnection *connection;  
};

G_DEFINE_TYPE_WITH_PRIVATE(IwdStation, iwd_station, G_TYPE_OBJECT);

// 私有方法：创建代理
static GDBusProxy* _station_create_proxy(GDBusConnection *conn, const gchar *path, GError **error)
{
    GDBusProxy *proxy = g_dbus_proxy_new_sync(
        conn,  // 使用传入的连接
        G_DBUS_PROXY_FLAGS_NONE,
        NULL,
        IWD_DBUS_SERVICE,
        path,
        IWD_STATION_INTERFACE,
        NULL,
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
    g_clear_object(&self->priv->connection);  // 释放连接
    
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
    g_return_val_if_fail(conn != NULL, NULL);  // 检查 conn 不为 NULL

    IwdStation *self = g_object_new(IWD_STATION_TYPE, NULL);
    self->priv->path = g_strdup(path);
    self->priv->connection = g_object_ref(conn);  // 保存连接引用
    
    self->priv->proxy = _station_create_proxy(conn, path, error);  // 修改 _station_create_proxy
    if (!self->priv->proxy) {
        g_object_unref(self);
        return NULL;
    }
    
    return self;
}

void iwd_station_delete(IwdStation *station)
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


/*int iwd_station_connect_network(IwdStation *station, 
                               IwdNetwork *network,
                               GError **error)
{
    g_return_val_if_fail(IWD_STATION_IS(station), -1);
    g_return_val_if_fail(IWD_NETWORK_IS(network), -1);
    
    // 获取网络路径
    gchar *network_path = iwd_network_get_path(network, error);
    if (!network_path) {
        return -1;
    }
    
    // 调用现有的连接方法
    int result = iwd_station_connect(station, network_path, error);
    
    g_free(network_path);
    return result;
}*/


// 断开连接
int iwd_station_disconnect(IwdStation *station, uint32_t timeout, GError **error)
{
    g_return_val_if_fail(IWD_STATION_IS(station), FALSE);
    
    GVariant *result = g_dbus_proxy_call_sync(
        station->priv->proxy,
        "Disconnect",
        NULL,  // parameters
        G_DBUS_CALL_FLAGS_NONE,
        timeout,  // 10秒超时建议
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

gchar* iwd_station_get_path(IwdStation *station, GError **error)
{
    g_return_val_if_fail(IWD_STATION_IS(station), NULL);
    
    if (station->priv->path) {
        return g_strdup(station->priv->path);
    }
    
    g_set_error(error, G_IO_ERROR, G_IO_ERROR_FAILED,
               "Station path is NULL");
    return NULL;
}


gboolean iwd_station_scan_async(IwdStation *station, GError **error)
{
    g_return_val_if_fail(IWD_STATION_IS(station), FALSE);
    
    // 检查是否已经在扫描
    GError *scan_status_error = NULL;
    gboolean is_scanning = iwd_station_is_scanning(station, &scan_status_error);
    
    if (scan_status_error) {
        g_warning("检查扫描状态失败: %s", scan_status_error->message);
        g_error_free(scan_status_error);
    } else if (is_scanning) {
        // 已经在扫描
        return TRUE;
    }
    
    // 开始新的扫描
    return iwd_station_scan(station, error) == 0;
}

//等待扫描完成
gboolean iwd_station_scan_and_wait(IwdStation *station, 
                                  int timeout_ms, 
                                  GError **error)
{
    g_return_val_if_fail(IWD_STATION_IS(station), FALSE);
    
    if (timeout_ms <= 0) {
        timeout_ms = 10000;  // 默认10秒
    }
    
    printf("开始扫描并等待完成，超时: %d ms\n", timeout_ms);
    
    // 1. 先检查是否已经在扫描
    GError *status_error = NULL;
    gboolean is_scanning = iwd_station_is_scanning(station, &status_error);
    
    if (status_error) {
        g_warning("检查扫描状态失败: %s", status_error->message);
        g_error_free(status_error);
    } else if (is_scanning) {
        printf("已经在扫描中，等待完成...\n");
    } else {
        // 2. 开始新扫描
        printf("开始新扫描...\n");
        GError *scan_error = NULL;
        if (iwd_station_scan(station, &scan_error) != 0) {
            g_warning("开始扫描失败: %s", scan_error->message);
            g_propagate_error(error, scan_error);
            return FALSE;
        }
    }
    
    // 3. 等待扫描完成
    gint64 end_time = g_get_monotonic_time() + timeout_ms * 1000;
    int poll_interval = 200;  // 200ms
    
    int check_count = 0;
    while (g_get_monotonic_time() < end_time) {
        GError *check_error = NULL;
        is_scanning = iwd_station_is_scanning(station, &check_error);
        
        if (check_error) {
            g_warning("检查扫描状态失败: %s", check_error->message);
            g_error_free(check_error);
        } else if (!is_scanning) {
            printf("扫描完成 (检查次数: %d)\n", check_count);
            return TRUE;  // 扫描完成
        }
        
        // 非阻塞等待
        g_usleep(poll_interval * 1000);
        check_count++;
    }
    
    // 4. 超时
    printf("扫描超时 (检查次数: %d)\n", check_count);
    g_set_error(error, G_IO_ERROR, G_IO_ERROR_TIMED_OUT,
               "扫描超时 (%d ms)", timeout_ms);
    return FALSE;
}

// 辅助函数：在网络列表中查找SSID
static gchar* _find_network_by_ssid_in_list(GPtrArray *networks, const gchar *ssid, GDBusConnection *conn)
{
    for (guint i = 0; i < networks->len; i++) {
        gchar *path = g_ptr_array_index(networks, i);
        IwdNetwork *network = iwd_network_create(conn, path, NULL);
        
        if (network) {
            gchar *network_name = iwd_network_get_name(network, NULL);
            if (network_name && g_strcmp0(network_name, ssid) == 0) {
                gchar *found_path = g_strdup(path);
                g_free(network_name);
                iwd_network_delete(network);
                return found_path;
            }
            g_free(network_name);
            iwd_network_delete(network);
        }
    }
    return NULL;
}

int iwd_station_connect_with_password(IwdStation *station,
                                     const gchar *network_path,
                                     const gchar *password,
                                     int timeout_ms,  // 添加超时参数
                                     GError **error)
{
    g_return_val_if_fail(IWD_STATION_IS(station), -1);
    g_return_val_if_fail(network_path != NULL, -1);
    
    if (timeout_ms <= 0) {
        timeout_ms = 30000;  // 默认30秒
    }
    
    printf("通过密码连接网络: %s (超时: %d ms)\n", network_path, timeout_ms);
    
    // 1. 创建网络对象
    IwdNetwork *network = iwd_network_create(station->priv->connection, 
                                            network_path, 
                                            error);
    if (!network) {
        return -1;
    }
    
    // 2. 通过 Network 接口连接（支持密码）
    gboolean result = iwd_network_connect(network, password, timeout_ms, error);
    
    // 3. 清理
    iwd_network_delete(network);
    
    return result ? 0 : -1;
}

int iwd_station_connect_by_ssid(IwdStation *station,
                               const gchar *ssid,
                               const gchar *password,  // 添加密码参数
                               int connect_timeout_ms, // 连接超时
                               GError **error)
{
    g_return_val_if_fail(IWD_STATION_IS(station), -1);
    g_return_val_if_fail(ssid != NULL, -1);

    if (connect_timeout_ms <= 0) {
        connect_timeout_ms = 30000;  // 默认30秒连接超时
    }
    
    printf("通过SSID连接: %s%s\n", 
           ssid, 
           password ? " (需要密码)" : " (已知网络)");
    
    // 1. 先检查当前网络列表
    printf("检查当前网络列表...\n");
    GPtrArray *current_networks = iwd_station_get_ordered_networks(station, error);
    gchar *network_path = NULL;
    
    if (current_networks) {
        // 尝试在当前列表中查找
        network_path = _find_network_by_ssid_in_list(current_networks, 
                                                    ssid, 
                                                    station->priv->connection);
        g_ptr_array_free(current_networks, TRUE);
    }
    
    // 2. 如果没找到，触发扫描
    if (!network_path) {
        printf("网络未找到，触发扫描...\n");
        
        // 扫描并等待完成
        GError *scan_error = NULL;
        if (!iwd_station_scan_and_wait(station, -1, &scan_error)) {
            g_warning("扫描失败: %s", scan_error->message);
            g_error_free(scan_error);
        } else {
            // 获取扫描后的网络列表
            printf("获取扫描后的网络列表...\n");
            current_networks = iwd_station_get_ordered_networks(station, error);
            if (current_networks) {
                network_path = _find_network_by_ssid_in_list(current_networks, 
                                                            ssid, 
                                                            station->priv->connection);
                g_ptr_array_free(current_networks, TRUE);
            }
        }
    }
    
    if (!network_path) {
        g_set_error(error, G_IO_ERROR, G_IO_ERROR_NOT_FOUND,
                   "未找到网络: %s", ssid);
        return -1;
    }
    
    // 3. 根据是否有密码选择连接方式
    int result = -1;
    
    if (password && password[0] != '\0') {
        // 使用密码连接
        printf("使用密码连接网络路径: %s (超时: %d ms)\n", 
               network_path, connect_timeout_ms);
        result = iwd_station_connect_with_password(station, 
                                                  network_path, 
                                                  password, 
                                                  connect_timeout_ms, 
                                                  error);
    } else {
        // 使用已知网络连接（无密码）
        printf("使用已知网络连接: %s (超时: %d ms)\n", 
               network_path, connect_timeout_ms);
        result = iwd_station_connect(station, network_path, error);
    }
    
    g_free(network_path);
    return result;
}


