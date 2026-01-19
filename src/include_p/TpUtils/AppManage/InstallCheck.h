#ifndef _ENVIR_CHECK_H_
#define _ENVIR_CHECK_H_

#include <stdlib.h>
#include "AppManage/AppmanageConf.h"
#include "ConfJson.h"


struct ArchMapping
{
    const char *architecture;
    const char *standard;
};

class TpAppmInstallCheck{
public:
    TpAppmInstallCheck(){}
    ~TpAppmInstallCheck(){}
public:
    static int appm_check_arch(struct PackageConfigInfo *conf);
    static int appm_check_space(struct PackageConfigInfo *conf);
    static int appm_check_version(struct PackageConfigInfo *conf);
    static int appm_get_app_version(const char *uuid, struct TpVersion *version);
    static int appm_get_app_is_install(struct PackageConfigInfo *conf);
    static int extract_config_info(const char *file_config, struct PackageConfigInfo *conf);
    static int free_AppPackageConfig(struct AppPackageConfig *conf);
    static PackageExportType get_config_export_key_type(const char *key);

    static int ExtractPackageJsonInfo(const char *file_config, struct PackageConfigInfo *conf);
};

#endif