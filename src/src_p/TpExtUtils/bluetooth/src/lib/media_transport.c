/*///------------------------------------------------------------------------------------------------------------------------//
		媒体传输相关接口
说 明 : 
日 期 : 2025.5.14

/*///------------------------------------------------------------------------------------------------------------------------//

#include <stdio.h>
#include "bluetooth_inc.h"
#include "g_utils.h"
#include "media_transport.h"

struct MediaTransportPrivate_{
	GDBusProxy *proxy;
	char *device;		//hci0,hci1...
	char *address;
};


G_DEFINE_TYPE_WITH_PRIVATE(MediaTransport, media_transport, G_TYPE_OBJECT);	

enum{
	PROP_0,		//固定格式占位

	PROP_DEVICE,
	PROP_ADDRESS,

	N_PROPERTIES	//固定格式占位
};
static GParamSpec *obj_props[N_PROPERTIES];

static void media_transport_class_init (MediaTransportClass *klass)
{
	obj_props[PROP_DEVICE] = 
			g_param_spec_string ("device", "Device",
								 "Local Bluetooth adapter name", 
								 NULL /* default value */,
								 G_PARAM_CONSTRUCT_ONLY | G_PARAM_READWRITE);

	obj_props[PROP_DEVICE] = 
			g_param_spec_string ("address", "Remote",
								 "Remote media device address", 
								 NULL /* default value */,
								 G_PARAM_CONSTRUCT_ONLY | G_PARAM_READWRITE);

	g_object_class_install_properties (
		G_OBJECT_CLASS (klass),
		N_PROPERTIES,
		obj_props);
}

//初始化，被自动调用
static void media_transport_init (MediaTransport *self)
{
	GError *error = NULL;
	self->priv = media_transport_get_instance_private (self);
	self->priv->proxy = NULL;

	self->priv->device  = g_strdup (g_object_get_data (G_OBJECT(self), "device"));
	self->priv->address = g_strdup (g_object_get_data (G_OBJECT(self), "address"));

//	g_assert(system_conn != NULL);
	media_transport_create_gdbus_proxy(self, MEDIA_TRANSPORT_DBUS_SERVER, &error);

	g_assert(error == NULL);
}


//device:本地使用的蓝牙适配器
//remote:要建立媒体传输的远程蓝牙设备地址
MediaTransport *media_transport_creat(const char *device ,const char *remote)
{
	MediaTransport *self = g_object_new(MEDIA_TRANSPORT_TYPE,"device",device,"address",remote,NULL);

	return self;
}

int media_transport_delete(MediaTransport *self )
{
	g_object_unref(self);
	return 0;
}


void media_transport_create_gdbus_proxy(MediaTransport *self, const gchar *dbus_server, GError **error)
{
	char *addr=convert_bt_addr_format(self->priv->address);
	char *path=(char *)malloc(64);
	snprintf(path,64,"/org/bluez/%s/dev_%s/player0",self->priv->device,addr);

	self->priv->proxy=g_dbus_proxy_new_sync(
		system_conn,                            // 已获取的连接
		G_DBUS_PROXY_FLAGS_NONE,         // 默认标志
		NULL,                            // 自动加载 introspection
		dbus_server,                  // D-Bus 服务名 
		path,                 // 对象路径
		"org.bluez.MediaTransport1",         // 接口名
		NULL,                            // GCancellable 
		error                           // 错误返回
	);
	if (!self->priv->proxy)
	{
		fprintf(stderr,"org.bluez.MediaTransport1 creat error\n");
	}
	free(addr);
	free(path);
}


void media_transport_set_profile(MediaTransport *self,const gchar *profile,GError **error)
{
	char *path=(char *)malloc(64);

	snprintf(path,64,"/org/bluez/%s",self->priv->device);
	GVariant *adapters = g_dbus_proxy_call_sync(
		self->priv->proxy,
		"SetProfile", 
		g_variant_new("(os)", path, "a2dp_sink"),  // 或 "hsp_ag"/"hfp_ag"   
		G_DBUS_CALL_FLAGS_NONE,
		-1,                           // 默认超时 
		NULL,
		error
	);
	free(path);
}



void media_transport_select_codec(MediaTransport *self,const gchar *codec,GError **error)
{
	GVariant *adapters = g_dbus_proxy_call_sync(
		self->priv->proxy,
		"SelectCodec", 
		g_variant_new("(s)", "aptx"), // 支持 sbc/aac/aptx/ldac   
		G_DBUS_CALL_FLAGS_NONE,
		-1,                           // 默认超时 
		NULL,
		error
	);
}

