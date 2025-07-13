/*///------------------------------------------------------------------------------------------------------------------------//
		以本机适配器为传入参数的相关操作
说 明 : 为上层代码提供接口，对DBUS和GIO的进一步封装
日 期 : 2025.4.9

/*///------------------------------------------------------------------------------------------------------------------------//

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif
#include <gio/gio.h>
#include <glib.h>
#include <string.h>
#include "bluetooth_inc.h"
#include "adapter.h"

#define ADAPTER_GET_PRIVATE(obj) (G_TYPE_INSTANCE_GET_PRIVATE((obj), ADAPTER_TYPE, AdapterPrivate))

struct _AdapterPrivate {
	GDBusProxy *proxy;
	Properties *properties;
	gchar *object_path;
};

G_DEFINE_TYPE_WITH_PRIVATE(Adapter, adapter, G_TYPE_OBJECT);

enum {
	PROP_0,
	PROP_DBUS_OBJECT_PATH /* readwrite, construct only */
};

static void _adapter_get_property(GObject *object, guint property_id, GValue *value, GParamSpec *pspec);
static void _adapter_set_property(GObject *object, guint property_id, const GValue *value, GParamSpec *pspec);

static void _adapter_create_gdbus_proxy(Adapter *self, const gchar *dbus_service_name, const gchar *dbus_object_path, GError **error);

static void adapter_dispose(GObject *gobject)
{
	Adapter *self = ADAPTER(gobject);

	/* Proxy free */
	g_clear_object (&self->priv->proxy);
	/* Properties free */
	g_clear_object(&self->priv->properties);
	/* Object path free */
	g_free(self->priv->object_path);
	/* Chain up to the parent class */
	G_OBJECT_CLASS(adapter_parent_class)->dispose(gobject);
}

static void adapter_finalize (GObject *gobject)
{
	Adapter *self = ADAPTER(gobject);
	G_OBJECT_CLASS(adapter_parent_class)->finalize(gobject);
}

static void adapter_class_init(AdapterClass *klass)
{
	GObjectClass *gobject_class = G_OBJECT_CLASS(klass);

	gobject_class->dispose = adapter_dispose;

	/* Properties registration */
	GParamSpec *pspec = NULL;

	gobject_class->get_property = _adapter_get_property;
	gobject_class->set_property = _adapter_set_property;
	
	/* object DBusObjectPath [readwrite, construct only] */
	pspec = g_param_spec_string("DBusObjectPath", "dbus_object_path", "Adapter D-Bus object path", NULL, G_PARAM_READWRITE | G_PARAM_CONSTRUCT_ONLY);
	g_object_class_install_property(gobject_class, PROP_DBUS_OBJECT_PATH, pspec);
	if (pspec)
		g_param_spec_unref(pspec);
}

static void adapter_init(Adapter *self)
{
	self->priv = adapter_get_instance_private (self);
	self->priv->proxy = NULL;
	self->priv->properties = NULL;
	self->priv->object_path = NULL;
	g_assert(system_conn != NULL);
}

static void _adapter_get_property(GObject *object, guint property_id, GValue *value, GParamSpec *pspec)
{
	Adapter *self = ADAPTER(object);

	switch (property_id) {
	case PROP_DBUS_OBJECT_PATH:
		g_value_set_string(value, adapter_get_dbus_object_path(self));
		break;

	default:
		G_OBJECT_WARN_INVALID_PROPERTY_ID(object, property_id, pspec);
		break;
	}
}

static void _adapter_set_property(GObject *object, guint property_id, const GValue *value, GParamSpec *pspec)
{
	Adapter *self = ADAPTER(object);
	GError *error = NULL;

	switch (property_id) {
	case PROP_DBUS_OBJECT_PATH:
		self->priv->object_path = g_value_dup_string(value);
		_adapter_create_gdbus_proxy(self, ADAPTER_DBUS_SERVICE, self->priv->object_path, &error);
		break;

	default:
		G_OBJECT_WARN_INVALID_PROPERTY_ID(object, property_id, pspec);
		break;
	}

	if (error != NULL)
		g_critical("%s", error->message);

	g_assert(error == NULL);
}

/* Constructor */
Adapter *adapter_new(const gchar *dbus_object_path)
{
	return g_object_new(ADAPTER_TYPE, "DBusObjectPath", dbus_object_path, NULL);
}

/* Private DBus proxy creation */
static void _adapter_create_gdbus_proxy(Adapter *self, const gchar *dbus_service_name, const gchar *dbus_object_path, GError **error)
{
	g_assert(ADAPTER_IS(self));
	self->priv->proxy = g_dbus_proxy_new_sync(system_conn, G_DBUS_PROXY_FLAGS_NONE, NULL, dbus_service_name, dbus_object_path, ADAPTER_DBUS_INTERFACE, NULL, error);

	if(self->priv->proxy == NULL)
		return;

	self->priv->properties = g_object_new(PROPERTIES_TYPE, "DBusConnection", system_conn, "DBusType", "system", "DBusServiceName", dbus_service_name, "DBusObjectPath", dbus_object_path, NULL);
	g_assert(self->priv->properties != NULL);
}

/* Methods */

/* Get DBus object path */
const gchar *adapter_get_dbus_object_path(Adapter *self)
{
	g_assert(ADAPTER_IS(self));
	g_assert(self->priv->proxy != NULL);
	return g_dbus_proxy_get_object_path(self->priv->proxy);
}

//移除设备(用于移除已配对的设备)
void adapter_remove_device(Adapter *self, const gchar *device, GError **error)
{
	g_assert(ADAPTER_IS(self));
	g_dbus_proxy_call_sync(self->priv->proxy, "RemoveDevice", g_variant_new ("(o)", device), G_DBUS_CALL_FLAGS_NONE, -1, NULL, error);
}

//开始扫描
void adapter_start_discovery(Adapter *self, GError **error)
{
	g_assert(ADAPTER_IS(self));
	g_dbus_proxy_call_sync(self->priv->proxy, "StartDiscovery", NULL, G_DBUS_CALL_FLAGS_NONE, -1, NULL, error);
}

//停止扫描
void adapter_stop_discovery(Adapter *self, GError **error)
{
	g_assert(ADAPTER_IS(self));
	g_dbus_proxy_call_sync(self->priv->proxy, "StopDiscovery", NULL, G_DBUS_CALL_FLAGS_NONE, -1, NULL, error);
}

//添加扫描过滤器
//transport:"auto","bredr","le"
void filter_add_transport(GVariantBuilder *builder, const gchar *transport)
{
	//此处添加检查程序
    g_variant_builder_add(builder,"{sv}","Transport",g_variant_new_string(transport));
}

//添加扫描过滤器
void filter_add_rssi(GVariantBuilder *builder, gint16 threshold)
{
	//此处添加检查程序
    g_variant_builder_add(builder,"{sv}","RSSI",g_variant_new_int16(threshold));
}

//设置扫描过滤器
int adapter_set_discovery_filter(Adapter *self, GVariantBuilder *filter_params, GError **error)
{
	g_assert(ADAPTER_IS(self));
	if(filter_params)
		return -1;
	g_dbus_proxy_call_sync(self->priv->proxy,
		"SetDiscoveryFilter",
		g_variant_new("(a{sv})", &filter_params),
		G_DBUS_CALL_FLAGS_NONE,
		-1, NULL, error);
	return 0;
}

/* Properties access methods */
GVariant *adapter_get_properties(Adapter *self, GError **error)
{
	g_assert(ADAPTER_IS(self));
	g_assert(self->priv->properties != NULL);
	return properties_get_all(self->priv->properties, ADAPTER_DBUS_INTERFACE, error);
}

void adapter_set_property(Adapter *self, const gchar *name, const GVariant *value, GError **error)
{
	g_assert(ADAPTER_IS(self));
	g_assert(self->priv->properties != NULL);
	g_proxy_properties_set(self->priv->properties, ADAPTER_DBUS_INTERFACE, name, value, error);
}

const gchar *adapter_get_address(Adapter *self, GError **error)
{
	g_assert(ADAPTER_IS(self));
	g_assert(self->priv->properties != NULL);
	GVariant *prop = g_proxy_properties_get(self->priv->properties, ADAPTER_DBUS_INTERFACE, "Address", error);
	if(prop == NULL)
		return NULL;
	const gchar *ret = g_variant_get_string(prop, NULL);
	g_variant_unref(prop);
	return ret;
}

const gchar *adapter_get_alias(Adapter *self, GError **error)
{
	g_assert(ADAPTER_IS(self));
	g_assert(self->priv->properties != NULL);
	GVariant *prop = g_proxy_properties_get(self->priv->properties, ADAPTER_DBUS_INTERFACE, "Alias", error);
	if(prop == NULL)
		return NULL;
	const gchar *ret = g_variant_get_string(prop, NULL);
	g_variant_unref(prop);
	return ret;
}

void adapter_set_alias(Adapter *self, const gchar *value, GError **error)
{
	g_assert(ADAPTER_IS(self));
	g_assert(self->priv->properties != NULL);
	g_proxy_properties_set(self->priv->properties, ADAPTER_DBUS_INTERFACE, "Alias", g_variant_new_string(value), error);
}

guint32 adapter_get_class(Adapter *self, GError **error)
{
	g_assert(ADAPTER_IS(self));
	g_assert(self->priv->properties != NULL);
	GVariant *prop = g_proxy_properties_get(self->priv->properties, ADAPTER_DBUS_INTERFACE, "Class", error);
	if(prop == NULL)
		return 0;
	guint32 ret = g_variant_get_uint32(prop);
	g_variant_unref(prop);
	return ret;
}

gboolean adapter_get_discoverable(Adapter *self, GError **error)
{
	g_assert(ADAPTER_IS(self));
	g_assert(self->priv->properties != NULL);
	GVariant *prop = g_proxy_properties_get(self->priv->properties, ADAPTER_DBUS_INTERFACE, "Discoverable", error);
	if(prop == NULL)
		return FALSE;
	gboolean ret = g_variant_get_boolean(prop);
	g_variant_unref(prop);
	return ret;
}

void adapter_set_discoverable(Adapter *self, const gboolean value, GError **error)
{
	g_assert(ADAPTER_IS(self));
	g_assert(self->priv->properties != NULL);
	g_proxy_properties_set(self->priv->properties, ADAPTER_DBUS_INTERFACE, "Discoverable", g_variant_new_boolean(value), error);
}

guint32 adapter_get_discoverable_timeout(Adapter *self, GError **error)
{
	g_assert(ADAPTER_IS(self));
	g_assert(self->priv->properties != NULL);
	GVariant *prop = g_proxy_properties_get(self->priv->properties, ADAPTER_DBUS_INTERFACE, "DiscoverableTimeout", error);
	if(prop == NULL)
		return 0;
	guint32 ret = g_variant_get_uint32(prop);
	g_variant_unref(prop);
	return ret;
}

void adapter_set_discoverable_timeout(Adapter *self, const guint32 value, GError **error)
{
	g_assert(ADAPTER_IS(self));
	g_assert(self->priv->properties != NULL);
	g_proxy_properties_set(self->priv->properties, ADAPTER_DBUS_INTERFACE, "DiscoverableTimeout", g_variant_new_uint32(value), error);
}

gboolean adapter_get_discovering(Adapter *self, GError **error)
{
	g_assert(ADAPTER_IS(self));
	g_assert(self->priv->properties != NULL);
	GVariant *prop = g_proxy_properties_get(self->priv->properties, ADAPTER_DBUS_INTERFACE, "Discovering", error);
	if(prop == NULL)
		return FALSE;
	gboolean ret = g_variant_get_boolean(prop);
	g_variant_unref(prop);
	return ret;
}

const gchar *adapter_get_modalias(Adapter *self, GError **error)
{
	g_assert(ADAPTER_IS(self));
	g_assert(self->priv->properties != NULL);
	GVariant *prop = g_proxy_properties_get(self->priv->properties, ADAPTER_DBUS_INTERFACE, "Modalias", error);
	if(prop == NULL)
		return NULL;
	const gchar *ret = g_variant_get_string(prop, NULL);
	g_variant_unref(prop);
	return ret;
}

const gchar *adapter_get_name(Adapter *self, GError **error)
{
	g_assert(ADAPTER_IS(self));
	g_assert(self->priv->properties != NULL);
	GVariant *prop = g_proxy_properties_get(self->priv->properties, ADAPTER_DBUS_INTERFACE, "Name", error);
	if(prop == NULL)
		return NULL;
	const gchar *ret = g_variant_get_string(prop, NULL);
	g_variant_unref(prop);
	return ret;
}

gboolean adapter_get_pairable(Adapter *self, GError **error)
{
	g_assert(ADAPTER_IS(self));
	g_assert(self->priv->properties != NULL);
	GVariant *prop = g_proxy_properties_get(self->priv->properties, ADAPTER_DBUS_INTERFACE, "Pairable", error);
	if(prop == NULL)
		return FALSE;
	gboolean ret = g_variant_get_boolean(prop);
	g_variant_unref(prop);
	return ret;
}

void adapter_set_pairable(Adapter *self, const gboolean value, GError **error)
{
	g_assert(ADAPTER_IS(self));
	g_assert(self->priv->properties != NULL);
	g_proxy_properties_set(self->priv->properties, ADAPTER_DBUS_INTERFACE, "Pairable", g_variant_new_boolean(value), error);
}

//
guint32 adapter_get_pairable_timeout(Adapter *self, GError **error)
{
	g_assert(ADAPTER_IS(self));
	g_assert(self->priv->properties != NULL);
	GVariant *prop = g_proxy_properties_get(self->priv->properties, ADAPTER_DBUS_INTERFACE, "PairableTimeout", error);
	if(prop == NULL)
		return 0;
	guint32 ret = g_variant_get_uint32(prop);
	g_variant_unref(prop);
	return ret;
}

//设置适配器配对超时时间
void adapter_set_pairable_timeout(Adapter *self, const guint32 value, GError **error)
{
	g_assert(ADAPTER_IS(self));
	g_assert(self->priv->properties != NULL);
	g_proxy_properties_set(self->priv->properties, ADAPTER_DBUS_INTERFACE, "PairableTimeout", g_variant_new_uint32(value), error);
}

//获取适配器电源状态
gboolean adapter_get_powered(Adapter *self, GError **error)
{
	g_assert(ADAPTER_IS(self));
	g_assert(self->priv->properties != NULL);
	GVariant *prop = g_proxy_properties_get(self->priv->properties, ADAPTER_DBUS_INTERFACE, "Powered", error);
	if(prop == NULL)
		return FALSE;
	gboolean ret = g_variant_get_boolean(prop);
	g_variant_unref(prop);
	return ret;
}

//设置适配器打开关闭
void adapter_set_powered(Adapter *self, const gboolean value, GError **error)
{
	g_assert(ADAPTER_IS(self));
	g_assert(self->priv->properties != NULL);
	g_proxy_properties_set(self->priv->properties, ADAPTER_DBUS_INTERFACE, "Powered", g_variant_new_boolean(value), error);
}

//获取适配器uuid
const gchar **adapter_get_uuids(Adapter *self, GError **error)
{
	g_assert(ADAPTER_IS(self));
	g_assert(self->priv->properties != NULL);
	GVariant *prop = g_proxy_properties_get(self->priv->properties, ADAPTER_DBUS_INTERFACE, "UUIDs", error);
	if(prop == NULL)
		return NULL;
	const gchar **ret = g_variant_get_strv(prop, NULL);
	g_variant_unref(prop);
	return ret;
}

