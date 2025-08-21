#ifndef _BLUET_SERVICE_REG_H_
#define _BLUET_SERVICE_REG_H_

#ifdef	__cplusplus
extern "C" {
#endif

#include <gio/gio.h>
#include <glib.h>
#include <glib-object.h>

void service_registry_init();
void service_registry_cleanup();
gboolean register_bluetooth_service(const gchar *service_id, const gchar *uuid, const gchar *name, guint channel, const gchar *role);
gboolean unregister_bluetooth_service(const gchar *service_id);
GList *get_registered_services();

#ifdef	__cplusplus
}
#endif

#endif

