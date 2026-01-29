#ifndef _NETWORK_APP_CONF_H_
#define _NETWORK_APP_CONF_H_

#ifdef __cplusplus
extern "C"
{
#endif

#include <stdio.h>
#include <stdint.h>
#include <gio/gio.h>
#include <glib.h>
#include <glib-object.h>
#include "Network/NmSettings.h"
#include "Network/NmConnection.h"


NmConnection *network_open_connection(GDBusConnection *dbus_conn, NmSettings *nms, const char *conn_name ,GError **error);



#ifdef __cplusplus
}
#endif

#endif
