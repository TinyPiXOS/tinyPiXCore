#ifndef _NETWORK_IWD_STATION_H_
#define _NETWORK_IWD_STATION_H_

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>
#include <stdbool.h>
#include <gio/gio.h>
#include <glib.h>
#include <glib-object.h>


#define IWD_STATION_INTERFACE 		"net.connman.iwd.Station"



#define IWD_STATION_TYPE			(iwd_station_get_type ())
#define IWD_STATION(obj)			(G_TYPE_CHECK_INSTANCE_CAST ((obj), IWD_STATION_TYPE, IwdStation))
#define IWD_STATION_IS(obj)			(G_TYPE_CHECK_INSTANCE_TYPE((obj), IWD_STATION_TYPE))


typedef struct IwdStation_ IwdStation;
typedef struct IwdStationPrivate_ IwdStationPrivate;
typedef struct IwdStationClass_ IwdStationClass;

struct IwdStation_{
	GObject parent_instance;		//父类
	IwdStationPrivate *priv;
};

struct IwdStationClass_ {
	GObjectClass parent_class;
};




GType iwd_station_get_type(void) G_GNUC_CONST;		//此函数由Glib根据G_DEFINE_TYPE_WITH_PRIVATE自动生成，此处声明是为了方便调用

IwdStation *iwd_station_create(GDBusConnection *conn, const char *devpath, GError **error);
int iwd_station_delete(IwdStation *self);

int iwd_station_scan(IwdStation *station, GError **error);
GPtrArray* iwd_station_get_ordered_networks(IwdStation *station, GError **error);
int iwd_station_connect(IwdStation *station, const gchar *network_path, GError **error);
int iwd_station_disconnect(IwdStation *station, GError **error);
char* iwd_station_get_state(IwdStation *station, GError **error);
char* iwd_station_get_connected_network(IwdStation *station, GError **error);
char* iwd_station_get_name(IwdStation *station, GError **error);
gboolean iwd_station_is_scanning(IwdStation *station, GError **error);

#ifdef __cplusplus
}
#endif

#endif
