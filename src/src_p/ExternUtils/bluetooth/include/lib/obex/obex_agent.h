#ifndef __OBEX_AGENT_H
#define __OBEX_AGENT_H

#ifdef	__cplusplus
extern "C" {
#endif

#include <glib-object.h>

#define OBEX_AGENT_DBUS_SERVICE "org.blueztools"
#define OBEX_AGENT_DBUS_INTERFACE "org.bluez.obex.Agent1"
#define OBEX_AGENT_DBUS_PATH "/org/blueztools/obex"

/*
 * Type macros
 */
#define OBEX_AGENT_TYPE				(obex_agent_get_type())
#define OBEX_AGENT(obj)				(G_TYPE_CHECK_INSTANCE_CAST((obj), OBEX_AGENT_TYPE, ObexAgent))
#define OBEX_AGENT_IS(obj)				(G_TYPE_CHECK_INSTANCE_TYPE((obj), OBEX_AGENT_TYPE))
#define OBEX_AGENT_CLASS(klass)		(G_TYPE_CHECK_CLASS_CAST((klass), OBEX_AGENT_TYPE, ObexAgentClass))
#define OBEX_AGENT_IS_CLASS(klass)		(G_TYPE_CHECK_CLASS_TYPE((klass), OBEX_AGENT_TYPE))
#define OBEX_AGENT_GET_CLASS(obj)		(G_TYPE_INSTANCE_GET_CLASS((obj), OBEX_AGENT_TYPE, ObexAgentClass))

typedef struct _ObexAgent ObexAgent;
typedef struct _ObexAgentClass ObexAgentClass;
typedef struct _ObexAgentPrivate ObexAgentPrivate;

struct _ObexAgent {
	GObject parent_instance;

	/*< private >*/
	ObexAgentPrivate *priv;
};

struct _ObexAgentClass {
	GObjectClass parent_class;
};

/* used by OBEX_AGENT_TYPE */
GType obex_agent_get_type(void) G_GNUC_CONST;

// agent released callback pointer function
typedef void (*ObexAgentReleasedCallback)(ObexAgent *obex_agent, gpointer user_data);

// agent approved callback pointer function
typedef void (*ObexAgentApprovedCallback)(ObexAgent *obex_agent, const gchar* obex_transfer_path, const gchar *name, const guint64 size, gpointer user_data);

/*
 * Constructor
 */
ObexAgent *obex_agent_new(const gchar *root_folder, const gboolean auto_accept);

/*
 * Method definitions
 */
void obex_agent_progress(ObexAgent *self, const gchar *transfer, guint64 transferred, GError **error);

void obex_agent_set_release_callback(ObexAgent *self, ObexAgentReleasedCallback callback_function, gpointer user_data);
void obex_agent_clear_release_callback(ObexAgent *self);

void obex_agent_set_approved_callback(ObexAgent *self, ObexAgentApprovedCallback callback_function, gpointer user_data);
void obex_agent_clear_approved_callback(ObexAgent *self);

#ifdef	__cplusplus
}
#endif

#endif /* __OBEX_AGENT_H */

