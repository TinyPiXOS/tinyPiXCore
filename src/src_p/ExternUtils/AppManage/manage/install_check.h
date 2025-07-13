#ifndef _ENVIR_CHECK_H_
#define _ENVIR_CHECK_H_

#include <stdlib.h>
#include "../inc/appmanage_conf.h"
#include "conf_json.h"


#ifdef __cplusplus
extern "C" {                //  告诉编译器下列代码要以C链接约定的模式进行链接
#endif




struct ArchMapping{
	const char *architecture;
	const char *standard;
};

int appm_check_arch(struct PackageConfigInfo *conf);
int appm_check_space(struct PackageConfigInfo *conf);
int appm_check_version(struct PackageConfigInfo *conf);
int appm_get_app_version(const char *uuid,struct tpVersion *version);
int Appm_Get_Arch_String();
int extract_config_info(const char *file_config,struct PackageConfigInfo *conf);
int free_AppPackageConfig(struct AppPackageConfig *conf);
int free_LibPackageConfig(struct LibPackageConfig *conf);
PackageExportType get_config_export_key_type(const char *key);
int appm_get_app_is_install(struct PackageConfigInfo *conf);


#ifdef __cplusplus
}
#endif

#endif