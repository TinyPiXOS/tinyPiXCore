#ifndef _MEDIA_TRANSPORT_H_
#define _MEDIA_TRANSPORT_H_

#ifdef	__cplusplus
extern "C" {
#endif

#include <stdint.h>
#include <gio/gio.h>
#include <glib.h>


#define MEDIA_TRANSPORT_DBUS_SERVER			"org.bluez"                  // D-Bus 服务名 
//#define MEDIA_TRANSPORT_DBUS_PATH			"/org/bluealsa"                 // 对象路径


#define MEDIA_TRANSPORT_TYPE                  (media_transport_get_type ())
#define MEDIA_TRANSPORT(obj)                  (G_TYPE_CHECK_INSTANCE_CAST ((obj), MEDIA_TRANSPORT_TYPE, MediaTransport))

typedef struct MediaTransport_ MediaTransport;
typedef struct MediaTransportPrivate_ MediaTransportPrivate;
typedef struct MediaTransportClass_ MediaTransportClass;

struct MediaTransport_{
	GObject parent_instance;		//父类
	
	MediaTransportPrivate *priv;
};

struct MediaTransportClass_ {
	GObjectClass parent_class;
};



GType media_transport_get_type(void) G_GNUC_CONST;		//此函数由Glib根据G_DEFINE_TYPE_WITH_PRIVATE自动生成，此处声明是为了方便调用


void media_transport_create_gdbus_proxy(MediaTransport *self, const gchar *dbus_server, GError **error);

void media_transport_set_profile(MediaTransport *self,const gchar *profile,GError **error);
void media_transport_select_codec(MediaTransport *self,const gchar *codec,GError **error);

#ifdef	__cplusplus
}
#endif

#endif
