/*///------------------------------------------------------------------------------------------------------------------------//
		蓝牙glib接口调用相关
说 明 : 
日 期 : 2025.3.21

/*///------------------------------------------------------------------------------------------------------------------------//

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <gio/gio.h>

#include "blt_glibscan.h"

#define ADAPTER_PATH "/org/bluez/hci1"



struct BluetScanFilter_{
	GDBusProxy      *proxy;
	GVariantBuilder  builder;		//用于构造过滤器字典
};

BluetScanFilter* scan_filter_new(GDBusConnection *conn,const gchar *adapter_path,GError **error)
{
    BluetScanFilter *self = g_new0(BluetScanFilter, 1);
    self->proxy = g_dbus_proxy_new_sync(
        conn,
        G_DBUS_PROXY_FLAGS_NONE,
        NULL,
        "org.bluez",
        adapter_path,
        "org.bluez.Adapter1",
        NULL, error
    );
    if (!self->proxy) {
        g_free(self);
        return NULL;
    }
    // 初始化空字典 builder a{sv}
    g_variant_builder_init(&self->builder, G_VARIANT_TYPE("a{sv}"));  /* :contentReference[oaicite:1]{index=1} */
    return self;
}










/*
 * 信号回调函数
 * BlueZ 发送的 InterfacesAdded 信号的参数格式通常是 (oa{sa{sv}})
 *   - 第一个元素：设备对象路径 (o)
 *   - 第二个元素：一个字典 a{sa{sv}}，其中包含设备接口和属性
 */
static void on_interfaces_added(GDBusConnection *connection,
                                const gchar     *sender_name,
                                const gchar     *object_path,
                                const gchar     *interface_name,
                                const gchar     *signal_name,
                                GVariant        *parameters,
                                gpointer         user_data)
{
    const gchar *device_path;
    const gchar *iface_name;
    GVariant *props;
	GVariant *properties_dict_variant;
    /* 打印原始参数 */
    gchar *param_str = g_variant_print(parameters, TRUE);
    g_print("收到的信号参数: %s\n", param_str);
    g_free(param_str);

    /* 解析信号参数，获取设备路径和接口的属性 */
    g_variant_get(parameters, "(o@a{sa{sv}})", &device_path, &properties_dict_variant);
    g_print("find new device: %s\n", device_path);

	GVariantIter interfaces_iter;
    g_variant_iter_init(&interfaces_iter, properties_dict_variant);

    /* 迭代接口属性 */
    while (g_variant_iter_next(&interfaces_iter, "{&s@a{sv}}", &iface_name, &props)) 
	{
		g_print("\nInterface: %s\n", iface_name);
        if (g_strcmp0(iface_name, "org.bluez.Device1") == 0) {
            GVariantIter prop_iter;
            const gchar *key;
            GVariant *value;
            gchar *name = NULL;
            gchar *address = NULL;

            /* 迭代设备属性 */
            g_variant_iter_init(&prop_iter, props);
            while (g_variant_iter_next(&prop_iter, "{&sv}", &key, &value)) {
                if (g_strcmp0(key, "Name") == 0) {
                    name = g_variant_dup_string(value, NULL);
                } else if (g_strcmp0(key, "Address") == 0) {
                    address = g_variant_dup_string(value, NULL);
                }
                g_variant_unref(value);
            }

            /* 打印设备名称和地址 */
            if (name)
                g_print("  设备名称: %s\n", name);
            if (address)
                g_print("  MAC 地址: %s\n", address);

            g_free(name);
            g_free(address);
        }
        g_variant_unref(props);
    }
}


int glib_scan_test()
{
	GError *error = NULL;
    GDBusConnection *connection;
    GMainLoop *loop;
    guint subscription_id;

    // 连接系统 DBus 总线 
    connection = g_bus_get_sync(G_BUS_TYPE_SYSTEM, NULL, &error);
    if (!connection) {
        g_printerr("无法连接到系统 DBus: %s\n", error->message);
        g_error_free(error);
        return 1;
    }

    // 订阅 InterfacesAdded 信号
    //   注意：尽管 introspection 中可能没有显示 org.freedesktop.DBus.ObjectManager 接口，
    //   BlueZ 仍然会通过该接口广播 InterfacesAdded 信号。
    
    subscription_id = g_dbus_connection_signal_subscribe(
        connection,
        "org.bluez",                           // 信号发送者
        "org.freedesktop.DBus.ObjectManager",  // 信号接口
        "InterfacesAdded",                     // 信号名
        NULL,                                  // 对象路径（NULL 表示所有对象）
        NULL,
        G_DBUS_SIGNAL_FLAGS_NONE,
        on_interfaces_added,                   // 回调函数
        NULL,
        NULL
    );

    // 调用 StartDiscovery 启动蓝牙扫描 
    GVariant *result = g_dbus_connection_call_sync(
        connection,
        "org.bluez",                           // 目标服务
        ADAPTER_PATH,                          // 适配器对象路径（例如 "/org/bluez/hci0"）
        "org.bluez.Adapter1",                  // 接口名称
        "StartDiscovery",                      // 方法名称，无参数
        NULL,                                  // 参数为空
        NULL,                                  // 不指定返回值签名
        G_DBUS_CALL_FLAGS_NONE,
        -1,                                    // 超时时间：无限等待
        NULL,
        &error
    );
    if (!result) {
        g_printerr("StartDiscovery 调用失败: %s\n", error->message);
        g_error_free(error);
        return 1;
    }
    g_variant_unref(result);
    g_print("蓝牙扫描已启动 (%s)。\n", ADAPTER_PATH);

    // 运行 GLib 主循环，自动处理 DBus 消息并调用信号回调 
    loop = g_main_loop_new(NULL, FALSE);
    g_main_loop_run(loop);

    //如果退出主循环，则取消信号订阅并释放资源 
    g_dbus_connection_signal_unsubscribe(connection, subscription_id);
    g_object_unref(connection);
    g_main_loop_unref(loop);

    return 0;
}
