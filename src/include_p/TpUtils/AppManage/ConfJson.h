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
    static int find_key_from_file(const char *file_path, const char *key, char *value);
    // 配置信息写入json
    static int config_add_to_json(PackageExportType type, json_object *export_obj, const char *value, const char *key);
    // 安全删除应用从install文件
    static int del_appuuid_install_safe(TpAppID uuid, const char *install_path);
    // 安全新增应用到install文件
    static int add_appuuid_install_safe(TpAppID uuid, const char *install_path);
    // 不加密写入json对象到文件
    static int write_json_object_file(json_object *root, const char *file_path);
    // 加密写入json文件
    static int write_json_object_file_encryption(json_object *root, const char *file_path);
    // 从加密的json文件读取字符串json
    static char *read_json_string_file_encryption(const char *file_path);
};

#endif
