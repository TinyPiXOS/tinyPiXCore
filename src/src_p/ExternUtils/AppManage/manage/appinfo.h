#ifndef _APPM_INFO_H_
#define _APPM_INFO_H_

#ifdef __cplusplus
extern "C" {                //  告诉编译器下列代码要以C链接约定的模式进行链接
#endif

#include <stdbool.h>
#include "appmanage_conf.h"

void appm_free(void *ptr);
char *appm_info_get_install_json(TpAppID *appid);
int appm_info_get_appid(const char *uuid,TpAppID *appid);
bool appm_info_is_install(TpAppID *appid);
char *appm_info_get_appuserid(TpAppID *appid);

#ifdef __cplusplus
}
#endif

#endif