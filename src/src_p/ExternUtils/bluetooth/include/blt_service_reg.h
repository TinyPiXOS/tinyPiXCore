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
guint register_multi_uuid_service(const gchar **uuids, guint num_uuids,const gchar *name,guint channel,const gchar *role);
guint  register_bluetooth_service(const gchar *uuid, const gchar *name, guint channel, const gchar *role);
gboolean unregister_bluetooth_service(guint service_handle);
gboolean get_service_info(guint service_handle, gchar ***uuids, guint *num_uuids,gchar **name);

#ifdef	__cplusplus
}
#endif

#endif

