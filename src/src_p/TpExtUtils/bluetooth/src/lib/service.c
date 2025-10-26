
#include "bluetooth_inc.h"


struct ServicePrivate_{
	GDBusProxy *proxy;

	gchar *object_path;
	gchar *uuid;
	gchar *name;
	gint rfcomm_channel;
	gint l2cap_psm;
};


G_DEFINE_TYPE_WITH_PRIVATE(Service, service, G_TYPE_OBJECT);



static void service_dispose(GObject *object) {
    Service *self = SERVICE(object);
    ServicePrivate *priv = service_get_instance_private(self);
    
    if (priv->proxy) {
        g_object_unref(priv->proxy);
        priv->proxy = NULL;
    }
    
    g_clear_pointer(&priv->object_path, g_free);
    g_clear_pointer(&priv->uuid, g_free);
    g_clear_pointer(&priv->name, g_free);
    
    G_OBJECT_CLASS(service_parent_class)->dispose(object);
}

static void service_finalize(GObject *object) {
    Service *self = SERVICE(object);
    ServicePrivate *priv = service_get_instance_private(self);
    
    // 清理私有结构
    g_clear_pointer(&priv->object_path, g_free);
    g_clear_pointer(&priv->uuid, g_free);
    g_clear_pointer(&priv->name, g_free);
    
    G_OBJECT_CLASS(service_parent_class)->finalize(object);
}

static void service_class_init(ServiceClass *klass) {
    GObjectClass *object_class = G_OBJECT_CLASS(klass);
    object_class->dispose = service_dispose;
    object_class->finalize = service_finalize;
    
    // 信号定义
    g_signal_new("service-ready",
                 G_TYPE_FROM_CLASS(klass),
                 G_SIGNAL_RUN_FIRST,
                 G_STRUCT_OFFSET(ServiceClass, service_ready),
                 NULL, NULL, NULL,
                 G_TYPE_NONE, 1, SERVICE_TYPE);
    
    g_signal_new("service-error",
                 G_TYPE_FROM_CLASS(klass),
                 G_SIGNAL_RUN_FIRST,
                 G_STRUCT_OFFSET(ServiceClass, service_error),
                 NULL, NULL, NULL,
                 G_TYPE_NONE, 2, SERVICE_TYPE, G_TYPE_STRING);
}

//初始化，被自动调用
static void service_init(Service *self) {
    ServicePrivate *priv = service_get_instance_private(self);
    self->priv = priv;
    
    priv->proxy = NULL;
    priv->object_path = NULL;
    priv->uuid = NULL;
    priv->name = NULL;
    priv->rfcomm_channel = -1;
    priv->l2cap_psm = -1;
}


Service *service_new(const gchar *object_path) {
    Service *service = g_object_new(SERVICE_TYPE, NULL);
    ServicePrivate *priv = service_get_instance_private(service);
    
    priv->object_path = g_strdup(object_path);
    
    // 创建DBus代理
    GError *error = NULL;
    priv->proxy = g_dbus_proxy_new_for_bus_sync(
        G_BUS_TYPE_SYSTEM,
        G_DBUS_PROXY_FLAGS_NONE,
        NULL,
        SERVICE_DBUS_SERVER,
        object_path,
        SERVICE_DBUS_INTERFACE,
        NULL,
        &error);
    
    if (error) {
        g_signal_emit_by_name(service, "service-error", service, error->message);
        g_error_free(error);
        return service;
    }
    
    // 获取服务属性
    GVariant *uuid_var = g_dbus_proxy_get_cached_property(priv->proxy, "UUID");
    if (uuid_var) {
        priv->uuid = g_variant_dup_string(uuid_var, NULL);
        g_variant_unref(uuid_var);
    }
    
    GVariant *name_var = g_dbus_proxy_get_cached_property(priv->proxy, "Name");
    if (name_var) {
        priv->name = g_variant_dup_string(name_var, NULL);
        g_variant_unref(name_var);
    }
    
    // 获取协议描述符
    GVariant *proto_list_var = g_dbus_proxy_get_cached_property(priv->proxy, "ProtocolDescriptorList");
    if (proto_list_var) {
        GVariantIter iter;
        g_variant_iter_init(&iter, proto_list_var);
        
        GVariant *proto_desc;
        while ((proto_desc = g_variant_iter_next_value(&iter))) {
            GVariant *uuid_var;
            g_variant_get_child(proto_desc, 0, "v", &uuid_var);
            
            const gchar *proto_uuid = g_variant_get_string(uuid_var, NULL);
            
            if (g_str_equal(proto_uuid, "000001000-0000-1000-8000-00805F9B34FB")) { // L2CAP
                GVariant *params_var;
                g_variant_get_child(proto_desc, 1, "v", &params_var);
                priv->l2cap_psm = g_variant_get_uint16(params_var);
                g_variant_unref(params_var);
            }
            else if (g_str_equal(proto_uuid, "000003000-0000-1000-8000-00805F9B34FB")) { // RFCOMM
                GVariant *params_var;
                g_variant_get_child(proto_desc, 1, "v", &params_var);
                priv->rfcomm_channel = g_variant_get_byte(params_var);
                g_variant_unref(params_var);
            }
            
            g_variant_unref(uuid_var);
            g_variant_unref(proto_desc);
        }
        g_variant_unref(proto_list_var);
    }
    
    g_signal_emit_by_name(service, "service-ready", service);
    return service;
}

gchar *service_get_uuid(Service *service) {
    g_return_val_if_fail(IS_SERVICE(service), NULL);
    ServicePrivate *priv = service_get_instance_private(service);
    return g_strdup(priv->uuid);
}

gchar *service_get_name(Service *service) {
    g_return_val_if_fail(IS_SERVICE(service), NULL);
    ServicePrivate *priv = service_get_instance_private(service);
    return g_strdup(priv->name);
}

gint service_get_rfcomm_channel(Service *service) {
    g_return_val_if_fail(IS_SERVICE(service), -1);
    ServicePrivate *priv = service_get_instance_private(service);
    return priv->rfcomm_channel;
}

gint service_get_l2cap_psm(Service *service) {
    g_return_val_if_fail(IS_SERVICE(service), -1);
    ServicePrivate *priv = service_get_instance_private(service);
    return priv->l2cap_psm;
}

gchar *service_get_object_path(Service *service) {
    g_return_val_if_fail(IS_SERVICE(service), NULL);
    ServicePrivate *priv = service_get_instance_private(service);
    return g_strdup(priv->object_path);
}

