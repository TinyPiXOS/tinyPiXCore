#ifndef __HELPERS_H
#define __HELPERS_H

#include <glib.h>
#include <stdio.h>
#include <string.h>

#include "bluetooth_inc.h"

#define bluet_object_free(device) g_object_unref(device)
/* DBus helpers */
gboolean intf_supported(const gchar *dbus_service_name, const gchar *dbus_object_path, const gchar *intf_name);

/* BlueZ helpers */
Adapter *find_adapter(const gchar *name, GError **error);
Device *find_device(Adapter *adapter, const gchar *name, GError **error);

/* Others helpers */
#define exit_if_error(error) G_STMT_START{ \
if (error) { \
	g_printerr("%s: %s\n", (error->domain == G_DBUS_ERROR && g_dbus_error_get_remote_error(error) != NULL && strlen(g_dbus_error_get_remote_error(error)) ? g_dbus_error_get_remote_error(error) : "Error"), error->message); \
	exit(EXIT_FAILURE); \
}; }G_STMT_END

/* Convert hex string to int */
int xtoi(const gchar *str);

/* UUID converters */
const char *uuid_to_name(const char *uuid);
const char *name_to_uuid(const char *name);

//蓝牙地址的冒号转换为下划线(附带有地址格式校验)
char* convert_bt_addr_format(const char* bt_addr);

/* FS helpers */
gboolean is_file(const gchar *filename, GError **error);
gboolean is_dir(const gchar *dirname, GError **error);
gboolean read_access(const gchar *path, GError **error);
gboolean write_access(const gchar *path, GError **error);
gchar *get_absolute_path(const gchar *path);
gboolean path_to_address(const gchar *object_path, gchar *out_addr, gsize out_len);

#endif /* __HELPERS_H */
