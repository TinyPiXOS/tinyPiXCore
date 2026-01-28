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
#include "Network/NetworkManagerPriv.h"
#include "Network/NetworkManager.h"
#include "Network/NmConnection.h"


static GDBusConnection *system_conn = NULL;
static void nm_connection_dispose(GObject *object);
static void nm_connection_finalize(GObject *object);
static GDBusProxy *nm_connection_create_proxy(const gchar *path, GError **error);

struct NmConnectionPrivate_{
	GDBusProxy *proxy;
	//gchar *object_path;	//网卡的object_path，暂时去掉，此文件专注于配置，剥离硬件
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
    //self->priv->object_path = NULL;
}

static void nm_connection_dispose(GObject *gobject)
{
    NmConnection *self = NM_CONNECTION(gobject);

    g_clear_object(&self->priv->proxy);
    //g_free(self->priv->object_path);

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
        system_conn,                        // 连接对象
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

NmConnection *nm_connection_create(GDBusConnection *conn, const char *object_path)
{
	GError *error;
    g_return_val_if_fail(conn != NULL, NULL);
    g_return_val_if_fail(object_path != NULL, NULL);

    NmConnection *self = g_object_new(NM_CONNECTION_TYPE, NULL);
    //self->priv->object_path = g_strdup(object_path);

    self->priv->proxy = nm_connection_create_proxy((const gchar *)object_path, &error);
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
	g_return_val_if_fail(NETWORK_MANAGER_IS(self), NULL);
	return self->priv->proxy;
}

static GVariant *nm_connection_get_settings(NmConnection *self, GError **error)
{
    g_return_val_if_fail(NETWORK_MANAGER_IS(self), NULL);
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
		return NULL;

	g_variant_get(ret, "(@a{sa{sv}})", &settings);
	g_variant_unref(ret);
	return settings;
}

static int nm_connection_update(NmConnection *self, GVariantBuilder *settings, GError **error)
{
	g_return_val_if_fail(NETWORK_MANAGER_IS(self), -1);
    g_return_val_if_fail(self->priv->proxy != NULL, -1);

	g_dbus_proxy_call_sync(self->priv->proxy,
		"Update",
		g_variant_new("(a{sa{sv}})", settings),
		G_DBUS_CALL_FLAGS_NONE,
		-1,
		NULL,
		error);
	return 0;
}



// 开启 DHCP
bool nm_connection_enable_ipv4_dhcp(NmConnection *self,
                                    const char *conn_name,
                                    const char *ifname,
                                    GError **error)
{
    g_return_val_if_fail(self != NULL, false);

    //此处增加配置检查

    // 设置 DHCP 自动
    nm_connection_set_ipv4_dhcp_is_enabled(self, true, error);
    if (error && *error) return false;

    // DNS 自动
    nm_connection_set_ipv4_dns_mode(self, true, error);
    if (error && *error) return false;

    // 应用连接
   // 此处增加配置链接
    return true;
}

// 设置静态 IP + 网关
void nm_connection_set_ipv4_static_ip(NmConnection *self,
                                      const char *ip,
                                      int prefix,
                                      const char *gateway,
                                      GError **error)
{
    g_return_if_fail(self != NULL);
    g_return_if_fail(ip != NULL);
    g_return_if_fail(gateway != NULL);

    GVariantBuilder ipv4;
    GVariantBuilder settings;

    g_variant_builder_init(&ipv4, G_VARIANT_TYPE("a{sv}"));
    g_variant_builder_add(&ipv4, "{sv}", "method", g_variant_new_string("manual"));

    gchar addr_buf[64];
    snprintf(addr_buf, sizeof(addr_buf), "%s/%d", ip, prefix);
    g_variant_builder_add(&ipv4, "{sv}", "address-data", g_variant_new_string(addr_buf));
    g_variant_builder_add(&ipv4, "{sv}", "gateway", g_variant_new_string(gateway));

    g_variant_builder_init(&settings, G_VARIANT_TYPE("a{sa{sv}}"));
    g_variant_builder_add(&settings, "{sa{sv}}", "ipv4", &ipv4);

    nm_connection_update(self, &settings, error);
}



//设置dhcp
void nm_connection_set_ipv4_dhcp_is_enabled(NmConnection *self,
                                     bool enable,
                                     GError **error)
{
	g_return_if_fail(NETWORK_MANAGER_IS(self));
    g_return_if_fail(self->priv->proxy != NULL);

    GVariantBuilder ipv4;
    GVariantBuilder settings;

    g_variant_builder_init(&ipv4, G_VARIANT_TYPE("a{sv}"));
    g_variant_builder_add(
        &ipv4, "{sv}",
        "method",
        g_variant_new_string(enable ? "auto" : "manual"));

    g_variant_builder_init(&settings, G_VARIANT_TYPE("a{sa{sv}}"));
    g_variant_builder_add(&settings, "{sa{sv}}", "ipv4", &ipv4);

    nm_connection_update(self, &settings, error);
}

// 查询 DHCP 状态：1=auto/DHCP, 0=manual/static, -1=error/no-active
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
void nm_connection_set_ipv4_dns_list(NmConnection *self,
                                 const char **dns,
                                 uint32_t dns_count,
                                 GError **error)
{
    g_return_if_fail(NETWORK_MANAGER_IS(self));
    g_return_if_fail(self->priv->proxy != NULL);

    GVariantBuilder dns_array;
    GVariantBuilder ipv4;
    GVariantBuilder settings;

    g_variant_builder_init(&dns_array, G_VARIANT_TYPE("au"));
    for (uint32_t i = 0; i < dns_count; i++) 
	{
        guint32 addr = inet_addr(dns[i]);
        g_variant_builder_add(&dns_array, "u", addr);
    }

    g_variant_builder_init(&ipv4, G_VARIANT_TYPE("a{sv}"));
    g_variant_builder_add(&ipv4, "{sv}", "dns", &dns_array);

    g_variant_builder_init(&settings, G_VARIANT_TYPE("a{sa{sv}}"));
    g_variant_builder_add(&settings, "{sa{sv}}", "ipv4", &ipv4);

    nm_connection_update(self, &settings, error);
}

//获取ipv4的静态dns列表
int nm_connection_get_ipv4_dns_list(NmConnection *self, char ***dns_list_out, GError **error)
{
    GVariant *settings;
    GVariant *ipv4;
    GVariant *dns_variant;
    gsize n;
    char **dns_list;

    g_return_val_if_fail(self != NULL, -1);
    g_return_val_if_fail(dns_list_out != NULL, -1);

    *dns_list_out = NULL;

    settings = nm_connection_get_settings(self, error);
    if (!settings)
        return -1;

    ipv4 = g_variant_lookup_value(settings, "ipv4", G_VARIANT_TYPE("a{sv}"));
    if (!ipv4) {
        g_variant_unref(settings);
        return 0;
    }

    dns_variant = g_variant_lookup_value(ipv4, "dns", G_VARIANT_TYPE("au"));
    if (!dns_variant) {
        g_variant_unref(ipv4);
        g_variant_unref(settings);
        return 0;
    }

    n = g_variant_n_children(dns_variant);
    if (n == 0) {
        g_variant_unref(dns_variant);
        g_variant_unref(ipv4);
        g_variant_unref(settings);
        return 0;
    }

    dns_list = g_new0(char *, n);   // ✅ 只分配 n 个

    for (gsize i = 0; i < n; i++) {
        guint32 addr;
        struct in_addr in;
        char buf[INET_ADDRSTRLEN];

        g_variant_get_child(dns_variant, i, "u", &addr);
        in.s_addr = addr;

        if (inet_ntop(AF_INET, &in, buf, sizeof(buf)))
            dns_list[i] = g_strdup(buf);
        else
            dns_list[i] = NULL; // 只做异常兜底
    }

    g_variant_unref(dns_variant);
    g_variant_unref(ipv4);
    g_variant_unref(settings);

    *dns_list_out = dns_list;
    return (int)n;
}

int nm_connection_free_ipv4_dns_list(char **dns_list_out)
{
	g_strfreev(dns_list_out);
}

//设置dns模式
//set:要设置的,暂时可选ipv4,ipv6
//mode:1动态，0静态
void nm_connection_set_dns_mode(NmConnection *self, const char *key, gboolean isauto, GError **error)
{
    g_return_if_fail(NETWORK_MANAGER_IS(self));
    g_return_if_fail(self->priv->proxy != NULL);

    GVariantBuilder ipv;
    GVariantBuilder settings;

    g_variant_builder_init(&ipv, G_VARIANT_TYPE("a{sv}"));

	//手动dns时需要忽略auto-dns
	g_variant_builder_add(&ipv, "{sv}", "ignore-auto-dns", g_variant_new_boolean(isauto==FALSE));

    g_variant_builder_init(&settings, G_VARIANT_TYPE("a{sa{sv}}"));
    g_variant_builder_add(&settings, "{sa{sv}}", key, &ipv);

    nm_connection_update(self, &settings, error);
}

//获取dns模式
int nm_connection_get_dns_mode(NmConnection *self, const char *key, GError **error)
{
    GVariant *settings = nm_connection_get_settings(self,error);
    if (!settings)
        return -1;

    GVariant *ipv = g_variant_lookup_value(settings, key, G_VARIANT_TYPE("a{sv}"));

    if (!ipv)
        goto out;

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


int nm_connection_disable_ipv4_dhcp_safe(
    NmConnection *self,
    const char *ip,
    int prefix,
    const char *gateway,
    const char **dns,
    uint32_t dns_count,
    GError **error
)
{
    g_return_val_if_fail(self != NULL, -1);

    GVariantBuilder ipv4;
    GVariantBuilder settings;

    g_variant_builder_init(&ipv4, G_VARIANT_TYPE("a{sv}"));

    // 1. manual
    g_variant_builder_add(&ipv4, "{sv}", "method", g_variant_new_string("manual"));

    // 2. IP
    gchar addr[64];
    snprintf(addr, sizeof(addr), "%s/%d", ip, prefix);
    g_variant_builder_add(&ipv4, "{sv}", "addresses", g_variant_new_strv((const gchar *[]){ addr }, 1));

    // 3. Gateway
    if (gateway)
        g_variant_builder_add(&ipv4, "{sv}", "gateway", g_variant_new_string(gateway));

    // 4. DNS（如果给了）
    if (dns && dns_count > 0) {
        GVariantBuilder dns_array;
        g_variant_builder_init(&dns_array, G_VARIANT_TYPE("au"));

        for (uint32_t i = 0; i < dns_count; i++)
            g_variant_builder_add(&dns_array, "u", inet_addr(dns[i]));

        g_variant_builder_add(&ipv4, "{sv}", "dns", &dns_array);
        g_variant_builder_add(&ipv4, "{sv}", "ignore-auto-dns", g_variant_new_boolean(TRUE));
    }

    g_variant_builder_init(&settings, G_VARIANT_TYPE("a{sa{sv}}"));
    g_variant_builder_add(&settings, "{sa{sv}}", "ipv4", &ipv4);

    return nm_connection_update(self, &settings, error);
}