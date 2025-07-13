#ifndef _SYSTEM_PERMISSION_H_
#define _SYSTEM_PERMISSION_H_

#include <stdio.h>
#include <stdbool.h>

#ifdef __cplusplus
extern "C" {                //  告诉编译器下列代码要以C链接约定的模式进行链接
#endif

typedef struct SystemPermission SystemPermission;


typedef struct SystemPermission{
//	int (*load_users)(SystemPermission *system);
//	int (*load_groups)(SystemPermission *system);
	int (*create_group)(const char *group, gid_t gid);
	int (*delete_group)(const char *group);
	int (*create_user)( const char *username,const char *password,const char *primary_group,const char *secondary_groups[]);
	int (*delete_user)(const char *username, bool remove_home);
	int (*add_user_to_group)(const char *groupname, const char *username);
	int (*del_user_from_group)(const char *groupname, const char *username);
}SystemPermission;




SystemPermission *system_permission_manage_create();
int system_permission_manage_delete(SystemPermission *system);

#ifdef __cplusplus
}
#endif

#endif