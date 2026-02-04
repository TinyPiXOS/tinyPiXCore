#ifndef _TPDBUS_NM_SETTINGS_H_
#define _TPDBUS_NM_SETTINGS_H_

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>
#include <gio/gio.h>
#include <glib.h>
#include <glib-object.h>
#include "Network/nm/NmConnection.h"

#define NM_SETTINGS_OBJECT_PATH       	"/org/freedesktop/NetworkManager/Settings"	//对象路径，Settings为单实例
#define NM_SETTINGS_INTERFACE 			"org.freedesktop.NetworkManager.Settings"


#define NM_SETTINGS_TYPE			(nm_device_get_type ())
#define NM_SETTINGS(obj)			(G_TYPE_CHECK_INSTANCE_CAST ((obj), NM_SETTINGS_TYPE, NmSettings))
#define NM_SETTINGS_IS(obj)			(G_TYPE_CHECK_INSTANCE_TYPE((obj), NM_SETTINGS_TYPE))


typedef struct NmSettings_ NmSettings;
typedef struct NmSettingsPrivate_ NmSettingsPrivate;
typedef struct NmSettingsClass_ NmSettingsClass;

struct NmSettings_{
	GObject parent_instance;		//父类
	NmSettingsPrivate *priv;
};

struct NmSettingsClass_ {
	GObjectClass parent_class;
};




GType nm_settings_get_type(void) G_GNUC_CONST;		//此函数由Glib根据G_DEFINE_TYPE_WITH_PRIVATE自动生成，此处声明是为了方便调用


NmSettings *nm_settings_create(GDBusConnection *conn, GError **error);
int nm_settings_delete(NmSettings *self);

//列出系统所有配置
GList *nm_settings_list_connections(NmSettings *self, GError **error);

//根据 配置名 查找 配置(connection)的object_path
//conn_name 配置名
char *nm_settings_find_connection_object(NmSettings *self, const char *conn_name, GError **error);

//setting添加配置
char *nm_settings_add_connection(NmSettings *self, GVariant *settings, GError **error);



#ifdef __cplusplus
}
#endif

#endif
