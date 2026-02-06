#ifndef _TP_NETWORK_IWD_KNOWN_NETWORK_H_
#define _TP_NETWORK_IWD_KNOWN_NETWORK_H_

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>
#include <stdbool.h>
#include <gio/gio.h>
#include <glib.h>
#include <glib-object.h>


#ifndef HAVE_G_VARIANT_ITER_CLEAR
// 如果没有这个函数，提供一个实现
static inline void
my_g_variant_iter_clear(GVariantIter *iter)
{
    GVariant *value;
    while (g_variant_iter_next(iter, "v", &value)) {
        g_variant_unref(value);
    }
}
#define g_variant_iter_clear my_g_variant_iter_clear
#endif

// KnownNetwork 管理器接口
#define IWD_KNOWN_NETWORK_MANAGER_INTERFACE "net.connman.iwd.KnownNetworkManager"
#define IWD_KNOWN_NETWORK_INTERFACE "net.connman.iwd.KnownNetwork"











#ifdef __cplusplus
}
#endif

#endif