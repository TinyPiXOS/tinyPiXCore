#ifndef _BLUET_PROFILE_H_
#define _BLUET_PROFILE_H_

/*#ifdef	__cplusplus
extern "C" {
#endif*/

#include <glib-object.h>


#define PROFILE_MANAGER_DBUS_SERVICE "org.bluez"
#define PROFILE_MANAGER_DBUS_INTERFACE "org.bluez.ProfileManager1"
#define PROFILE_MANAGER_DBUS_PATH "/org/bluez"
#define PROFILE_DBUS_INTERFACE "org.bluez.Profile1"


/*
 * Type macros
 */
#define PROFILE_MANAGER_TYPE				(profile_manager_get_type())
#define PROFILE_MANAGER(obj)				(G_TYPE_CHECK_INSTANCE_CAST((obj), PROFILE_MANAGER_TYPE, ProfileManager))
#define PROFILE_MANAGER_IS(obj)				(G_TYPE_CHECK_INSTANCE_TYPE((obj), PROFILE_MANAGER_TYPE))
#define PROFILE_MANAGER_CLASS(klass)		(G_TYPE_CHECK_CLASS_CAST((klass), PROFILE_MANAGER_TYPE, ProfileManagerClass))
#define PROFILE_MANAGER_IS_CLASS(klass)		(G_TYPE_CHECK_CLASS_TYPE((klass), PROFILE_MANAGER_TYPE))
#define PROFILE_MANAGER_GET_CLASS(obj)		(G_TYPE_INSTANCE_GET_CLASS((obj), PROFILE_MANAGER_TYPE, ProfileManagerClass))

typedef struct _ProfileManager ProfileManager;
typedef struct _ProfileManagerClass ProfileManagerClass;
typedef struct _ProfileManagerPrivate ProfileManagerPrivate;


struct _ProfileManager {
	GObject parent_instance;

	/*< private >*/
	ProfileManagerPrivate *priv;
};

struct _ProfileManagerClass {
	GObjectClass parent_class;
};


GType profile_manager_get_type(void) G_GNUC_CONST;

ProfileManager *profile_manager_new();



/*#ifdef	__cplusplus
}
#endif*/


#endif