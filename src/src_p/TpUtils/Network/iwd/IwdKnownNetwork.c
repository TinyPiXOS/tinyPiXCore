


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

