#ifndef _CONF_JSON_H_
#define _CONF_JSON_H_

#include "AppManage/AppmanageConf.h"
#include "json.h"

enum PackageExportType
{
    EXPORT_NONE = 0,
    EXPORT_LIBS = 1,
    EXPORT_DEPEND = 2,
    EXPORT_MUST = 3
};

class ConfigJsonParser
{
public:
    // 配置文件中的export行目解析
    static int config_export_analysis_json(char *line, json_object *export_obj);
    // 配置文件普通行目解析
    static int config_keyvalue_analysis_json(char *line, json_object *export_obj);
    // 从未加密的json文件中查找key的值
    static int findKeyFromFile(const TpString &file_path, const TpString &key, TpString &value);
    // 配置信息写入json
    static int configAddToJson(PackageExportType type, json_object *exportObj, const TpString &value, const TpString &key);
    // 安全删除应用从install文件
    // static int del_appuuid_install_safe(TpAppID uuid, const TpString &installPath);
    // 安全新增应用到install文件
    // static int add_appuuid_install_safe(TpAppID uuid, const TpString &installPath);
    // 不加密写入json对象到文件
    static int writeJsonObjectFile(json_object *root, const TpString &filePath);
    // 加密写入json文件
    static int writeJsonObjectFileEncryption(json_object *root, const TpString &filePath);
    // 从加密的json文件读取字符串json
    static char *readJsonStrFileEncryption(const TpString &filePath);
};

#endif
