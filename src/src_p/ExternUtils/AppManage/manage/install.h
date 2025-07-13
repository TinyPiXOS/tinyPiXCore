#ifndef _INSTALL_H_
#define _INSTALL_H_
#include <stdint.h>
#include <stdlib.h>
#include <pthread.h>
#include "string.h"
#include "../inc/appmanage_conf.h"

#ifdef __cplusplus
extern "C" {                //  告诉编译器下列代码要以C链接约定的模式进行链接
#endif


#define CONFIG_FILE_NAME_APPLIB		"APPLIB"	
#define CONFIG_FILE_NAME_STARTSH	"START_SH"
#define CONFIG_FILE_NAME_REMOVESH 	"REMOVE_SH"


//文件安装目录
typedef enum {
	INSTALL_PATH_TYPE_NONE=0,
	INSTALL_PATH_TYPE_ROOT=1,
	INSTALL_PATH_TYPE_RES=2,
	INSTALL_PATH_TYPE_BIN=3
}AppInstallPathType;

typedef enum{
	CONFIG_KEYTYPE_PATH,	//路径配置
	CONFIG_KEYTYPE_TERM	//普通配置项
}ConfigFileKeyType;

//app uuid
struct AppUuid{
	uint32_t time_low;
	uint16_t time_mid;
	uint16_t time_high_and_version;
	uint8_t clock_seq_hi_and_reserved;
	uint8_t clock_seq_low;
	uint8_t node[6];
};

//app的 信息
struct tpAppInfo{
	char *uuid;			//uuid
	TypePackage type;	//安装包类型
	const char *path_pik;	//原始包名
	char *pikname;		//安装包名字(不含有后缀)
	char *config_file;
	char *icon;		//图标路径
};

struct AppInstallInfo{
	struct tpAppInfo *app;
	//char *uuid;			//uuid
	//TypePackage type;	//安装包类型
	//char *pikname;		//安装包名字(不含有后缀)
	int (*install_must)(struct AppInstallInfo *app ,const char *s,const char *d); //必要文件安装程序，普通单个文件安装也可以使用此接口
	int (*install_other)(struct AppInstallInfo *app,const char *path,const char *s,const char *d);//其他文件安装程序，这里面会根据config文件一个个安装
	//以下是从pik提取使用
	//char *path_pik;	//原始包名
	struct archive *a;	//archive
};

//安装进度
struct InstallSchedule{
	pthread_rwlock_t rwlock;  // 读写锁
	uint8_t schedule;
//	int (*write_install_schedule)(struct InstallSchedule *,uint8_t);
//	int (*read_install_schedule)(struct InstallSchedule *);
};

//应用信息

//路径==============================================================
//用户应用信息文件目录(json位置)
#define APP_JSON_PATH "/System/conf/app"	//
//系统应用。。
#define APPS_JSON_PATH "/System/conf/app_s"

//用户应用安装根目录
#define APP_INSTALL_PATH "/System/app"
//系统应用。。
#define APPS_INSTALL_PATH "/System/app_s"

//系统库安装目录
#define LIBS_INSTALL_PATH "/System/lib"

#define APP_TEMP 	"temp"	

//目录
#define APP_LIBRARY			"applib"	//前面拼uuid
#define APP_INSTALL_RES		"res"		//资源文件，应用的各种文件，包括assert也在这下面,/System/app/<uuid>/res/
#define APP_INSTALL_BIN		"bin"		//bin文件，可执行文件放在这里，/System/app/<uuid>/bin/
#define APP_INSTALL_CONFIG	"config"
#define APP_INSTALL_DATA	"data"
#define APP_INSTALL_TEMP 	"temp"	

#define APP_APPNAME	" "		//非固定文件

//install.conf文件完整路径
#define APP_INSTALL_CONF_PATH	"/System/app/install.conf"



//文件===================================================
#define APP_CONFIG 	"config.conf"	//配置文件
#define APP_METAINF "META-INF"	//数字签名文件
#define APP_REMOVE_SH "remove.sh"
#define APP_START_SH "start.sh"

//安装包信息
#define PIK_CONFIG 	"config"	//配置文件


//=======================================================
#define USERGROUP_USER_APP		"tinyPixUserAppGroup"		//应用所属的用户组

#define TMP_FILE_PATH			"/System/tmp"

#define TMP_CACHE_FILE_PATH		"/System/data/cache"	

struct InstallFunction{


};

struct InstallSchedule *creat_install_schedule();
int delete_install_schedule(struct InstallSchedule *sch);
//解包的形式安装文件到目标位置
int install_file_extract(struct AppInstallInfo *app,const char *path,const char *file_s,const char *file_d);
int install_file_copy(struct AppInstallInfo *app,const char *path,const char *file_s,const char *file_d);
AppInstallPathType install_config_export_type(char *line);

//上层调用接口
int Appm_Install_Unpack(struct tpAppInfo *app,struct PackageUserParam *user);
int appm_install_pik(const char *path_pik,TypePackage type,struct AppPackageConfig *conf,struct PackageUserParam *user);

int app_install_test(const char *pik_name);
int appm_install_package(const char *path_pack,struct PackageConfigInfo *conf,struct PackageUserParam *user);

int appm_install_get_schedule(struct PackageUserParam *user);
const char *appm_install_get_icon(struct PackageConfigInfo *conf);

#ifdef __cplusplus
}
#endif


#endif
