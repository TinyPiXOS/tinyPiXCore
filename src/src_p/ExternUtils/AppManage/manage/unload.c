/*///------------------------------------------------------------------------------------------------------------------------//
		应用卸载程序
说 明 : 
日 期 : 2024.8.21

/*///------------------------------------------------------------------------------------------------------------------------//

#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include "install.h"
#include "unload.h"
#include "utilslib.h"
#include "purview.h"
#include "conf_json.h"
#include "fileUtils.h"

int appm_app_unload(tpAppID uuid)
{
	char command[256];

	if(is_valid_uuid(uuid.value)!=tpTURE)
	{
		fprintf(stderr,"uuid error\n");
		return -1;
	}

	//调用接口杀死进程


	//执行remove.sh
	char path[PATH_MAX_LENGTH];
	snprintf(path,sizeof(path),"%s/%s",APP_INSTALL_PATH,uuid.value);
	if(find_directory(path,APP_REMOVE_SH)>0)
	{
		system_command("./" APP_REMOVE_SH);
	}

	//删除安装文件
	snprintf(command, sizeof(command),"%s/%s",APP_INSTALL_PATH,uuid.value);	//rm -r
	if(remove_dir(command)<0)
	{
		fprintf(stderr,"delete app error\n");
	}

	//删除conf里面的uuid.json文件
	snprintf(command, sizeof(command),"%s/%s.json",APP_JSON_PATH,uuid.value);
	if(remove_dir(command)<0)
	{
		fprintf(stderr,"delete app conf json error\n");
		//return -1;
	}

	//修改install_conf
/*	if(del_appuuid_install_safe(uuid,APP_INSTALL_CONF_PATH)<0)
	{
		fprintf(stderr,"delete install json error\n");
		//return -1;
	}
*/
	//删除用户
	if(Appm_Remove_Purview(uuid)<0)
	{
		fprintf(stderr,"delete user error\n");
	}
	return 0;
}
