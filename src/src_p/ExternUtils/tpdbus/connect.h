#ifndef _BLT_DBUS_H_
#define _BLT_DBUS_H_

#ifdef __cplusplus
extern "C" {
#endif

#include <gio/gio.h>
#include <dbus/dbus.h>
#include <stdint.h>
#include <pthread.h>

#define tpDbusConnection GDBusConnection 

extern GDBusConnection *session_conn;
extern GDBusConnection *system_conn;


typedef enum{
	DBUS_MAIN_THREAD_TYPE_LOOP,			//循环
	DBUS_MAIN_THREAD_TYPE_ITERATION		//触发
}DbusMainThreadType;

typedef struct DbusMainThread{
	GMainLoop *loop;
	GMainContext *context;
	gboolean running;
	pthread_mutex_t mutex;
	pthread_t thread;
}DbusMainThread;


void dbus_connect_init();
gboolean dbus_session_connect(GError **error);
void dbus_session_disconnect();
gboolean dbus_system_connect(GError **error);
void dbus_system_disconnect();
void dbus_disconnect();


DbusMainThread *dbus_main_thread_creat(DbusMainThreadType type);
DbusMainThread *dbus_main_thread_creat_once(DbusMainThreadType type);
void dbus_main_thread_delete(DbusMainThread *main);
void dbus_main_thread_delete_once(DbusMainThread *main);
void *dbus_main_thread_get_main_thread(DbusMainThread *main);

int bluet_dbus_set_match_rule(DBusConnection *conn,DBusError *err);

#ifdef __cplusplus
}
#endif


#endif
