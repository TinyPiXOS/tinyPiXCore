#ifndef _BLT_FILE_H_
#define _BLT_FILE_H_

#ifdef	__cplusplus
extern "C" {
#endif

#include <unistd.h>
#include <gio/gio.h>
#include <glib/gstdio.h>
#include <glib.h>
#include "../include/blt_dbussignal.h"

#define BLUET_OBEX_CONNECT		0x80	//
#define BLUET_OBEX_DISCONNECT	0x81

#define BLUET_OBEX_VERSION	0X10	//版本号，高四位为主版本号，低四位为次版本号



typedef struct BluetObexPush_ BluetObexPush;
typedef struct BluetObexPushPrivate_ BluetObexPushPrivate;

typedef struct BluetObexAgent_ BluetObexAgent;
typedef struct BluetObexAgentPrivate_ BluetObexAgentPrivate;

struct BluetObexPush_{
	BluetObexPushPrivate *priv;
};

struct BluetObexAgent_{
	BluetObexAgentPrivate *priv;
};


int bluet_get_obex_port(const char *bt_addr);

//请求操作码：

int send_file_obex(const char *addr);
int send_file_dbus(const char *bt_addr);


int recv_file_obex();
int recv_file_dbus();


int send_file_dbus_(const char *dst_address,const char *opp_file_arg);
//结束接收文件
void bluet_exit_g_loop(GMainLoop *loop);



BluetObexAgent *bluet_obex_agent_creat(const char *path);
int bluet_obex_agent_delete(BluetObexAgent *agent);
BluetDbusSignal *bluet_obex_agent_signal_subscribe_properties(BluetObexAgent *agent,void *userdata);
BluetDbusSignal *bluet_obex_agent_signal_subscribe_objext_manager(BluetObexAgent *agent,void *userdata);

BluetObexPush *bluet_obex_push_creat();
int bluet_obex_push_delete(BluetObexPush *push);
BluetDbusSignal *bluet_obex_push_signal_subscribe_properties(BluetObexPush *push,void *userdata);
BluetDbusSignal *bluet_obex_push_signal_subscribe_objext_manager(BluetObexPush *push,void *userdata);

int bluet_obex_signal_subscribe_delete(BluetDbusSignal *sig);

int bluet_obex_push_session_creat(BluetObexPush *push, const char *dst_address);
int bluet_obex_send_file(BluetObexPush *push, const char *file);
int bluet_obex_stop_send_file(BluetObexPush *push);


int bluet_obex_hash_creat();
int bluet_obex_hash_free();


#ifdef	__cplusplus
}
#endif

#endif