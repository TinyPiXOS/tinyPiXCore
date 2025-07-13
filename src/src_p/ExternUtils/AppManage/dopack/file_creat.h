#ifndef _FILE_CREAT_H_
#define _FILE_CREAT_H_

#ifdef __cplusplus
extern "C" {				//告诉编译器下列代码要以C链接约定的模式进行链接
#endif

#include <stdint.h>
#include "appm_creat.h"
#include "../inc/appmanage_conf.h"
#include "app_script.h"



#define PATH_SYSTEM_LIB	"/System/lib"

// 启动脚本配置结构体
struct ScriptInfo{
	char *env_type[MAX_ITEMS];		// 环境变量
	char *env_vars[MAX_ITEMS]; 		// 环境变量值
	int env_var_count;

	char *dependencies[MAX_ITEMS]; 	// 用户自己的依赖库位置，暂时不使用
	int dep_count;

	char *args[MAX_ITEMS];			// 启动参数
	int arg_count;

	char *log_file; 				// 日志文件
	char *config_file; 				// 配置文件
	char exec_path[64]; 			// 可执行文件
//	char app_name[64];
//	char *script_path; 				// 启动文件输出路径
} ;





int appm_generate_package_source(struct AppPackageConfig *config,char *path,TypePackage type);
int appm_generate_startup_script(struct ScriptInfo *config, const char *output_file);


const char *get_architecture_string(tpEnumArchType arch);
void init_script_config(struct ScriptInfo *config);
void add_env_var(struct ScriptInfo *config, const char *key, const char *value);
void add_dependency(struct ScriptInfo *config, const char *lib);
void add_arg(struct ScriptInfo *config, const char *arg);
void set_log_file(struct ScriptInfo *config, const char *log_file);
void set_config_file(struct ScriptInfo *config, const char *config_file);
void set_exec_path(struct ScriptInfo *config, const char *exec_path);

int file_startsh_creat(char *path);

int file_config_creat_lib(struct archive *a,const char *path,struct LibPackageConfig *conf);

#ifdef __cplusplus
}
#endif


#endif
