/*///------------------------------------------------------------------------------------------------------------------------//
		网卡管理(DBUS)Setting.Connection接口相关操作
说 明 : 使用DBUS的org.freedesktop.NetworkManager.Settings.Connection接口，主要是对网络配置的管理，和网卡硬件并非对应关系，一个硬件
		可能会有多种配置，但是多个硬件也可以使用一样的配置
日 期 : 2026.1.27
作 者 : Chingan

/*///------------------------------------------------------------------------------------------------------------------------//

#include <stdio.h>
#include <string.h>
#include <arpa/inet.h>
#include "Network/nm/NetworkManagerPriv.h"
#include "Network/nm/NetworkManager.h"
#include "Network/nm/NmUtils.h"
#include "Network/nm/NmConnection.h"


static GDBusConnection *dbus_conn = NULL;
static void nm_connection_dispose(GObject *object);
static void nm_connection_finalize(GObject *object);
static GDBusProxy *nm_connection_create_proxy(const gchar *path, GError **error);

struct NmConnectionPrivate_{
	GDBusProxy *proxy;
	//gchar *conn_path;	//网卡的conn_path，暂时去掉
};

//告诉glib自己的类型和私有结构体,前缀等
G_DEFINE_TYPE_WITH_PRIVATE(NmConnection, nm_connection, G_TYPE_OBJECT);

static void nm_connection_class_init(NmConnectionClass *klass)
{
    GObjectClass *gobject_class = G_OBJECT_CLASS(klass);
    gobject_class->dispose = nm_connection_dispose;
    gobject_class->finalize = nm_connection_finalize;
}

static void nm_connection_init(NmConnection *self)
{
    self->priv = nm_connection_get_instance_private(self);
    self->priv->proxy = NULL;
    //self->priv->conn_path = NULL;
}

static void nm_connection_dispose(GObject *gobject)
{
    NmConnection *self = NM_CONNECTION(gobject);

    g_clear_object(&self->priv->proxy);
    //g_free(self->priv->conn_path);

    G_OBJECT_CLASS(nm_connection_parent_class)->dispose(gobject);
}

static void nm_connection_finalize(GObject *gobject)
{
    // 如果有额外资源释放可以放这里
    G_OBJECT_CLASS(nm_connection_parent_class)->finalize(gobject);
}


static GDBusProxy *nm_connection_create_proxy(const gchar *path, GError **error)
{
    GDBusProxy *proxy = g_dbus_proxy_new_sync(
        dbus_conn,                        // 连接对象
        G_DBUS_PROXY_FLAGS_NONE,            // 标志
        NULL,                               // 自动 introspection
        NETWORK_MANAGER_DBUS_SERVER,		// D-Bus 服务
        path,                                // 对象路径
        NM_CONNECTION_INTERFACE,             // 接口名
        NULL,                                // GCancellable
        error
    );
    if (!proxy && error && *error) {
        g_warning("Failed to create NmConnection proxy: %s", (*error)->message);
		return NULL;
    }
	return proxy;
}

/// @brief 创建配置
/// @param conn dbus连接
/// @param conn_path 配置的路径，一般为"/org/freedesktop/NetworkManager/Settings/Connection/xxxx"，通常从NmSettings根据配置名获取
/// @return 
NmConnection *nm_connection_create(GDBusConnection *conn, const char *conn_path, GError **error)
{
    g_return_val_if_fail(conn != NULL, NULL);
    g_return_val_if_fail(conn_path != NULL, NULL);
	dbus_conn=conn;
    NmConnection *self = g_object_new(NM_CONNECTION_TYPE, NULL);
    //self->priv->conn_path = g_strdup(conn_path);

    self->priv->proxy = nm_connection_create_proxy((const gchar *)conn_path, error);
    if (!self->priv->proxy) {
        g_object_unref(self);
        return NULL;
    }

    return self;
}

int nm_connection_delete(NmConnection *self)
{
	if(!self)
		return 0;

	g_object_unref(self);
	return 0;
}

//私有,只有NetWorkManager可用
GDBusProxy* nm_connection_get_proxy_internal(NmConnection *self)
{
	g_return_val_if_fail(NM_CONNECTION_IS(self), NULL);
	return self->priv->proxy;
}

GVariant *nm_connection_get_settings(NmConnection *self, GError **error)
{
    g_return_val_if_fail(NM_CONNECTION_IS(self), NULL);
    g_return_val_if_fail(self->priv->proxy != NULL, NULL);

	GVariant *ret = NULL;
	GVariant *settings = NULL;

	ret = g_dbus_proxy_call_sync(self->priv->proxy, 
			"GetSettings", 
			NULL, 
			G_DBUS_CALL_FLAGS_NONE, 
			-1, 
			NULL, 
			error);

	if (!ret)
	{
		fprintf(stderr,"GetSetting error\n");
		return NULL;
	}

	g_variant_get(ret, "(@a{sa{sv}})", &settings);
	g_variant_unref(ret);
	return settings;
}
void nm_connection_free_settings(GVariant *settings)
{
	g_variant_unref(settings);
}

//注意：此接口外部不要轻易使用，GVariant的接口必须是"(a{sa{sv}})"
int nm_connection_update(NmConnection *self, GVariant *settings, GError **error)  
{  
    g_return_val_if_fail(NM_CONNECTION_IS(self), -1);  
    g_return_val_if_fail(self->priv->proxy != NULL, -1);  
	if (!g_variant_is_of_type(settings, G_VARIANT_TYPE("a{sa{sv}}"))) {  
        printf("[ERROR] Invalid settings type: %s\n",   
               g_variant_get_type_string(settings));  
        return -1;  
    }  
    // 正确构造参数：settings 需要被包装在 tuple 中  
	GVariant *params = network_variant_tuple1(settings);  

	// 打印即将发送的数据  
    gchar *settings_str = g_variant_print(settings, TRUE);  
    printf("[DEBUG] Sending settings: %s\n", settings_str);  
    g_free(settings_str);  

    GVariant *ret = g_dbus_proxy_call_sync(self->priv->proxy,  
        "Update",  
        params,
		G_DBUS_CALL_FLAGS_NONE,  
        -1,  
        NULL,  
        error);  
//	g_variant_unref(params);

	if (!ret) {  
        if (error && *error) {  
            printf("[ERROR] Update failed: %s\n", (*error)->message);  
            printf("[ERROR] Domain: %d, Code: %d\n", (*error)->domain, (*error)->code);  
              
            // 尝试获取 D-Bus 错误名称  
            if (g_error_matches(*error, G_DBUS_ERROR, G_DBUS_ERROR_FAILED)) {  
                printf("[ERROR] D-Bus error: org.freedesktop.DBus.Error.Failed\n");  
            }  
        } else {  
            printf("[ERROR] Update failed with NULL error - D-Bus call returned NULL\n");  
        }  
        return -1;  
    }  


	printf("[Debug]:Update success\n");
    g_variant_unref(ret);  
    return 0;  
}


  
// 通用设置修改函数  
int nm_connection_modify_setting(NmConnection *self,  
                               const char *setting_name,  
                               NmSettingModifyFunc modify_func,  
                               gpointer user_data,  
                               GError **error)  
{  
    g_return_val_if_fail(NM_CONNECTION_IS(self), -1);  
    g_return_val_if_fail(setting_name != NULL, -1);  
    g_return_val_if_fail(modify_func != NULL, -1);  
      
    // 获取当前完整设置  
    GVariant *current_settings = nm_connection_get_settings(self, error);  
    if (!current_settings) {  
        return -1;  
    }  
      
    // 创建新的设置构建器  
    GVariantBuilder new_settings;  
    g_variant_builder_init(&new_settings, G_VARIANT_TYPE("a{sa{sv}}"));  
      
    // 复制所有现有设置  
    GVariantIter *iter;  
    const gchar *cur_setting_name;  
    GVariant *setting_value;  
      
    g_variant_get(current_settings, "a{sa{sv}}", &iter);  
    while (g_variant_iter_loop(iter, "{s@a{sv}}", &cur_setting_name, &setting_value)) {  
        if (g_strcmp0(cur_setting_name, setting_name) == 0) {  
            // 修改指定的设置  
            GVariant *modified = modify_func(setting_value, user_data, error);  
            if (!modified) {  
                g_variant_iter_free(iter);  
                g_variant_unref(current_settings);  
                return -1;  
            }  
            g_variant_builder_add(&new_settings, "{s@a{sv}}",   
                                 cur_setting_name, modified);  
        } else {  
            // 直接复制其他设置  
            g_variant_builder_add(&new_settings, "{s@a{sv}}",   
                                 cur_setting_name, setting_value);  
        }  
    }  
    g_variant_iter_free(iter);  
      
    // 更新连接  
    GVariant *v = g_variant_builder_end(&new_settings);  
    int ret = nm_connection_update(self, v, error);  
      
    g_variant_unref(v);  
    g_variant_unref(current_settings);  
    return ret;  
}


//设置dhcp是否使能
void nm_connection_set_ipv4_dhcp_is_enabled(NmConnection *self,
                                     bool enable,
                                     GError **error)
{
	g_return_if_fail(NM_CONNECTION_IS(self));
    g_return_if_fail(self->priv->proxy != NULL);

    GVariantBuilder ipv4;
    GVariantBuilder settings;

    g_variant_builder_init(&ipv4, G_VARIANT_TYPE("a{sv}"));
    g_variant_builder_add(&ipv4, "{sv}", "method", g_variant_new_string(enable ? "auto" : "manual"));

    g_variant_builder_init(&settings, G_VARIANT_TYPE("a{sa{sv}}"));
    g_variant_builder_add(&settings, "{sa{sv}}", "ipv4", &ipv4);

	GVariant *v = g_variant_builder_end(&settings);	//封装成GVariant更安全
	nm_connection_update(self, v, error);
	g_variant_unref(v);
}

// 获取 DHCP 状态：1=auto/DHCP, 0=manual/static, -1=error/no-active
int nm_connection_get_ipv4_dhcp_state(NmConnection *self,
                                          GError **error)
{
    GVariant *settings = NULL;
    GVariant *ipv4 = NULL;
    GVariant *method = NULL;

    settings = nm_connection_get_settings(self,error);
    if (!settings)
        return -1;

    ipv4 = g_variant_lookup_value(settings, "ipv4", G_VARIANT_TYPE("a{sv}"));

    if (!ipv4)
        goto out;

    method = g_variant_lookup_value(ipv4, "method", G_VARIANT_TYPE_STRING);

    if (!method)
        goto out;

    const gchar *m = g_variant_get_string(method, NULL);

    int state;
    if (g_strcmp0(m, "auto") == 0)
        state = 1;
    else
        state = 0;
    g_variant_unref(method);
    g_variant_unref(ipv4);
    g_variant_unref(settings);

    return state;

out:
    if (method)   g_variant_unref(method);
    if (ipv4)     g_variant_unref(ipv4);
    if (settings) g_variant_unref(settings);
    return -1;
}

//设置ipv4的静态dns列表
/*void nm_connection_set_ipv4_dns_list(NmConnection *self,
                                 const char **dns,
                                 uint32_t dns_count,
                                 GError **error)
{
    g_return_if_fail(NM_CONNECTION_IS(self));
    g_return_if_fail(self->priv->proxy != NULL);
	printf("nm_connection_set_ipv4_dns_list\n");
    GVariantBuilder dns_array;
    GVariantBuilder ipv4;
    GVariantBuilder settings;

    g_variant_builder_init(&dns_array, G_VARIANT_TYPE("as"));
    for (uint32_t i = 0; i < dns_count; i++) 
	{
		printf("dns[%d]:%s\n",i,dns[i]);
        guint32 addr = inet_addr(dns[i]);
        g_variant_builder_add(&dns_array, "s", dns[i]);
    }
	GVariant *dns_v = g_variant_builder_end(&dns_array);
	printf("dns_v 完成\n");

    g_variant_builder_init(&ipv4, G_VARIANT_TYPE("a{sv}"));
	g_variant_builder_add(&ipv4, "{sv}", "method", g_variant_new_string("manual")); // DHCP
	g_variant_builder_add(&ipv4, "{sv}", "ignore-auto-dns", g_variant_new_boolean(TRUE));
    g_variant_builder_add(&ipv4, "{sv}", "dns-data", (dns_v));	//当dns_v被加到ipv4时，dns_v的生命周期交给ipv4，释放时只需要释放父指针
	//g_variant_new_variant(dns_v)

    g_variant_builder_init(&settings, G_VARIANT_TYPE("a{sa{sv}}"));
    g_variant_builder_add(&settings, "{sa{sv}}", "ipv4", &ipv4);

    GVariant *v = g_variant_builder_end(&settings);	
	nm_connection_update(self, v, error);

	//g_variant_unref(dns_v);//dns_v的生命周期交给ipv4再交给v
	g_variant_unref(v);
}
//获取ipv4的静态dns列表
int nm_connection_get_ipv4_dns_list(NmConnection *self, char ***dns_list_out, GError **error)
{
    g_return_val_if_fail(self != NULL, -1);
    g_return_val_if_fail(dns_list_out != NULL, -1);

    *dns_list_out = NULL;
    printf("[DEBUG] Start nm_connection_get_ipv4_dns_list\n");

    GVariant *settings = nm_connection_get_settings(self, error);
    if (!settings) {
        printf("[DEBUG] Failed to get settings\n");
        return -1;
    }
    printf("[DEBUG] Got settings\n");

    GVariant *ipv4 = g_variant_lookup_value(settings, "ipv4", G_VARIANT_TYPE("a{sv}"));
    if (!ipv4) {
        printf("[DEBUG] No ipv4 section found\n");
        g_variant_unref(settings);
        return 0;
    }
    printf("[DEBUG] Got ipv4 section\n");

    // 尝试取 dns 字段
    GVariant *dns_field = g_variant_lookup_value(ipv4, "dns-data", G_VARIANT_TYPE("as"));
    if (!dns_field) {
        printf("[DEBUG] No dns field found\n");
        g_variant_unref(ipv4);
        g_variant_unref(settings);
        return 0;
    }

    printf("[DEBUG] Got dns field, type: %s\n", g_variant_get_type_string(dns_field));

    GVariant *dns_array = NULL;

    if (g_variant_is_of_type(dns_field, G_VARIANT_TYPE_VARIANT)) {
        dns_array = g_variant_get_variant(dns_field);
        printf("[DEBUG] dns_field is a variant\n");
    } else if (g_variant_is_of_type(dns_field, G_VARIANT_TYPE("as"))) {
        dns_array = dns_field;
        printf("[DEBUG] dns_field is directly au array\n");
    } else {
        printf("[DEBUG] dns_field type unexpected: %s\n", g_variant_get_type_string(dns_field));
        g_variant_unref(dns_field);
        g_variant_unref(ipv4);
        g_variant_unref(settings);
        return 0;
    }

    gsize n = g_variant_n_children(dns_array);
    printf("[DEBUG] dns count: %zu\n", n);
    if (n == 0) {
        if (dns_array != dns_field) g_variant_unref(dns_array);
        g_variant_unref(dns_field);
        g_variant_unref(ipv4);
        g_variant_unref(settings);
        return 0;
    }

    char **dns_list = g_new0(char *, n);
    for (gsize i = 0; i < n; i++) {
        guint32 addr;
        struct in_addr in;
        char buf[INET_ADDRSTRLEN];

        g_variant_get_child(dns_array, i, "u", &addr);
        in.s_addr = addr;

        if (inet_ntop(AF_INET, &in, buf, sizeof(buf)))
            dns_list[i] = g_strdup(buf);
        else
            dns_list[i] = NULL;

        printf("[DEBUG] dns_list[%zu]: %s\n", i, dns_list[i]);
    }

    if (dns_array != dns_field) g_variant_unref(dns_array);
    g_variant_unref(dns_field);
    g_variant_unref(ipv4);
    g_variant_unref(settings);

    *dns_list_out = dns_list;
    printf("[DEBUG] Finished nm_connection_get_ipv4_dns_list\n");

    return (int)n;
}*/



void nm_connection_set_ipv4_dns_list(NmConnection *self,  
                                 const char **dns,  
                                 uint32_t dns_count,  
                                 GError **error)  
{  
    g_return_if_fail(NM_CONNECTION_IS(self));  
    g_return_if_fail(self->priv->proxy != NULL);  
      
    printf("nm_connection_set_ipv4_dns_list\n");  
      
    // 获取当前完整设置  
    GVariant *current_settings = nm_connection_get_settings(self, error);  
    if (!current_settings) {  
        printf("[ERROR] Failed to get current settings\n");  
        return;  
    }  
      
    // 创建新的设置构建器  
    GVariantBuilder new_settings;  
    g_variant_builder_init(&new_settings, G_VARIANT_TYPE("a{sa{sv}}"));  
      
    // 复制所有现有设置  
    GVariantIter *iter;  
    const gchar *setting_name;  
    GVariant *setting_value;  
      
    g_variant_get(current_settings, "a{sa{sv}}", &iter);  
    while (g_variant_iter_loop(iter, "{s@a{sv}}", &setting_name, &setting_value)) {  
        if (g_strcmp0(setting_name, "ipv4") == 0) {  
            // 修改IPv4设置中的DNS  
            GVariantBuilder ipv4_builder;  
            GVariantBuilder dns_array;  
            GVariantIter *ipv4_iter;  
            const gchar *prop_name;  
            GVariant *prop_value;  
              
            // 构建DNS数组 - 使用 uint32 数组类型 "au"  
            g_variant_builder_init(&dns_array, G_VARIANT_TYPE("au"));  
            for (uint32_t i = 0; i < dns_count; i++) {  
                printf("dns[%d]:%s\n", i, dns[i]);  
                // 将IP地址转换为网络字节序的uint32  
                struct in_addr addr;  
                if (inet_aton(dns[i], &addr)) {  
                    // 直接使用网络字节序地址，不需要转换  
                    g_variant_builder_add(&dns_array, "u", addr.s_addr);  
                } else {  
                    printf("[WARNING] Invalid IP address: %s\n", dns[i]);  
                }  
            }  
              
            g_variant_builder_init(&ipv4_builder, G_VARIANT_TYPE("a{sv}"));  
              
            // 复制现有的IPv4属性  
            g_variant_get(setting_value, "a{sv}", &ipv4_iter);  
            while (g_variant_iter_loop(ipv4_iter, "{s@v}", &prop_name, &prop_value)) {  
                // 跳过旧的DNS属性  
                if (g_strcmp0(prop_name, "dns") != 0 &&   
                    g_strcmp0(prop_name, "dns-data") != 0) {  
                    g_variant_builder_add(&ipv4_builder, "{s@v}", prop_name, prop_value);  
                }  
            }  
            g_variant_iter_free(ipv4_iter);  
              
            // 添加新的DNS设置 - 使用旧的 "dns" 属性  
            g_variant_builder_add(&ipv4_builder, "{sv}", "dns",   
                                  g_variant_builder_end(&dns_array));  
            g_variant_builder_add(&ipv4_builder, "{sv}", "ignore-auto-dns",   
                                  g_variant_new_boolean(TRUE));  
              
            // 添加修改后的IPv4设置  
            g_variant_builder_add(&new_settings, "{s@a{sv}}", "ipv4",   
                                  g_variant_builder_end(&ipv4_builder));  
        } else {  
            // 直接复制其他设置  
            g_variant_builder_add(&new_settings, "{s@a{sv}}", setting_name, setting_value);  
        }  
    }  
    g_variant_iter_free(iter);  
      
    // 更新连接  
    GVariant *v = g_variant_builder_end(&new_settings);  
      
    printf("Updating connection...\n");  
    int ret = nm_connection_update(self, v, error);  
      
    if (ret == 0) {  
        printf("DNS设置更新成功\n");  
    } else {  
        printf("DNS设置更新失败，错误代码: %d\n", ret);  
        if (error && *error) {  
            printf("错误详情: %s\n", (*error)->message);  
        }  
    }  
      
    g_variant_unref(v);  
    g_variant_unref(current_settings);  
}

int nm_connection_get_ipv4_dns_list(NmConnection *self, char ***dns_list_out, GError **error)  
{  
    g_return_val_if_fail(self != NULL, -1);  
    g_return_val_if_fail(dns_list_out != NULL, -1);  
  
    *dns_list_out = NULL;  
    printf("[DEBUG] Start nm_connection_get_ipv4_dns_list\n");  
  
    GVariant *settings = nm_connection_get_settings(self, error);  
    if (!settings) {  
        printf("[DEBUG] Failed to get settings\n");  
        return -1;  
    }  
      
    GVariant *ipv4 = g_variant_lookup_value(settings, "ipv4", G_VARIANT_TYPE("a{sv}"));  
    if (!ipv4) {  
        printf("[DEBUG] No ipv4 section found\n");  
        g_variant_unref(settings);  
        return 0;  
    }  
      
    // 查找 "dns" 字段（旧的 uint32 数组格式）  
    GVariant *dns_field = g_variant_lookup_value(ipv4, "dns", G_VARIANT_TYPE("au"));  
    if (!dns_field) {  
        printf("[DEBUG] No dns field found\n");  
        g_variant_unref(ipv4);  
        g_variant_unref(settings);  
        return 0;  
    }  
  
    gsize n = g_variant_n_children(dns_field);  
    printf("[DEBUG] dns count: %zu\n", n);  
    if (n == 0) {  
        g_variant_unref(dns_field);  
        g_variant_unref(ipv4);  
        g_variant_unref(settings);  
        return 0;  
    }  
  
    char **dns_list = g_new0(char *, n + 1);  
    for (gsize i = 0; i < n; i++) {  
        guint32 addr;  
        struct in_addr in;  
        char buf[INET_ADDRSTRLEN];  
          
        g_variant_get_child(dns_field, i, "u", &addr);  
        in.s_addr = addr;  // 直接使用，不需要转换  
          
        if (inet_ntop(AF_INET, &in, buf, sizeof(buf))) {  
            dns_list[i] = g_strdup(buf);  
        } else {  
            dns_list[i] = NULL;  
        }  
          
        printf("[DEBUG] dns_list[%zu]: %s\n", i, dns_list[i] ? dns_list[i] : "NULL");  
    }  
  
    g_variant_unref(dns_field);  
    g_variant_unref(ipv4);  
    g_variant_unref(settings);  
  
    *dns_list_out = dns_list;  
    printf("[DEBUG] Finished nm_connection_get_ipv4_dns_list\n");  
  
    return (int)n;  
}


int nm_connection_free_ipv4_dns_list(char **dns_list_out)
{
	g_strfreev(dns_list_out);
}

//设置dns模式
//set:要设置的,暂时可选:ipv4,ipv6
//mode:1动态，0静态
void nm_connection_set_dns_mode(NmConnection *self, const char *key, gboolean isauto, GError **error)
{
    g_return_if_fail(NM_CONNECTION_IS(self));
    g_return_if_fail(self->priv->proxy != NULL);

    GVariantBuilder ipv;
    GVariantBuilder settings;

    g_variant_builder_init(&ipv, G_VARIANT_TYPE("a{sv}"));

	//手动dns时需要忽略auto-dns
	g_variant_builder_add(&ipv, "{sv}", "ignore-auto-dns", g_variant_new_boolean(isauto==FALSE));

    g_variant_builder_init(&settings, G_VARIANT_TYPE("a{sa{sv}}"));
    g_variant_builder_add(&settings, "{sa{sv}}", key, &ipv);
	printf("更新配置\n");
    GVariant *v = g_variant_builder_end(&settings);	//封装成GVariant更安全
	nm_connection_update(self, v, error);
	g_variant_unref(v);
	printf("更新配置成功\n");
}

//获取dns模式
int nm_connection_get_dns_mode(NmConnection *self, const char *key, GError **error)
{
	if(!self)
	{
		printf("self is null\n");
	}
    GVariant *settings = nm_connection_get_settings(self,error);
    if (!settings)
	{
		printf("get settings error\n");
		return -1;
	}

    GVariant *ipv = g_variant_lookup_value(settings, key, G_VARIANT_TYPE("a{sv}"));

    if (!ipv)
	{
		printf("get a{sv} error\n");
        goto out;
	}

    GVariant *ignore = g_variant_lookup_value(ipv, "ignore-auto-dns", G_VARIANT_TYPE_BOOLEAN);

    // 默认行为：auto DNS
    int mode = 1;

    if (ignore) {

        if (g_variant_get_boolean(ignore))
            mode = 0;
        g_variant_unref(ignore);
    }

    g_variant_unref(ipv);
    g_variant_unref(settings);
    return mode;

out:
    g_variant_unref(settings);
	printf("nm_connection_get_dns_mode error\n");
    return -1;
}

void nm_connection_set_ipv6_dns_mode(NmConnection *self, bool isauto, GError **error)
{
	nm_connection_set_dns_mode(self, "ipv6", (gboolean)isauto, error);
}

void nm_connection_set_ipv4_dns_mode(NmConnection *self, bool isauto, GError **error)
{
	nm_connection_set_dns_mode(self, "ipv4", (gboolean)isauto, error);
}

//返回1为自动，0为静态，-1为失败
int nm_connection_get_ipv6_dns_mode(NmConnection *self, GError **error)
{
	return nm_connection_get_dns_mode(self, "ipv6", error);
}

//返回1为自动，0为静态，-1为失败
int nm_connection_get_ipv4_dns_mode(NmConnection *self, GError **error)
{
	return nm_connection_get_dns_mode(self, "ipv4", error);
}



bool nm_connection_ipv4_manual_needs_params(NmConnection *self, GError **error)
{
    GVariant *settings = nm_connection_get_settings(self, error);
    if (!settings)
        return true;

    GVariant *ipv4 = g_variant_lookup_value(settings, "ipv4", G_VARIANT_TYPE("a{sv}"));
    if (!ipv4) {
        g_variant_unref(settings);
        return true;
    }

    GVariant *method = g_variant_lookup_value(ipv4, "method", G_VARIANT_TYPE_STRING);
    if (!method) {
        g_variant_unref(ipv4);
        g_variant_unref(settings);
        return true;
    }

    const char *m = g_variant_get_string(method, NULL);
    if (g_strcmp0(m, "manual") != 0) {
        g_variant_unref(method);
        g_variant_unref(ipv4);
        g_variant_unref(settings);
        return false;   // DHCP 模式，不需要
    }

    // manual 模式，必须有 address-data 或 addresses
    gboolean has_addr = (g_variant_lookup_value(ipv4, "address-data", NULL) || g_variant_lookup_value(ipv4, "addresses", NULL));

    g_variant_unref(method);
    g_variant_unref(ipv4);
    g_variant_unref(settings);

    return !has_addr;
}


