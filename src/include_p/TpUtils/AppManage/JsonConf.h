#ifndef JSON_CONFIG_PARSER_H
#define JSON_CONFIG_PARSER_H

#include "AppManage/appmanage_conf.h"
#include "TpFileCreat.h"
#include <TpJsonDocument.h>
#include <TpJsonObject.h>
#include <TpJsonArray.h>
#include <TpJsonValue.h>
#include <TpString.h>
#include <exception>
#include <stdexcept>
#include <memory>
#include <TpVector.h>

class JsonConfigException : public std::exception
{
private:
    TpString m_message;

public:
    JsonConfigException(const TpString &message) : m_message(message) {}
    const char *what() const noexcept override { return m_message.c_str(); }
};

// ==================== 修改部分：配置解析器类声明 ====================
class JsonConfigParser
{
public:
    // 主解析函数
    static void GetPackageConfig(const TpString &configPath, AppPackageConfig &conf);
    static void GetStartupInfo(const TpString &configPath, ScriptInfo &scriptInfo);
};

// int json_conf_get_package_config(const char *config_path, struct AppPackageConfig *conf);
// int json_conf_get_startup(const char *config_path, struct ScriptInfo *script);

#endif