

#include "blt_service.h"
#include "bluetooth_inc.h"

struct _ServiceScannerPrivate {
    GDBusObjectManager *manager;
    GList *services; // List of Service objects
    GMainLoop *loop;
};

G_DEFINE_TYPE_WITH_PRIVATE(ServiceScanner, service_scanner, G_TYPE_OBJECT)

static void service_scanner_dispose(GObject *object) {
    ServiceScanner *self = SERVICE_SCANNER(object);
    ServiceScannerPrivate *priv = service_scanner_get_instance_private(self);
    
    if (priv->manager) {
        g_object_unref(priv->manager);
        priv->manager = NULL;
    }
    
    if (priv->services) {
        g_list_free_full(priv->services, g_object_unref);
        priv->services = NULL;
    }
    
    if (priv->loop) {
        g_main_loop_unref(priv->loop);
        priv->loop = NULL;
    }
    
    G_OBJECT_CLASS(service_scanner_parent_class)->dispose(object);
}

static void service_scanner_class_init(ServiceScannerClass *klass) {
    GObjectClass *object_class = G_OBJECT_CLASS(klass);
    object_class->dispose = service_scanner_dispose;
}

static void service_scanner_init(ServiceScanner *self) {
    ServiceScannerPrivate *priv = service_scanner_get_instance_private(self);
    priv->manager = NULL;
    priv->services = NULL;
    priv->loop = NULL;
}

ServiceScanner *service_scanner_new(void) {
    return g_object_new(SERVICE_SCANNER_TYPE, NULL);
}



static void on_object_added(GDBusObjectManager *manager, 
                           GDBusObject *object, 
                           gpointer user_data) {
    ServiceScanner *scanner = SERVICE_SCANNER(user_data);
    ServiceScannerPrivate *priv = service_scanner_get_instance_private(scanner);
    
    const gchar *path = g_dbus_object_get_object_path(object);
    
    // 检查是否是服务对象
    if (g_str_has_prefix(path, "/org/bluez/service")) {
        GDBusInterface *iface = g_dbus_object_get_interface(object, SERVICE_DBUS_INTERFACE);
        
        if (iface) {
            // 创建服务对象
            Service *service = service_new(path);
            priv->services = g_list_append(priv->services, service);
            
            // 发出服务发现信号
            g_signal_emit_by_name(scanner, "service-discovered", service);
            
            g_object_unref(iface);
        }
    }
}

void service_scanner_start(ServiceScanner *scanner) {
    ServiceScannerPrivate *priv = service_scanner_get_instance_private(scanner);
    
    if (priv->manager) {
        return; // 已经在运行
    }
    
    // 创建对象管理器
    GError *error = NULL;
    priv->manager = g_dbus_object_manager_client_new_for_bus_sync(
        G_BUS_TYPE_SYSTEM,
        G_DBUS_OBJECT_MANAGER_CLIENT_FLAGS_NONE,
        "org.bluez",
        "/",
        NULL, NULL, NULL, NULL, &error);
    
    if (error) {
        g_warning("Failed to create object manager: %s", error->message);
        g_error_free(error);
        return;
    }
    
    // 连接信号
    g_signal_connect(priv->manager, "object-added", 
                    G_CALLBACK(on_object_added), scanner);
    
    // 获取现有对象
    GList *objects = g_dbus_object_manager_get_objects(priv->manager);
    for (GList *l = objects; l != NULL; l = l->next) {
        on_object_added(priv->manager, l->data, scanner);
    }
    g_list_free_full(objects, g_object_unref);
    
    // 创建主循环
    priv->loop = g_main_loop_new(NULL, FALSE);
    g_main_loop_run(priv->loop);
}

void service_scanner_stop(ServiceScanner *scanner) {
    ServiceScannerPrivate *priv = service_scanner_get_instance_private(scanner);
    
    if (priv->loop) {
        g_main_loop_quit(priv->loop);
    }
}

GList *service_scanner_get_services(ServiceScanner *scanner) {
    ServiceScannerPrivate *priv = service_scanner_get_instance_private(scanner);
    return priv->services;
}




// 服务发现回调
static void on_service_discovered(ServiceScanner *scanner, Service *service, gpointer user_data) {
    g_print("发现服务:\n");
    g_print("  UUID: %s\n", service_get_uuid(service));
    g_print("  名称: %s\n", service_get_name(service));
    
    gint channel = service_get_rfcomm_channel(service);
    if (channel != -1) {
        g_print("  RFCOMM通道: %d\n", channel);
    }
    
    gint psm = service_get_l2cap_psm(service);
    if (psm != -1) {
        g_print("  L2CAP PSM: %d\n", psm);
    }
    
    g_print("\n");
}

int blt_service_test() {
   // g_type_init();
    
    // 创建服务扫描器
    ServiceScanner *scanner = service_scanner_new();
    
    // 连接服务发现信号
    g_signal_connect(scanner, "service-discovered", 
                   G_CALLBACK(on_service_discovered), NULL);
    
    // 启动扫描
    g_print("开始扫描蓝牙服务...\n");
    service_scanner_start(scanner);
    
    // 清理
    g_object_unref(scanner);
    return 0;
}
