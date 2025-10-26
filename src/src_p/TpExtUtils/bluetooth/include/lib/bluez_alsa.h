#ifndef _BLUEZ_ALSA_H_
#define _BLUEZ_ALSA_H_

#ifdef	__cplusplus
extern "C" {
#endif

#include <stdint.h>
#include <gio/gio.h>
#include <glib.h>


#define BLUEZ_ALSA_DBUS_SERVER			"org.bluealsa"                  // D-Bus 服务名 
#define BLUEZ_ALSA_DBUS_PATH			"/org/bluealsa"                 // 对象路径


#define BLUEZ_ALSA_TYPE                  (bluez_alsa_get_type ())
#define BLUEZ_ALSA(obj)                  (G_TYPE_CHECK_INSTANCE_CAST ((obj), BLUEZ_ALSA_TYPE, BluezAlsa))

typedef struct BluezAlsa_ BluezAlsa;
typedef struct BluezAlsaPrivate_ BluezAlsaPrivate;
typedef struct BluezAlsaClass_ BluezAlsaClass;

struct BluezAlsa_{
	GObject parent_instance;		//父类
	
	BluezAlsaPrivate *priv;
};

struct BluezAlsaClass_ {
	GObjectClass parent_class;
};






GType bluez_alsa_get_type(void) G_GNUC_CONST;		//此函数由Glib根据G_DEFINE_TYPE_WITH_PRIVATE自动生成，此处声明是为了方便调用


BluezAlsa *bluez_alsa_creat();
int bluez_alsa_delete(BluezAlsa *alsa );

void bluez_alsa_create_gdbus_proxy(BluezAlsa *alsa, const gchar *dbus_server, const gchar *dbus_path,GError **error);
void bluez_alsa_get_adapters(BluezAlsa *alsa,GError **error);

gboolean ensure_bluealsa_running (GError **error);


#ifdef	__cplusplus
}
#endif

#endif
