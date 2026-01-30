#ifndef _NETWORK_APP_CONF_H_
#define _NETWORK_APP_CONF_H_

#ifdef __cplusplus
extern "C"
{
#endif

#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>
#include <gio/gio.h>
#include <glib.h>
#include <glib-object.h>
#include "Network/NmSettings.h"
#include "Network/NmConnection.h"

#define NETWORK_CONFIG_PREFIX	"TpNetworkManager_"

NmConnection *network_open_connection(GDBusConnection *dbus_conn, NmSettings *nms, const char *conn_name, const char *ifname, GError **error);

int network_set_connection_static_ipv4(NmConnection *self, const char *ip, int prefix, const char *gateway, bool dns_flag);

int network_set_connection_ipv4_dhcp(NmConnection *self);

#ifdef __cplusplus
}
#endif

#endif
