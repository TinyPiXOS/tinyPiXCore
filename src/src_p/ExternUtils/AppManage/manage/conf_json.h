#ifndef _CONF_JSON_H_
#define _CONF_JSON_H_

#include "../inc/appmanage_conf.h"
#include "json.h"




typedef enum{
	EXPORT_NONE = 0,
	EXPORT_LIBS = 1,
	EXPORT_DEPEND = 2,
	EXPORT_MUST = 3
}PackageExportType;



int config_export_analysis_json(char *line,json_object *export_obj);
int config_keyvalue_analysis_json(char *line,json_object *export_obj);
int find_key_from_file(const char *file_path,const char *key,char *value);
int config_add_to_json(PackageExportType type,json_object *export_obj,const char *value,const char *key);
int del_appuuid_install_safe(tpAppID uuid,const char *install_path);
int add_appuuid_install_safe(tpAppID uuid,const char *install_path);
int write_json_object_file(json_object *root,const char *file_path);
int write_json_object_file_encryption(json_object *root,const char *file_path);
char *read_json_string_file_encryption(const char *file_path);
#endif
