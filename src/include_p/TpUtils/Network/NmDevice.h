#ifndef _TPDBUS_NM_DEVICE_H_
#define _TPDBUS_NM_DEVICE_H_

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>
#include <gio/gio.h>
#include <glib.h>
#include <glib-object.h>

//#define NM_DEVICE_OBJECT_PATH 		//Device是多实例模式，每个设备都有自己的路径，此处不需要
#define NM_DEVICE_INTERFACE 			"org.freedesktop.NetworkManager.Device"


#define NM_DEVICE_TYPE			(nm_device_get_type ())
#define NM_DEVICE(obj)			(G_TYPE_CHECK_INSTANCE_CAST ((obj), NM_DEVICE_TYPE, NmDevice))
#define NM_DEVICE_IS(obj)		(G_TYPE_CHECK_INSTANCE_TYPE((obj), NM_DEVICE_TYPE))


typedef struct NmDevice_ NmDevice;
typedef struct NmDevicePrivate_ NmDevicePrivate;
typedef struct NmDeviceClass_ NmDeviceClass;

struct NmDevice_{
	GObject parent_instance;		//父类
	NmDevicePrivate *priv;
};

struct NmDeviceClass_ {
	GObjectClass parent_class;
};




GType nm_device_get_type(void) G_GNUC_CONST;		//此函数由Glib根据G_DEFINE_TYPE_WITH_PRIVATE自动生成，此处声明是为了方便调用

NmDevice *nm_device_create(GDBusConnection *conn, const char *devpath, GError **error);
int nm_device_delete(NmDevice *self);


#ifdef __cplusplus
}
#endif

#endif
