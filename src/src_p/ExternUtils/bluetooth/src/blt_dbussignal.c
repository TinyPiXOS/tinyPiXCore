
/*///------------------------------------------------------------------------------------------------------------------------//
		蓝牙D-BUS相关的信号注册注销接口
说 明 : 
日 期 : 2025.3.25

/*///------------------------------------------------------------------------------------------------------------------------//

#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <gio/gio.h>
#include <dbus/dbus.h>
#include "blt_dbussignal.h"
#include "bluetooth_inc.h"
#include "blt_hard.h"

struct BluetDbusSignalPrivate_{
	guint sig_sub_id;
};





static BluetDbusSignal *bluet_dbus_signal_creat()
{
	BluetDbusSignal *sig=(BluetDbusSignal *)malloc(sizeof(BluetDbusSignal));
	if(!sig)
		return NULL;

	BluetDbusSignalPrivate *priv=(BluetDbusSignalPrivate *)malloc(sizeof(BluetDbusSignalPrivate ));
	if(!priv)
	{	
		free(sig);
		return NULL;
	}
	sig->priv=priv;
	return sig;
}

int bluet_dbus_signal_delete(GDBusConnection *connection,BluetDbusSignal *sig)
{
	if(!sig)
		return 0;
	g_dbus_connection_signal_unsubscribe(connection, sig->priv->sig_sub_id);

	free(sig->priv);
	free(sig);
}

//调用系统的g_dbus_connection_signal_subscribe
BluetDbusSignal *bluet_dbus_signal_subscribe(
						GDBusConnection *connection,	//通常为session_conn或system_conn
						const gchar *sender,			//通常为"org.bluez"或"org.bluez.obex"或
						const gchar *interface_name,	//通常为"org.freedesktop.DBus.ObjectManager"或"org.freedesktop.DBus.Properties",
						const gchar *member,
						const gchar *object_path,
						GDBusSignalCallback callback,	//回调函数
						gpointer user_data,				//回调函数用户参数
						GDestroyNotify user_data_free_func)	//回调函数的释放
{
	BluetDbusSignal *sig_sub=bluet_dbus_signal_creat();
	if(!sig_sub)
		return NULL;

	guint object_sig_sub_id = g_dbus_connection_signal_subscribe(
									connection, 
									sender, 
									interface_name,
									member, 
									object_path, 
									NULL, 
									G_DBUS_SIGNAL_FLAGS_NONE, 
									callback,
									user_data,
									user_data_free_func);
	sig_sub->priv->sig_sub_id=object_sig_sub_id;
	return sig_sub;
}



BluetDbusSignal *bluet_dbus_signal_subscribe_interfaces_added(GDBusConnection *connection,    
												GDBusSignalCallback callback,
												gpointer user_data,
												GDestroyNotify user_data_free_func)
{
	BluetDbusSignal *sig_sub=bluet_dbus_signal_creat();
	if(!sig_sub)
		return NULL;

	guint object_sig_sub_id = g_dbus_connection_signal_subscribe(
									connection, 
									BLUEZ_DBUS_SERVICE_NAME, 
									"org.freedesktop.DBus.ObjectManager", 
									"InterfacesAdded", 
									NULL, NULL, 
									G_DBUS_SIGNAL_FLAGS_NONE, 
									callback,
									user_data,
									user_data_free_func);
	sig_sub->priv->sig_sub_id=object_sig_sub_id;
	return sig_sub;
}

BluetDbusSignal *bluet_dbus_signal_subscribe_interfaces_removed(GDBusConnection *connection,    
												GDBusSignalCallback callback,
												gpointer user_data,
												GDestroyNotify user_data_free_func)
{
	BluetDbusSignal *sig_sub=bluet_dbus_signal_creat();
	if(!sig_sub)
		return NULL;

	guint object_sig_sub_id = g_dbus_connection_signal_subscribe(
									connection, 
									BLUEZ_DBUS_SERVICE_NAME, 
									"org.freedesktop.DBus.ObjectManager", 
									"InterfacesRemoved", 
									NULL, NULL, 
									G_DBUS_SIGNAL_FLAGS_NONE, 
									callback,
									user_data,
									user_data_free_func);
	sig_sub->priv->sig_sub_id=object_sig_sub_id;
	return sig_sub;
}

BluetDbusSignal *bluet_dbus_signal_subscribe_properties_changed(GDBusConnection *connection, Adapter *adapter,
												GDBusSignalCallback callback,
												gpointer user_data,
												GDestroyNotify user_data_free_func)
{
	BluetDbusSignal *sig_sub=bluet_dbus_signal_creat();
	if(!sig_sub)
		return NULL;

	guint prop_sig_sub_id = g_dbus_connection_signal_subscribe(
									connection, 
									BLUEZ_DBUS_SERVICE_NAME, 
									"org.freedesktop.DBus.Properties", 
									"PropertiesChanged", 
									adapter_get_dbus_object_path(adapter), 
									NULL, 
									G_DBUS_SIGNAL_FLAGS_NONE, 
									callback,user_data,user_data_free_func);
//									_adapter_property_changed, 
//									mainloop, 
//									NULL);
	sig_sub->priv->sig_sub_id=prop_sig_sub_id;
	return sig_sub;
}

