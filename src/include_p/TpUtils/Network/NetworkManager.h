#ifndef _TPDBUS_NETWORK_MANAGER_H_
#define _TPDBUS_NETWORK_MANAGER_H_

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>
#include <gio/gio.h>
#include <glib.h>
#include <glib-object.h>
#include "Network/NmDevice.h"
#include "Network/NmConnection.h"

#define NETWORK_MANAGER_DBUS_SERVER			"org.freedesktop.NetworkManager"          // D-Bus 服务名 
#define NETWORK_MANAGER_DBUS_PATH			"/org/freedesktop/NetworkManager"         // 对象路径，NetworkManager只有一个，单实例模式
#define NETWORK_MANAGER_DBUS_INTERFACE		"org.freedesktop.NetworkManager"		  // 接口


#define NETWORK_MANAGER_TYPE                  (network_manager_get_type ())
#define NETWORK_MANAGER(obj)                  (G_TYPE_CHECK_INSTANCE_CAST ((obj), NETWORK_MANAGER_TYPE, NetworkManager))
#define NETWORK_MANAGER_IS(obj) (G_TYPE_CHECK_INSTANCE_TYPE((obj), NETWORK_MANAGER_TYPE))
#define NETWORK_MANAGER_CLASS(klass) (G_TYPE_CHECK_CLASS_CAST((klass), NETWORK_MANAGER_TYPE, NetworkManagerClass))
#define NETWORK_MANAGER_IS_CLASS(klass) (G_TYPE_CHECK_CLASS_TYPE((klass), NETWORK_MANAGER_TYPE))
#define NETWORK_MANAGER_GET_CLASS(obj) (G_TYPE_INSTANCE_GET_CLASS((obj), NETWORK_MANAGER_TYPE, NetworkManagerClass))

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


NetworkManager *network_manager_create(GDBusConnection *conn);
int network_manager_delete(NetworkManager *self);

//备用的设备路径获取接口，当GetDeviceByIpIface不可用时候可以使用此接口
char *network_manager_get_device_path_by_iface_fallback(NetworkManager *self, const char *iface_name, GError **error);
//设备路径获取接口
char *network_manager_get_device_path_by_iface(NetworkManager *self, const char *iface_name, GError **error);
//设备路径释放后接口
void network_manager_free_device_path(char *device);
//连接激活接口
int network_manager_activate_connection_to_device(NetworkManager *self, NmConnection *conn, NmDevice *dev, GError **error);
//连接是否激活
int network_manager_is_connection_active(NetworkManager *self, const char *conn_name);

#ifdef __cplusplus
}
#endif

#endif
