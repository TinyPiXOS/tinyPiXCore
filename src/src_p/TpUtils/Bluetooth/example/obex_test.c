#include <gio/gio.h>
#include <glib.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

// 配置区：硬编码适配器和设备路径
#define DEFAULT_ADAPTER "/org/bluez/hci1"
#define DEVICE_PATH_TEMPLATE "/org/bluez/hci1/dev_%s"

// 调试宏
#define debug(fmt, ...) fprintf(stderr, "[DEBUG] %s:%d " fmt "\n", __FILE__, __LINE__, ##__VA_ARGS__)

// 创建设备路径（替换地址中的冒号为下划线）
static gchar* build_device_path(const gchar *address) {
    gchar *sanitized = g_strdup(address);
    for (int i = 0; sanitized[i]; i++) {
        if (sanitized[i] == ':') sanitized[i] = '_';
    }
    gchar *path = g_strdup_printf(DEVICE_PATH_TEMPLATE, sanitized);
    g_free(sanitized);
    return path;
}

// 初始化D-Bus连接
static GDBusConnection* init_dbus() {
    GError *error = NULL;
    GDBusConnection *bus = g_bus_get_sync(G_BUS_TYPE_SYSTEM, NULL, &error);
    if (!bus) {
        debug("D-Bus连接失败: %s", error->message);
        g_error_free(error);
        exit(EXIT_FAILURE);
    }
    return bus;
}

// 确保设备已连接
static void ensure_connected(GDBusConnection *bus, const gchar *device_path) {
    GDBusProxy *dev = g_dbus_proxy_new_sync(bus, G_DBUS_PROXY_FLAGS_NONE, NULL,
        "org.bluez", device_path, "org.bluez.Device1", NULL, NULL);
    if (!dev) {
        debug("创建设备代理失败: %s", device_path);
        exit(EXIT_FAILURE);
    }

    // 检查连接状态
    GVariant *connected = g_dbus_proxy_get_cached_property(dev, "Connected");
    if (connected && g_variant_get_boolean(connected)) {
        g_variant_unref(connected);
        g_object_unref(dev);
        return;
    }

    // 执行配对和连接
    GVariant *result = g_dbus_proxy_call_sync(dev, "Pair", NULL, 
        G_DBUS_CALL_FLAGS_NONE, 30, NULL, NULL);
    if (!result) {
        debug("配对失败");
        exit(EXIT_FAILURE);
    }
    g_variant_unref(result);

    result = g_dbus_proxy_call_sync(dev, "Connect", NULL,
        G_DBUS_CALL_FLAGS_NONE, 30, NULL, NULL);
    if (!result) {
        debug("连接失败");
        exit(EXIT_FAILURE);
    }
    g_variant_unref(result);
    g_object_unref(dev);
}

// 执行文件传输
static void perform_transfer(GDBusConnection *bus, const gchar *address, const gchar *file) {
    // 创建OBEX客户端
    GDBusProxy *obex = g_dbus_proxy_new_sync(bus, G_DBUS_PROXY_FLAGS_NONE, NULL,
        "org.bluez.obex", "/org/bluez/obex", "org.bluez.obex.Client1", NULL, NULL);
    if (!obex) {
        debug("OBEX初始化失败");
        exit(EXIT_FAILURE);
    }

    // 创建传输会话
    GVariantBuilder *builder = g_variant_builder_new(G_VARIANT_TYPE("a{sv}"));
    g_variant_builder_add(builder, "{sv}", "Target", g_variant_new_string("OPP"));
    
    GVariant *session = g_dbus_proxy_call_sync(obex, "CreateSession",
        g_variant_new("(sa{sv})", address, builder),
        G_DBUS_CALL_FLAGS_NONE, -1, NULL, NULL);
    if (!session) {
        debug("OBEX会话创建失败");
        exit(EXIT_FAILURE);
    }

    // 解析会话路径
    const gchar *session_path;
    g_variant_get(session, "(o)", &session_path);
    debug("OBEX会话路径: %s", session_path);

    // 创建传输接口
    GDBusProxy *transfer = g_dbus_proxy_new_sync(bus, G_DBUS_PROXY_FLAGS_NONE, NULL,
        "org.bluez.obex", session_path, "org.bluez.obex.Transfer1", NULL, NULL);
    if (!transfer) {
        debug("传输接口创建失败");
        exit(EXIT_FAILURE);
    }

    // 发送文件
    GVariant *result = g_dbus_proxy_call_sync(transfer, "SendFile",
        g_variant_new("(s)", file), G_DBUS_CALL_FLAGS_NONE, -1, NULL, NULL);
    if (!result) {
        debug("文件传输失败");
        exit(EXIT_FAILURE);
    }
    debug("传输已启动");

    // 清理资源
    g_variant_unref(session);
    g_object_unref(transfer);
    g_object_unref(obex);
}

int main(int argc, char **argv) {
    if (argc != 3) {
        g_print("用法: %s <蓝牙地址> <文件路径>\n示例: %s 00:11:22:33:44:55 /tmp/test.txt\n", 
               argv[0], argv[0]);
        return EXIT_FAILURE;
    }

    // 初始化核心组件
    GDBusConnection *bus = init_dbus();
    gchar *device_path = build_device_path(argv[1]);

    // 设备连接准备
    ensure_connected(bus, device_path);

    // 执行传输
    perform_transfer(bus, argv[1], argv[2]);

    // 保持事件循环
    GMainLoop *loop = g_main_loop_new(NULL, FALSE);
    g_main_loop_run(loop);

    // 清理资源
    g_free(device_path);
    g_object_unref(bus);
    return EXIT_SUCCESS;
}