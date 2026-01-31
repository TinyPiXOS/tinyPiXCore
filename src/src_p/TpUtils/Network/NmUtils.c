
#include "Network/NmUtils.h"

//自动引用计数加一，不用手动释放，主要用于g_dbus_proxy_call_sync的第三个参数创建 
GVariant *network_variant_tuple1(GVariant *v)
{
    return g_variant_new_tuple((GVariant *[]){ g_variant_ref(v) }, 1);
}