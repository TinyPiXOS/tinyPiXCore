#ifndef _PLAYER_H_
#define _PLAYER_H_

#ifdef __cplusplus
extern "C" {				//告诉编译器下列代码要以C链接约定的模式进行链接
#endif

#include <stdio.h>
#include "archive.h"
#include "archive_entry.h"
#include "../inc/appmanage_conf.h"
#include "file_creat.h"

#define PKGFILE_LIBRARY		"lib"
#define PKGFILE_SIGNATURE	"signature"
#define PKGFILE_STATIC		"assert"
#define PKGFILE_APP			" "
#define PKGFILE_CONFIG		"config"

#define MAX_LEN_PATH   1024
#define MAX_LEN_CONFIG	1024


int appm_creat_package_path(const char * path, const char * archive_name);
int appm_creat_libpackage_config(const char *archive_name,struct LibPackageConfig *conf);
int appm_creat_apppackage_config(const char *archive_name,struct AppPackageConfig *conf);
int add_file_to_archive(struct archive *a, const char *file_path, const char *entry_name);
int appm_analysis_dopack_json(const char *json_path,struct AppPackageConfig *conf,struct ScriptInfo *script);

#ifdef __cplusplus
}
#endif

#endif
