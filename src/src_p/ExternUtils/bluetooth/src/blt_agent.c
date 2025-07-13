/*///------------------------------------------------------------------------------------------------------------------------//
		蓝牙的配对流程相关服务相关功能
说 明 : 蓝牙接受配对连接等
		此接口只需要调用一次即可
		Agent必须注册到总线才行
日 期 : 2025.4.10

/*///------------------------------------------------------------------------------------------------------------------------//

#include <stdio.h>
#include "obex_agent.h"
#include "obex_agent_manager.h"
#include "../include/blt_device.h"
#include "lib/agent-helper.h"
#include "bluetooth_inc.h"
#include "device.h"
#include "adapter.h"
#include "g_utils.h"
#include "blt_device.h"
#include "blt_agent.h"
#include "bluetooth_inc.h"

struct BluetAgentPrivate_{
	AgentManager *agent_manager;
	DbusMainThread *mainthread;
//	GMainLoop *mainloop;
};



BluetAgent *bluet_agent_creat()
{
	BluetAgent *self=(BluetAgent *)malloc(sizeof(BluetAgent));
	if(!self)
		return NULL;

	BluetAgentPrivate *priv=(BluetAgentPrivate *)malloc(sizeof(BluetAgentPrivate));
	if(!priv)
	{	
		free(self);
		return NULL;
	}
	
	AgentManager *agent_manager = agent_manager_new();
	if(!agent_manager)
	{
		free(priv);
		free(self);
		return NULL;
	}

	DbusMainThread *mainthread = dbus_main_thread_creat(DBUS_MAIN_THREAD_TYPE_LOOP);
	if(!mainthread)
	{
		g_object_unref(self->priv->agent_manager);
		free(priv);
		free(self);
	}

	self->priv=priv;
	self->priv->mainthread=mainthread;
//	self->priv->mainloop=main->loop=g_main_loop_new (NULL,FALSE);
	self->priv->agent_manager=agent_manager;

	GError *error = NULL;
	register_agent_callbacks(TRUE, NULL, dbus_main_thread_get_main_thread(self->priv->mainthread), &error);		//注册一个agent对象
	agent_manager_register_agent(agent_manager, AGENT_PATH, "DisplayYesNo", &error);
	agent_manager_request_default_agent(agent_manager, AGENT_PATH, &error);
	if(error)
	{
		g_clear_error(&error);
		bluet_agent_delete(self);
		return NULL;
	}

	return self;
}

int bluet_agent_delete(BluetAgent *self)
{
	GError *error = NULL;
	if(!self)
		return 0;
	if(self->priv->agent_manager)
	{
		agent_manager_unregister_agent(self->priv->agent_manager, AGENT_PATH, &error);
		g_object_unref(self->priv->agent_manager);
	}

	dbus_main_thread_delete(self->priv->mainthread);
	/*if(self->priv->mainloop)
	{
		g_main_loop_unref(self->priv->mainloop);
		self->priv->mainloop=NULL;
	}*/

	unregister_agent_callbacks(&error);
	
	free(self->priv);
	free(self);
	return 0;
}

//	register_agent_callbacks(TRUE, NULL, self->priv->mainloop, &error);		//注册一个agent对象

//	agent_manager_register_agent(agent_manager, AGENT_PATH, "DisplayYesNo", &error);	//把上一步注册的agent注册到总线
//等待配对
int bluet_agent()
{
	
	GError *error = NULL;
	GMainLoop *mainloop = NULL;
	AgentManager *agent_manager = agent_manager_new();
	if(!agent_manager)
		return -1;
	
	agent_manager_register_agent(agent_manager, AGENT_PATH, "DisplayYesNo", &error);
	agent_manager_request_default_agent(agent_manager, AGENT_PATH, &error);

	g_main_loop_run(mainloop);

	//退出
	agent_manager_unregister_agent(agent_manager, AGENT_PATH, &error);

	g_main_loop_unref(mainloop);

	unregister_agent_callbacks(NULL);
	g_object_unref(agent_manager);
}


