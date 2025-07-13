#ifndef __ADAPTER_H_
#define __ADAPTER_H_

#ifdef	__cplusplus
extern "C" {
#endif

#include <glib-object.h>

#define ADAPTER_DBUS_SERVICE "org.bluez"
#define ADAPTER_DBUS_INTERFACE "org.bluez.Adapter1"

/*
 * Type macros
 */
#define ADAPTER_TYPE				(adapter_get_type())
#define ADAPTER(obj)				(G_TYPE_CHECK_INSTANCE_CAST((obj), ADAPTER_TYPE, Adapter))
#define ADAPTER_IS(obj)				(G_TYPE_CHECK_INSTANCE_TYPE((obj), ADAPTER_TYPE))
#define ADAPTER_CLASS(klass)		(G_TYPE_CHECK_CLASS_CAST((klass), ADAPTER_TYPE, AdapterClass))
#define ADAPTER_IS_CLASS(klass)		(G_TYPE_CHECK_CLASS_TYPE((klass), ADAPTER_TYPE))
#define ADAPTER_GET_CLASS(obj)		(G_TYPE_INSTANCE_GET_CLASS((obj), ADAPTER_TYPE, AdapterClass))

typedef struct _Adapter Adapter;
typedef struct _AdapterClass AdapterClass;
typedef struct _AdapterPrivate AdapterPrivate;

struct _Adapter {
	GObject parent_instance;

	/*< private >*/
	AdapterPrivate *priv;
};

struct _AdapterClass {
	GObjectClass parent_class;
};

/* used by ADAPTER_TYPE */
GType adapter_get_type(void) G_GNUC_CONST;

/*
 * Constructor
 */
Adapter *adapter_new(const gchar *dbus_object_path);

/*
 * Method definitions
 */
const gchar *adapter_get_dbus_object_path(Adapter *self);

void adapter_remove_device(Adapter *self, const gchar *device, GError **error);
void adapter_start_discovery(Adapter *self, GError **error);
void adapter_stop_discovery(Adapter *self, GError **error);

GVariant *adapter_get_properties(Adapter *self, GError **error);
void adapter_set_property(Adapter *self, const gchar *name, const GVariant *value, GError **error);

const gchar *adapter_get_address(Adapter *self, GError **error);
const gchar *adapter_get_alias(Adapter *self, GError **error);
void adapter_set_alias(Adapter *self, const gchar *value, GError **error);
guint32 adapter_get_class(Adapter *self, GError **error);
gboolean adapter_get_discoverable(Adapter *self, GError **error);
void adapter_set_discoverable(Adapter *self, const gboolean value, GError **error);
guint32 adapter_get_discoverable_timeout(Adapter *self, GError **error);
void adapter_set_discoverable_timeout(Adapter *self, const guint32 value, GError **error);
gboolean adapter_get_discovering(Adapter *self, GError **error);
const gchar *adapter_get_modalias(Adapter *self, GError **error);
const gchar *adapter_get_name(Adapter *self, GError **error);
gboolean adapter_get_pairable(Adapter *self, GError **error);
void adapter_set_pairable(Adapter *self, const gboolean value, GError **error);
guint32 adapter_get_pairable_timeout(Adapter *self, GError **error);
void adapter_set_pairable_timeout(Adapter *self, const guint32 value, GError **error);
gboolean adapter_get_powered(Adapter *self, GError **error);
void adapter_set_powered(Adapter *self, const gboolean value, GError **error);
const gchar **adapter_get_uuids(Adapter *self, GError **error);

#ifdef	__cplusplus
}
#endif

#endif /* __ADAPTER_H */

