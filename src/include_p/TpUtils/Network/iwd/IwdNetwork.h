#ifndef _NETWORK_IWD_NETWORK_H_
#define _NETWORK_IWD_NETWORK_H_

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>
#include <stdbool.h>
#include <gio/gio.h>
#include <glib.h>
#include <glib-object.h>

//#define NM_DEVICE_OBJECT_PATH 		//Device是多实例模式，每个设备都有自己的路径，此处不需要

#define IWD_NETWORK_INTERFACE 		"net.connman.iwd.Network"



#define IWD_NETWORK_TYPE			(iwd_network_get_type ())
#define IWD_NETWORK(obj)			(G_TYPE_CHECK_INSTANCE_CAST ((obj), IWD_NETWORK_TYPE, IwdNetwork))
#define IWD_NETWORK_IS(obj)			(G_TYPE_CHECK_INSTANCE_TYPE((obj), IWD_NETWORK_TYPE))


typedef struct IwdNetwork_ IwdNetwork;
typedef struct IwdNetworkPrivate_ IwdNetworkPrivate;
typedef struct IwdNetworkClass_ IwdNetworkClass;

struct IwdNetwork_{
	GObject parent_instance;		//父类
	IwdNetworkPrivate *priv;
};

struct IwdNetworkClass_ {
	GObjectClass parent_class;
};




GType iwd_network_get_type(void) G_GNUC_CONST;		//此函数由Glib根据G_DEFINE_TYPE_WITH_PRIVATE自动生成，此处声明是为了方便调用

IwdNetwork *iwd_network_create(GDBusConnection *conn, const char *devpath, GError **error);
int iwd_network_delete(IwdNetwork *self);
bool iwd_network_connect(IwdNetwork *network, const gchar *password, int timeout, GError **error);

char* iwd_network_get_name(IwdNetwork *network, GError **error);
char* iwd_network_get_type_str(IwdNetwork *network, GError **error);
bool iwd_network_get_connected(IwdNetwork *network, GError **error);
int iwd_network_get_signal_strength(IwdNetwork *network, GError **error);


#ifdef __cplusplus
}
#endif

#endif
