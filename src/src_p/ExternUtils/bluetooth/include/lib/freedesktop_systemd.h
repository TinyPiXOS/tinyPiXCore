#ifndef _FREEDESKTOP_SYSTEMD_H_
#define _FREEDESKTOP_SYSTEMD_H_

#ifdef	__cplusplus
extern "C" {
#endif

#include <stdint.h>
#include <gio/gio.h>
#include <glib.h>

#define FREEDESKTOP_SYSTEMD_DBUS_SERVER			"org.freedesktop.systemd1"                  // D-Bus 服务名 
#define FREEDESKTOP_SYSTEMD_DBUS_PATH			"/org/freedesktop/systemd1"                 // 对象路径


#define FREEDESKTOP_SYSTEMD_TYPE                  (desktop_system_get_type ())
#define FREEDESKTOP_SYSTEMD(obj)                  (G_TYPE_CHECK_INSTANCE_CAST ((obj), BLUEZ_ALSA_TYPE, DesktopSystem))

typedef struct DesktopSystem_ DesktopSystem;
typedef struct DesktopSystemPrivate_ DesktopSystemPrivate;
typedef struct DesktopSystemClass_ DesktopSystemClass;

struct DesktopSystem_{
	GObject parent_instance;		//父类
	
	DesktopSystemPrivate *priv;
};

struct DesktopSystemClass_ {
	GObjectClass parent_class;
};






GType desktop_system_get_type(void) G_GNUC_CONST;		//此函数由Glib根据G_DEFINE_TYPE_WITH_PRIVATE自动生成，此处声明是为了方便调用


DesktopSystem *desktop_system_creat();
int desktop_system_delete(DesktopSystem *system);

int desktop_system_start_unit(DesktopSystem *system, const char *server, const char *mode,GError **error);
int desktop_system_stop_unit(DesktopSystem *system, const char *server, const char *mode,GError **error);
int desktop_system_restart_unit(DesktopSystem *system, const char *server, const char *mode,GError **error);
char *desktop_system_get_unit(DesktopSystem *system, const char *server, GError **error);

char *desktop_system_get_service_active_state(DesktopSystem *system,const char *server, GError **error);



#ifdef	__cplusplus
}
#endif

#endif
