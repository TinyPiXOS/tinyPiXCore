#ifndef _TPDBUS_LOGIND_H_
#define _TPDBUS_LOGIND_H_

#ifdef __cplusplus
extern "C" {
#endif


#include <stdint.h>
#include <gio/gio.h>
#include <glib.h>

#define LOGIND_DBUS_SERVER			"org.freedesktop.login1"                  // D-Bus 服务名 
#define LOGIND_DBUS_PATH			"/org/freedesktop/login1"                 // 对象路径
#define LOGIND_DBUS_INTERFACE		"org.freedesktop.login1.Manager"		  // 接口


#define LOGIND_TYPE                  (logind_get_type ())
#define LOGIND(obj)                  (G_TYPE_CHECK_INSTANCE_CAST ((obj), LOGIND_TYPE, Logind))

typedef struct Logind_ Logind;
typedef struct LogindPrivate_ LogindPrivate;
typedef struct LogindClass_ LogindClass;

struct Logind_{
	GObject parent_instance;		//父类
	LogindPrivate *priv;
};

struct LogindClass_ {
	GObjectClass parent_class;
};




GType logind_get_type(void) G_GNUC_CONST;		//此函数由Glib根据G_DEFINE_TYPE_WITH_PRIVATE自动生成，此处声明是为了方便调用


Logind *logind_creat(GDBusConnection *conn);
int logind_delete(Logind *self);

int logind_power_off(Logind *self);
int logind_reboot(Logind *self);
int logind_suspend(Logind *self);


#ifdef __cplusplus
}
#endif

#endif
