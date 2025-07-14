//从JSON文件解析安装信息

#include <stdio.h>
#include <string.h>
#include "../inc/appmanage_conf.h"
#include "file_creat.h"
#include "json_conf.h"


//环境变量解析environment
static int json_conf_environment(struct json_object *dependencies,	char *types[MAX_ITEMS], char *values[MAX_ITEMS])
{
	if (json_object_get_type(dependencies) != json_type_array) 
		return -1;

	const char *type,*value;	
	int depend_count = json_object_array_length(dependencies);
	if(depend_count>MAX_ITEMS)
		depend_count=MAX_ITEMS;
	for (int i = 0; i < depend_count; i++) 
	{
		json_object *item = json_object_array_get_idx(dependencies, i);
		json_object *object;
		json_object_object_get_ex(item,"item",&object);
		if (json_object_object_get_ex((const json_object *)item, "type", &object)) {
			type=json_object_get_string((json_object *)object);
			if(!type)
				return -1;
		}
		if (json_object_object_get_ex((const json_object *)item, "value", &object)) {
			value=json_object_get_string((json_object *)object);
			if(!value)
				return -1;
		}

		types[i]=strdup(type);
		values[i]=strdup(value);
	}
	return depend_count;
}

//依赖项解析
static int json_conf_dependencies(struct json_object *dependencies,	char *depend[MAX_ITEMS])
{
	if (json_object_get_type(dependencies) != json_type_array) 
		return -1;

	const char *name,*version;	
	int depend_count = json_object_array_length(dependencies);
	if(depend_count>MAX_ITEMS)
		depend_count=MAX_ITEMS;
	for (int i = 0; i < depend_count; i++) 
	{
		json_object *item = json_object_array_get_idx(dependencies, i);
		json_object *object;
		json_object_object_get_ex(item,"item",&object);
		if (json_object_object_get_ex((const json_object *)item, "name", &object)) {
			name=json_object_get_string(object);
			if(!name)
				return -1;
		}
		if (json_object_object_get_ex((const json_object *)item, "version", &object)) {
			version=json_object_get_string(object);
			if(!version)
				return -1;
		}
		depend[i]=(char *)malloc(strlen(name)+strlen(version)+2);
		sprintf(depend[i],"%s@%s",name,version);
		
	}
	return depend_count;
}

static int json_conf_author(struct json_object *object,char *author,int author_max,char *contact,int contact_max)
{	
	struct json_object *item;
	if (json_object_object_get_ex((const json_object *)object, "name", &item)) {
		const char *name=json_object_get_string(item);
		if(!name)
			return -1;
		strncpy(author,name,author_max);
	}
	if (json_object_object_get_ex((const json_object *)object, "email", &item)) {
		const char *email=json_object_get_string(item);
		if(!email)
			return -1;
		strncpy(contact,email,contact_max);
	}
	return 0;
}

static int json_conf_version(struct json_object *object,struct tpVersion *ver)
{
	const char *str=json_object_get_string(object);
	char *str_tok=strdup(str);
	char *token;

   	token = strtok(str_tok, ".");
	for(int i=0;i<3;i++)
	{
		if(!token)
			break;
		switch(i){
			case 0:	ver->x=atoi(token);break;
			case 1:	ver->y=atoi(token);break;
			case 2:	ver->z=atoi(token);break;
			default:break;
		}
		token = strtok(NULL, ".");
	}
	free(str_tok);
	return 0;
}

//单纯数组解析
//包含startupParameters，otherFiles,assertFiles,binFiles,fileExtension
static int json_conf_array(struct json_object *array,char *data[])
{
	if (json_object_get_type(array) != json_type_array) 
		return -1;

	char *name,*version;	
	int count = json_object_array_length((const json_object *)array);
	if(count>MAX_ITEMS)
		count=MAX_ITEMS;
	for (int i = 0; i < count; i++) 
	{
		json_object *item = json_object_array_get_idx(array, i);
		const char *str=json_object_get_string(item);
		data[i]=strdup(str);
	}
	return count;
}


int json_conf_get_package_config(const char *config_path, struct AppPackageConfig *conf)
{
	struct json_object *root = json_object_from_file(config_path);
	if (root == NULL) {
		printf("errr\n");
		return -1;
	}

	struct json_object *object;
	if (json_object_object_get_ex(root, "appID", &object)) {
		const char *str=json_object_get_string(object);
		strncpy(conf->app_id,str,sizeof(conf->app_id));
	}

/*	if (json_object_object_get_ex(root, "appType", &object)) {
		const char *str=json_object_get_string(object);
		strncpy(conf->app_name,str,sizeof(conf->app_name));
	}*/

	if (json_object_object_get_ex(root, "appName", &object)) {
		const char *str=json_object_get_string(object);
		strncpy(conf->app_name,str,sizeof(conf->app_name));
	}

	if (json_object_object_get_ex(root, "organization", &object)) {
		const char *str=json_object_get_string(object);
		strncpy(conf->organization,str,sizeof(conf->organization));
	} 
	if (json_object_object_get_ex(root, "version", &object)) {
		json_conf_version(object,&conf->version);
	}
	if (json_object_object_get_ex(root, "appexecName", &object)) {
		const char *str=json_object_get_string(object);
		conf->appexec_name=strdup(str);
	}
	if (json_object_object_get_ex(root, "architecture", &object)) {
		const char *str=json_object_get_string(object);
		strncpy(conf->architecture,str,sizeof(conf->architecture));
	}
	if (json_object_object_get_ex(root, "section", &object)) {
		const char *str=json_object_get_string(object);
		strncpy(conf->section,str,sizeof(conf->section));
	}
	if (json_object_object_get_ex(root, "priority", &object)) {
		const char *str=json_object_get_string(object);
		strncpy(conf->priority,str,sizeof(conf->priority));
	}
	if (json_object_object_get_ex(root, "essential", &object)) {
		const char *str=json_object_get_string(object);
		strncpy(conf->essential,str,sizeof(conf->essential));
	}
	if (json_object_object_get_ex(root, "diskspace", &object)) {
		conf->diskspace=json_object_get_int((const json_object *)object);
	}
	if (json_object_object_get_ex(root, "description", &object)) {
		const char *str=json_object_get_string(object);
		conf->description=strdup(str);
	}
	if (json_object_object_get_ex(root, "signature", &object)) {
		const char *str=json_object_get_string(object);
		conf->signature=strdup(str);
	}
	if (json_object_object_get_ex(root, "icon", &object)) {
		const char *str=json_object_get_string(object);
		conf->icon=strdup(str);
	}

	/*if (json_object_object_get_ex(root, "startupFile", &object)) {
		char *str=json_object_get_string(object);
		strncpy(conf->app_name,str,sizeof(conf->app_name));
	}
	if (json_object_object_get_ex(root, "removeFile", &object)) {
		char *str=json_object_get_string(object);
		strncpy(conf->app_name,str,sizeof(conf->app_name));
	}*/


	if (json_object_object_get_ex(root, "author", &object)) {
		json_conf_author(object,conf->author,sizeof(conf->author),conf->contact,sizeof(conf->contact));
	}

	if (json_object_object_get_ex(root, "otherFiles", &object)) {
		conf->otherfile_count=json_conf_array(object,conf->otherfile);
	}
	if (json_object_object_get_ex(root, "fileExtension", &object)) {
		conf->extension_count=json_conf_array(object,conf->file_extension);
	}
	if (json_object_object_get_ex(root, "binFiles", &object)) {
		conf->bin_count=json_conf_array(object,conf->bin);
	}
	if (json_object_object_get_ex(root, "assertFiles", &object)) {
		conf->assert_count=json_conf_array(object,conf->assert);
	}

	json_object_put(root);
	return 0;
}

//获取启动脚本文件信息
int json_conf_get_startup(const char *config_path, struct ScriptInfo *script)
{
	struct json_object *root = json_object_from_file(config_path);
	if (root == NULL) {
		printf("errr\n");
		return -1;
	}

	struct json_object *object;
	if (json_object_object_get_ex(root, "startupParameters", &object)) {
		script->arg_count=json_conf_array(object,script->args);
	}

	if (json_object_object_get_ex(root, "environment", &object)) {
		script->env_var_count=json_conf_environment(object,script->env_type,script->env_vars);
	}



	json_object_put(root);
	return 0;
}
