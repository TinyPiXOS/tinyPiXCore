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
#include "Network/NetworkManager.h"
#include "Network/NmSettings.h"
#include "Network/NmConnection.h"
#include "Network/NmDevice.h"

#define NETWORK_CONFIG_PREFIX	"TpNetworkManager_"

NmConnection *network_open_nm_connection(GDBusConnection *dbus_conn, NmSettings *nms, const char *conn_name, const char *ifname);
void network_close_nm_connection(NmConnection *nmc);

int network_set_connection_static_ipv4(NmConnection *self, const char *ip, int prefix, const char *gateway, bool dns_flag);

int network_set_connection_ipv4_dhcp(NmConnection *self);

NmDevice *network_open_nm_device(GDBusConnection *dbus_conn, NetworkManager *nm, const char *name);
void network_close_nm_device(NmDevice *nmd);

#ifdef __cplusplus
}
#endif

#endif
