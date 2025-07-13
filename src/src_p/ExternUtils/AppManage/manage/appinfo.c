/*///------------------------------------------------------------------------------------------------------------------------//
		应用信息
说 明 : 
日 期 : 2025.6.11

/*///------------------------------------------------------------------------------------------------------------------------//

#include <stdio.h>
#include <string.h>
#include "conf_json.h"
#include "utilslib.h"
#include "install.h"
#include "appinfo.h"

void appm_free(void *ptr)
{
	free(ptr);
}

//获取安装的json文件的解密后的字符串
char *appm_info_get_install_json(tpAppID *appid)
{
	char path[PATH_MAX_LENGTH];

	snprintf(path,PATH_MAX_LENGTH,"%s/%s.json",APP_JSON_PATH,appid->value);

	return read_json_string_file_encryption(path);
}

//把普通的字符串uuid转换成标准的appid
int appm_info_get_appid(const char *uuid,tpAppID *appid)
{
	if(is_valid_uuid(uuid)!=1)
		return -1;
	strncpy(appid->value, uuid,sizeof(appid->value));
	return 0;
}

//获app对应的用户id
char *appm_info_get_appuserid(tpAppID *appid)
{
	char *userid=malloc(strlen(appid->value));
	uuid_remove_hyphens(appid->value,userid);
	return userid;
}

//int find_directory(const char *path, const char *target_directory) 
//查找应用是否已安装(使用uuid)
bool appm_info_is_install(tpAppID *appid)
{
	if(find_directory(APP_INSTALL_PATH,appid->value)>0)
		return true;
	return false;
}


//设置应用权限
int appm_info_set_permission(tpAppID *appid)
{
	
}


