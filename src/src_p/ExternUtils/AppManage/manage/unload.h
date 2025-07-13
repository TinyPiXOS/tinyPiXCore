#ifndef _UNLOAD_H_
#define _UNLOAD_H_

#ifdef __cplusplus
extern "C" {                //  告诉编译器下列代码要以C链接约定的模式进行链接
#endif

#include <stdio.h>
#include "../inc/appmanage_conf.h"

int appm_app_unload(tpAppID uuid);

#ifdef __cplusplus
}
#endif

#endif