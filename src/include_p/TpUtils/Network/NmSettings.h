#ifndef _TPDBUS_NM_SETTINGS_H_
#define _TPDBUS_NM_SETTINGS_H_

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>
#include <gio/gio.h>
#include <glib.h>
#include <glib-object.h>

#define NM_SETTINGS_OBJECT_PATH       	"/org/freedesktop/NetworkManager/Settings"
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




#ifdef __cplusplus
}
#endif

#endif
