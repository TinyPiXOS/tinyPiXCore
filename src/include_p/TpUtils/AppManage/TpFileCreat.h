#ifndef _FILE_CREAT_H_
#define _FILE_CREAT_H_

#include <stdint.h>
#include "AppManage/AppmanageConf.h"
#include "AppScript.h"
#include "TpString.h"
#include "TpVector.h"

#define PATH_SYSTEM_LIB "/System/lib"

// 启动脚本配置结构体
struct ScriptInfo
{
    TpVector<TpString> env_type; // 环境变量
    TpVector<TpString> env_vars; // 环境变量值

    TpVector<TpString> dependencies; // 用户自己的依赖库位置，暂时不使用

    TpVector<TpString> args; // 启动参数

    TpString log_file;    // 日志文件
    TpString config_file; // 配置文件
    TpString exec_path;   // 可执行文件
    //	char app_name[64];
    //	char *script_path; 				// 启动文件输出路径
};

class TpFileCreat
{
public:
    TpFileCreat() {}
    ~TpFileCreat() {}

    // 生成原始打包文件(未压缩的源文件)
    // config:配置信息
    // path:源文件生成的路径
    // type:安装包类型，app，sapp，lib，default
    static int appm_generate_package_source(struct AppPackageConfig *config, const TpString &path, TypePackage type);

    // output_file:启动文件生成位置，通常在安装包根目录
    static int appmGenerateStartupScript(struct ScriptInfo *config, const TpString &outputFile);

    static const char *get_architecture_string(TpEnumArchType arch);
    static void init_script_config(struct ScriptInfo *config);
    static void add_env_var(struct ScriptInfo *config, const TpString &key, const TpString &value);
    static void add_dependency(struct ScriptInfo *config, const char *lib);
    // 添加启动参数
    static void add_arg(struct ScriptInfo *config, const char *arg);
    // 设置日志文件路径
    static void set_log_file(struct ScriptInfo *config, const char *log_file);
    // 设置配置文件路径
    static void set_config_file(struct ScriptInfo *config, const char *config_file);
    static void set_exec_path(struct ScriptInfo *config, const char *exec_path);

    static int file_startsh_creat(char *path);

    // 生成库安装包中的配置文件并打包
    static int file_config_creat_lib(struct archive *a, const char *path, struct LibPackageConfig *conf);
};

#endif
