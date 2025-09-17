#ifndef __HELPERS_H
#define __HELPERS_H

#include <glib.h>
#include <stdio.h>
#include <string.h>

#include "bluetooth_inc.h"

#define BLUETOOTH_BASE_UUID_STR "00000000-0000-1000-8000-00805F9B34FB"

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
const char *uuid_to_name(uint16_t uuid);
uint16_t name_to_uuid(const char *name);

//蓝牙地址的冒号转换为下划线(附带有地址格式校验)
char* convert_bt_addr_format(const char* bt_addr);

/* FS helpers */
gboolean is_file(const gchar *filename, GError **error);
gboolean is_dir(const gchar *dirname, GError **error);
gboolean read_access(const gchar *path, GError **error);
gboolean write_access(const gchar *path, GError **error);
gchar *get_absolute_path(const gchar *path);
gboolean path_to_address(const gchar *object_path, gchar *out_addr, gsize out_len);


// 蓝牙基础UUID（大端序）
static const uint8_t BLUETOOTH_BASE_UUID[16] = {
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x10, 0x00,
    0x80, 0x00, 0x00, 0x80, 0x5F, 0x9B, 0x34, 0xFB
};


gboolean is_base_uuid_format(const uint8_t uuid128[16]);
void uuid16_to_uuid128(uint16_t uuid16, uint8_t uuid128[16]);
void uuid32_to_uuid128(uint32_t uuid32, uint8_t uuid128[16]);
gboolean uuid128_to_uuid16(const uint8_t uuid128[16], uint16_t *uuid16);
gboolean uuid128_to_uuid32(const uint8_t uuid128[16], uint32_t *uuid32);
void uuid128_to_uuidstr(const uint8_t uuid128[16], gchar *str_buf);
gboolean uuidstr_to_uuid128(const gchar *uuidstr, uint8_t uuid128[16]);
void uuid16_to_uuidstr(uint16_t uuid16, gchar *str_buf);
void uuid32_to_uuidstr(uint32_t uuid32, gchar *str_buf);
gboolean uuidstr_to_uuid16(const gchar *uuidstr, uint16_t *uuid16);
gboolean uuidstr_to_uuid32(const gchar *uuidstr, uint32_t *uuid32);
uint32_t uuid16_to_uuid32(uint16_t uuid16);
gboolean uuid32_to_uuid16(uint32_t uuid32, uint16_t *uuid16);
gboolean get_short_uuid_format(const uint8_t uuid128[16], gchar *buf);


#endif /* __HELPERS_H */
