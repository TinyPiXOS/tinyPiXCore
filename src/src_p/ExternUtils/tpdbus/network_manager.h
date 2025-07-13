#ifndef _TPDBUS_NETWORK_MANAGER_H_
#define _TPDBUS_NETWORK_MANAGER_H_

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>
#include <gio/gio.h>
#include <glib.h>

#define NETWORK_MANAGER_DBUS_SERVER			"org.freedesktop.NetworkManager"                  // D-Bus 服务名 
#define NETWORK_MANAGER_DBUS_PATH			"/org/freedesktop/NetworkManager"                 // 对象路径
#define NETWORK_MANAGER_DBUS_INTERFACE		"org.freedesktop.NetworkManager"		  // 接口


#define NETWORK_MANAGER_TYPE                  (network_manager_get_type ())
#define NETWORK_MANAGER(obj)                  (G_TYPE_CHECK_INSTANCE_CAST ((obj), NETWORK_MANAGER_TYPE, NetworkManager))

typedef struct NetworkManager_ NetworkManager;
typedef struct NetworkManagerPrivate_ NetworkManagerPrivate;
typedef struct NetworkManagerClass_ NetworkManagerClass;

struct NetworkManager_{
	GObject parent_instance;		//父类
	NetworkManagerPrivate *priv;
};

struct NetworkManagerClass_ {
	GObjectClass parent_class;
};




GType network_manager_get_type(void) G_GNUC_CONST;		//此函数由Glib根据G_DEFINE_TYPE_WITH_PRIVATE自动生成，此处声明是为了方便调用


NetworkManager *network_manager_creat(GDBusConnection *conn);
int network_manager_delete(NetworkManager *self);

int network_manager_get_dhcp_status(const char *ifname);


#ifdef __cplusplus
}
#endif

#endif
