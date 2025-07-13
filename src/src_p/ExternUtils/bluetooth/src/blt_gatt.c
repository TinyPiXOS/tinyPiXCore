/*///------------------------------------------------------------------------------------------------------------------------//
		蓝牙服务发现协议的封装
说 明 : 主要利用蓝牙的GATT，用于BLE蓝牙
日 期 : 2025.3.31

/*///------------------------------------------------------------------------------------------------------------------------//


#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <bluetooth/bluetooth.h>
#include <dbus/dbus.h>

// 蓝牙适配器相关定义
#define ADAPTER_PATH "/org/bluez/hci1"
#define ADAPTER_INTERFACE "org.bluez.Adapter1"
#define PROPERTIES_INTERFACE "org.freedesktop.DBus.Properties"

// OBEX相关定义
#define OBEX_SERVICE "org.bluez.obex"
#define OBEX_AGENT_MANAGER_PATH "/org/bluez/obex"
#define OBEX_AGENT_MANAGER_INTERFACE "org.bluez.obex.AgentManager1"
// 为了创建会话，调用 OBEX 客户端接口（CreateSession 方法通常在 Client1 接口下）
#define OBEX_CLIENT_INTERFACE "org.bluez.obex.Client1"

#define OBEX_AGENT_MANAGER_IFACE "org.bluez.obex.AgentManager1"

// 我们在本程序中实现的 OBEX Agent 对象路径
#define OBEX_AGENT_PATH "/com/example/obex_agent"

//---------------------------------------------------------------------
// 使用系统总线通过 DBus 设置蓝牙适配器为 Discoverable 状态
void set_discoverable(DBusConnection *conn) {
    DBusMessage *msg, *reply;
    DBusError err;
    dbus_error_init(&err);

    // 构造对 org.freedesktop.DBus.Properties 接口 Set 方法的调用消息
    msg = dbus_message_new_method_call("org.bluez",   // 目标服务
                                       ADAPTER_PATH,  // 对象路径
                                       PROPERTIES_INTERFACE, // 接口
                                       "Set");        // 方法名
    if (msg == NULL) {
        fprintf(stderr, "创建消息失败\n");
        exit(1);
    }

    // 参数：接口名称、属性名称、以及封装 BOOL 值的 variant
    const char *interface_name = ADAPTER_INTERFACE;
    const char *property_name = "Discoverable";
    dbus_bool_t discoverable = TRUE;

    DBusMessageIter args, variant_iter;
    dbus_message_iter_init_append(msg, &args);
    if (!dbus_message_iter_append_basic(&args, DBUS_TYPE_STRING, &interface_name) ||
        !dbus_message_iter_append_basic(&args, DBUS_TYPE_STRING, &property_name)) {
        fprintf(stderr, "附加参数失败\n");
        exit(1);
    }

    if (!dbus_message_iter_open_container(&args, DBUS_TYPE_VARIANT, "b", &variant_iter)) {
        fprintf(stderr, "打开 variant 容器失败\n");
        exit(1);
    }
    if (!dbus_message_iter_append_basic(&variant_iter, DBUS_TYPE_BOOLEAN, &discoverable)) {
        fprintf(stderr, "写入 variant 数据失败\n");
        exit(1);
    }
    dbus_message_iter_close_container(&args, &variant_iter);

    reply = dbus_connection_send_with_reply_and_block(conn, msg, -1, &err);
    dbus_message_unref(msg);

    if (dbus_error_is_set(&err)) {
        fprintf(stderr, "设置 Discoverable 状态错误: %s\n", err.message);
        dbus_error_free(&err);
    } else {
        printf("蓝牙设备已设置为可发现状态\n");
    }
    if (reply) {
        dbus_message_unref(reply);
    }
}

//---------------------------------------------------------------------
// 注册 OBEX Agent，用于处理 OBEX 操作中的授权或其他交互请求
static void register_obex_agent(DBusConnection *session_conn) {
    DBusMessage *msg, *reply;
    DBusError err;
    dbus_error_init(&err);

    // 调用 OBEX Agent Manager 接口的 RegisterAgent 方法
    msg = dbus_message_new_method_call(OBEX_SERVICE,            // 目标服务
                                       OBEX_AGENT_MANAGER_PATH, // 对象路径
                                       OBEX_AGENT_MANAGER_INTERFACE, // 接口
                                       "RegisterAgent");        // 方法名
    if (msg == NULL) {
        fprintf(stderr, "无法创建注册 OBEX Agent 的消息\n");
        exit(1);
    }

    // 参数：我们的 Agent 对象路径和能力描述（例如 "default"）
    const char *agent_path = OBEX_AGENT_PATH;
    const char *capability = "default";
	if (!dbus_message_append_args(msg,
								DBUS_TYPE_OBJECT_PATH, &agent_path,
								DBUS_TYPE_INVALID)) {
		fprintf(stderr, "附加参数失败\n");
		exit(1);
	}

    reply = dbus_connection_send_with_reply_and_block(session_conn, msg, -1, &err);
    dbus_message_unref(msg);

    if (dbus_error_is_set(&err)) {
        fprintf(stderr, "注册 OBEX Agent 失败: %s\n", err.message);
        dbus_error_free(&err);
    } else {
        printf("OBEX Agent 注册成功\n");
    }
    if (reply) {
        dbus_message_unref(reply);
    }
}

//---------------------------------------------------------------------
// 创建 OBEX 会话，用于发送/接收文件（此处以 OPP 文件传输为例）
// remote_address 为目标设备地址，如 "00:11:22:33:44:55"
static void create_obex_session(DBusConnection *session_conn, const char* remote_address) {
    DBusMessage *msg, *reply;
    DBusError err;
    dbus_error_init(&err);

    // 调用 OBEX 客户端接口的 CreateSession 方法
    msg = dbus_message_new_method_call(OBEX_SERVICE,
                                       OBEX_AGENT_MANAGER_PATH, // OBEX 管理对象路径
                                       OBEX_CLIENT_INTERFACE,
                                       "CreateSession");
    if (msg == NULL) {
        fprintf(stderr, "无法创建 CreateSession 消息\n");
        exit(1);
    }

    // 构造参数：首先附加远程设备地址（字符串），再附加一个选项字典，
    // 其中我们设置 "Target" 键为 "opp" 表示使用 OBEX 对象推送协议（OPP）
    DBusMessageIter args, dict_iter, entry_iter, variant_iter;
    dbus_message_iter_init_append(msg, &args);

    // 附加远程设备地址
    if (!dbus_message_iter_append_basic(&args, DBUS_TYPE_STRING, &remote_address)) {
        fprintf(stderr, "附加远程设备地址失败\n");
        exit(1);
    }

    // 打开字典容器：数组类型，元素为字典条目 {sv}
    if (!dbus_message_iter_open_container(&args, DBUS_TYPE_ARRAY, "{sv}", &dict_iter)) {
        fprintf(stderr, "打开字典容器失败\n");
        exit(1);
    }
    // 添加一个字典条目：键 "Target"，值 "opp"
    const char* key = "Target";
    const char* value = "opp";
    if (!dbus_message_iter_open_container(&dict_iter, DBUS_TYPE_DICT_ENTRY, NULL, &entry_iter)) {
        fprintf(stderr, "打开字典条目失败\n");
        exit(1);
    }
    if (!dbus_message_iter_append_basic(&entry_iter, DBUS_TYPE_STRING, &key)) {
        fprintf(stderr, "附加字典键失败\n");
        exit(1);
    }
    if (!dbus_message_iter_open_container(&entry_iter, DBUS_TYPE_VARIANT, "s", &variant_iter)) {
        fprintf(stderr, "打开 variant 容器失败\n");
        exit(1);
    }
    if (!dbus_message_iter_append_basic(&variant_iter, DBUS_TYPE_STRING, &value)) {
        fprintf(stderr, "附加 variant 值失败\n");
        exit(1);
    }
    dbus_message_iter_close_container(&entry_iter, &variant_iter);
    dbus_message_iter_close_container(&dict_iter, &entry_iter);
    dbus_message_iter_close_container(&args, &dict_iter);

    reply = dbus_connection_send_with_reply_and_block(session_conn, msg, -1, &err);
    dbus_message_unref(msg);

    if (dbus_error_is_set(&err)) {
        fprintf(stderr, "创建 OBEX 会话失败: %s\n", err.message);
        dbus_error_free(&err);
    } else {
        printf("OBEX 会话创建成功\n");
    }
    if (reply) {
        dbus_message_unref(reply);
    }
}

//---------------------------------------------------------------------
// 主函数：分别连接系统总线和会话总线，然后依次设置蓝牙设备、注册 OBEX Agent、创建 OBEX 会话
int gatt_test() {
    DBusError err;
    DBusConnection *sys_conn, *session_conn;

    // 连接系统总线（用于蓝牙适配器设置）
    dbus_error_init(&err);
    sys_conn = dbus_bus_get(DBUS_BUS_SYSTEM, &err);
    if (dbus_error_is_set(&err)) {
        fprintf(stderr, "连接系统总线错误: %s\n", err.message);
        dbus_error_free(&err);
        exit(1);
    }
    if (sys_conn == NULL) {
        fprintf(stderr, "无法获取系统总线连接\n");
        exit(1);
    }

    // 连接会话总线（用于 OBEX 操作）
    session_conn = dbus_bus_get(DBUS_BUS_SESSION, &err);
    if (dbus_error_is_set(&err)) {
        fprintf(stderr, "连接会话总线错误: %s\n", err.message);
        dbus_error_free(&err);
        exit(1);
    }
    if (session_conn == NULL) {
        fprintf(stderr, "无法获取会话总线连接\n");
        exit(1);
    }

    // 设置蓝牙设备可发现
    set_discoverable(sys_conn);

    // 注册 OBEX Agent
    register_obex_agent(session_conn);

    // 假设远程设备地址由用户提供，这里以虚拟地址作为示例
    const char* remote_address = "6C:D1:99:69:BF:F0";
    // 创建 OBEX 会话（OPP 文件传输）
    create_obex_session(session_conn, remote_address);

    // 简单循环监听两个连接上的消息（实际中建议使用更完善的事件循环处理）
    while (1) {
        dbus_connection_read_write(sys_conn, 0);
        DBusMessage *msg = dbus_connection_pop_message(sys_conn);
        if (msg) {
            // 此处可以对系统总线消息进行处理
            dbus_message_unref(msg);
        }
        dbus_connection_read_write(session_conn, 0);
        msg = dbus_connection_pop_message(session_conn);
        if (msg) {
            // 此处可以对会话总线消息进行处理
            dbus_message_unref(msg);
        }
        usleep(100000);
    }

    return 0;
}




static DBusHandlerResult agent_message_handler(DBusConnection *conn, DBusMessage *msg, void *user_data) {
    const char *member = dbus_message_get_member(msg);
    
    if (strcmp(member, "Release") == 0) {
        printf("OBEX Agent: Release 方法被调用\n");
        DBusMessage *reply = dbus_message_new_method_return(msg);
        dbus_connection_send(conn, reply, NULL);
        dbus_message_unref(reply);
        return DBUS_HANDLER_RESULT_HANDLED;
    } else if (strcmp(member, "AuthorizePush") == 0) {
        /* 
         * 当远程设备请求向本机推送文件时，
         * OBEXd 会调用此方法，参数为会话对象路径。
         */
        const char *session;
        if (!dbus_message_get_args(msg, NULL,
                                     DBUS_TYPE_OBJECT_PATH, &session,
                                     DBUS_TYPE_INVALID)) {
            fprintf(stderr, "OBEX Agent: 无法获取 AuthorizePush 参数\n");
            return DBUS_HANDLER_RESULT_NOT_YET_HANDLED;
        }
        printf("OBEX Agent: AuthorizePush 被调用，会话路径: %s\n", session);
        /* 这里直接同意推送，实际应用中可以加入交互或策略判断 */
        DBusMessage *reply = dbus_message_new_method_return(msg);
        dbus_connection_send(conn, reply, NULL);
        dbus_message_unref(reply);
        return DBUS_HANDLER_RESULT_HANDLED;
    } else if (strcmp(member, "Cancel") == 0) {
        printf("OBEX Agent: Cancel 方法被调用\n");
        DBusMessage *reply = dbus_message_new_method_return(msg);
        dbus_connection_send(conn, reply, NULL);
        dbus_message_unref(reply);
        return DBUS_HANDLER_RESULT_HANDLED;
    }
    return DBUS_HANDLER_RESULT_NOT_YET_HANDLED;
}

/* DBus 对象注册时需要提供的 vtable */
static const DBusObjectPathVTable agent_vtable = {
    .message_function = agent_message_handler,
    .unregister_function = NULL,
};

int gatt_test2() {
    DBusError err;
    DBusConnection *conn;
    DBusMessage *msg, *reply;
    
    dbus_error_init(&err);
    
    /* 连接会话总线 */
    conn = dbus_bus_get(DBUS_BUS_SESSION, &err);
    if (dbus_error_is_set(&err)) {
        fprintf(stderr, "连接会话总线出错: %s\n", err.message);
        dbus_error_free(&err);
        exit(1);
    }
    if (!conn) {
        fprintf(stderr, "无法获得会话总线连接\n");
        exit(1);
    }
    
    /* 在本机上导出 OBEX Agent 对象 */
    if (!dbus_connection_register_object_path(conn, OBEX_AGENT_PATH, &agent_vtable, NULL)) {
         fprintf(stderr, "注册 OBEX Agent 对象失败\n");
         exit(1);
    }
    printf("OBEX Agent 对象已注册在 %s\n", OBEX_AGENT_PATH);
    
    /* 调用 OBEX Agent Manager 的 RegisterAgent 方法注册代理对象
       根据新版接口，只传入对象路径（签名为 "o"） */
    msg = dbus_message_new_method_call(OBEX_SERVICE,
                                       OBEX_AGENT_MANAGER_PATH,
                                       OBEX_AGENT_MANAGER_IFACE,
                                       "RegisterAgent");
    if (!msg) {
         fprintf(stderr, "创建 RegisterAgent 消息失败\n");
         exit(1);
    }
    const char *agent_path = OBEX_AGENT_PATH;
    if (!dbus_message_append_args(msg,
                                  DBUS_TYPE_OBJECT_PATH, &agent_path,
                                  DBUS_TYPE_INVALID)) {
         fprintf(stderr, "附加 RegisterAgent 参数失败\n");
         exit(1);
    }
    
    reply = dbus_connection_send_with_reply_and_block(conn, msg, -1, &err);
    dbus_message_unref(msg);
    if (dbus_error_is_set(&err)) {
         fprintf(stderr, "注册 OBEX Agent 失败: %s\n", err.message);
         dbus_error_free(&err);
         exit(1);
    }
    dbus_message_unref(reply);
    printf("OBEX Agent 成功注册到 OBEX Agent Manager\n");
    
    /* 进入简单的循环，等待处理 DBus 调用 */
    while (1) {
         dbus_connection_read_write(conn, 100);
         DBusMessage *incoming = dbus_connection_pop_message(conn);
         if (incoming) {
              /* 消息会由 agent_message_handler 处理 */
              dbus_message_unref(incoming);
         }
         /* 可适当 sleep */
         usleep(100000);
    }
    
    return 0;
}