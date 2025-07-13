#ifndef __OBEX_OBJECT_PUSH_H
#define __OBEX_OBJECT_PUSH_H

#ifdef	__cplusplus
extern "C" {
#endif

#include <glib-object.h>

#define OBEX_OBJECT_PUSH_DBUS_SERVICE "org.bluez.obex"
#define OBEX_OBJECT_PUSH_DBUS_INTERFACE "org.bluez.obex.ObjectPush1"

/*
 * Type macros
 */
#define OBEX_OBJECT_PUSH_TYPE				(obex_object_push_get_type())
#define OBEX_OBJECT_PUSH(obj)				(G_TYPE_CHECK_INSTANCE_CAST((obj), OBEX_OBJECT_PUSH_TYPE, ObexObjectPush))
#define OBEX_OBJECT_PUSH_IS(obj)				(G_TYPE_CHECK_INSTANCE_TYPE((obj), OBEX_OBJECT_PUSH_TYPE))
#define OBEX_OBJECT_PUSH_CLASS(klass)		(G_TYPE_CHECK_CLASS_CAST((klass), OBEX_OBJECT_PUSH_TYPE, ObexObjectPushClass))
#define OBEX_OBJECT_PUSH_IS_CLASS(klass)		(G_TYPE_CHECK_CLASS_TYPE((klass), OBEX_OBJECT_PUSH_TYPE))
#define OBEX_OBJECT_PUSH_GET_CLASS(obj)		(G_TYPE_INSTANCE_GET_CLASS((obj), OBEX_OBJECT_PUSH_TYPE, ObexObjectPushClass))

typedef struct _ObexObjectPush ObexObjectPush;
typedef struct _ObexObjectPushClass ObexObjectPushClass;
typedef struct _ObexObjectPushPrivate ObexObjectPushPrivate;

struct _ObexObjectPush {
	GObject parent_instance;

	/*< private >*/
	ObexObjectPushPrivate *priv;
};

struct _ObexObjectPushClass {
	GObjectClass parent_class;
};

/* used by OBEX_OBJECT_PUSH_TYPE */
GType obex_object_push_get_type(void) G_GNUC_CONST;

/*
 * Constructor
 */
ObexObjectPush *obex_object_push_new(const gchar *dbus_object_path);
const gchar *obex_object_push_get_dbus_object_path(ObexObjectPush *self);

GVariant *obex_object_push_exchange_business_cards(ObexObjectPush *self, const gchar *clientfile, const gchar *targetfile, GError **error);
GVariant *obex_object_push_pull_business_card(ObexObjectPush *self, const gchar *targetfile, GError **error);
GVariant *obex_object_push_send_file_sync(ObexObjectPush *self, const gchar *sourcefile, GError **error);
void obex_object_push_send_file_async(ObexObjectPush *self, const gchar *sourcefile, GError **error);
#ifdef	__cplusplus
}
#endif

#endif /* __OBEX_OBJECT_PUSH_H */

