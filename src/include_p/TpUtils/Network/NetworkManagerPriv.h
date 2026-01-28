//此文件为NetworkManager内部使用，外部禁止调用
#ifndef _NETWORK_MANAGER_PRIV_H
#define _NETWORK_MANAGER_PRIV_H

#include "NmConnection.h"
#include "NmDevice.h"
#include <gio/gio.h>

// 以下接口只允许 NetworkManager 内部使用
GDBusProxy* nm_connection_get_proxy_internal(NmConnection *self);
GDBusProxy* nm_device_get_proxy_internal(NmDevice *self);

#endif