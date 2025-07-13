#ifndef _UNPACK_H_
#define _UNPACK_H_

#ifdef __cplusplus
extern "C" {                //  告诉编译器下列代码要以C链接约定的模式进行链接
#endif

#include <stdlib.h>
#include "archive.h"
#include "archive_entry.h"
#include "conf_json.h"
#include "install.h"

//#define PACKAGE_TYPE_APP 	0X10
//#define PACKAGE_TYPE_APPS 	0X11
//#define PACKAGE_TYPE_LIB	0X20



struct UnpackEntry {
	struct archive *a;
	struct archive_entry *entry;
	int (*open)(struct UnpackEntry *,const char *);
	int (*close)(struct UnpackEntry *);
};

int extract_from_archive(struct archive *a, const char *sour_dir, const char *dest_dir);
int extract_archive_package_config(struct AppInstallInfo *app, json_object *root) ;
int extract_file_pack(const char *pack,const char *entry,char *unpack_path);		//
int extract_archive_file(const char *filename, const char *sour_dir,const char *dest_dir);
void create_directories(const char *path);
int Appm_Unpack(const char *archive_name,uint8_t type) ;
int appm_get_package_info(const char *filename,struct PackageConfigInfo *conf);
int appm_free_package_info(struct PackageConfigInfo *conf);
#ifdef __cplusplus
}
#endif

#endif
