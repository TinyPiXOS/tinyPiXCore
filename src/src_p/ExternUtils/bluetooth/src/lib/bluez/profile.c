/*///------------------------------------------------------------------------------------------------------------------------//
		蓝牙相关功能库接口封装
说 明 : 蓝牙SDP服务记录注册注销
日 期 : 2025.4.15

/*///------------------------------------------------------------------------------------------------------------------------//

#include <stdio.h>
#include <gio/gio.h>
#include <glib.h>
#include <string.h>
#include "bluetooth_inc.h"
#include "profile.h"

#define PROFILE_MANAGER_GET_PRIVATE(obj) (G_TYPE_INSTANCE_GET_PRIVATE((obj), PROFILE_MANAGER_TYPE, ProfileManagerPrivate))

struct _ProfileManagerPrivate {
	GDBusProxy *proxy;
};

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




//注册回调，暂时未使用
int register_profile_callbacks(GDBusConnection *connection,GError **error)
{
	guint registration_id;
	GDBusInterfaceVTable interface_vtable;
    memset(&interface_vtable, 0x0, sizeof(interface_vtable));

    GDBusNodeInfo *introspection_data = g_dbus_node_info_new_for_xml(introspection_xml, error);
    if (!introspection_data) {
        printf("解析 introspection XML 失败: \n");
        return -1;
    }
	GDBusInterfaceInfo *interface_info = g_dbus_node_info_lookup_interface(introspection_data, PROFILE_DBUS_INTERFACE);

	//设置回调
	//interface_vtable.call=

	registration_id = g_dbus_connection_register_object(connection,
                                                        "org/bluez/profile",
                                                        interface_info,
                                                        &interface_vtable,
                                                        NULL,   //user_data
                                                        NULL,   //user_data_free_func
                                                        error);
	g_dbus_node_info_unref(introspection_data);
	if (registration_id == 0) {
        printf("注册对象失败:\n");
        return -1;
    }
}



G_DEFINE_TYPE_WITH_PRIVATE(ProfileManager, profile_manager, G_TYPE_OBJECT);

enum {
	PROP_0,
};

static void _profile_manager_get_property(GObject *object, guint property_id, GValue *value, GParamSpec *pspec);
static void _profile_manager_set_property(GObject *object, guint property_id, const GValue *value, GParamSpec *pspec);

static void profile_manager_create_gdbus_proxy(ProfileManager *self, const gchar *dbus_service_name, const gchar *dbus_object_path, GError **error);

static void profile_manager_dispose(GObject *gobject)
{
	ProfileManager *self = PROFILE_MANAGER(gobject);

	/* Proxy free */
	g_clear_object (&self->priv->proxy);
	/* Chain up to the parent class */
	G_OBJECT_CLASS(profile_manager_parent_class)->dispose(gobject);
}

static void profile_manager_finalize (GObject *gobject)
{
	ProfileManager *self = PROFILE_MANAGER(gobject);
	G_OBJECT_CLASS(profile_manager_parent_class)->finalize(gobject);
}

static void profile_manager_class_init(ProfileManagerClass *klass)
{
	GObjectClass *gobject_class = G_OBJECT_CLASS(klass);

	gobject_class->dispose = profile_manager_dispose;

	/* Properties registration */
	GParamSpec *pspec = NULL;

	gobject_class->get_property = _profile_manager_get_property;
	gobject_class->set_property = _profile_manager_set_property;
	if (pspec)
		g_param_spec_unref(pspec);
}

static void profile_manager_init(ProfileManager *self)
{
	self->priv = profile_manager_get_instance_private (self);
	self->priv->proxy = NULL;
	g_assert(system_conn != NULL);
	GError *error = NULL;
	profile_manager_create_gdbus_proxy(self, PROFILE_MANAGER_DBUS_SERVICE, PROFILE_MANAGER_DBUS_PATH, &error);
	g_assert(error == NULL);
}

static void _profile_manager_get_property(GObject *object, guint property_id, GValue *value, GParamSpec *pspec)
{
	ProfileManager *self = PROFILE_MANAGER(object);

	switch (property_id) {
	default:
		G_OBJECT_WARN_INVALID_PROPERTY_ID(object, property_id, pspec);
		break;
	}
}

static void _profile_manager_set_property(GObject *object, guint property_id, const GValue *value, GParamSpec *pspec)
{
	ProfileManager *self = PROFILE_MANAGER(object);
	GError *error = NULL;

	switch (property_id) {

	default:
		G_OBJECT_WARN_INVALID_PROPERTY_ID(object, property_id, pspec);
		break;
	}

	if (error != NULL)
		g_critical("%s", error->message);

	g_assert(error == NULL);
}


ProfileManager *profile_manager_new()
{
	return g_object_new(PROFILE_MANAGER_TYPE, NULL);
}


//新建一个profile的proxy
static void profile_manager_create_gdbus_proxy(ProfileManager *self, const gchar *dbus_service_name, const gchar *dbus_object_path, GError **error)
{
	g_assert(PROFILE_MANAGER_IS(self));
	self->priv->proxy = g_dbus_proxy_new_sync(system_conn, 
											G_DBUS_PROXY_FLAGS_NONE, 
											NULL, 
											dbus_service_name, 
											dbus_object_path, 
											PROFILE_MANAGER_DBUS_INTERFACE, 
											NULL, error);

	if(self->priv->proxy == NULL)
		return;
}

//注册一条服务记录(proxy)
void profile_manager_proxy_register_profile(ProfileManager *self, const gchar *profile, const gchar *uuid, const GVariant *options, GError **error)
{
	g_assert(PROFILE_MANAGER_IS(self));
	g_dbus_proxy_call_sync(self->priv->proxy, "RegisterProfile", g_variant_new ("(os@a{sv})", profile, uuid, options), G_DBUS_CALL_FLAGS_NONE, -1, NULL, error);
}

//注销一条服务记录(proxy)
void profile_manager_proxy_unregister_profile(ProfileManager *self, const gchar *profile, GError **error)
{
	g_assert(PROFILE_MANAGER_IS(self));
	g_dbus_proxy_call_sync(self->priv->proxy, "UnregisterProfile", g_variant_new ("(o)", profile), G_DBUS_CALL_FLAGS_NONE, -1, NULL, error);
}

//注册一条服务记录(connect)
void profile_manager_connect_register_profile(ProfileManager *self, const gchar *profile, const gchar *uuid, const GVariant *options, GError **error)
{

}

//注销一条服务记录(connect)
void profile_manager_connect_unregister_profile(ProfileManager *self, const gchar *profile, GError **error)
{

}

//设置/切换服务
void profile_manager_proxy_set_profile(ProfileManager *self, const gchar *device ,const gchar *profile, GError **error)
{
	g_assert(PROFILE_MANAGER_IS(self));
	g_dbus_proxy_call_sync(self->priv->proxy, "SetProfile", g_variant_new ("(os)", "/org/bluez/hci0/dev_XX_XX_XX_XX_XX_XX",profile), G_DBUS_CALL_FLAGS_NONE, -1, NULL, error);
}
