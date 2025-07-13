/*///------------------------------------------------------------------------------------------------------------------------//
		脚本文件生成的库
说 明 : 
日 期 : 2024.9.3

/*///------------------------------------------------------------------------------------------------------------------------//
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "file_creat_lib.h"
/*
void init_script_config(struct ScriptInfo *config) {
    memset(config, 0, sizeof(struct ScriptInfo));
}*/

//==============================配置文件===================================
// 添加依赖库（一般是系统通用的库）
//库名字，版本
void add_depend_lib(struct ConfigInfo *config, const char *lib,const char *version) {
	if (config->depend_lib_count < MAX_ITEMS) {
		char *entry = (char*)malloc(strlen(lib) + strlen(version) + 2); // +2 for '@s' and '\0'
		sprintf(entry, "%s@%s", lib, version);
		config->depend_lib[config->depend_lib_count++] = entry;
	}
}
//添加作者自己的库（一般是本应用使用或者是使用了开源库但是只给本应用使用）
void add_private_lib(struct ConfigInfo *config, const char *lib) {
	if (config->private_lib_count < MAX_ITEMS) {
		config->private_lib[config->private_lib_count++] = strdup(lib);
	}
}
//添加其他export变量
void add_export_info(struct ConfigInfo *config,const char *key ,const char *value)
{
	if (config->export_count < MAX_ITEMS) {
		char *entry = (char*)malloc(strlen(key) + strlen(value) + 2); // +2 for '=' and '\0'
		sprintf(entry, "%s=%s", key, value);
		config->depend_lib[config->depend_lib_count++] = entry;
	}
}

//添加更新信息
void add_update_info(struct ConfigInfo *config,const char *key ,int value)
{
	if (config->export_count < MAX_ITEMS) {
		char *entry = (char*)malloc(strlen(key) +3); // +2 for '=' and '\0'
		sprintf(entry, "%s=%d", key, value);
		config->update[config->update_count++] = entry;
	}
}

void generate_config(struct ConfigInfo *config, const char *output_file) {
	FILE *fp = fopen(output_file, "w");
	if (!fp) {
		perror("Failed to create config");
		return;
	}
	//写入基本描述信息


	// 写入依赖库
	if (config->depend_lib_count > 0) 
	{
		fprintf(fp, "export depend=");
		for (int i = 0; i < config->depend_lib_count; i++) {
			fprintf(fp, "%s \n", config->depend_lib[i]);
		}
	}

	if (config->private_lib_count > 0) 
	{
		fprintf(fp, "export lib=");
		for (int i = 0; i < config->private_lib_count; i++) {
			fprintf(fp, "%s \n", config->private_lib[i]);
		}
	}

	// 写入export
	if (config->export_count > 0) 
	{
		for (int i = 0; i < config->export_count; i++) {
			fprintf(fp, "export %s\n", config->export_info[i]);
		}
	}

    // 写入更新信息
	if (config->update_count > 0) 
	{
		for (int i = 0; i < config->update_count; i++) {
			fprintf(fp, "update %s\n", config->update[i]);
		}
	}	

	fclose(fp);
}





