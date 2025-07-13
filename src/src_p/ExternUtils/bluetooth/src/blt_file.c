/*///------------------------------------------------------------------------------------------------------------------------//
		蓝牙文件传输
说 明 : 主要利用蓝牙的OBEX及OPP协议，可以使用openobex工具辅助测试
日 期 : 2025.3.13

/*///------------------------------------------------------------------------------------------------------------------------//

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <libgen.h>
#include <signal.h>
#include <bluetooth/bluetooth.h>
#include <bluetooth/rfcomm.h>
//#include <openobex/obex.h>
//#include <openobex/obex_const.h>
#include <dbus/dbus.h>
#include "blt_file.h"
#include "blt_sdp.h"
#include "sdpregister.h"
#include "bluetooth_inc.h"


static int obex_stop_send_file(GMainLoop *mainloop);

//回调的数据
struct CallbackDataObjectMange{
	//可以增加用户回调函数
	void *userdata;//用户回调的数据，
	GMainLoop *mainloop;
};

struct BluetObexPushPrivate_{
	ObexClient *client;
	ObexSession *session;
	ObexObjectPush *oop;
	GVariant *device_dict;
	gchar *session_path;
	struct CallbackDataObjectMange *data_m;

	GMainLoop *mainloop;
};

struct BluetObexAgentPrivate_{
	ObexAgent *agent;
	ObexAgentManager *manager;

	DbusMainThread *mainthread;
};


int bluet_get_obex_port(const char *bt_addr)
{
	struct SdpAttrValue *attr_data=(struct SdpAttrValue *)malloc(sizeof(struct SdpAttrValue));
	if(!attr_data)
		return -1;
	attr_data[0].attr=SDP_ATTR_PROTO_DESC_LIST;
	if(bluet_quere_profile_attr(bt_addr,OBEX_FILETRANS_SVCLASS_ID,attr_data,sizeof(attr_data)/sizeof(uint32_t))<0)
		return -1;
	return attr_data[0].val.uint8;
}

//高版本的函数
#define obex_return_val_if_fail(test, val) \
        do { if (!(test)) return val; } while(0)

int OBEX_CharToUnicode(uint8_t *uc, const uint8_t *c, int size)
{
	int len, n;

//	DEBUG(4, "\n");

	obex_return_val_if_fail(uc != NULL, -1);
	obex_return_val_if_fail(c != NULL, -1);

	len = n = strlen((char *) c);
	obex_return_val_if_fail(n*2+2 <= size, -1);

	uc[n*2+1] = 0;
	uc[n*2] = 0;

	while (n--) {
		uc[n*2+1] = c[n];
		uc[n*2] = 0;
	}

	return (len * 2) + 2;
}

int OBEX_UnicodeToChar(uint8_t *c, const uint8_t *uc, int size)
{
	int n;

//	DEBUG(4, "\n");

	obex_return_val_if_fail(uc != NULL, -1);
	obex_return_val_if_fail(c != NULL, -1);

	/* Make sure buffer is big enough! */
	for (n = 0; uc[n*2+1] != 0; n++);

	obex_return_val_if_fail(n < size, -1);

	for (n = 0; uc[n*2+1] != 0; n++)
		c[n] = uc[n*2+1];
	c[n] = 0;

	return 0;
}


//send:obex
//使用原始的openobex库进行发送
//data
/*
typedef struct {
	int mode;
	int event;
	int cmd;
	int rsp;
} userdata;

//callback
void callback(obex_t *handle, obex_object_t *obj,
		int mode, int event, int obex_cmd, int obex_rsp) {
	userdata *ud;

	printf("callback: %s | %s | %s | %s\n",
		obex_name(mode, obex_modenames),
		obex_name(event, obex_evnames),
		obex_name(obex_cmd, obex_cmdnames),
		(obex_rsp==0)?"":obex_name(obex_rsp, obex_rspnames));

	ud=(userdata *) OBEX_GetUserData(handle);
	ud->mode=mode;
	ud->event=event;
	ud->cmd=obex_cmd;
	ud->rsp=obex_rsp;
}

//request
int request_handle(obex_t *handle, obex_object_t *object) {
	int res;
	userdata *ud;

	ud=(userdata *) OBEX_GetUserData(handle);

	if(0>(res=OBEX_Request(handle, object))) {
		printf("request: %d\n", -res);
		return res;
	}

	do {
		printf("handle input: %d\n", OBEX_HandleInput(handle, 1));
	} while(ud->event==OBEX_EV_PROGRESS);
}

int put(obex_t *handle, char *name, char *data, int len) {
	obex_object_t *object;
	obex_headerdata_t header;
	char buf[1000];
	int namelen;

	object=OBEX_ObjectNew(handle, OBEX_CMD_PUT);

	namelen=OBEX_CharToUnicode(buf, name, 1000);
	header.bs=buf;
	OBEX_ObjectAddHeader(handle, object, OBEX_HDR_NAME, header, namelen, 0);

	header.bq4=len;
	OBEX_ObjectAddHeader(handle, object, OBEX_HDR_LENGTH, header, 4, 0);

	header.bs=data;
	OBEX_ObjectAddHeader(handle, object, OBEX_HDR_BODY, header, len, 0);

	request_handle(handle, object);
}


int send_file_obex(const char *addr) {

	obex_t *obex_handle;
	obex_object_t *object;
	bdaddr_t ba;
	int res;
	int channel=12;

	int fd;
	int size;
	char *data;
	const char *filename="/home/jiyuchao/桌面/phone.wav";
	if(0>(fd=open(filename, O_RDONLY))) {
		printf("cannot open file \"%s\"\n", filename);
		exit(1);
	}
	size=lseek(fd, 0, SEEK_END);
	lseek(fd, 0, SEEK_SET);
	data=malloc(size);
	if(!data) {
		printf("file \"%s\" too large\n", filename);;
		exit(1);
	}
	if(size!=read(fd, data, size)) {
		printf("cannot read whole file \"%s\"\n", filename);
		exit(1);
	}
	close(fd);
	str2ba(addr, &ba);

	obex_handle=OBEX_Init(OBEX_TRANS_BLUETOOTH, callback, 0);
	if(obex_handle==NULL) {
		printf("cannot OBEX_Init\n");
		exit(1);
	}
	OBEX_SetUserData(obex_handle, malloc(sizeof(userdata)));

	res=BtOBEX_TransportConnect(obex_handle, BDADDR_ANY, &ba, channel);//BDADDR_ANY
	if(0>res) {
		printf("cannot connect\n");
		exit(1);
	}
	printf("connected\n");

	object=OBEX_ObjectNew(obex_handle, OBEX_CMD_CONNECT);
	request_handle(obex_handle, object);

	put(obex_handle, basename(filename), data, size);

	object=OBEX_ObjectNew(obex_handle, OBEX_CMD_DISCONNECT);
	request_handle(obex_handle, object);

	OBEX_TransportDisconnect(obex_handle);
	OBEX_Cleanup(obex_handle);
	printf("disconnected\n");

    return 0;
}
*/



//recv: D-BUS
#include "obex_transfer.h"
#include "obex_session.h"
#include "bluetooth_inc.h"
#include "obex_agent.h"
#include "obex_agent_manager.h"
#include "blt_hard.h"
#include "bluetooth_inc.h"

#define OBEX_AGENT_OBJECT_PATH "/com/example/obex_agent"
#define OBEX_MANAGER_INTERFACE "org.bluez.obex.AgentManager1"
#define OBEX_TRANSFER_INTERFACE "org.bluez.obex.Transfer1"
#define OBEX_SESSION_DBUS_INTERFACE "org.bluez.obex.Session1"
#define MANAGER_DBUS_PATH       "/"
#define MANAGER_DBUS_INTERFACE  "org.freedesktop.DBus.ObjectManager"

static GHashTable *_transfers = NULL;
static GHashTable *_transfer_infos = NULL;
static gboolean _update_progress = FALSE;

static gboolean auto_accept = TRUE;
static gchar *_root_path = "/home/pix/bluetooth";

typedef struct _ObexTransferInfo ObexTransferInfo;

struct _ObexTransferInfo {
    gchar *filename;
    guint64 filesize;
    gchar *obex_root;
    gchar *status;
};


static void stop_obexd() {
    GError *error = NULL;
    gchar *stdout_str = NULL;
    gchar *stderr_str = NULL;
    /* 此命令尝试杀掉 obexd 进程，请根据实际情况调整命令 */
    gboolean ok = g_spawn_command_line_sync("pkill obexd", &stdout_str, &stderr_str, NULL, &error);
    if (!ok) {
        g_printerr("Failed to stop obexd: %s\n", error->message);
        g_error_free(error);
    } else {
        g_print("obexd stopped successfully.\n");
    }
    g_free(stdout_str);
    g_free(stderr_str);
}

void bluet_exit_g_loop(GMainLoop *loop)
{
    if (g_main_loop_is_running(loop))
        g_main_loop_quit(loop);
}


//客户端回调
static void _obex_opp_client_object_manager_handler(GDBusConnection *connection, const gchar *sender_name, const gchar *object_path, const gchar *interface_name, const gchar *signal_name, GVariant *parameters, gpointer user_data)
{
    if(g_strcmp0(signal_name, "InterfacesAdded") == 0)
    {
        const gchar *interface_object_path = g_variant_get_string(g_variant_get_child_value(parameters, 0), NULL);
        GVariant *interfaces_and_properties = g_variant_get_child_value(parameters, 1);
        GVariant *properties = NULL;
        
        if(g_variant_lookup(interfaces_and_properties, OBEX_TRANSFER_DBUS_INTERFACE, "@a{sv}", &properties))
        {
            // g_print("[OBEX Client] Transfer started\n");
            ObexTransfer *t = obex_transfer_new(interface_object_path);
            g_hash_table_insert(_transfers, g_strdup(interface_object_path), t);

            ObexTransferInfo *info = g_malloc0(sizeof(ObexTransferInfo));
            info->filesize = g_variant_get_uint64(g_variant_lookup_value(properties, "Size", NULL));
            info->filename = g_strdup(g_variant_get_string(g_variant_lookup_value(properties, "Name", NULL), NULL));
            info->status = g_strdup(g_variant_get_string(g_variant_lookup_value(properties, "Status", NULL), NULL));
            ObexSession *session = obex_session_new(g_variant_get_string(g_variant_lookup_value(properties, "Session", NULL), NULL));
            
            info->obex_root = g_strdup(obex_session_get_root(session, NULL));
            
            g_object_unref(session);
            
            g_hash_table_insert(_transfer_infos, g_strdup(interface_object_path), info);
            if(g_strcmp0(info->status, "queued") == 0)
                g_print("[Transfer#%s] Waiting...\n", info->filename);
        }
        
        if(g_variant_lookup(interfaces_and_properties, OBEX_SESSION_DBUS_INTERFACE, "@a{sv}", &properties))
        {
            // g_print("[OBEX Client] OBEX session opened\n");
        }
        
        g_variant_unref(interfaces_and_properties);
        if(properties)
            g_variant_unref(properties);
    }
    else if(g_strcmp0(signal_name, "InterfacesRemoved") == 0)
    {
        const gchar *interface_object_path = g_variant_get_string(g_variant_get_child_value(parameters, 0), NULL);
        GVariant *interfaces = g_variant_get_child_value(parameters, 1);
        const gchar **inf_array = g_variant_get_strv(interfaces, NULL);
        g_variant_unref(interfaces);
        const gchar **inf = NULL;
        for(inf = inf_array; *inf != NULL; inf++)
        {
            if(g_strcmp0(*inf, OBEX_TRANSFER_DBUS_INTERFACE) == 0)
            {
                // g_print("[OBEX Client] OBEX transfer closed\n");
                ObexTransfer *transfer = g_hash_table_lookup(_transfers, interface_object_path);
                g_hash_table_remove(_transfers, interface_object_path);
                g_object_unref(transfer);
                g_free(g_hash_table_lookup(_transfer_infos, interface_object_path));
                g_hash_table_remove(_transfer_infos, interface_object_path);
                //if (g_main_loop_is_running(mainloop))
                //    g_main_loop_quit(mainloop);
            }
            
            if(g_strcmp0(*inf, OBEX_SESSION_DBUS_INTERFACE) == 0)
            {
                // g_print("[OBEX Client] OBEX session closed\n");
            }
        }
        g_free(inf_array);
    }
}

//客户端的属性改变回调
static void _obex_opp_client_properties_handler(GDBusConnection *connection, const gchar *sender_name, const gchar *object_path, const gchar *interface_name, const gchar *signal_name, GVariant *parameters, gpointer user_data)
{
	struct CallbackDataObjectMange *data=(struct CallbackDataObjectMange *)user_data;


    const gchar *arg0 = g_variant_get_string(g_variant_get_child_value(parameters, 0), NULL);
    GVariant *changed_properties = g_variant_get_child_value(parameters, 1);
    
    if(g_strcmp0(arg0, OBEX_TRANSFER_DBUS_INTERFACE) == 0)
    {
        ObexTransfer *transfer = g_hash_table_lookup(_transfers, object_path);
        ObexTransferInfo *info = g_hash_table_lookup(_transfer_infos, object_path);
        
        guint64 size = 0x0;
        guint64 transferred = 0x0;
        obex_transfer_get_size(transfer, NULL);
        g_variant_lookup(changed_properties, "Size", "t", &size);		//Size：总字节数
        if(!size)
            size = obex_transfer_get_size(transfer, NULL);
        g_variant_lookup(changed_properties, "Transferred", "t", &transferred);		//Transferred：已传输的字节数
        
        if(size && transferred && g_strcmp0(info->status, "active") == 0)
        {
            guint pp = (transferred / (gfloat) size)*100;

            if (!_update_progress)
            {
                g_print("[Transfer#%s] Progress: %3u%%", obex_transfer_get_name(transfer, NULL), pp);
                _update_progress = TRUE;
            }
            else
            {
                g_print("\b\b\b\b%3u%%", pp);
            }

            if (pp == 100)
            {
                g_print("\n");
                _update_progress = FALSE;
            }
        }
        
        gchar *status = NULL;
        g_variant_lookup(changed_properties, "Status", "s", &status);
        
        if(status)
        {
            g_free(info->status);
            info->status = g_strdup(status);
            
            if(g_strcmp0(status, "active") == 0)
            {
                // g_print("[Client Server] Transfer active\n");
            }
            else if(g_strcmp0(status, "complete") == 0)
            {
                if(_update_progress)
                {
                    _update_progress = FALSE;
                    g_print("\b\b\b\b%3u%%", 100);
                    g_print("\n");
                }
				
                g_print("[Transfer#%s] Completed\n", info->filename);
				g_main_loop_quit(data->mainloop);
            }
            else if(g_strcmp0(status, "error") == 0)
            {
                if(_update_progress)
                {
                    _update_progress = FALSE;
                    g_print("\n");
                }
				g_main_loop_quit(data->mainloop);
                g_print("[Transfer#%s] Failed\n", info->filename);
            }
            else if(g_strcmp0(status, "queued") == 0)
            {
                // g_print("[OBEX Client] Transfer queued\n");
            }
            else if(g_strcmp0(status, "suspended") == 0)
            {
                if(_update_progress)
                {
                    _update_progress = FALSE;
                    g_print("\n");
                }
                    
                g_print("[Transfer#%s] Suspended\n", info->filename);
            }
            g_free(status);
        }
    }
    
    g_variant_unref(changed_properties);
}

//
static void _obex_server_object_manager_handler(GDBusConnection *connection, 
												const gchar *sender_name, 
												const gchar *object_path, 
												const gchar *interface_name, 
												const gchar *signal_name, 
												GVariant *parameters, 
												gpointer user_data)
{
	printf("[DEBUG:]org.freedesktop.DBus.ObjectManager\n");
    if(g_strcmp0(signal_name, "InterfacesAdded") == 0)
    {
        const gchar *interface_object_path = g_variant_get_string(g_variant_get_child_value(parameters, 0), NULL);
        GVariant *interfaces_and_properties = g_variant_get_child_value(parameters, 1);
        GVariant *properties = NULL;
        
        if(g_variant_lookup(interfaces_and_properties, OBEX_TRANSFER_DBUS_INTERFACE, "@a{sv}", &properties))
        {
            g_print("[OBEX Server] Transfer started\n");
            ObexTransfer *t = obex_transfer_new(interface_object_path);
            g_hash_table_insert(_transfers, g_strdup(interface_object_path), t);
            
            ObexTransferInfo *info = g_malloc0(sizeof(ObexTransferInfo));
            info->filesize = g_variant_get_uint64(g_variant_lookup_value(properties, "Size", NULL));
            info->status = g_strdup(g_variant_get_string(g_variant_lookup_value(properties, "Status", NULL), NULL));
            ObexSession *session = obex_session_new(g_variant_get_string(g_variant_lookup_value(properties, "Session", NULL), NULL));
            
            info->obex_root = g_strdup(obex_session_get_root(session, NULL));
            
            g_object_unref(session);
            
            g_hash_table_insert(_transfer_infos, g_strdup(interface_object_path), info);
        }
        
        if(g_variant_lookup(interfaces_and_properties, OBEX_SESSION_DBUS_INTERFACE, "@a{sv}", &properties))
        {
            g_print("[OBEX Server] OBEX session opened\n");
        }
        
        g_variant_unref(interfaces_and_properties);
        if(properties)
            g_variant_unref(properties);
    }
    else if(g_strcmp0(signal_name, "InterfacesRemoved") == 0)
    {
        const gchar *interface_object_path = g_variant_get_string(g_variant_get_child_value(parameters, 0), NULL);
        GVariant *interfaces = g_variant_get_child_value(parameters, 1);
        const gchar **inf_array = g_variant_get_strv(interfaces, NULL);
        g_variant_unref(interfaces);
        const gchar **inf = NULL;
        for(inf = inf_array; *inf != NULL; inf++)
        {
            if(g_strcmp0(*inf, OBEX_TRANSFER_DBUS_INTERFACE) == 0)
            {
                g_print("[OBEX Server] OBEX transfer closed\n");
                ObexTransfer *transfer = g_hash_table_lookup(_transfers, interface_object_path);
                g_hash_table_remove(_transfers, interface_object_path);
                g_object_unref(transfer);
                g_free(g_hash_table_lookup(_transfer_infos, interface_object_path));
                g_hash_table_remove(_transfer_infos, interface_object_path);
            }
            
            if(g_strcmp0(*inf, OBEX_SESSION_DBUS_INTERFACE) == 0)
            {
                g_print("[OBEX Server] OBEX session closed\n");
            }
        }
        g_free(inf_array);
    }
}

static void _obex_server_properties_handler(GDBusConnection *connection, const gchar *sender_name, const gchar *object_path, const gchar *interface_name, const gchar *signal_name, GVariant *parameters, gpointer user_data)
{
	printf("[DEBUG:]org.freedesktop.DBus.Properties\n");
    const gchar *arg0 = g_variant_get_string(g_variant_get_child_value(parameters, 0), NULL);
    GVariant *changed_properties = g_variant_get_child_value(parameters, 1);
    
    if(g_strcmp0(arg0, OBEX_TRANSFER_DBUS_INTERFACE) == 0)
    {
        ObexTransfer *transfer = g_hash_table_lookup(_transfers, object_path);
        
        guint64 size = 0x0;
        guint64 transferred = 0x0;
        obex_transfer_get_size(transfer, NULL);
        g_variant_lookup(changed_properties, "Size", "t", &size);
        if(!size)
            size = obex_transfer_get_size(transfer, NULL);
        g_variant_lookup(changed_properties, "Transferred", "t", &transferred);
        
        if(size && transferred)
        {
            guint pp = (transferred / (gfloat) size)*100;

            if (!_update_progress)
            {
                g_print("[OBEXTransfer] Progress: %3u%%", pp);
                _update_progress = TRUE;
            }
            else
            {
                g_print("\b\b\b\b%3u%%", pp);
            }

            if (pp == 100)
            {
                g_print("\n");
                _update_progress = FALSE;
            }
        }
        
        gchar *status = NULL;
        g_variant_lookup(changed_properties, "Status", "s", &status);
        
        if(status)
        {
            if(g_strcmp0(status, "active") == 0)
            {
                // g_print("[OBEX Server] Transfer active\n");
            }
            else if(g_strcmp0(status, "complete") == 0)
            {
                g_print("[OBEX Server] Transfer succeeded\n");
                ObexTransferInfo *info = g_hash_table_lookup(_transfer_infos, object_path);
                g_rename(g_build_filename(info->obex_root, info->filename, NULL), g_build_filename(_root_path, info->filename, NULL));
            }
            else if(g_strcmp0(status, "error") == 0)
            {
                g_print("[OBEX Server] Transfer failed\n");
            }
            else if(g_strcmp0(status, "queued") == 0)
            {
                g_print("[OBEX Server] Transfer queued\n");
            }
            else if(g_strcmp0(status, "suspended") == 0)
            {
                g_print("[OBEX Server] Transfer halted\n");
            }
            g_free(status);
        }
    }
    
    g_variant_unref(changed_properties);
}

void _agent_approved_callback(ObexAgent *obex_agent, const gchar* obex_transfer_path, const gchar *name, const guint64 size, gpointer user_data)
{
	printf("[DEBUG]:_agent_approved_callback\n");
    ObexTransferInfo *info = g_hash_table_lookup(_transfer_infos, obex_transfer_path);
    if(!info)
    {
        info = g_malloc0(sizeof(ObexTransferInfo));
        g_hash_table_insert(_transfer_infos, g_strdup(obex_transfer_path), info);
        ObexTransfer *transfer = g_hash_table_lookup(_transfers, obex_transfer_path);
        ObexSession *session = obex_session_new(obex_transfer_get_session(transfer, NULL));
        info->obex_root = g_strdup(obex_session_get_root(session, NULL));
    }
    info->filename = g_strdup(name);
    info->filesize = size;
}

/* 监听 OBEX 传输状态变化 */
static void on_transfer_properties_changed(GDBusConnection *connection,
                                           const gchar     *sender_name,
                                           const gchar     *object_path,
                                           const gchar     *interface_name,
                                           const gchar     *signal_name,
                                           GVariant        *parameters,
                                           gpointer         user_data)
{
    const gchar *iface;
    GVariant *changed_properties, *invalidated_properties;
    g_variant_get(parameters, "(&sa{sv}as)", &iface, &changed_properties, &invalidated_properties);

    if (g_strcmp0(iface, OBEX_TRANSFER_INTERFACE) == 0) {
        g_print("Transfer status update from %s:\n", object_path);
        GVariantIter iter;
        const gchar *key;
        GVariant *value;
        g_variant_iter_init(&iter, changed_properties);
        while (g_variant_iter_next(&iter, "{&sv}", &key, &value)) {
            if (g_strcmp0(key, "Status") == 0) {
                const gchar *status = g_variant_get_string(value, NULL);
                g_print("  Status: %s\n", status);
            } else if (g_strcmp0(key, "Path") == 0) {
                const gchar *path = g_variant_get_string(value, NULL);
                g_print("  File received at: %s\n", path);
            }
            g_variant_unref(value);
        }
    }
    g_variant_unref(changed_properties);
    g_variant_unref(invalidated_properties);
}


//服务端ObjectManager注册和回调
BluetDbusSignal *bluet_obex_agent_signal_subscribe_properties(BluetObexAgent *agent,void *userdata)
{
	return bluet_dbus_signal_subscribe(session_conn,
										"org.bluez.obex", 
										"org.freedesktop.DBus.ObjectManager", 
										NULL,NULL, 
										_obex_server_object_manager_handler,
										userdata, NULL);
}

//服务端Properties注册和回调
BluetDbusSignal *bluet_obex_agent_signal_subscribe_objext_manager(BluetObexAgent *agent,void *userdata)
{
	return bluet_dbus_signal_subscribe(session_conn,
										"org.bluez.obex", 
										"org.freedesktop.DBus.Properties", 
										"PropertiesChanged",NULL, 
										_obex_server_properties_handler,
										userdata, NULL);
}

//信号注册的删除
int bluet_obex_signal_subscribe_delete(BluetDbusSignal *sig)
{
	bluet_dbus_signal_delete(session_conn,sig);
}


int bluet_obex_hash_creat()
{
	_transfers = g_hash_table_new(g_str_hash, g_str_equal);
	_transfer_infos = g_hash_table_new(g_str_hash, g_str_equal);
}


int bluet_obex_hash_free()
{
	GHashTableIter iter;
	gpointer key, value;
	
	g_hash_table_iter_init(&iter, _transfers);			//初始化哈希表迭代器，用于遍历哈希表的键值对
	while (g_hash_table_iter_next(&iter, &key, &value))
	{
		ObexTransfer *t = OBEX_TRANSFER(value);
		obex_transfer_cancel(t, NULL); // skip errors
		g_object_unref(t);
		g_hash_table_iter_remove(&iter);
	}
	g_hash_table_unref(_transfers);
	
	// Remove transfer information objects
	g_hash_table_iter_init(&iter, _transfer_infos);
	while (g_hash_table_iter_next(&iter, &key, &value))
	{
		g_free(value);
		g_hash_table_iter_remove(&iter);
	}
	g_hash_table_unref(_transfer_infos);
}



BluetObexAgent *bluet_obex_agent_creat(const char *path)
{
	GError *error = NULL;
	BluetObexAgent *self=(BluetObexAgent *)malloc(sizeof(BluetObexAgent));
	if(!self)
		return NULL;

	BluetObexAgentPrivate *priv=(BluetObexAgentPrivate *)malloc(sizeof(BluetObexAgentPrivate));
	if(!priv)
	{	
		free(self);
		return NULL;
	}

	_transfers = g_hash_table_new(g_str_hash, g_str_equal);
	_transfer_infos = g_hash_table_new(g_str_hash, g_str_equal);

	ObexAgentManager *manager = obex_agent_manager_new();
	ObexAgent *agent = obex_agent_new(_root_path, auto_accept);
	obex_agent_set_approved_callback(agent, _agent_approved_callback, NULL);

    obex_agent_manager_register_agent(manager, OBEX_AGENT_DBUS_PATH, &error);

	self->priv=priv;
	self->priv->manager=manager;
	self->priv->agent=agent;
	self->priv->mainthread=dbus_main_thread_creat_once(DBUS_MAIN_THREAD_TYPE_LOOP);
	return self;
}

int bluet_obex_agent_delete(BluetObexAgent *self)
{
	if(!self)
		return 0;
	if(self->priv->mainthread)
		dbus_main_thread_delete_once(self->priv->mainthread);

	obex_agent_manager_unregister_agent(self->priv->manager, OBEX_AGENT_DBUS_PATH, NULL);
	if(self->priv->agent)
		g_object_unref(self->priv->agent);
	self->priv->agent=NULL;

	if(self->priv->manager)
		g_object_unref(self->priv->manager);
	self->priv->manager=NULL;	

	free(self->priv);
	free(self);
	return 0;
}

int bluet_obex_agent_manager_creat(BluetObexAgent *agent)
{

}

int recv_file_dbus() {
	BluetObexAgent *bluet_agent=bluet_obex_agent_creat(NULL);

	GError *error = NULL;
    GDBusConnection *connection;
    guint registration_id, signal_sub_id;
    GMainLoop *loop;



	dbus_connect_init();

	if (!dbus_system_connect(&error))
    {
        g_printerr("Couldn't connect to DBus system bus: %s\n", error->message);
        exit(EXIT_FAILURE);
    }
	printf("dbus_system_connect ok\n");
    if (!dbus_session_connect(&error))
    {
        g_printerr("Couldn't connect to DBus session bus: %s\n", error->message);
        exit(EXIT_FAILURE);
    }
	printf("dbus_session_connect ok\n");

	// Check, that bluetooth daemon is running 
	if (!intf_supported(BLUEZ_BUS_NAME, MANAGER_DBUS_PATH, MANAGER_DBUS_INTERFACE))
	{
		g_printerr("%s: bluez service is not found\n", g_get_prgname());
		g_printerr("Did you forget to run bluetoothd?\n");
		exit(EXIT_FAILURE);
	}
	// Check, that obexd daemon is running 
	if (!intf_supported(BLUEZ_OBEX_DBUS_SERVICE_NAME, MANAGER_DBUS_PATH, MANAGER_DBUS_INTERFACE))
	{
		g_printerr("%s: obex service is not found\n", g_get_prgname());
		g_printerr("Did you forget to run obexd?\n");
		exit(EXIT_FAILURE);
	}
	
	_transfers = g_hash_table_new(g_str_hash, g_str_equal);
	_transfer_infos = g_hash_table_new(g_str_hash, g_str_equal);
	
	//事件？
	BluetDbusSignal *sig_registration=bluet_obex_agent_signal_subscribe_objext_manager(NULL,NULL);
	/*registration_id = g_dbus_connection_signal_subscribe(session_conn, 
														"org.bluez.obex", 
														"org.freedesktop.DBus.ObjectManager", 
														NULL, 
														NULL, 
														NULL, 
														G_DBUS_SIGNAL_FLAGS_NONE, 
														_obex_server_object_manager_handler, 
														NULL, NULL);*/

    //属性变化？
	BluetDbusSignal *signal_sub=bluet_obex_agent_signal_subscribe_properties(NULL,NULL);
    /*signal_sub_id = g_dbus_connection_signal_subscribe(session_conn,
														"org.bluez.obex",
														"org.freedesktop.DBus.Properties",
														"PropertiesChanged",
														NULL,
														NULL,
														G_DBUS_SIGNAL_FLAGS_NONE,
														_obex_server_properties_handler,
														NULL,
														NULL);*/

	ObexAgentManager *manager = obex_agent_manager_new();
	ObexAgent *agent = obex_agent_new(_root_path, auto_accept);
	obex_agent_set_approved_callback(agent, _agent_approved_callback, NULL);

    obex_agent_manager_register_agent(manager, OBEX_AGENT_DBUS_PATH, &error);

    g_print("Listening for OBEX file transfer signals...\n");

    /* 进入主循环 */
    loop = g_main_loop_new(NULL, FALSE);
    g_main_loop_run(loop);

    /* 清理资源 */
    //g_dbus_connection_signal_unsubscribe(session_conn, signal_sub_id);
    //g_dbus_connection_unregister_object(session_conn, registration_id);
	bluet_obex_signal_subscribe_delete(sig_registration);
	bluet_obex_signal_subscribe_delete(signal_sub);
    g_object_unref(session_conn);

	obex_agent_manager_unregister_agent(manager, OBEX_AGENT_DBUS_PATH, &error);
	g_object_unref(agent);
    g_object_unref(manager);
    g_main_loop_unref(loop);
	bluet_obex_agent_delete(bluet_agent);
    return EXIT_SUCCESS;
}




BluetObexPush *bluet_obex_push_creat()
{
	BluetObexPush *push=(BluetObexPush *)malloc(sizeof(BluetObexPush));
	if(!push)
		return NULL;

	BluetObexPushPrivate *priv=(BluetObexPushPrivate *)malloc(sizeof(BluetObexPushPrivate));
	if(!priv)
	{	
		free(push);
		return NULL;
	}

	push->priv=priv;

	push->priv->client=NULL;
	push->priv->session=NULL;
	push->priv->oop=NULL;
	push->priv->device_dict=NULL;	
	push->priv->data_m=NULL;
	push->priv->mainloop=g_main_loop_new(NULL, FALSE);
	return push;
}

int bluet_obex_push_delete(BluetObexPush *push)
{
	if(!push)
		return 0;
	if(push->priv->mainloop)
		g_main_loop_unref(push->priv->mainloop);


	push->priv->mainloop=NULL;

	if(push->priv->client)
		g_object_unref(push->priv->client);
	if(push->priv->device_dict)	
		g_variant_unref(push->priv->device_dict);
	if(push->priv->data_m)
		free(push->priv->data_m);
	free(push->priv);
	free(push);
	return 0;
}


//构造ObexSession对象
int bluet_obex_push_session_creat(BluetObexPush *push, const char *dst_address)
{
	GError *error = NULL;
	GVariantBuilder *b = g_variant_builder_new(G_VARIANT_TYPE_VARDICT);
	g_variant_builder_add(b, "{sv}", "Target", g_variant_new_string("opp"));
	//g_variant_builder_add(b, "{sv}", "Source", g_variant_new_string(src_address));
	push->priv->device_dict = g_variant_builder_end(b);
	g_variant_builder_unref(b);
	push->priv->device_dict = g_variant_ref_sink(push->priv->device_dict);	//转 sink 浮动引用，变成 normal reference,如果是同步调用sendfile就不需要，异步调用sendfile则需要

	push->priv->client = obex_client_new();
	const gchar *session_path = obex_client_create_session(push->priv->client, dst_address, push->priv->device_dict, &error);
	exit_if_error(error);
	push->priv->session = obex_session_new(session_path);
	push->priv->oop = obex_object_push_new(obex_session_get_dbus_object_path(push->priv->session));

}

//用户端ObjectManager注册和回调
BluetDbusSignal *bluet_obex_push_signal_subscribe_properties(BluetObexPush *push,void *userdata)
{
	return bluet_dbus_signal_subscribe(session_conn,
										"org.bluez.obex", 
										"org.freedesktop.DBus.ObjectManager", 
										NULL,NULL, 
										_obex_opp_client_object_manager_handler, 
										userdata, NULL);
}

//用户端Properties注册和回调
BluetDbusSignal *bluet_obex_push_signal_subscribe_objext_manager(BluetObexPush *push,void *userdata)
{
	struct CallbackDataObjectMange *data=(struct CallbackDataObjectMange *)malloc(sizeof(struct CallbackDataObjectMange));
	if(!data)
		return NULL;

	data->userdata=userdata;
	data->mainloop=push->priv->mainloop;
	push->priv->data_m=data;

	return bluet_dbus_signal_subscribe(session_conn,
										"org.bluez.obex", 
										"org.freedesktop.DBus.Properties", 
										"PropertiesChanged",NULL, 
										_obex_opp_client_properties_handler, 
										data, NULL);
}


//阻塞发送文件
int bluet_obex_send_file(BluetObexPush *push, const char *file)
{
	GError *error = NULL;
	obex_object_push_send_file_async(push->priv->oop, file, &error);
	exit_if_error(error);
	g_main_loop_run(push->priv->mainloop);
}

//停止发送文件
int bluet_obex_stop_send_file(BluetObexPush *push)
{
	obex_stop_send_file(push->priv->mainloop);
}

static int obex_stop_send_file(GMainLoop *mainloop)
{
	//g_main_loop_quit(mainloop);
	g_main_loop_unref(mainloop);

	mainloop=NULL;
}





int send_file_dbus_(const char *dst_address,const char *files_to_send) 
{
	BluetObexPush *obex_push=bluet_obex_push_creat();


	GError *error = NULL;
    GDBusConnection *connection;
    GMainLoop *mainloop;
	dbus_connect_init();

	if (!dbus_system_connect(&error))
    {
        g_printerr("Couldn't connect to DBus system bus: %s\n", error->message);
        exit(EXIT_FAILURE);
    }
	printf("dbus_system_connect ok\n");
    if (!dbus_session_connect(&error))
    {
        g_printerr("Couldn't connect to DBus session bus: %s\n", error->message);
        exit(EXIT_FAILURE);
    }
	printf("dbus_session_connect ok\n");

	// Check, that bluetooth daemon is running 
	if (!intf_supported(BLUEZ_BUS_NAME, MANAGER_DBUS_PATH, MANAGER_DBUS_INTERFACE))
	{
		g_printerr("%s: bluez service is not found\n", g_get_prgname());
		g_printerr("Did you forget to run bluetoothd?\n");
		exit(EXIT_FAILURE);
	}
	// Check, that obexd daemon is running 
	if (!intf_supported(BLUEZ_OBEX_DBUS_SERVICE_NAME, MANAGER_DBUS_PATH, MANAGER_DBUS_INTERFACE))
	{
		g_printerr("%s: obex service is not found\n", g_get_prgname());
		g_printerr("Did you forget to run obexd?\n");
		exit(EXIT_FAILURE);
	}
	
        
        bluet_obex_hash_creat();

        // Get source address (address of adapter) 
        /*Adapter *adapter = find_adapter("hci0", &error);
        exit_if_error(error);
        gchar *src_address = g_strdup(adapter_get_address(adapter, &error));
        exit_if_error(error);
        
        // Get destination address (address of remote device) 
		Device *device = find_device(adapter, dst_address, &error);
		exit_if_error(error);
		g_object_unref(device);

        g_object_unref(adapter);*/

        /* Build arguments */
		bluet_obex_push_session_creat(obex_push,dst_address);

        // initialize GDBus OBEX OPP client callbacks
		
		BluetDbusSignal *sub_m=bluet_obex_push_signal_subscribe_objext_manager(obex_push, NULL);
		BluetDbusSignal *sub_p=bluet_obex_push_signal_subscribe_properties(obex_push, NULL);
        
        /* Sending file(s) */
        bluet_obex_send_file(obex_push, files_to_send);

        /* Sending files process here ?? */

        //bluet_obex_stop_send_file(obex_push);

       	bluet_obex_signal_subscribe_delete(sub_m);
		bluet_obex_signal_subscribe_delete(sub_p);
        
        // Stop active transfers 
		bluet_obex_hash_free();
//        bluet_obex_stop_send_file(obex_push);
		bluet_obex_push_delete(obex_push);

//        g_free(src_address);	
}
