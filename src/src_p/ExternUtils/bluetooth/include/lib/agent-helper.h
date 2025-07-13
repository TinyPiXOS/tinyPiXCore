#ifndef __AGENT_HELPER_H
#define __AGENT_HELPER_H

#ifdef	__cplusplus
extern "C" {
#endif

#include <stdio.h>

#include "bluetooth_inc.h"

#define AGENT_DBUS_INTERFACE "org.bluez.Agent1"
#define AGENT_PATH "/org/blueztools"

extern gboolean agent_need_unregister;


typedef void (*AgentMethodUserCallback)(const gchar *method_name,
                                    GVariant *parameters,
                                    GDBusMethodInvocation *invocation);
//用户的回调函数
struct AgentMethodUserCallFun{
	AgentMethodUserCallback request_confirmation;

};



void register_agent_callbacks(gboolean interactive_console, GHashTable *pin_dictonary, gpointer main_loop_object, GError **error);
void unregister_agent_callbacks(GError **error);

#ifdef	__cplusplus
}
#endif

#endif /* __AGENT_HELPER_H */
