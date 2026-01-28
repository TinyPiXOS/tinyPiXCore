#ifndef _TPDBUS_NM_CONNECTION_H_
#define _TPDBUS_NM_CONNECTION_H_

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>
#include <stdbool.h>
#include <gio/gio.h>
#include <glib.h>
#include <glib-object.h>

#define NM_CONNECTION_INTERFACE 			"org.freedesktop.NetworkManager.Settings.Connection"


#define NM_CONNECTION_TYPE                  (nm_connection_get_type ())
#define NM_CONNECTION(obj)                  (G_TYPE_CHECK_INSTANCE_CAST ((obj), NM_CONNECTION_TYPE, NmConnection))
#define NM_CONNECTION_IS(obj) (G_TYPE_CHECK_INSTANCE_TYPE((obj), NM_CONNECTION_TYPE))
#define NM_CONNECTION_CLASS(klass) (G_TYPE_CHECK_CLASS_CAST((klass), NM_CONNECTION_TYPE, NmConnectionClass))
#define NM_CONNECTION_IS_CLASS(klass) (G_TYPE_CHECK_CLASS_TYPE((klass), NM_CONNECTION_TYPE))
#define NM_CONNECTION_GET_CLASS(obj) (G_TYPE_INSTANCE_GET_CLASS((obj), NM_CONNECTION_TYPE, NmConnectionClass))

typedef struct NmConnection_ NmConnection;
typedef struct NmConnectionPrivate_ NmConnectionPrivate;
typedef struct NmConnectionClass_ NmConnectionClass;

struct NmConnection_{
	GObject parent_instance;		//父类
	NmConnectionPrivate *priv;
};

struct NmConnectionClass_ {
	GObjectClass parent_class;
};




GType nm_connection_get_type(void) G_GNUC_CONST;		//此函数由Glib根据G_DEFINE_TYPE_WITH_PRIVATE自动生成，此处声明是为了方便调用

NmConnection *nm_connection_create(GDBusConnection *conn, const char *object_path);
int nm_connection_delete(NmConnection *self);				   


//设置dhcp
void nm_connection_set_ipv4_dhcp_is_enabled(NmConnection *self, bool enable, GError **error);
// 查询 DHCP 状态：1=auto/DHCP, 0=manual/static, -1=error/no-active
int nm_connection_get_ipv4_dhcp_state(NmConnection *self,  GError **error);

//设置静态dns列表
void nm_connection_set_ipv4_dns_list(NmConnection *self, const char **dns, uint32_t dns_count, GError **error);
//获取dns列表，注意获取成功后必须使用nm_connection_free_ipv4_dns_list释放
int nm_connection_get_ipv4_dns_list(NmConnection *self, char ***dns_list_out, GError **error);
//释放，注意：使用nm_connection_get_ipv4_dns_list获取后必须使用此函数释放
int nm_connection_free_ipv4_dns_list(char **dns_list_out);	

void nm_connection_set_ipv6_dns_mode(NmConnection *self, bool isauto, GError **error);

void nm_connection_set_ipv4_dns_mode(NmConnection *self, bool isauto, GError **error);

//返回1为自动，0为静态，-1为失败
int nm_connection_get_ipv6_dns_mode(NmConnection *self, GError **error);
//返回1为自动，0为静态，-1为失败
int nm_connection_get_ipv4_dns_mode(NmConnection *self, GError **error);



#ifdef __cplusplus
}
#endif

#endif
