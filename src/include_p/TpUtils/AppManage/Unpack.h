#ifndef _UNPACK_H_
#define _UNPACK_H_

#include <stdlib.h>
#include "archive.h"
#include "archive_entry.h"
#include "ConfJson.h"
#include "Install.h"
#include "TpJsonObject.h"

// #define PACKAGE_TYPE_APP 	0X10
// #define PACKAGE_TYPE_APPS 	0X11
// #define PACKAGE_TYPE_LIB	0X20

struct UnpackEntry
{
    struct archive *a;
    struct archive_entry *entry;
    int (*open)(struct UnpackEntry *, const char *);
    int (*close)(struct UnpackEntry *);
};



class TpAppmUnpack{
public:
    TpAppmUnpack(){}
    ~TpAppmUnpack(){}
public:
    static int extract_from_archive(struct archive *a, const char *sour_dir, const char *dest_dir);
    static int extract_archive_package_config(struct AppInstallInfo *app,  TpJsonObject root);
    static int extract_file_pack(const char *pack, const char *entry, char *unpack_path);
    static int extract_archive_file(const char *filename, const char *sour_dir, const char *dest_dir);
    static int appm_get_package_info(const char *filename, struct PackageConfigInfo *conf);
    static int appm_free_package_info(struct PackageConfigInfo *conf);
    static int Appm_Unpack(const char *archive_name, uint8_t type);
    static void create_directories(const char *path);
};


#endif
