/*///------------------------------------------------------------------------------------------------------------------------//
		网卡管理(DBUS)Device接口相关操作
说 明 : org.freedesktop.NetworkManager.Device接口
日 期 : 2026.1.27
作 者 : Chingan

/*///------------------------------------------------------------------------------------------------------------------------//

#include "Network/NetworkManagerPriv.h"
#include "Network/NetworkManager.h"
#include "Network/NmDevice.h"

static GDBusConnection *system_conn = NULL;

struct NmDevicePrivate_{
	GDBusProxy *proxy;
};

//告诉glib自己的类型和私有结构体,前缀等
G_DEFINE_TYPE_WITH_PRIVATE(NmDevice, nm_device, G_TYPE_OBJECT);

static void nm_device_finalize(GObject *gobject);
static void nm_device_dispose(GObject *gobject);

static void nm_device_class_init(NmDeviceClass *klass)
{
    GObjectClass *gobject_class = G_OBJECT_CLASS(klass);
    gobject_class->dispose = nm_device_dispose;
    gobject_class->finalize = nm_device_finalize;
}

static void nm_device_init(NmDevice *self)
{
    self->priv = nm_device_get_instance_private(self);
    self->priv->proxy = NULL;
}


static void nm_device_dispose(GObject *gobject)
{
    NmDevice *self = NM_DEVICE(gobject);
    g_clear_object(&self->priv->proxy);
    G_OBJECT_CLASS(nm_device_parent_class)->dispose(gobject);
}

static void nm_device_finalize(GObject *gobject)
{
    // 如果有额外资源释放可以放这里
    G_OBJECT_CLASS(nm_device_parent_class)->finalize(gobject);
}

//私有成员创建
static GDBusProxy *nm_device_create_proxy(const gchar *path, GError **error)
{
	GDBusProxy *proxy=g_dbus_proxy_new_sync(
		system_conn,                            // 已获取的连接
		G_DBUS_PROXY_FLAGS_NONE,				// 默认标志
		NULL,									// 自动加载 introspection
		NETWORK_MANAGER_DBUS_SERVER,			// D-Bus 服务名 
		path,								// 对象路径
		NM_DEVICE_INTERFACE,         		// 接口名
		NULL, 								// GCancellable 
		error);								// 错误返回
	if (!proxy && *error)
	{
		g_printerr("Failed to create proxy for %s: %s\n", path, (*error)->message);
		return NULL;
	}
	return proxy;

}


NmDevice *nm_device_create(GDBusConnection *conn,
                               GError **error)
{
    g_return_val_if_fail(conn != NULL, NULL);

	system_conn=conn;
    NmDevice *self = g_object_new(NM_DEVICE_TYPE, NULL);

	self->priv->proxy = nm_device_create_proxy(NM_DEVICE_OBJECT_PATH, error);
    if (!self->priv->proxy) {
        g_object_unref(self);
        return NULL;
    }

    return self;
}

int nm_device_delete(NmDevice *self)
{
	if(!self)
		return 0;

	g_object_unref(self);
	return 0;
}


GDBusProxy* nm_device_get_proxy_internal(NmDevice *self)
{
	g_return_val_if_fail(NM_DEVICE_IS(self), NULL);
	return self->priv->proxy;
}
