#ifndef __SDP_SERVICE_H
#define __SDP_SERVICE_H

#ifdef	__cplusplus
extern "C" {
#endif

#include <stdio.h>

#define SERVICE_DBUS_SERVER			"org.bluez"                  // D-Bus 服务名 
#define SERVICE_DBUS_PATH			"/org/bluez"                 // 对象路径
#define SERVICE_DBUS_INTERFACE 		"org.bluez.Service1"		// 接口


#define SERVICE_TYPE                  	(service_get_type ())
#define SERVICE(obj)                  	(G_TYPE_CHECK_INSTANCE_CAST ((obj), SERVICE_TYPE, Service))
#define IS_SERVICE(obj)					(G_TYPE_CHECK_INSTANCE_TYPE((obj), SERVICE_TYPE))
#define SERVICE_CLASS(klass) 			(G_TYPE_CHECK_CLASS_CAST((klass), SERVICE_TYPE, ServiceClass))
#define IS_SERVICE_CLASS(klass)			(G_TYPE_CHECK_CLASS_TYPE((klass), SERVICE_TYPE))
#define SERVICE_GET_CLASS(obj)			(G_TYPE_INSTANCE_GET_CLASS((obj), SERVICE_TYPE, ServiceClass))


typedef struct Service_ Service;
typedef struct ServiceClass_ ServiceClass;
typedef struct ServicePrivate_ ServicePrivate;

struct Service_{
	GObject parent_instance;
	ServicePrivate *priv;	//私有数据
};

struct ServiceClass_ {
	GObjectClass parent_class;

	void (*service_ready)(Service *service);
    void (*service_error)(Service *service, const gchar *error);
};


GType service_get_type(void) G_GNUC_CONST;		//此函数由Glib根据G_DEFINE_TYPE_WITH_PRIVATE自动生成，此处声明是为了方便调用



// 公共方法
Service *service_new(const gchar *object_path);
gchar *service_get_uuid(Service *service);
gchar *service_get_name(Service *service);
gint service_get_rfcomm_channel(Service *service);
gint service_get_l2cap_psm(Service *service);
gchar *service_get_object_path(Service *service);


#ifdef	__cplusplus
}
#endif

#endif
