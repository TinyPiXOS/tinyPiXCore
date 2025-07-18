#include <gio/gio.h>
//#include "bluetooth_inc.h"
#include "manager.h"

struct _ManagerPrivate
{
	GDBusConnection *conn;
    GDBusProxy *proxy;

	gchar *dbus_service_name;
};

G_DEFINE_TYPE_WITH_PRIVATE(Manager, manager, G_TYPE_OBJECT)

enum
{
	PROP_0,
	PROP_DBUS_CONNECTION, 
	PROP_DBUS_SERVICE_NAME,
	N_PROPERTIES
};

static GParamSpec *obj_props[N_PROPERTIES]={NULL,};

static void manager_set_property(GObject *object, guint prop_id, const GValue *value, GParamSpec *pspec);
static void manager_get_property(GObject *object, guint prop_id, GValue *value, GParamSpec *pspec);
static void manager_constructed(GObject *gobject);

static void manager_dispose (GObject *gobject)
{
    Manager *self = MANAGER (gobject);

    /* In dispose(), you are supposed to free all types referenced from this
     * object which might themselves hold a reference to self. Generally,
     * the most simple solution is to unref all members on which you own a 
     * reference.
     */

    /* dispose() might be called multiple times, so we must guard against
     * calling g_object_unref() on an invalid GObject by setting the member
     * NULL; g_clear_object() does this for us, atomically.
     */
    // g_clear_object (&self->priv->an_object);
    g_clear_object (&self->priv->proxy);
	g_clear_object (&self->priv->conn);		//不确定是否需要这一步

    /* Always chain up to the parent class; there is no need to check if
     * the parent class implements the dispose() virtual function: it is
     * always guaranteed to do so
     */
    G_OBJECT_CLASS (manager_parent_class)->dispose (gobject);
}

static void manager_finalize (GObject *gobject)
{
    Manager *self = MANAGER(gobject);

    // g_free(self->priv->a_string);

    /* Always chain up to the parent class; as with dispose(), finalize()
     * is guaranteed to exist on the parent's class virtual function table
     */
    G_OBJECT_CLASS (manager_parent_class)->finalize (gobject);
}


static void manager_class_init(ManagerClass *klass)
{
	GObjectClass *gobject_class = G_OBJECT_CLASS (klass);

	gobject_class->set_property = manager_set_property;
	gobject_class->get_property = manager_get_property;
	gobject_class->constructed = manager_constructed;

	obj_props[PROP_DBUS_CONNECTION] = 
		g_param_spec_object("connection",
						"Connection",
						"System or session bus connection",
						G_TYPE_DBUS_CONNECTION,
						G_PARAM_CONSTRUCT_ONLY | G_PARAM_READWRITE);

	obj_props[PROP_DBUS_SERVICE_NAME] =
    g_param_spec_string ("DBusServiceName",
                         "dbus_service_name",
                         "Properties D-Bus service name",
                         NULL /* default value */,
                         G_PARAM_CONSTRUCT_ONLY | G_PARAM_READWRITE);

	g_object_class_install_properties (G_OBJECT_CLASS (klass),N_PROPERTIES,obj_props);
}

static void manager_init(Manager *self)
{
    self->priv = manager_get_instance_private(self);
    GError *error = NULL;
	self->priv->dbus_service_name = NULL;
    /*g_assert(self->priv->conn != NULL);
    self->priv->proxy = g_dbus_proxy_new_sync(self->priv->conn, G_DBUS_PROXY_FLAGS_NONE, NULL, "org.bluez", MANAGER_DBUS_PATH, MANAGER_DBUS_INTERFACE, NULL, &error);

    if (self->priv->proxy == NULL)
    {
        g_critical("%s", error->message);
    }
    g_assert(error == NULL);*/
}

static void manager_constructed(GObject *gobject)
{
	Manager    *self = MANAGER(gobject);
	self->priv = manager_get_instance_private(self);
	GError     *error = NULL;

	/* 只有在所有 construct-only 属性都注入后才会运行 */
	g_assert(self->priv->conn != NULL);
	self->priv->proxy = g_dbus_proxy_new_sync(self->priv->conn, G_DBUS_PROXY_FLAGS_NONE, NULL, "org.bluez", MANAGER_DBUS_PATH, MANAGER_DBUS_INTERFACE, NULL, &error);
	
	if (self->priv->proxy == NULL)
    {
        g_critical("%s", error->message);
    }
    g_assert(error == NULL);
	/* 链到父类 constructed，完成生命周期 */
	G_OBJECT_CLASS(manager_parent_class)->constructed(gobject);
}

static void manager_set_property(GObject *object, guint prop_id, const GValue *value, GParamSpec *pspec)
{
    Manager *self = MANAGER(object);
	GError *error = NULL;

    switch (prop_id) {
		case PROP_DBUS_CONNECTION:
			if (self->priv->conn)
				g_object_unref(self->priv->conn);
			self->priv->conn = g_value_get_object(value);		//g_value_dup_object
			break;
		case PROP_DBUS_SERVICE_NAME:
			if(self->priv->dbus_service_name)
				g_free(self->priv->dbus_service_name);
			self->priv->dbus_service_name = g_value_dup_string(value);
			// _properties_set_dbus_object_path(self, g_value_dup_string(value), &error);
			//_properties_create_gdbus_proxy(self, &error);
			break;
		default:
			G_OBJECT_WARN_INVALID_PROPERTY_ID(object, prop_id, pspec);
    }
	
	g_assert(error == NULL);
}

static void manager_get_property(GObject *object, guint prop_id, GValue *value, GParamSpec *pspec)
{
	ManagerPrivate *priv = manager_get_instance_private(MANAGER(object));

	switch (prop_id) {
		case PROP_DBUS_CONNECTION:
			/* 将私有的 conn 以返回引用方式暴露 */
			g_value_set_object(value, priv->conn);
			break;
		default:
			G_OBJECT_WARN_INVALID_PROPERTY_ID(object, prop_id, pspec);
	}
}

Manager *manager_new(GDBusConnection *conn)
{
    return g_object_new(MANAGER_TYPE, "connection", conn, NULL);
}

GVariant *manager_get_managed_objects(Manager *self, GError **error)
{
    g_assert(MANAGER_IS(self));

    GVariant *retVal = NULL;
    retVal = g_dbus_proxy_call_sync(self->priv->proxy, "GetManagedObjects", NULL, G_DBUS_CALL_FLAGS_NONE, -1, NULL, error);

    if (retVal != NULL)
        retVal = g_variant_get_child_value(retVal, 0);

    return retVal;
}

const gchar *manager_default_adapter(Manager *self, GError **error)
{
    g_assert(MANAGER_IS(self));

    GVariant *objects = NULL;
    objects = manager_get_managed_objects(self, error);
    if (objects == NULL)
        return NULL;

    const gchar *object_path;
    GVariant *ifaces_and_properties;
    GVariantIter i;

    g_variant_iter_init(&i, objects);
    while (g_variant_iter_next(&i, "{&o@a{sa{sv}}}", &object_path, &ifaces_and_properties))
    {
        const gchar *interface_name;
        GVariant *properties;
        GVariantIter ii;
        g_variant_iter_init(&ii, ifaces_and_properties);
        while (g_variant_iter_next(&ii, "{&s@a{sv}}", &interface_name, &properties))
        {
            if (g_strstr_len(g_ascii_strdown(interface_name, -1), -1, "adapter"))
            {
                const gchar *retVal = g_strdup(object_path);
                g_variant_unref(properties);
                g_variant_unref(ifaces_and_properties);
                g_variant_unref(objects);
                return retVal;
            }
            g_variant_unref(properties);
        }
        g_variant_unref(ifaces_and_properties);
    }
    g_variant_unref(objects);

    return NULL;
}

const gchar *manager_find_adapter(Manager *self, const gchar *pattern, GError **error)
{
    g_assert(MANAGER_IS(self));

    GVariant *objects = NULL;
    objects = manager_get_managed_objects(self, error);
    if (objects == NULL)
        return NULL;

    const gchar *object_path;
    GVariant *ifaces_and_properties;
    GVariantIter i;

    g_variant_iter_init(&i, objects);
    while (g_variant_iter_next(&i, "{&o@a{sa{sv}}}", &object_path, &ifaces_and_properties))
    {
        const gchar *interface_name;
        GVariant *properties;
        GVariantIter ii;
        g_variant_iter_init(&ii, ifaces_and_properties);
        while (g_variant_iter_next(&ii, "{&s@a{sv}}", &interface_name, &properties))
        {
            if (g_strstr_len(g_ascii_strdown(interface_name, -1), -1, "adapter"))
            {
                const gchar *object_base_name = g_path_get_basename(object_path);
                if (g_strstr_len(g_ascii_strdown(object_base_name, -1), -1, g_ascii_strdown(pattern, -1)))
                {
                    const gchar *retVal = g_strdup(object_path);
                    g_variant_unref(properties);
                    g_variant_unref(ifaces_and_properties);
                    g_variant_unref(objects);
                    return retVal;
                }
                const gchar *address = g_variant_get_string(g_variant_lookup_value(properties, "Address", NULL), NULL);
                if (g_strstr_len(g_ascii_strdown(address, -1), -1, g_ascii_strdown(pattern, -1)))
                {
                    gchar *retVal = g_strdup(object_path);
                    g_variant_unref(properties);
                    g_variant_unref(ifaces_and_properties);
                    g_variant_unref(objects);
                    return retVal;
                }
            }
            g_variant_unref(properties);
        }
        g_variant_unref(ifaces_and_properties);
    }
    g_variant_unref(objects);

    return NULL;
}

GPtrArray *manager_get_adapters(Manager *self)
{
    g_assert(MANAGER_IS(self));

    GVariant *objects = NULL;
    GError *error = NULL;
    objects = manager_get_managed_objects(self, &error);
    if (objects == NULL)
    {
        g_critical("%s", error->message);
        g_error_free(error);
        return NULL;
    }

    GPtrArray *adapter_array = g_ptr_array_new();

    const gchar *object_path;
    GVariant *ifaces_and_properties;
    GVariantIter i;

    g_variant_iter_init(&i, objects);
    while (g_variant_iter_next(&i, "{&o@a{sa{sv}}}", &object_path, &ifaces_and_properties))
    {
        const gchar *interface_name;
        GVariant *properties;
        GVariantIter ii;
        g_variant_iter_init(&ii, ifaces_and_properties);
        while (g_variant_iter_next(&ii, "{&s@a{sv}}", &interface_name, &properties))
        {
            if (g_strstr_len(g_ascii_strdown(interface_name, -1), -1, "adapter"))
                g_ptr_array_add(adapter_array, (gpointer) g_strdup(object_path));
            g_variant_unref(properties);
        }
        g_variant_unref(ifaces_and_properties);
    }
    g_variant_unref(objects);

    return adapter_array;
}

const gchar **manager_get_devices(Manager *self, const gchar *adapter_pattern)
{
    g_assert(MANAGER_IS(self));

    GVariant *objects = NULL;
    GError *error = NULL;
    objects = manager_get_managed_objects(self, &error);
    if (objects == NULL)
    {
        g_critical("%s", error->message);
        g_error_free(error);
        return NULL;
    }
    
    GRegex *adapter_regex = g_regex_new(adapter_pattern, 0, 0, &error);
    if (adapter_regex == NULL)
    {
        g_critical("%s", error->message);
        g_error_free(error);
    }
    
    GPtrArray *device_array = g_ptr_array_new();
    
    const gchar *object_path;
    GVariant *ifaces_and_properties;
    GVariantIter i;
    
    g_variant_iter_init(&i, objects);
    while (g_variant_iter_next(&i, "{&o@a{sa{sv}}}", &object_path, &ifaces_and_properties))
    {
        const gchar *interface_name;
        GVariant *properties;
        GVariantIter ii;
        g_variant_iter_init(&ii, ifaces_and_properties);
        while (g_variant_iter_next(&ii, "{&s@a{sv}}", &interface_name, &properties))
        {
            if (g_strcmp0(interface_name, "org.bluez.Device1") == 0)
            {
                const gchar *adapter_prop = g_variant_get_string(g_variant_lookup_value(properties, "Adapter", G_VARIANT_TYPE_OBJECT_PATH), NULL);
                if(g_regex_match(adapter_regex, adapter_prop, 0, NULL))
                    g_ptr_array_add(device_array, (gpointer) g_strdup(object_path));
            }
            g_variant_unref(properties);
        }
        g_variant_unref(ifaces_and_properties);
    }
    g_variant_unref(objects);

    g_regex_unref(adapter_regex);
    
    if(device_array->len > 0)
    {
        // Top it off with a NULL pointer
        g_ptr_array_add(device_array, (gpointer) NULL);
        return (const gchar**) g_ptr_array_free(device_array, FALSE);
    }
    else
        return NULL;
}