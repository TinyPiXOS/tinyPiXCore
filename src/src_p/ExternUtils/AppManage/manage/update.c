/*///------------------------------------------------------------------------------------------------------------------------//
		应用升级程序
说 明 : 
日 期 : 2024.8.21

/*///------------------------------------------------------------------------------------------------------------------------//

#include <stdio.h>
#include <string.h>
#include "install.h"
#include "update.h"
#include "utilslib.h"


//
//升级的拷贝

//读配置文件升级参数
int update_get_config(char * appname)
{
	char path_conf[128];		//config目录

	char key_prefix[64];
	//拼接安装包config目录
	memset(path_conf,0,sizeof(path_conf));
	snprintf(path_conf,sizeof(path_conf),"%s/%s/%s/config",APP_INSTALL_PATH,APP_TEMP,appname);
	FILE *file_s = fopen(path_conf, "r");
	if (!file_s) {
        perror("fopen s_config");
		printf("path:%s\n",path_conf);
        return -1;
    }
	fseek(file_s, 0, SEEK_SET);
	
	char line[CONFIG_MAX_LENGTH];
	while (fgets(line, CONFIG_MAX_LENGTH, file_s)) 
	{
		char *flag_update = strstr(line, "update ");
        if (flag_update == NULL) 
			continue;
		//cp -r assert/ 
		if((flag_update=strstr(line,UPDATE_ASSERT))!=NULL)
		{
			

		}
		//cp –exclude=assert -r source/ destination/
    }




	fclose(file_s);
	return 0;

}




int Appm_Update()
{

	char *uuid="fb1412cf-84de-4138-b401-215b6e9b1c11";
	if(find_directory(APP_INSTALL_PATH,uuid)==0)
	{
		printf("app not installed\n");
		return -1;
	}

	return 0;

}
