#ifndef _BLUET_DBUS_SIGNAL_H_
#define _BLUET_DBUS_SIGNAL_H_

#ifdef __cplusplus
extern "C" {
#endif

#include <gio/gio.h>
#include <dbus/dbus.h>
#include "bluetooth_inc.h"


typedef struct BluetDbusSignal_ BluetDbusSignal;
typedef struct BluetDbusSignalPrivate_ BluetDbusSignalPrivate;

struct BluetDbusSignal_ {
	/*< private >*/
	BluetDbusSignalPrivate *priv;
};


BluetDbusSignal *bluet_dbus_signal_subscribe(GDBusConnection *connection,const gchar *sender,const gchar *interface_name,const gchar *member,const gchar *object_path,GDBusSignalCallback callback,gpointer user_data,	GDestroyNotify user_data_free_func);
BluetDbusSignal *bluet_dbus_signal_subscribe_interfaces_added(GDBusConnection *connection,GDBusSignalCallback callback,gpointer user_data,GDestroyNotify user_data_free_func);
BluetDbusSignal *bluet_dbus_signal_subscribe_interfaces_removed(GDBusConnection *connection,GDBusSignalCallback callback,gpointer user_data,GDestroyNotify user_data_free_func);

BluetDbusSignal *bluet_dbus_signal_subscribe_properties_changed(GDBusConnection *connection,Adapter *adapter,GDBusSignalCallback callback,gpointer user_data,GDestroyNotify user_data_free_func);

int bluet_dbus_signal_delete(GDBusConnection *connection,BluetDbusSignal *sig);


#ifdef __cplusplus
}
#endif

#endif