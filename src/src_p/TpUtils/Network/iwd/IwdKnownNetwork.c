


/*///------------------------------------------------------------------------------------------------------------------------//
        通过 KnownNetwork 连接
说明: 已知网络是持久化的，连接更快
日期: 2026.2.6
*///------------------------------------------------------------------------------------------------------------------------//

#include "Network/iwd/Iwd.h"
#include "Network/iwd/IwdKnownNetwork.h"



// 获取已知网络管理器
static GDBusProxy* get_known_network_manager(GDBusConnection *system_bus, GError **error)
{
	if(!system_bus)
		return NULL;
	static GDBusProxy *proxy = NULL;
		
	proxy = g_dbus_proxy_new_sync(
		system_bus,
		G_DBUS_PROXY_FLAGS_NONE,
		NULL,
		IWD_DBUS_SERVICE,
		IWD_DBUS_PATH,  // KnownNetworkManager 路径
		IWD_KNOWN_NETWORK_MANAGER_INTERFACE,
		NULL,
		error);    

	return proxy;
}

// 通过 SSID 查找已知网络路径
gchar* find_known_network_by_ssid(GDBusConnection *system_bus, const gchar *ssid, GError **error)
{
	GDBusProxy *manager = get_known_network_manager(system_bus,error);
	if (!manager) {
		return NULL;
	}

	// 获取所有已知网络
	GVariant *result = g_dbus_proxy_call_sync(
		manager,
		"GetKnownNetworks",
		NULL,
		G_DBUS_CALL_FLAGS_NONE,
		-1,
		NULL,
		error);

	if (!result) {
		return NULL;
	}

	GVariantIter iter;
	gchar *found_path = NULL;
	gchar *path = NULL;

	g_variant_get(result, "(ao)", &iter);

	while (g_variant_iter_next(&iter, "o", &path)) 
	{
		// 创建 KnownNetwork 代理
		GDBusProxy *known_proxy = g_dbus_proxy_new_sync(
			system_bus,
			G_DBUS_PROXY_FLAGS_NONE,
			NULL,
			IWD_DBUS_SERVICE,
			path,
			IWD_KNOWN_NETWORK_INTERFACE,
			NULL,
			NULL);
		if(!known_proxy)
		{
			g_free(path);
			continue;
		}
			
		GVariant *name_var = g_dbus_proxy_get_cached_property(known_proxy, "Name");
		if (name_var) {
			const gchar *known_ssid = g_variant_get_string(name_var, NULL);
			
			if (g_strcmp0(known_ssid, ssid) == 0) {
				found_path = g_strdup(path);
				g_variant_unref(name_var);
				g_object_unref(known_proxy);
				g_free(path);
				break;
			}
			
			g_variant_unref(name_var);
		}
		
		g_object_unref(known_proxy);
		
		g_free(path);
	}

	g_variant_iter_clear(&iter);
	g_variant_unref(result);

	return found_path;
	}

// 通过已知网络连接
gboolean connect_via_known_network(GDBusConnection *system_bus, const gchar *ssid, const gchar *password, GError **error)
{
	// 1. 查找已知网络
	gchar *known_path = find_known_network_by_ssid(system_bus, ssid, error);
	if (!known_path) {
		// 没有已知网络，需要先创建
		return FALSE;
	}

	// 2. 创建 KnownNetwork 对象
	GDBusConnection *bus = g_bus_get_sync(G_BUS_TYPE_SYSTEM, NULL, error);
	GDBusProxy *known_proxy = g_dbus_proxy_new_sync(
		bus,
		G_DBUS_PROXY_FLAGS_NONE,
		NULL,
		IWD_DBUS_SERVICE,
		known_path,
		IWD_KNOWN_NETWORK_INTERFACE,
		NULL,
		error);

	g_free(known_path);

	if (!known_proxy) {
		return FALSE;
	}

	// 3. 连接
	GVariant *result = g_dbus_proxy_call_sync(
		known_proxy,
		"Connect",
		NULL,  // 已知网络通常不需要密码
		G_DBUS_CALL_FLAGS_NONE,
		30000,
		NULL,
		error);

	g_object_unref(known_proxy);

	if (!result) {
		return FALSE;
	}

	g_variant_unref(result);
	return TRUE;
}


/*gboolean smart_connect(WiFiHybridManager *mgr,
                      const gchar *ssid,
                      const gchar *password,
                      GError **error)
{
	// 第1级：已知网络
	printf("尝试通过已知网络连接...\n");
	gchar *known_path = find_known_network_by_ssid(ssid, NULL);
	if (known_path) {
		printf("找到已知网络，尝试连接...\n");
		
		// 通过已知网络连接
		GDBusConnection *bus = g_bus_get_sync(G_BUS_TYPE_SYSTEM, NULL, error);
		GDBusProxy *known_proxy = g_dbus_proxy_new_sync(
			bus,
			G_DBUS_PROXY_FLAGS_NONE,
			NULL,
			IWD_DBUS_SERVICE,
			known_path,
			IWD_KNOWN_NETWORK_INTERFACE,
			NULL,
			error);
		
		g_free(known_path);
		
		if (known_proxy) {
			GVariant *result = g_dbus_proxy_call_sync(
				known_proxy,
				"Connect",
				NULL,
				G_DBUS_CALL_FLAGS_NONE,
				30000,
				NULL,
				error);
			
			g_object_unref(known_proxy);
			
			if (result) {
				g_variant_unref(result);
				return TRUE;
			}
		}
	}

	// 第2级：缓存路径
	printf("已知网络未找到，检查缓存...\n");
	gchar *cached_path = iwd_path_cache_lookup(mgr->path_cache, ssid);
	if (cached_path) {
		printf("从缓存找到路径: %s\n", cached_path);
		
		IwdNetwork *network = iwd_network_new(cached_path, error);
		g_free(cached_path);
		
		if (network) {
			gboolean connected = iwd_network_connect(network, password, error);
			iwd_network_free(network);
			
			if (connected) {
				return TRUE;
			}
		}
	}

	// 第3级：强制刷新
	printf("缓存未命中，强制刷新...\n");

	// 获取站点
	GPtrArray *station_paths = iwd_manager_get_stations(mgr->iwd_manager, error);
	if (!station_paths || station_paths->len == 0) {
		return FALSE;
	}

	gchar *station_path = g_ptr_array_index(station_paths, 0);
	IwdStation *station = iwd_station_new(station_path, error);
	g_ptr_array_free(station_paths, TRUE);

	if (!station) {
		return FALSE;
	}

	// 执行快速扫描
	printf("执行快速扫描...\n");
	if (!iwd_station_scan(station, NULL, NULL, error)) {
		iwd_station_free(station);
		return FALSE;
	}

	// 等待较短时间
	g_usleep(1500000);  // 1.5秒

	// 获取结果
	GPtrArray *network_paths = iwd_station_get_ordered_networks(station, error);
	if (!network_paths) {
		iwd_station_free(station);
		return FALSE;
	}

	// 查找目标网络
	gchar *found_path = NULL;
	for (guint i = 0; i < network_paths->len; i++) {
		gchar *path = g_ptr_array_index(network_paths, i);
		IwdNetwork *network = iwd_network_new(path, NULL);
		
		if (network) {
			gchar *current_ssid = (gchar *)iwd_network_get_name(network, NULL);
			if (current_ssid && g_strcmp0(current_ssid, ssid) == 0) {
				found_path = g_strdup(path);
				
				// 连接
				gboolean connected = iwd_network_connect(network, password, error);
				
				g_free(current_ssid);
				iwd_network_free(network);
				
				if (connected) {
					// 更新缓存
					pthread_mutex_lock(&mgr->path_cache->mutex);
					IwdPathCache *entry = g_hash_table_lookup(mgr->path_cache->cache, ssid);
					if (!entry) {
						entry = g_new0(IwdPathCache, 1);
						entry->ssid = g_strdup(ssid);
						g_hash_table_insert(mgr->path_cache->cache, g_strdup(ssid), entry);
					}
					g_free(entry->path);
					entry->path = g_strdup(path);
					entry->timestamp = g_get_real_time();
					entry->is_valid = TRUE;
					pthread_mutex_unlock(&mgr->path_cache->mutex);
					
					g_free(found_path);
					g_ptr_array_free(network_paths, TRUE);
					iwd_station_free(station);
					return TRUE;
				}
				
				break;
			}
			
			g_free(current_ssid);
			iwd_network_free(network);
		}
	}

	g_free(found_path);
	g_ptr_array_free(network_paths, TRUE);
	iwd_station_free(station);

	g_set_error(error, G_IO_ERROR, G_IO_ERROR_FAILED,
				"无法连接到网络: %s", ssid);
	return FALSE;
}*/