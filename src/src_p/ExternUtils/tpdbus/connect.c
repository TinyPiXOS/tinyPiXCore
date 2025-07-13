/*///------------------------------------------------------------------------------------------------------------------------//
		蓝牙D-BUS相关的基础接口
说 明 : DBUS的连接，线程main接口
日 期 : 2025.3.20

/*///------------------------------------------------------------------------------------------------------------------------//

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include <stdio.h>
#include <unistd.h>
#include <gio/gio.h>
#include <dbus/dbus.h>
#include "connect.h"

GDBusConnection *session_conn = NULL;
GDBusConnection *system_conn = NULL;

static gboolean dbus_initialized = FALSE;

void dbus_connect_init()
{
    dbus_initialized = TRUE;
}

// 手动启动 dbus-launch 并设置环境变量
static gboolean ensure_session_bus_env() {
    const char *addr = g_getenv("DBUS_SESSION_BUS_ADDRESS");
    if (addr && strlen(addr) > 0) {
        return TRUE;  // 已有 bus 地址，无需再启动
    }

    FILE *fp = popen("dbus-launch", "r");
    if (!fp) {
        fprintf(stderr, "无法运行 dbus-launch\n");
        return FALSE;
    }

    char line[512];
    while (fgets(line, sizeof(line), fp)) {
        if (strncmp(line, "DBUS_SESSION_BUS_ADDRESS=", 25) == 0) {
            char *val = strchr(line, '=');
            if (val) {
                val[strcspn(val, "\n")] = '\0';
                setenv("DBUS_SESSION_BUS_ADDRESS", val + 1, 1);
            }
        } else if (strncmp(line, "DBUS_SESSION_BUS_PID=", 22) == 0) {
            char *val = strchr(line, '=');
            if (val) {
                val[strcspn(val, "\n")] = '\0';
                setenv("DBUS_SESSION_BUS_PID", val + 1, 1);
            }
        }
    }

    pclose(fp);

    // 检查是否真的设置成功了
    return (g_getenv("DBUS_SESSION_BUS_ADDRESS") != NULL);
}

gboolean dbus_session_connect(GError **error_)
{
	/*if (!ensure_session_bus_env()) {
        fprintf(stderr, "无法确保 DBUS_SESSION_BUS_ADDRESS 存在，连接失败\n");
        return NULL;
    }*/

    GError *error = NULL;

	session_conn=g_bus_get_sync(G_BUS_TYPE_SESSION, NULL, &error);
    if (!session_conn) {
        fprintf(stderr, "连接 Session Bus 失败：%s\n", error ? error->message : "(未知错误)");
        g_clear_error(&error);
		return FALSE;
    }

    return TRUE;
}

void dbus_session_disconnect()
{
    g_object_unref(session_conn);
}

gboolean dbus_system_connect(GError **error)
{
	g_assert(dbus_initialized == TRUE);
	system_conn = g_bus_get_sync(G_BUS_TYPE_SYSTEM, NULL, error);
	if (!system_conn)
	{
		return FALSE;
	}
	printf("system addr:%p\n",system_conn);
	return TRUE;
}

void dbus_system_disconnect()
{
	g_object_unref(system_conn);
}

void dbus_disconnect()
{
	if (system_conn)
		dbus_system_disconnect();
	if (session_conn)
		dbus_session_disconnect();
}

static DbusMainThread *main_thread_once = NULL;			//单例模式的main——loop
static guint        ref_count = 0;				//引用计数
static  pthread_mutex_t ref_mutex = PTHREAD_MUTEX_INITIALIZER;			//互斥锁

static gboolean dbus_main_thread_is_runing(DbusMainThread *main)
{
	pthread_mutex_lock(&main->mutex);
	gboolean is_running=main->running;
	pthread_mutex_unlock(&main->mutex);
	return is_running;
}

static void dbus_main_thread_set_runing(DbusMainThread *main,gboolean is_runing)
{
	pthread_mutex_lock(&main->mutex);
	main->running=is_runing;
	pthread_mutex_unlock(&main->mutex);
}

static void *pthread_main_thread_loop(void *arg)
{
	DbusMainThread *main=(DbusMainThread *)arg;
	if(main->loop)
	{
		g_main_loop_run (main->loop);
	}
	else if(main->context){
		while(dbus_main_thread_is_runing(main))
		{
			g_main_context_iteration(main->context, TRUE);
		}
	}
}

//创建mainloop
DbusMainThread *dbus_main_thread_creat(DbusMainThreadType type)
{
	DbusMainThread *main=(DbusMainThread *)malloc(sizeof(DbusMainThread));
	main->context=NULL;
	main->loop=NULL;
	main->running=FALSE;
	pthread_mutex_init(&main->mutex, NULL);

	if(type==DBUS_MAIN_THREAD_TYPE_LOOP)
	{
		main->loop=g_main_loop_new (NULL,FALSE);
	}
	else
	{
		main->context= g_main_context_default();
	}

	if((pthread_create(&main->thread, NULL, pthread_main_thread_loop, (void *)main)) != 0) {    
		dbus_main_thread_delete(main);
		return NULL;
	}
	dbus_main_thread_set_runing(main,TRUE);
	main_thread_once=main;
	return main;
}

//创建只允许调用一次的mainloop,多个进程调用依旧会多次启动
DbusMainThread *dbus_main_thread_creat_once(DbusMainThreadType type)
{
	DbusMainThread *main=NULL;
	pthread_mutex_lock(&ref_mutex);

	if (ref_count == 0) {
		// 第一次获取，分配单例
		main=dbus_main_thread_creat(type);
	}

	ref_count++;
	printf("dbus_main_thread_delete_once:第%d次创建\n",ref_count);
	pthread_mutex_unlock(&ref_mutex);

	return main;
}

void dbus_main_thread_delete(DbusMainThread *main)
{
	if(!main)
		return ;
	dbus_main_thread_set_runing(main,FALSE);
	if(main->loop)
		g_main_loop_quit(main->loop);
	pthread_join(main->thread, NULL);
	if(main->loop)
		g_main_loop_unref(main->loop);
	pthread_mutex_destroy(&main->mutex); 
	free(main);
	main=NULL;
}

void dbus_main_thread_delete_once(DbusMainThread *main)
{
	pthread_mutex_lock(&ref_mutex);

	if (ref_count == 0) {
		// 防止过度释放
		pthread_mutex_unlock(&ref_mutex);
		return;
	}

	printf("dbus_main_thread_delete_once:第%d次释放\n",ref_count);
	ref_count--;
	
	if (ref_count == 0) {
		// 最后一个释放时，销毁单例
		dbus_main_thread_delete(main);
		main=NULL;
	}

	pthread_mutex_unlock(&ref_mutex);
}

void *dbus_main_thread_get_main_thread(DbusMainThread *main)
{
	if(main->loop)
		return (void *)main->loop;
	else	
		return (void *)main->context;
}




//设置匹配规则
int bluet_dbus_set_match_rule(DBusConnection *conn,DBusError *err)
{
	const char *match_rule = "type='signal',interface='org.freedesktop.DBus.ObjectManager',member='InterfacesAdded',sender='org.bluez'";
	dbus_bus_add_match(conn, match_rule, err);
	if (dbus_error_is_set(err)) {
		fprintf(stderr, "添加匹配规则失败: %s\n", err->message);
		dbus_error_free(err);
		return 1;
	}
	dbus_connection_flush(conn);		//使得匹配规则立即生效
}



