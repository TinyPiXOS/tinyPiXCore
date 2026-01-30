/*///------------------------------------------------------------------------------------------------------------------------//
		对NetworkManager相关的接口的初级封装
说 明 : 
日 期 : 2026.1.28

/*///------------------------------------------------------------------------------------------------------------------------//

#include <arpa/inet.h>
#include "Network/NetworkAppConf.h"

static void network_gvariant_build_add_ipv4_empty(GVariantBuilder *settings)
{
    GVariantBuilder ipv4;
    g_variant_builder_init(&ipv4, G_VARIANT_TYPE("a{sv}"));
    /* 什么都不加，就是“存在但无立场” */
    g_variant_builder_add(settings, "{sa{sv}}", "ipv4", &ipv4);
}

//设置ipv4为dhcp
static void network_gvariant_build_add_ipv4_dhcp(GVariantBuilder *settings)
{
    GVariantBuilder ipv4;
    g_variant_builder_init(&ipv4, G_VARIANT_TYPE("a{sv}"));
    g_variant_builder_add(&ipv4, "{sv}", "method", g_variant_new_string("auto"));
    g_variant_builder_add(settings, "{sa{sv}}", "ipv4", &ipv4);
}

//设置ipv4为static
//dns可以为空
static void network_gvariant_build_add_ipv4_static(GVariantBuilder *settings,
                            const char *ip,
                            guint prefix,
                            const char *gateway,   
							const char **dns,
    						uint32_t dns_count)
{
    GVariantBuilder ipv4;
    GVariantBuilder addr;
    GVariantBuilder addrs;

    g_variant_builder_init(&ipv4,  G_VARIANT_TYPE("a{sv}"));
    g_variant_builder_init(&addr,  G_VARIANT_TYPE("a{sv}"));
    g_variant_builder_init(&addrs, G_VARIANT_TYPE("aa{sv}"));

    /* 单地址 */
    g_variant_builder_add(&addr, "{sv}", "address", g_variant_new_string(ip));
    g_variant_builder_add(&addr, "{sv}", "prefix", g_variant_new_uint32(prefix));

    if (gateway && *gateway) {
        g_variant_builder_add(&ipv4, "{sv}", "gateway", g_variant_new_string(gateway));
    }

    g_variant_builder_add(&addrs, "a{sv}", &addr);

    g_variant_builder_add(&ipv4, "{sv}", "method", g_variant_new_string("manual"));
    g_variant_builder_add(&ipv4, "{sv}", "addresses", g_variant_builder_end(&addrs));

	 // 4. DNS（如果给了）
    if (dns && dns_count > 0) {
        GVariantBuilder dns_array;
        g_variant_builder_init(&dns_array, G_VARIANT_TYPE("au"));

        for (uint32_t i = 0; i < dns_count; i++)
            g_variant_builder_add(&dns_array, "u", inet_addr(dns[i]));

        g_variant_builder_add(&ipv4, "{sv}", "dns", &dns_array);
        g_variant_builder_add(&ipv4, "{sv}", "ignore-auto-dns", g_variant_new_boolean(TRUE));
    }

    g_variant_builder_add(settings, "{sa{sv}}", "ipv4", &ipv4);
}

//设置ipv6为ignore
static void network_gvariant_build_add_ipv6_ignore(GVariantBuilder *settings)
{
    GVariantBuilder ipv6;
    g_variant_builder_init(&ipv6, G_VARIANT_TYPE("a{sv}"));
    g_variant_builder_add(&ipv6, "{sv}", "method", g_variant_new_string("ignore"));
    g_variant_builder_add(settings, "{sa{sv}}", "ipv6", &ipv6);
}

//创建一个最小基础配置
static void network_gvariant_build_add_connection_basic(GVariantBuilder *settings,
                                 const char *conn_name,
                                 const char *type,
                                 const char *ifname) /* 可 NULL */
{
    GVariantBuilder conn;
    g_variant_builder_init(&conn, G_VARIANT_TYPE("a{sv}"));

    g_variant_builder_add(&conn, "{sv}", "id", g_variant_new_string(conn_name));
    g_variant_builder_add(&conn, "{sv}", "type", g_variant_new_string(type));
    g_variant_builder_add(&conn, "{sv}", "autoconnect", g_variant_new_boolean(TRUE));

    if (ifname && *ifname) {
        g_variant_builder_add(&conn, "{sv}", "interface-name", g_variant_new_string(ifname));
    }

    g_variant_builder_add(settings, "{sa{sv}}", "connection", &conn);
}

//打开网卡配置,使用
//配置不存在就创建，存在就直接打开并返回
//dbus_conn：dbus连接
//nms：系统网卡设置接口句柄
//ifname：网卡名称,可以传空，表示不指定网卡，即使设置了网卡只代表次配置优先对改网卡生效，如果使用热插拔的网卡建议不要指定
NmConnection *network_open_nm_connection(GDBusConnection *dbus_conn, 
									NmSettings *nms, 
									const char *conn_name, 
									const char *ifname)
{
    g_return_val_if_fail(dbus_conn != NULL, NULL);
    g_return_val_if_fail(nms != NULL, NULL);
    g_return_val_if_fail(conn_name != NULL, NULL);
	printf("network_open_nm_connection start\n");
	GError *error = NULL;
	NmConnection *nmc;
	//查找配置
	char *path = nm_settings_find_connection_object(nms, conn_name, &error);
	if(path)
	{
		nmc = nm_connection_create(dbus_conn, path, &error);
		g_free(path);
		printf("已有配置，打开配置成功\n");
		return nmc;
	}
	g_free(path);
	printf("不存在配置，创建默认配置\n");
	//不存在就创建默认的空配置
    GVariantBuilder settings_builder;
    g_variant_builder_init(&settings_builder, G_VARIANT_TYPE("a{sa{sv}}"));

    network_gvariant_build_add_connection_basic(&settings_builder, conn_name, "802-3-ethernet", ifname);

    network_gvariant_build_add_ipv4_empty(&settings_builder);

	GVariant *v = g_variant_builder_end(&settings_builder);	//封装成GVariant
    path = nm_settings_add_connection(nms, v, &error);
	g_variant_unref(v);
	if(!path)
		return NULL;
	printf("创建默认配置成功\n");
	nmc = nm_connection_create(dbus_conn, path, &error);
	free(path);
	return nmc;
}
void network_close_nm_connection(NmConnection *nmc)
{
	if(nmc)
		nm_connection_delete(nmc);
}


//设置网络配置为静态，如需应用于网卡需要调用 network_manager_activate_connection_to_device 接口应用到网卡
//ip:IP地址
//prefix:掩码
//gateway:网关
//dns_flag:是否启用DNS
int network_set_connection_static_ipv4(
    NmConnection *self,
    const char *ip,
    int prefix,
    const char *gateway,
	bool dns_flag)
{
	GError *error = NULL;
	g_return_val_if_fail(self != NULL, -1);
	g_return_val_if_fail(ip != NULL, -1);
	g_return_val_if_fail(prefix > 0 && prefix <= 32, -1);

    GVariantBuilder settings;
    g_variant_builder_init(&settings, G_VARIANT_TYPE("a{sa{sv}}"));
	network_gvariant_build_add_ipv4_static(&settings, ip, prefix, gateway, NULL, 0);
	network_gvariant_build_add_ipv6_ignore(&settings);
    
	if (!dns_flag) {
        GVariantBuilder ipv4;
        g_variant_builder_init(&ipv4, G_VARIANT_TYPE("a{sv}"));
        g_variant_builder_add(&ipv4, "{sv}", "ignore-auto-dns", g_variant_new_boolean(TRUE));
        g_variant_builder_add(&settings, "{sa{sv}}", "ipv4", &ipv4);
    }


	//GVariant *v = g_variant_builder_end(&settings);	//封装成GVariant更安全
	//nm_connection_update()
	//g_variant_unref(v);

	nm_connection_update(self, &settings, &error);
}

//设置网络配置为动态DHCP，如需应用于网卡需要调用 network_manager_activate_connection_to_device 接口应用到网卡
int network_set_connection_ipv4_dhcp(NmConnection *self)
{
    GError *error = NULL;
    g_return_val_if_fail(self != NULL, -1);

    GVariantBuilder settings;
    g_variant_builder_init(&settings, G_VARIANT_TYPE("a{sa{sv}}"));

    network_gvariant_build_add_ipv4_dhcp(&settings);
    network_gvariant_build_add_ipv6_ignore(&settings);

    return nm_connection_update(self, &settings, &error);
}


//打开网卡设备(NmDevice)
NmDevice *network_open_nm_device(GDBusConnection *dbus_conn, NetworkManager *nm, const char *name)
{
	GError *error = NULL;
	char *path=network_manager_get_device_path_by_iface(nm, name, &error);

	if(!path)
		path = network_manager_get_device_path_by_iface_fallback(nm, name, &error);	//使用保底方案尝试

	if(!path)
		return NULL;

	NmDevice *nmd = nm_device_create(dbus_conn, path, &error);
	free(path);
	return nmd;
}

//关闭网卡设备(NmDevice)
void network_close_nm_device(NmDevice *nmd)
{
	if(nmd)
		nm_device_delete(nmd);
}
