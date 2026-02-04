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
#include "Network/nm/NetworkManager.h"
#include "Network/nm/NmSettings.h"
#include "Network/nm/NmConnection.h"
#include "Network/nm/NmDevice.h"

#define NETWORK_CONFIG_PREFIX	"TpNetworkManager_"

typedef union NetworkMidContext NetworkMidContext;


NmConnection *network_open_nm_connection(GDBusConnection *dbus_conn, NmSettings *nms, const char *conn_name, const char *ifname);
void network_close_nm_connection(NmConnection *nmc);

int network_set_connection_static_ipv4(NetworkMidContext *self, const char *ip, int prefix, const char *gateway, bool dns_flag);

int network_set_connection_ipv4_dhcp(NetworkMidContext *self);

int network_set_ipv4_dns_list(NetworkMidContext *ctx, const char **dns, uint32_t dns_count);

int network_set_ipv6_dns_mode(NetworkMidContext *ctx, bool isauto);

int network_set_ipv4_dns_mode(NetworkMidContext *ctx, bool isauto);

int network_get_ipv6_dns_mode(NetworkMidContext *ctx);

int network_get_ipv4_dns_mode(NetworkMidContext *ctx);

int network_get_ipv4_dns_list(NetworkMidContext *ctx, char ***dns);

int network_get_ipv4_dhcp_state(NetworkMidContext *ctx);

NmDevice *network_open_nm_device(GDBusConnection *dbus_conn, NetworkManager *nm, const char *name);
void network_close_nm_device(NmDevice *nmd);

#ifdef __cplusplus
}
#endif

#endif
