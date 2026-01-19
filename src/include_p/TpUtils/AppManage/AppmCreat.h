#ifndef _PLAYER_H_
#define _PLAYER_H_

#include <stdio.h>
#include "archive.h"
#include "archive_entry.h"
#include "AppManage/AppmanageConf.h"
#include "AppManage/TpFileCreat.h"

#define PKGFILE_LIBRARY		"lib"
#define PKGFILE_SIGNATURE	"signature"
#define PKGFILE_STATIC		"assert"
#define PKGFILE_APP			" "
#define PKGFILE_CONFIG		"config"

#define MAX_LEN_PATH   1024
#define MAX_LEN_CONFIG	1024
struct ScriptInfo;



class TpAppmCreat{
public:
    TpAppmCreat(){}
    ~TpAppmCreat(){}
public:
    static int AddFileToArchive(struct archive *a, const char *file_path, const char *entry_name);
    static int CreatPackageFile(const char * path, const char * archive_name);
    static int CreatLibPackageConfig(const char *archive_name,struct LibPackageConfig *conf);
    static int appm_creat_apppackage_config(const char *archive_name,struct AppPackageConfig *conf);    //暂时不支持
    static int AnalysisDopackJson(const TpString& json_path,struct AppPackageConfig *conf,struct ScriptInfo *script);
};

#endif
