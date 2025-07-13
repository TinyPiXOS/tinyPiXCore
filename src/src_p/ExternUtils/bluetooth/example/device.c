//设备的基础测试程序
//配对
//等待配对
//sdp注册
#include <stdio.h>
#include "g_utils.h"
#include "bluetooth_inc.h"
#include "blt_device.h"
#include "sdpregister.h"
#include "blt_agent.h"




//和远程设备配对
int example_pair(GError **error)
{
	Adapter *adapter = find_adapter(NULL, error);
	BluetAgent *agent=bluet_agent_creat();
	BluetDevice *device=bluet_device_creat(adapter,"6C:D1:99:69:BF:F0");

	bluet_device_pair_with_remote(device,0);

	sleep(10);

	bluet_device_delete(device);
	bluet_agent_delete(agent);
    g_object_unref(adapter);
    

}

//等待远程设备配对
int example_pair_agent()
{

}

//
int example_reg_sdp()
{
	sdp_session_t *session=bluet_sdp_connect();
	if(!session)
		return 0;
	printf("注册\n");
	sdp_record_t *record=bluet_register_sdp_service(session,"OBEX Object Push",0x1105,NULL,NULL,"l2cap,rfcomm",12);
	sleep(10);
	bluet_unregister_sdp_service(session,record);
	printf("结束，断开连接\n");
	bluet_sdp_close(session);
}


int dbus_connect()
{


}


int main()
{
	GError *error = NULL;

	dbus_connect_init();

	if (!dbus_system_connect(&error))
    {
        g_printerr("Couldn't connect to DBus system bus: %s\n", error->message);
        return -1;
    }

    
    if (!intf_supported(BLUEZ_DBUS_SERVICE_NAME, MANAGER_DBUS_PATH, MANAGER_DBUS_INTERFACE))
    {
        g_printerr("%s: bluez service is not found\n", g_get_prgname());
        g_printerr("Did you forget to run bluetoothd?\n");
       return -1;
    }


	example_pair(&error);

	dbus_disconnect();

//	example_reg_sdp();
}


#include <gio/gio.h>
#include <glib.h>
#include <stdio.h>

/* D-Bus introspection XML，描述 Profile 接口 */
static const gchar introspection_xml[] =
  "<node>"
  "  <interface name='org.bluez.Profile1'>"
  "    <method name='Release'/>"
  "    <method name='NewConnection'>"
  "      <arg type='o' name='device' direction='in'/>"
  "      <arg type='h' name='fd' direction='in'/>"
  "      <arg type='a{sv}' name='fd_properties' direction='in'/>"
  "    </method>"
  "    <method name='RequestDisconnection'>"
  "      <arg type='o' name='device' direction='in'/>"
  "    </method>"
  "  </interface>"
  "</node>";

/* Profile 接口方法回调 */
static void handle_method_call (GDBusConnection       *connection,
                                const gchar           *sender,
                                const gchar           *object_path,
                                const gchar           *interface_name,
                                const gchar           *method_name,
                                GVariant              *parameters,
                                GDBusMethodInvocation *invocation,
                                gpointer               user_data)
{
    if (g_strcmp0(method_name, "Release") == 0) {
        g_print("Received Release\n");
        g_dbus_method_invocation_return_value(invocation, NULL);
    } else if (g_strcmp0(method_name, "NewConnection") == 0) {
        const gchar *device;
        gint fd;
        GVariant *fd_properties;
        g_variant_get(parameters, "(&oh@a{sv})", &device, &fd, &fd_properties);
        g_print("Received NewConnection from %s, fd %d\n", device, fd);
        g_dbus_method_invocation_return_value(invocation, NULL);
    } else if (g_strcmp0(method_name, "RequestDisconnection") == 0) {
        const gchar *device;
        g_variant_get(parameters, "(&o)", &device);
        g_print("Received RequestDisconnection for %s\n", device);
        g_dbus_method_invocation_return_value(invocation, NULL);
    } else {
        g_dbus_method_invocation_return_error(invocation,
                                              G_IO_ERROR,
                                              G_IO_ERROR_NOT_SUPPORTED,
                                              "Method %s not supported", method_name);
    }
}

static const GDBusInterfaceVTable interface_vtable = {
    handle_method_call,
    NULL,
    NULL,
};

int main__ (int argc, char *argv[])
{
    GMainLoop *loop;
    GDBusConnection *connection;
    GError *error = NULL;
    guint registration_id;
    GDBusNodeInfo *introspection_data;
    const gchar *profile_path = "/org/bluez/example/profile";
    /* 示例 UUID，此处采用串口服务 UUID (SPP) */
    const gchar *profile_uuid = "00001101-0000-1000-8000-00805F9B34FB";

    /* 1. 获取系统总线连接 */
    connection = g_bus_get_sync(G_BUS_TYPE_SYSTEM, NULL, &error);
    if (!connection) {
        g_printerr("Failed to get system bus: %s\n", error->message);
        g_error_free(error);
        return 1;
    }
    g_print("Got system bus connection.\n");

    /* 2. 解析 introspection XML */
    introspection_data = g_dbus_node_info_new_for_xml(introspection_xml, &error);
    if (!introspection_data) {
        g_printerr("Failed to parse introspection XML: %s\n", error->message);
        g_error_free(error);
        return 1;
    }
    g_print("Parsed introspection XML.\n");

    /* 3. 查找 org.bluez.Profile1 接口 */
    GDBusInterfaceInfo *profile_iface;
    profile_iface = g_dbus_node_info_lookup_interface(introspection_data, "org.bluez.Profile1");
    if (!profile_iface) {
        g_printerr("Failed to find interface org.bluez.Profile1\n");
        return 1;
    }
    g_print("Found interface org.bluez.Profile1.\n");

    /* 4. 在系统总线上注册 Profile 对象 */
    registration_id = g_dbus_connection_register_object(connection,
                                                        profile_path,
                                                        profile_iface,
                                                        &interface_vtable,
                                                        NULL,
                                                        NULL,
                                                        &error);
    if (registration_id == 0) {
        g_printerr("Failed to register object: %s\n", error->message);
        g_error_free(error);
        return 1;
    }
    g_print("Profile object registered at %s\n", profile_path);

    /* 5. 创建 GDBusProxy 对象，用于调用 BlueZ 的 ProfileManager1 接口 */
    GDBusProxy *proxy = g_dbus_proxy_new_sync(
        connection,
        G_DBUS_PROXY_FLAGS_NONE,
        NULL,
        "org.bluez",
        "/org/bluez",
        "org.bluez.ProfileManager1",
        NULL,
        &error
    );
    if (!proxy) {
        g_printerr("Failed to create proxy: %s\n", error->message);
        g_error_free(error);
        return 1;
    }
    g_print("Created proxy for org.bluez.ProfileManager1.\n");

    /* 6. 构造包含选项的字典，增加 Channel 和完整的 ServiceRecord */
    GVariantBuilder *builder = g_variant_builder_new(G_VARIANT_TYPE("a{sv}"));
    g_variant_builder_add(builder, "{sv}", "Channel", g_variant_new_int32(22));
    /* 此 SDP XML 示例描述一个简单的串口服务 */
	const gchar *sdp_xml =
		"<?xml version=\"1.0\" encoding=\"UTF-8\" ?>\n"
		"<record>\n"
		"  <attribute id=\"0x0001\">\n"
		"    <sequence>\n"
		"      <uuid value=\"00001101-0000-1000-8000-00805F9B34FB\"/>\n"
		"    </sequence>\n"
		"  </attribute>\n"
		"  <attribute id=\"0x0004\">\n"
		"    <sequence>\n"
		"      <sequence>\n"
		"        <uuid value=\"00001000-0000-1000-8000-00805F9B34FB\"/>\n"
		"        <uint8 value=\"22\"/>\n"
		"      </sequence>\n"
		"    </sequence>\n"
		"  </attribute>\n"
		"  <attribute id=\"0x0100\">\n"
		"    <text value=\"MySerialPort\"/>\n"
		"  </attribute>\n"
		"</record>";
    g_variant_builder_add(builder, "{sv}", "ServiceRecord", g_variant_new_string(sdp_xml));
    GVariant *options = g_variant_builder_end(builder);
    g_variant_builder_unref(builder);
    g_print("Constructed options with ServiceRecord:\n%s\n", sdp_xml);

    /* 7. 使用 proxy 调用 RegisterProfile 方法，注意格式字符串使用了 '@' */
    GVariant *result = g_dbus_proxy_call_sync(
        proxy,
        "RegisterProfile",
        g_variant_new("(os@a{sv})", profile_path, profile_uuid, options),
        G_DBUS_CALL_FLAGS_NONE,
        -1,
        NULL,
        &error
    );
	
    if (!result) {
        g_printerr("Failed to register profile: %s\n", error->message);
        g_error_free(error);
        return 1;
    }
    g_print("Profile registered successfully via RegisterProfile.\n");
    g_variant_unref(result);

    /* 8. 进入主循环，等待 BlueZ 调用 Profile 对象的方法 */
    loop = g_main_loop_new(NULL, FALSE);
    g_print("Entering main loop. You can now check the SDP records with 'sudo sdptool browse local'\n");
    g_main_loop_run(loop);

    /* 退出前清理 */
    g_dbus_connection_unregister_object(connection, registration_id);
    g_main_loop_unref(loop);
    g_dbus_node_info_unref(introspection_data);
    g_object_unref(proxy);
    return 0;
}

