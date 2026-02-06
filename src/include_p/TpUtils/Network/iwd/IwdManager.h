#ifndef _NETWORK_IWD_MANAGER_H_
#define _NETWORK_IWD_MANAGER_H_

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>
#include <stdbool.h>
#include <gio/gio.h>
#include <glib.h>
#include <glib-object.h>


#define IWD_MANAGER_PATH        	"/"
#define IWD_MANAGER_INTERFACE 		"net.connman.iwd.Manager"



#define IWD_MANAGER_TYPE			(iwd_manager_get_type ())
#define IWD_MANAGER(obj)			(G_TYPE_CHECK_INSTANCE_CAST ((obj), IWD_MANAGER_TYPE, IwdManager))
#define IWD_MANAGER_IS(obj)			(G_TYPE_CHECK_INSTANCE_TYPE((obj), IWD_MANAGER_TYPE))


typedef struct IwdManager_ IwdManager;
typedef struct IwdManagerPrivate_ IwdManagerPrivate;
typedef struct IwdManagerClass_ IwdManagerClass;

struct IwdManager_{
	GObject parent_instance;		//父类
	IwdManagerPrivate *priv;
};

struct IwdManagerClass_ {
	GObjectClass parent_class;
};

// 网络信息结构
typedef struct {
    gchar *ssid;
    gchar *path;
    gint signal_strength;
    gboolean is_connected;
    gboolean is_protected;
} IwdNetworkInfo;

// 设备信息结构
typedef struct {
    gchar *path;
    gchar *name;
    gchar *model;
    gchar *address;
    gboolean is_powered;
} IwdDeviceInfo;



GType iwd_manager_get_type(void) G_GNUC_CONST;		//此函数由Glib根据G_DEFINE_TYPE_WITH_PRIVATE自动生成，此处声明是为了方便调用

IwdManager *iwd_manager_create(GDBusConnection *conn, GError **error);
int iwd_manager_delete(IwdManager *self);


void iwd_network_info_free(IwdNetworkInfo *info);
void iwd_device_info_free(IwdDeviceInfo *info);

#ifdef __cplusplus
}
#endif

#endif
