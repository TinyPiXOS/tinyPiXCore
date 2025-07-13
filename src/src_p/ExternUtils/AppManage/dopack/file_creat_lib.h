#ifndef _SCRIPTLIB_H_
#define _SCRIPTLIB_H_


#define MAX_ITEMS 128  // 最大支持 128 项

struct ConfigInfo{
	const char *app_id;
	const char *app_name;
	const char *organization;
	const char *version;
	const char *appexec_name;
	const char *architecture;
	const char *section;
	const char *priority;
	const char *essential;
	const char *author;
	const char *provides;
	const char *description;
	const char *icon;
	const char *start;
	const char *remove;

	char *depend_lib[MAX_ITEMS]; 	// 依赖公共库
	int depend_lib_count;

	char *private_lib[MAX_ITEMS]; 	// 使用的私有库
	int private_lib_count;

	char *export_info[MAX_ITEMS];	//	
	int export_count;

	char *update[MAX_ITEMS];		// 使	
	int update_count;
};


//配置文件
void add_describe_info(struct ConfigInfo *config, const char *key,const char *value);
void add_depend_lib(struct ConfigInfo *config, const char *lib,const char *version);
void add_private_lib(struct ConfigInfo *config, const char *lib);
void add_export_info(struct ConfigInfo *config,const char *key ,const char *value);
void add_update_info(struct ConfigInfo *config,const char *key ,int value);




#endif
