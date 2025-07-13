#ifndef _FREEDESKTOP_UPOWER_H_
#define _FREEDESKTOP_UPOWER_H_

#ifdef	__cplusplus
extern "C" {
#endif

#include <stdint.h>
#include <gio/gio.h>
#include <glib.h>

#define FREEDESKTOP_UPOWER_DBUS_SERVER			"org.freedesktop.UPower"                  // D-Bus 服务名 
#define FREEDESKTOP_UPOWER_DBUS_PATH			"/org/freedesktop/UPower"                 // 对象路径


#define FREEDESKTOP_UPOWER_TYPE                  (desktop_upower_get_type ())
#define FREEDESKTOP_UPOWER(obj)                  (G_TYPE_CHECK_INSTANCE_CAST ((obj), FREEDESKTOP_UPOWER_TYPE, DesktopSystem))

typedef struct DesktopUPower_ DesktopUPower;
typedef struct DesktopUPowerPrivate_ DesktopUPowerPrivate;
typedef struct DesktopUPowerClass_ DesktopUPowerClass;

struct DesktopUPower_{
	GObject parent_instance;		//父类
	
	DesktopUPowerPrivate *priv;
};

struct DesktopUPowerClass_ {
	GObjectClass parent_class;
};



typedef enum{
	UPOWER_STATE_CHARGE=1,
	UPOWER_STATE_DISCHARGE=2,
	UPOWER_STATE_FULL=4
	//1 = 正在充电，2 = 放电中，4 = 已充满
}UpowerStateType;

typedef void (*UpowerDeviceListCallback)(const char *device_path, void* user_data);


GType desktop_upower_get_type(void) G_GNUC_CONST;		//此函数由Glib根据G_DEFINE_TYPE_WITH_PRIVATE自动生成，此处声明是为了方便调用


DesktopUPower *desktop_upower_creat(GDBusConnection *conn);
int desktop_upower_delete(DesktopUPower *self);

int upower_get_path_list(DesktopUPower *self,UpowerDeviceListCallback callback, void *user);

#ifdef	__cplusplus
}
#endif

#endif
