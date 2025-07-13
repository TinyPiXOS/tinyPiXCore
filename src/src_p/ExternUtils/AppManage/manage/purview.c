/*///------------------------------------------------------------------------------------------------------------------------//
		应用权限管理
说 明 : 
日 期 : 2024.10.12

/*///------------------------------------------------------------------------------------------------------------------------//

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <pwd.h>
#include <grp.h>
#include "purview.h"
#include "../inc/appmanage_conf.h"
#include "install.h"
#include "utilslib.h"
#include "system_permission.h"



//检查用户是否存在
int check_user_exist(const char *user)
{
	struct passwd *pw = getpwnam(user);
	if (pw != NULL)
		return 1;  // 用户存在
	return 0;
}

//检查用户组是否纯在
int check_group_exist(const char *group)
{
	struct group *grp = getgrnam(group);
	if (grp != NULL)
		return 1;  // 用户组存在
	return 0;  // 用户组不存在
}


//使用uuid创建用户
static int creat_user(const char *user,const char *passwd,const char *shell)
{
	char command[128];
	memset(command, 0, sizeof(command));
	snprintf(command, sizeof(command), "useradd -m %s", user);
	if(system_command(command)<0)
		return -1;
	if(shell)
	{
		snprintf(command, sizeof(command), "usermod -s %s %s", shell, user);
		return system_command(command);
	}
	return 0;
}

//删除用户
int delete_user(const char *user)
{
	char command[128];
	memset(command, 0, sizeof(command));
	snprintf(command, sizeof(command), "userdel -r %s", user);
	return system_command(command);
}

//创建用户组
int creat_group(const char *group)
{
	char command[128];
	memset(command, 0, sizeof(command));
	snprintf(command, sizeof(command), "groupadd %s", group);
	return system_command(command);
}

//删除用户组
int delete_group(const char *group)
{
    char command[128];
	memset(command, 0, sizeof(command));
    snprintf(command, sizeof(command), "groupdel %s", group);
    return system_command(command);
}

//设置文件目录所属用户和用户组
int set_files_user_and_group( const char *path, const char *uuid, const char *group)
{
	char command[PATH_MAX_LENGTH];
	memset(command, 0, sizeof(command));
	snprintf(command, sizeof(command), "chown -R %s:%s %s", uuid, group, path);
	return system_command(command);
}

//设置权限
int set_path_purview(const char *path,const char *purview)
{
	char command[PATH_MAX_LENGTH];
	memset(command, 0, sizeof(command));
	snprintf(command, sizeof(command), "chmod -R %s %s",purview, path);
	return system_command(command);
}

//初始化(系统启动调用,可能暂时不需要)
int Appm_Install_Purview_Init()
{
	if(check_group_exist(USERGROUP_USER_APP)==0)
	{
		if(creat_group(USERGROUP_USER_APP)<0)
			return -1;

		//根据已经安装的应用的uuid判断是否有用户
		DIR *dir;
		struct dirent *entry;
		char path[PATH_MAX_LENGTH];	
		snprintf(path, sizeof(path),"%s", APP_INSTALL_PATH);	
		if ((dir = opendir(APP_INSTALL_PATH)) == NULL) {
			fprintf(stderr,"opendir() %s error\n",APP_INSTALL_PATH);
			return -1;
		}

		while ((entry = readdir(dir)) != NULL) 
		{
			if (entry->d_type == DT_DIR) {
				// Skip "." and ".."
				if (strcmp(entry->d_name, ".") == 0 || strcmp(entry->d_name, "..") == 0) {
					continue;
				}
				if(is_valid_uuid(entry->d_name)==0)
					continue;

				if(check_user_exist(entry->d_name))
				{
					creat_user(entry->d_name,NULL,NULL);
					snprintf(path, sizeof(path),"%s/%s", APP_INSTALL_PATH,entry->d_name);	
					set_files_user_and_group( path, entry->d_name, USERGROUP_USER_APP);
					set_path_purview(path,"700");
				}
			}
		}
		closedir(dir);
			
	}
	return 0;
}

//安装权限设置
//注意：需要提前设置/System/lib的权限
int Appm_Install_Purview(tpAppID uuid,TypePackage type)
{
	if(is_valid_uuid(uuid.value)!=tpTURE)
		return -1;
	char user_uuid[33];	
	uuid_remove_hyphens(uuid.value,user_uuid);
	user_uuid[32]='\0';
	printf("userid:%s\n",user_uuid);
	if(creat_user(user_uuid,NULL,NULL)<0)
	{
		printf("user creat error\n");
		return -1;
	}
	printf("userid:%s\n",user_uuid);
	char *install_path;
	if(type==TYPE_PACKAGE_SAPP) install_path = APPS_INSTALL_PATH;
	else install_path = APP_INSTALL_PATH;
	char path[PATH_MAX_LENGTH];
	memset(path,0,sizeof(path));
	snprintf(path,sizeof(path),"%s/%s",install_path,uuid.value);
	if(set_files_user_and_group(path,user_uuid,"root")<0)//设置所属的用户和用户组
		return -1;
	if(set_path_purview(path,"700")<0)		//设置安装目录的权限
		return -1;

	return 0;
}

//卸载调用
//删除用户
int Appm_Remove_Purview(tpAppID uuid)
{
	if(is_valid_uuid(uuid.value)!=tpTURE)
		return -1;
	char user_uuid[33];	
	uuid_remove_hyphens(uuid.value,user_uuid);
	user_uuid[32]='\0';
	if(delete_user(user_uuid)<0)
		return -1;
	return 0;
}



