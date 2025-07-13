#ifndef _JSON_CONF_H_
#define _JSON_CONF_H_

#ifdef __cplusplus
extern "C" {
#endif

#include "json.h"

int json_conf_get_package_config(const char *config_path, struct AppPackageConfig *conf);
int json_conf_get_startup(const char *config_path, struct ScriptInfo *script);


#ifdef __cplusplus
}
#endif

#endif