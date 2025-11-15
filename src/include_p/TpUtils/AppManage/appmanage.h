#ifndef _CPP_APP_MANEGE_H
#define _CPP_APP_MANAGE_H

#include <string>
#include <stdint.h>
#include <vector>
#include "install.h"
#include "unload.h"
#include "../inc/AppmanageConf.h"
#include "install_check.h"

#ifdef __cplusplus
extern "C" {                //  告诉编译器下列代码要以C链接约定的模式进行链接
#endif
int appm_check_arch(struct PackageConfigInfo *conf);
int appm_check_space(struct PackageConfigInfo *conf);
int appm_check_version(struct PackageConfigInfo *conf);
int appm_get_app_version(const char *uuid,struct TpVersion *version);
int appm_install_pik(const char *path_pik,TypePackage type,struct AppPackageConfig *conf,struct PackageUserParam *user);
int appm_install_package(const char *path_pack,struct PackageConfigInfo *conf,struct PackageUserParam *user);
int appm_install_get_schedule(struct PackageUserParam *user);
int appm_app_unload(TpAppID uuid);
int appm_get_package_info(const char *filename,struct PackageConfigInfo *conf);
int appm_free_package_info(struct PackageConfigInfo *conf);

#ifdef __cplusplus
}
#endif


#define TpString	std::string



#endif
