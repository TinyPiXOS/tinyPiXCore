/*///------------------------------------------------------------------------------------------------------------------------//
		音频bluez-alsa相关基础接口
说 明 : 
日 期 : 2025.5.9
作 者 : Chingan

/*///------------------------------------------------------------------------------------------------------------------------//

#include <stdio.h>
#include "bluetooth_inc.h"
#include "bluez_alsa.h"



struct BluezAlsaPrivate_{
	GDBusProxy *proxy;

};

//G_DEFINE_TYPE_WITH_PRIVATE(TN, t_n, T_P)：GLib/GObject 提供的编译时宏(在编译时展开，运行时触发)，完成以下功能：
//		定义并实现类型注册函数 t_n_get_type()，其首次调用时会向 GObject 系统注册新类型（父类型为 T_P）。
//		生成类初始化（t_n##_class_init）和实例初始化（t_n##_init）函数的声明与连接逻辑。
//		为每个实例分配私有数据区（大小 sizeof(TN##Private)），并生成访问函数 t_n##_get_instance_private()。
//第一个参数：对应 C 结构体名称与类结构体名称（驼峰命名）
//第二个参数：是函数与宏名的前缀，如 obex_client,用于构造 obex_client_get_type()、obex_client_get_instance_private()、obex_client_class_init()、obex_client_init() 等函数名(下划线命名)
//第三个参数：指定新类型派生自哪个基类，例如 G_TYPE_OBJECT、GTK_TYPE_WIDGET 或其他自定义类型的 *_TYPE 宏
G_DEFINE_TYPE_WITH_PRIVATE(BluezAlsa, bluez_alsa, G_TYPE_OBJECT);	


static void bluez_alsa_class_init (BluezAlsaClass *klass)
{

}

//初始化，被自动调用
static void bluez_alsa_init (BluezAlsa *self)
{
	self->priv = bluez_alsa_get_instance_private (self);
	self->priv->proxy = NULL;
//	g_assert(system_conn != NULL);
	GError *error = NULL;

	bluez_alsa_create_gdbus_proxy(self, BLUEZ_ALSA_DBUS_SERVER, BLUEZ_ALSA_DBUS_PATH, &error);

	g_assert(error == NULL);
}



BluezAlsa *bluez_alsa_creat()
{
	BluezAlsa *alsa = g_object_new(BLUEZ_ALSA_TYPE,NULL);
	
	return alsa;
}

int bluez_alsa_delete(BluezAlsa *alsa )
{
	g_object_unref(alsa);
	return 0;
}


void bluez_alsa_create_gdbus_proxy(BluezAlsa *alsa, const gchar *dbus_server, const gchar *dbus_path,GError **error)
{
	alsa->priv->proxy=g_dbus_proxy_new_sync(
		system_conn,                            // 已获取的连接
		G_DBUS_PROXY_FLAGS_NONE,         // 默认标志
		NULL,                            // 自动加载 introspection
		dbus_server,                  // D-Bus 服务名 
		dbus_path,                 // 对象路径
		"org.bluealsa.Manager1",         // 接口名
		NULL,                            // GCancellable 
		error                           // 错误返回
	);
	if (!alsa->priv->proxy)
	{
		fprintf(stderr,"org.blueals creat error\n");
	}
}

void bluez_alsa_get_adapters(BluezAlsa *alsa,GError **error)
{
	GVariant *adapters = g_dbus_proxy_call_sync(
		alsa->priv->proxy,
		"GetAdapters",                /* 方法名 */
		NULL,                         /* 入参 */
		G_DBUS_CALL_FLAGS_NONE,
		-1,                           /* 默认超时 */
		NULL,                         /* GCancellable */
		error
	);
}

int bluez_alsa_get_version(BluezAlsa *alsa)
{
	GVariant *ver = g_dbus_proxy_get_cached_property(alsa->priv->proxy, "Version");
	if (ver) {
		g_print("BlueALSA 版本: %s\n", g_variant_get_string(ver, NULL));
		g_variant_unref(ver);
	}
}



