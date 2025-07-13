/*///------------------------------------------------------------------------------------------------------------------------//
		系统级DBUS接口相关接口（开关机接口）
说 明 : org.freedesktop.login1接口
日 期 : 2025.5.28
作 者 : Chingan

/*///------------------------------------------------------------------------------------------------------------------------//

#include <stdio.h>
#include <string.h>
#include "logind.h"

static GDBusConnection *system_conn = NULL;
static void logind_create_gdbus_proxy(Logind *self, const gchar *dbus_server, const gchar *dbus_path,GError **error);

struct LogindPrivate_{
	GDBusProxy *proxy;
	
};

G_DEFINE_TYPE_WITH_PRIVATE(Logind, logind, G_TYPE_OBJECT);	


static void logind_class_init (LogindClass *klass)
{

}

//初始化，会被自动调用
static void logind_init (Logind *self)
{
	self->priv = logind_get_instance_private (self);
	self->priv->proxy = NULL;
//	g_assert(system_conn != NULL);
	GError *error = NULL;

	logind_create_gdbus_proxy(self, LOGIND_DBUS_SERVER, LOGIND_DBUS_PATH, &error);

	g_assert(error == NULL);
}


static void logind_create_gdbus_proxy(Logind *self, const gchar *dbus_server, const gchar *dbus_path,GError **error)
{
	self->priv->proxy=g_dbus_proxy_new_sync(
		system_conn,                            // 已获取的连接
		G_DBUS_PROXY_FLAGS_NONE,         // 默认标志
		NULL,                            // 自动加载 introspection
		dbus_server,                  // D-Bus 服务名 
		dbus_path,                 // 对象路径
		LOGIND_DBUS_INTERFACE,         // 接口名
		NULL,                            // GCancellable 
		error);                           // 错误返回
	if (!self->priv->proxy)
	{
		fprintf(stderr,"org.freedesktop.login1 creat error\n");
	}
}


Logind *logind_creat(GDBusConnection *conn)
{
	system_conn=conn;
	Logind *self = g_object_new(LOGIND_TYPE,NULL);
	return self;
}

int logind_delete(Logind *self)
{
	if(!self)
		return 0;

	g_object_unref(self);
	return 0;
}


static int logind_action(Logind *self, const gchar *method, GError **error) {
    GVariant *result = g_dbus_proxy_call_sync(
        self->priv->proxy,
        method,
        g_variant_new("(b)", FALSE),  // 参数：interactive=false,该值表示师傅需要图形界面弹窗来确认操作
        G_DBUS_CALL_FLAGS_NONE,
        -1,  // 无超时
        NULL,  // cancellable
        error
    );

    if (!result) {
        return -1;
    }

    g_variant_unref(result);
    return 0;
}



int logind_power_off(Logind *self)
{
	return logind_action(self,"PowerOff",NULL);
}

int logind_reboot(Logind *self)
{
	return logind_action(self,"Reboot",NULL);
}

int logind_suspend(Logind *self)
{
	return logind_action(self,"Suspend",NULL);
}