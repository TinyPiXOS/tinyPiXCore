/*///------------------------------------------------------------------------------------------------------------------------//
		蓝牙SPP协议客户端
说 明 : 使用Bluez+Glib的方式
日 期 : 2025.8.6

/*///------------------------------------------------------------------------------------------------------------------------//

#include <glib.h>
#include <gio/gio.h>
#include <fcntl.h>
#include "spp_client.h"
#include "bluetooth_inc.h"


#define RECV_BUF_SIZE           1024

static const gchar introspect_xml[] =
"<node>"
"  <interface name='org.bluez.Profile1'>"
"    <method name='Release'/>"
"    <signal name='NewConnection'>"
"      <arg name='device' type='o'/>"
"      <arg name='fd'     type='h'/>"
"      <arg name='options'type='a{sv}'/>"
"    </signal>"
"  </interface>"
"</node>";


struct BluetSppClientPrivate_{
	GMainLoop *mainloop;
	Device *target_device;
	int client_fd;
};


BluetSppClient *bluet_spp_client_creat()
{
	BluetSppClient *self=(BluetSppClient *)malloc(sizeof(BluetSppClient));
	if(!self)
		return NULL;

	BluetSppClientPrivate *priv=(BluetSppClientPrivate *)malloc(sizeof(BluetSppClientPrivate));
	if(!priv)
	{	
		free(self);
		return NULL;
	}

	self->priv=priv;

	self->priv->mainloop=g_main_loop_new(NULL, FALSE);

}

bluet_spp_client_delete(BluetSppClient *self)
{
	if(!self)
		return 0;
	if(self->priv->mainloop)
		g_main_loop_unref(self->priv->mainloop);

	self->priv->mainloop=NULL;

	free(self->priv);
	free(self);
	return 0;
}


// 2. 发送数据
static int send_spp_data(BluetSppClient *self, const uint8_t *data, uint64_t len) 
{
	if(!self)
		return -1;

    if (self->priv->client_fd < 0) {
        g_print("没有活动的SPP连接\n");
        return;
    }
    
    ssize_t bytes_written = write(self->priv->client_fd, data, len);
    if (bytes_written < 0) {
        perror("发送数据失败");
    } else {
        g_print("成功发送: %s\n", data);
    }
	return bytes_written;
}

// 1. 处理SPP数据
static gboolean handle_spp_data(gint fd, GIOCondition condition, gpointer user_data) 
{
	BluetSppClient *self=(BluetSppClient *)user_data;

    if (condition & (G_IO_ERR | G_IO_HUP | G_IO_NVAL)) {
        g_print("SPP连接已断开\n");
        close(self->priv->client_fd);
        self->priv->client_fd = -1;
        return FALSE;
    }
    
    if (condition & G_IO_IN) {
        char buffer[1024];
        ssize_t bytes_read = read(fd, buffer, sizeof(buffer) - 1);
        
        if (bytes_read > 0) {
            buffer[bytes_read] = '\0';
            g_print("接收: %s\n", buffer);
        } else if (bytes_read == 0) {
            g_print("对端关闭连接\n");
            close(fd);
            self->priv->client_fd = -1;
            return FALSE;
        } else if (bytes_read < 0 && errno != EAGAIN && errno != EWOULDBLOCK) {
            perror("读取错误");
            close(fd);
            self->priv->client_fd = -1;
            return FALSE;
        }
    }
    
    return TRUE;
}


//新链接处理回调
static void on_new_connection(GDBusConnection *connection,
                              const gchar *sender,
                              const gchar *object_path,
                              const gchar *interface,
                              const gchar *signal_name,
                              GVariant *parameters,
                              gpointer user_data) {
    
	BluetSppClient *self=(BluetSppClient *)user_data;
    const gchar *device_path;
    gint32 fd_index;
    GVariant *properties;
    
    // 获取文件描述符列表
    GDBusMessage *message = g_dbus_method_invocation_get_message((GDBusMethodInvocation*)parameters);
    GUnixFDList *fd_list = g_dbus_message_get_unix_fd_list(message);
    
    if (!fd_list) {
        g_printerr("未收到文件描述符列表\n");
        return;
    }
    
    g_variant_get(parameters, "(oh@a{sv})", &device_path, &fd_index, &properties);
    
    // 只处理目标设备的连接
    if (self->priv->target_device == NULL || !g_str_equal(device_path, device_get_dbus_object_path(self->priv->target_device))) {
        g_print("忽略非目标设备的连接: %s\n", device_path);
        return;
    }
    
    GError *error = NULL;
    int client_fd = g_unix_fd_list_get(fd_list, fd_index, &error);
    
    if (error) {
        g_printerr("获取文件描述符失败: %s\n", error->message);
        g_error_free(error);
        return;
    }
    
    g_print("SPP连接已建立！FD: %d\n", client_fd);
    
    // 设置非阻塞模式
    int flags = fcntl(client_fd, F_GETFL, 0);
    fcntl(client_fd, F_SETFL, flags | O_NONBLOCK);
    
	self->priv->client_fd=client_fd;

    // 添加监控
    g_unix_fd_add(client_fd, 
                 G_IO_IN | G_IO_ERR | G_IO_HUP | G_IO_NVAL, 
                 handle_spp_data, 
                 self); 

    g_print("已准备好发送和接收数据!\n");
}


int connect_spp_service(BluetSppClient  *self) {
    if (self->priv->target_device == NULL) {
        g_print("没有找到目标设备\n");
        return -1;
    }
    
    GError *error = NULL;
	
    device_connect_profile(self->priv->target_device, name_to_uuid("SerialPort"), &error); // SPP UUID
    
    if (error) {
        g_printerr("连接到SPP服务失败: %s\n", error->message);
        g_error_free(error);
    } else {
        g_print("已请求连接到SPP服务\n");
    }
	return 0;
}




BluetDbusSignal *bluet_spp_client_signal_subscribe_new_connection(BluetSppClient *self,void *userdata)
{
	return bluet_dbus_signal_subscribe(system_conn,
										"org.bluez", 
										NULL, 
										"NewConnection",NULL, 
										on_new_connection, 
										userdata, NULL);
}


