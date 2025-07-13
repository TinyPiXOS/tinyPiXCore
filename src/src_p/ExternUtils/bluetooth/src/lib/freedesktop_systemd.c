/*///------------------------------------------------------------------------------------------------------------------------//
		系统级DBUS接口相关接口
说 明 : org.freedesktop.systemd1相关接口
	    unit的接口需要自己的proxy，因此后续可分离出去
日 期 : 2025.5.13
作 者 : Chingan

/*///------------------------------------------------------------------------------------------------------------------------//

#include <stdio.h>
#include <string.h>
#include "bluetooth_inc.h"
#include "freedesktop_systemd.h"


static void system_manager_create_gdbus_proxy(DesktopSystem *system, const gchar *dbus_server, const gchar *dbus_path,GError **error);

struct DesktopSystemPrivate_{
	GDBusProxy *proxy;

};



G_DEFINE_TYPE_WITH_PRIVATE(DesktopSystem, desktop_system, G_TYPE_OBJECT);	


static void desktop_system_class_init (DesktopSystemClass *klass)
{

}

//初始化，会被自动调用
static void desktop_system_init (DesktopSystem *self)
{
	self->priv = desktop_system_get_instance_private (self);
	self->priv->proxy = NULL;
//	g_assert(system_conn != NULL);
	GError *error = NULL;

	system_manager_create_gdbus_proxy(self, FREEDESKTOP_SYSTEMD_DBUS_SERVER, FREEDESKTOP_SYSTEMD_DBUS_PATH, &error);

	g_assert(error == NULL);
}


static void system_manager_create_gdbus_proxy(DesktopSystem *system, const gchar *dbus_server, const gchar *dbus_path,GError **error)
{
	system->priv->proxy=g_dbus_proxy_new_sync(
		system_conn,                            // 已获取的连接
		G_DBUS_PROXY_FLAGS_NONE,         // 默认标志
		NULL,                            // 自动加载 introspection
		dbus_server,                  // D-Bus 服务名 
		dbus_path,                 // 对象路径
		"org.freedesktop.systemd1.Manager",         // 接口名
		NULL,                            // GCancellable 
		error                           // 错误返回
	);
	if (!system->priv->proxy)
	{
		fprintf(stderr,"org.freedesktop.systemd1 creat error\n");
	}
}


DesktopSystem *desktop_system_creat()
{
	DesktopSystem *system = g_object_new(FREEDESKTOP_SYSTEMD_TYPE,NULL);
	
	return system;
}

int desktop_system_delete(DesktopSystem *system)
{
	g_object_unref(system);
	return 0;
}

//启动或重新启动服务
//server：要启动的服务单元名称
//mode：启动模式
int desktop_system_start_unit(DesktopSystem *system, const char *server, const char *mode,GError **error)
{
	GVariant *result = g_dbus_proxy_call_sync(
		system->priv->proxy,
		"StartUnit",
		g_variant_new("(ss)", server, mode),
		G_DBUS_CALL_FLAGS_NONE,
		-1, NULL, error);
	if (result)
		g_variant_unref(result);
}

//停止服务
int desktop_system_stop_unit(DesktopSystem *system, const char *server, const char *mode,GError **error)
{
	GVariant *result = g_dbus_proxy_call_sync(
		system->priv->proxy,
		"StopUnit",
		g_variant_new("(ss)", server, mode),
		G_DBUS_CALL_FLAGS_NONE,
		-1, NULL, error);
	if (result)
		g_variant_unref(result);
}

//重启服务
int desktop_system_restart_unit(DesktopSystem *system, const char *server, const char *mode,GError **error)
{
	GVariant *result = g_dbus_proxy_call_sync(
		system->priv->proxy,
		"RestartUnit",
		g_variant_new("(ss)", server, mode),
		G_DBUS_CALL_FLAGS_NONE,
		-1, NULL, error);
	if (result)
		g_variant_unref(result);
}

//查询某单元对应的 object path
//server：要查询的服务单元名称
char *desktop_system_get_unit(DesktopSystem *system, const char *server, GError **error)
{
	GVariant *result = g_dbus_proxy_call_sync(
		system->priv->proxy,
		"GetUnit",
		g_variant_new("(s)", server),
		G_DBUS_CALL_FLAGS_NONE,
		-1,
		NULL,
		error);
	if (!result) {
        g_printerr("GetUnit failed: %s\n", (*error)->message);
        g_clear_error(error);
		return NULL;
    }
	gchar *unit_path;
    g_variant_get(result, "(o)", &unit_path);
	g_variant_unref(result);
	return (char *)unit_path;
}




//获取unit的对象 proxy  
static GDBusProxy *system_unit_create_gdbus_proxy(DesktopSystem *system,const gchar *dbus_server, const gchar *dbus_path,GError **error)
{
	GDBusProxy *unit_proxy = g_dbus_proxy_new_for_bus_sync(
        G_BUS_TYPE_SYSTEM,
        G_DBUS_PROXY_FLAGS_NONE,
        NULL,
        dbus_server,           // 服务名不变 :contentReference[oaicite:3]{index=3}
        dbus_path,                            // 刚获取到的单元对象路径
        "org.freedesktop.systemd1.Unit",      // Unit 通用接口 :contentReference[oaicite:4]{index=4}
        NULL,
        error);
    if (!unit_proxy) {
        g_printerr("Failed to create Unit proxy: %s\n", (*error)->message);
        return NULL;
    }
	return unit_proxy;
}

//获取运行状态
char *desktop_system_get_service_active_state(DesktopSystem *system,const char *server, GError **error)
{
	char *path=desktop_system_get_unit(system,server,error);
	if(!path)
		return NULL;

	GDBusProxy *unit_proxy=system_unit_create_gdbus_proxy(system,FREEDESKTOP_SYSTEMD_DBUS_SERVER,path,error);
	if(!unit_proxy)
	{
		g_free(path);
		return NULL;
	}

	GVariant *state=g_dbus_proxy_get_cached_property(unit_proxy,"ActiveState");//可以获取"ActiveState"，"SubState"，"LoadState"
	if (!state) {
		fprintf(stderr,"get state error\n");
		g_clear_error(error);
		g_free(unit_proxy);
		g_free(path);
		return NULL;
	}
	const char *tmp=g_variant_get_string(state, NULL);
	char *value=strdup(tmp);
	g_variant_unref(state);
	g_object_unref(unit_proxy);
	g_free(path);
	return value;
}








