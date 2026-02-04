/*///------------------------------------------------------------------------------------------------------------------------//
		对NetworkManager相关的接口的初级封装
说 明 : 
日 期 : 2026.1.28

/*///------------------------------------------------------------------------------------------------------------------------//

#include <arpa/inet.h>
#include "Network/NetworkAppConf.h"
#include "Network/NetworkMidInterface.h"

//此函数调用后必须手动为配置设置method字段，这是强制要求的，若不想手动处理，可以使用network_gvariant_build_add_ipv4_dhcp
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
//conn_name：配置名
//ifname：网卡名称,可以传空，表示不指定网卡，即使设置了网卡只代表次配置优先对改网卡生效，如果使用热插拔的网卡建议不要指定
NmConnection *network_open_nm_connection(GDBusConnection *dbus_conn, 
									NmSettings *nms, 
									const char *conn_name, 
									const char *ifname)
{
    g_return_val_if_fail(dbus_conn != NULL, NULL);
    g_return_val_if_fail(nms != NULL, NULL);
    g_return_val_if_fail(conn_name != NULL, NULL);

	GError *error = NULL;
	NmConnection *nmc;
	//查找配置
	char *path = nm_settings_find_connection_object(nms, conn_name, &error);
	if(path)
	{
		nmc = nm_connection_create(dbus_conn, path, &error);
		g_free(path);
		return nmc;
	}
	g_free(path);
	printf("不存在配置，创建默认配置\n");
	//不存在就创建默认的空配置
    GVariantBuilder settings_builder;
    g_variant_builder_init(&settings_builder, G_VARIANT_TYPE("a{sa{sv}}"));
    network_gvariant_build_add_connection_basic(&settings_builder, conn_name, "802-3-ethernet", ifname);
    network_gvariant_build_add_ipv4_dhcp(&settings_builder);
	GVariant *v = g_variant_builder_end(&settings_builder);	//封装成GVariant

    path = nm_settings_add_connection(nms, v, &error);
	g_variant_unref(v);
	if(!path)
		return NULL;
	printf("创建默认配置成功\n");
	nmc = nm_connection_create(dbus_conn, path, &error);
	g_free(path);
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
    NetworkMidContext *ctx,
    const char *ip,
    int prefix,
    const char *gateway,
	bool dns_flag)
{

	NmConnection *self=ctx->nmc;
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

	GVariant *v = g_variant_builder_end(&settings);	//封装成GVariant更安全
	nm_connection_update(self, v, &error);
	g_variant_unref(v);
}

//设置网络配置为动态DHCP，如需应用于网卡需要调用 network_manager_activate_connection_to_device 接口应用到网卡
int network_set_connection_ipv4_dhcp(NetworkMidContext *ctx)
{
	NmConnection *self = ctx->nmc;
    GError *error = NULL;
    g_return_val_if_fail(self != NULL, -1);

    GVariant *current_settings = nm_connection_get_settings(self, &error);  
    if (!current_settings) {  
        printf("[ERROR] Failed to get current settings: %s\n",   
               error ? error->message : "Unknown error");  
        return -1;  
    }  
	GVariantBuilder new_settings;  
    g_variant_builder_init(&new_settings, G_VARIANT_TYPE("a{sa{sv}}"));  

    GVariantIter *iter;  
    const gchar *setting_name;  
    GVariant *setting_value;  
      
    g_variant_get(current_settings, "a{sa{sv}}", &iter);  
    while (g_variant_iter_loop(iter, "{s@a{sv}}", &setting_name, &setting_value)) 
	{  
        if (g_strcmp0(setting_name, "ipv4") == 0) 
		{  
            // 修改IPv4设置为DHCP  
            GVariantBuilder ipv4_builder;  
            GVariantIter *ipv4_iter;  
            const gchar *prop_name;  
            GVariant *prop_value;  
              
            g_variant_builder_init(&ipv4_builder, G_VARIANT_TYPE("a{sv}"));  

            // 复制现有的IPv4属性（除了method）  
            g_variant_get(setting_value, "a{sv}", &ipv4_iter);
            while (g_variant_iter_loop(ipv4_iter, "{s@v}", &prop_name, &prop_value)) {  
                if (g_strcmp0(prop_name, "method") != 0) {  
                    g_variant_builder_add(&ipv4_builder, "{s@v}", prop_name, prop_value);  
                }  
            }  
            g_variant_iter_free(ipv4_iter);

            // 设置DHCP方法  
            g_variant_builder_add(&ipv4_builder, "{sv}", "method", g_variant_new_string("auto"));  
              
            // 添加修改后的IPv4设置
            g_variant_builder_add(&new_settings, "{s@a{sv}}", "ipv4", g_variant_builder_end(&ipv4_builder));  
        }
		else 
		{
            // 直接复制其他设置（包括connection）
            g_variant_builder_add(&new_settings, "{s@a{sv}}", setting_name, setting_value);  
        }
    }  
    g_variant_iter_free(iter);  
      
    // 更新连接  
    GVariant *v = g_variant_builder_end(&new_settings);  
    int ret = nm_connection_update(self, v, &error);  
      
    if (ret == 0) {  
        printf("DHCP设置更新成功\n");  
    } else {  
        printf("DHCP设置更新失败: %s\n",   
               error ? error->message : "Unknown error");  
    }  
      
    g_variant_unref(v);  
    g_variant_unref(current_settings);  
    return ret;  
}

int network_set_ipv6_dns_mode(NetworkMidContext *ctx, bool isauto)
{
	NmConnection *self = ctx->nmc;
	GError *error = NULL;
	nm_connection_set_dns_mode(self, "ipv6", (gboolean)isauto, &error);
	if (error != NULL) {
		g_printerr("设置IPv6 DNS失败: %s (code: %d)\n", error->message, error->code);        
		g_error_free(error);
		return -1;
	}
	return 0;
}

int network_set_ipv4_dns_mode(NetworkMidContext *ctx, bool isauto)
{
	NmConnection *self = ctx->nmc;
	GError *error = NULL;
	nm_connection_set_dns_mode(self, "ipv6", (gboolean)isauto, &error);
	if (error != NULL) {
		g_printerr("设置IPv4 DNS失败: %s (code: %d)\n", error->message, error->code);        
		g_error_free(error);
		return -1;
	}
	return 0;
}

int network_get_ipv6_dns_mode(NetworkMidContext *ctx)
{
	NmConnection *self = ctx->nmc;
	GError *error = NULL;
	return nm_connection_get_dns_mode(self, "ipv6", &error);
}

int network_get_ipv4_dns_mode(NetworkMidContext *ctx)
{
	NmConnection *self = ctx->nmc;
	GError *error = NULL;
	return nm_connection_get_dns_mode(self, "ipv4", &error);
}


int network_set_ipv4_dns_list(NetworkMidContext *ctx, 
								const char **dns,  
                                uint32_t dns_count)
{
	NmConnection *self = ctx->nmc;
    GError *error = NULL;

	nm_connection_set_ipv4_dns_list(self, dns, dns_count, &error);
	if (error != NULL) {
        g_printerr("设置DNS失败: %s (code: %d)\n", error->message, error->code);        
        g_error_free(error);
		return -1;
    }
	return 0;
}

int network_get_ipv4_dns_list(NetworkMidContext *ctx, char ***dns)
{
	NmConnection *self = ctx->nmc;
	GError *error = NULL;
	return nm_connection_get_ipv4_dns_list(self, dns, &error);
}

int network_get_ipv4_dhcp_state(NetworkMidContext *ctx)
{
	NmConnection *self = ctx->nmc;
	GError *error = NULL;
	return nm_connection_get_ipv4_dhcp_state(self, &error);
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







/*c++接口：

tpInt32 TpNetworkConfig::setDhcp()
{
	TpNetworkConfigData *device = static_cast<TpNetworkConfigData *>(data_);
	network_set_connection_ipv4_dhcp(device->nmc);
	return network_manager_activate_connection_to_device(device->nm, device->nmc,device->nmd,NULL);
}


tpInt32 TpNetworkConfig::setStatic(const TpString &ip, const TpString &gatway, const TpString &netmask, TpList<TpString> &dns)
{
	TpNetworkConfigData *device = static_cast<TpNetworkConfigData *>(data_);

    int prefix = netmask_to_prefix(netmask.c_str());
    if (prefix < 0)
    {
        fprintf(stderr, "Netmask type is error\n");
        return -1;
    }
	bool dns_flag = dns.size() == 0 ? true : false;

    if (network_set_connection_static_ipv4(device->nmc, ip.c_str(), prefix, gatway.c_str(), dns_flag) < 0)
	{
		if(network_manager_activate_connection_to_device(device->nm, device->nmc,device->nmd,NULL)<0)
			return -1;
	}
    if (dns_flag == 1)
        return 0;
    if (setDns(TP_FALSE, dns) < 0)
        return -1;
    return 0;
}

tpBool TpNetworkConfig::isDhcp()
{
    TpNetworkConfigData *device = static_cast<TpNetworkConfigData *>(data_);

    int ret = nm_connection_get_ipv4_dhcp_state(device->nmc,NULL);
    if (ret == 1)
        return TP_TRUE;
    else if(ret == 0)
		return TP_FALSE;
    fprintf(stderr, "get dhcp status error\n");
    return TP_FALSE;
}

TpList<TpString> TpNetworkConfig::dns()
{
    TpNetworkConfigData *device = static_cast<TpNetworkConfigData *>(data_);
    TpList<TpString> list;
    char **dns_c;
    int count = nm_connection_get_ipv4_dns_list(device->nmc, &dns_c, NULL); //
    for (int i = 0; i < count; i++)
    {
        TpString dns(dns_c[i]);
        list.emplace_back(dns);
    }
	nm_connection_free_ipv4_dns_list(dns_c);
	return list;
}

tpInt32 TpNetworkConfig::setDns(tpBool autoDns, const TpList<TpString> &dnsList)
{
    TpNetworkConfigData *device = static_cast<TpNetworkConfigData *>(data_);
    if (autoDns)
	{
        nm_connection_set_ipv4_dns_mode(device->nmc, true, NULL);
		network_manager_activate_connection_to_device(device->nm, device->nmc, device->nmd, NULL);
		return 0;
	}

	nm_connection_set_ipv4_dns_mode(device->nmc, false, NULL);
    int len = dnsList.size();
    char *dns_servers[len];
    int count = 0;
    for (auto &dns : dnsList)
    {
        if (count == len)
            break;
        dns_servers[count] = strdup(dns.c_str());
        count++;
    }
	
    nm_connection_set_ipv4_dns_list(device->nmc, (const char **)dns_servers, count, NULL);
	if (network_manager_activate_connection_to_device(device->nm, device->nmc, device->nmd, NULL) < 0)
    {
        printf("[ERROR] Failed to activate connection\n");
    }
	printf("设置dns列表成功\n");
    do
    {
        count--;
        free(dns_servers[count]);
    } while (count);
    return 0;
}

tpBool TpNetworkConfig::isStaticDns()
{
	TpNetworkConfigData *device = static_cast<TpNetworkConfigData *>(data_);
	if(!device->nmc)
	{
		printf("device->nmc是空的？？？？\n");
	}
	int ret = nm_connection_get_ipv4_dns_mode(device->nmc,NULL);
	if(ret == 1)
		return TP_FALSE;
	else if(ret == 0)
		return TP_TRUE;
	fprintf(stderr,"get dns status error\n");
	return TP_TRUE;
}
*/
