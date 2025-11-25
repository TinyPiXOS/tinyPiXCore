// 从JSON文件解析安装信息

#include <stdio.h>
#include <string.h>
#include "AppManage/AppmanageConf.h"
#include "TpFileCreat.h"
#include "JsonConf.h"

TpString GetStringValue(const TpJsonObject &obj, const TpString &fieldName, bool required = false)
{
    if (!obj.contains(fieldName))
    {
        if (required)
        {
            throw JsonConfigException("Missing required field: " + fieldName);
        }
        return TpString();
    }

    TpJsonValue value = obj.value(fieldName);
    if (!value.isString())
    {
        throw JsonConfigException(fieldName + " is not a string");
    }

    return value.toString();
}

int GetIntValue(const TpJsonObject &obj, const TpString &fieldName, int defaultValue = 0)
{
    if (!obj.contains(fieldName))
    {
        return defaultValue;
    }

    TpJsonValue value = obj.value(fieldName);
    if (value.isInt() || value.isUint())
    {
        return value.toInt();
    }

    return defaultValue;
}

TpVersion ParseVersion(const TpJsonValue &versionVal)
{
    if (!versionVal.isString())
    {
        throw JsonConfigException("Version is not a string");
    }

    TpString versionStr = versionVal.toString();
    TpList<TpString> parts = versionStr.split('.');

    TpVersion version = {0, 0, 0};
    if (parts.size() >= 1 && !parts.at(0).empty())
    {
        version.x = parts.at(0).toInt();
    }
    if (parts.size() >= 2 && !parts.at(1).empty())
    {
        version.y = parts.at(1).toInt();
    }
    if (parts.size() >= 3 && !parts.at(2).empty())
    {
        version.z = parts.at(2).toInt();
    }

    return version;
}

TpVector<TpString> ParseStringArray(const TpJsonArray &array)
{
    TpVector<TpString> result;

    if (array.isEmpty())
    {
        return result;
    }

    int count = array.count();
    for (int i = 0; i < count; i++)
    {
        TpJsonValue item = array.at(i);
        if (!item.isString())
        {
            throw JsonConfigException("Array item is not a string");
        }
        result.append(item.toString());
    }

    return result;
}

void ParseEnvironment(const TpJsonArray &envArray, TpVector<TpString> &types, TpVector<TpString> &values)
{
    if (envArray.isEmpty())
        return;

    int count = envArray.count();
    for (int i = 0; i < count; i++)
    {
        TpJsonValue item = envArray.at(i);
        if (!item.isObject())
        {
            throw JsonConfigException("Environment item is not an object");
        }

        TpJsonObject envObj = item.toObject();
        TpString typeStr = GetStringValue(envObj, "Type");
        TpString valueStr = GetStringValue(envObj, "Value");

        types.append(typeStr);
        values.append(valueStr);
    }
}

void ParseAuthor(const TpJsonObject &authorObj, TpString &author, TpString &contact)
{
    author = GetStringValue(authorObj, "Name");
    contact = GetStringValue(authorObj, "Email");
}

// ==================== 主解析函数实现 ====================
void JsonConfigParser::GetPackageConfig(const TpString &configPath, AppPackageConfig &config)
{
    // 使用TpJsonDocument解析JSON文件
    TpJsonDocument doc = TpJsonDocument::fromJson(configPath);
    if (doc.doc_.HasParseError())
    {
        throw JsonConfigException("Failed to parse JSON document: " + configPath);
    }

    TpJsonObject root = doc.object();
    if (root.isEmpty())
    {
        throw JsonConfigException("Empty JSON document: " + configPath);
    }

	std::cout<< "json:\n" << doc.toJson() << std::endl;
    // 解析基本字段
    config.appID = GetStringValue(root, "AppID", true);
    config.appName = GetStringValue(root, "AppName", true);
    config.organization = GetStringValue(root, "Organization");
    config.appexecName = GetStringValue(root, "AppexecName");
    config.architecture = GetStringValue(root, "Architecture");
    config.section = GetStringValue(root, "Section");
    config.priority = GetStringValue(root, "Priority");
    config.essential = GetStringValue(root, "Essential");
    config.diskspace = GetIntValue(root, "Diskspace");
    config.description = GetStringValue(root, "Description");
    config.signature = GetStringValue(root, "Signature");
    config.icon = GetStringValue(root, "Icon");

    // 解析版本号
    if (root.contains("Version"))
    {
        TpJsonValue versionVal = root.value("Version");
        config.version = ParseVersion(versionVal);
    }

    // 解析作者信息
    if (root.contains("Author"))
    {
        TpJsonValue authorVal = root.value("Author");
        if (authorVal.isObject())
        {
            TpJsonObject authorObj = authorVal.toObject();
            ParseAuthor(authorObj, config.author, config.contact);
        }
    }

    // 解析数组字段
    if (root.contains("OtherFiles"))
    {
        TpJsonValue arrayVal = root.value("OtherFiles");
        if (arrayVal.isArray())
        {
            config.otherFiles = ParseStringArray(arrayVal.toArray());
        }
    }

    if (root.contains("FileExtension"))
    {
        TpJsonValue arrayVal = root.value("FileExtension");
        if (arrayVal.isArray())
        {
            config.fileExtension = ParseStringArray(arrayVal.toArray());
        }
    }

    if (root.contains("BinFiles"))
    {
        TpJsonValue arrayVal = root.value("BinFiles");
        if (arrayVal.isArray())
        {
            config.binFiles = ParseStringArray(arrayVal.toArray());
        }
    }

    if (root.contains("AssertFiles"))
    {
        TpJsonValue arrayVal = root.value("AssertFiles");
        if (arrayVal.isArray())
        {
            config.assertFiles = ParseStringArray(arrayVal.toArray());
        }
    }
}

void JsonConfigParser::GetStartupInfo(const TpString &configPath, ScriptInfo &scriptInfo)
{
    // 使用TpJsonDocument解析JSON文件
    TpJsonDocument doc = TpJsonDocument::fromJson(configPath);
    if (doc.doc_.HasParseError())
    {
        throw JsonConfigException("Failed to parse JSON document: " + configPath);
    }

    TpJsonObject root = doc.object();
    if (root.isEmpty())
    {
        throw JsonConfigException("Empty JSON document: " + configPath);
    }

    // 解析启动参数
    if (root.contains("StartupParameters"))
    {
        TpJsonValue arrayVal = root.value("StartupParameters");
        if (arrayVal.isArray())
        {
            scriptInfo.args = ParseStringArray(arrayVal.toArray());
        }
    }

    // 解析环境变量
    if (root.contains("Environment"))
    {
        TpJsonValue arrayVal = root.value("Environment");
        if (arrayVal.isArray())
        {
            ParseEnvironment(arrayVal.toArray(), scriptInfo.env_type, scriptInfo.env_vars);
        }
    }
}
