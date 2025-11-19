#include "TpConfig.h"
#include "TpFile.h"
#include "TpMap.h"
#include <algorithm>

// 内部数据结构定义
struct TpConfigData
{
    TpConfig::Status status = TpConfig::NoError;
    TpString fileName = "";
    bool atomicSync = true; // 默认启用原子同步

    // 存储配置数据：Section -> (Key -> Value)
    TpMap<TpString, TpMap<TpString, TpString>> data;

    // 组栈，用于支持分组操作
    TpVector<TpString> groupStack;

    // 当前组前缀
    TpString currentGroupPrefix() const
    {
        if (groupStack.empty())
            return "";

        TpString prefix;
        for (const auto &group : groupStack)
        {
            if (!prefix.empty())
                prefix += "/";
            prefix += group;
        }
        return prefix;
    }
};

// 转义值中的特殊字符[7](@ref)
TpString escapeValue(const TpString &value)
{
    TpString result;
    result.reserve(value.length());

    for (size_t i = 0; i < value.length(); ++i)
    {
        char c = value[i];
        switch (c)
        {
        case '\n':
            result += "\\n";
            break;
        case '\r':
            result += "\\r";
            break;
        case '\t':
            result += "\\t";
            break;
        case '\\':
            result += "\\\\";
            break;
        case '\"':
            result += "\\\"";
            break;
        case ';':
            result += "\\;";
            break;
        case '#':
            result += "\\#";
            break;
        case '=':
            result += "\\=";
            break;
        default:
            result += c;
            break;
        }
    }
    return result;
}

// 反转义值中的特殊字符[7](@ref)
TpString unescapeValue(const TpString &value)
{
    TpString result;
    result.reserve(value.length());

    for (size_t i = 0; i < value.length(); ++i)
    {
        if (value[i] == '\\' && i + 1 < value.length())
        {
            switch (value[i + 1])
            {
            case 'n':
                result += '\n';
                break;
            case 'r':
                result += '\r';
                break;
            case 't':
                result += '\t';
                break;
            case '\\':
                result += '\\';
                break;
            case '\"':
                result += '\"';
                break;
            case ';':
                result += ';';
                break;
            case '#':
                result += '#';
                break;
            case '=':
                result += '=';
                break;
            default:
                result += value[i + 1];
                break; // 未知转义序列，按原样处理
            }
            i++; // 跳过下一个字符
        }
        else
        {
            result += value[i];
        }
    }
    return result;
}

TpConfig::TpConfig()
{
    TpConfigData *configData = new TpConfigData();
    data_ = configData;
}

TpConfig::TpConfig(const TpString &fileName)
{
    TpConfigData *configData = new TpConfigData();
    data_ = configData;
    load(fileName);
}

TpConfig::~TpConfig()
{
    TpConfigData *configData = static_cast<TpConfigData *>(data_);
    if (configData)
    {
        delete configData;
        data_ = nullptr;
    }
}

bool TpConfig::load(const TpString &fileName)
{
    TpConfigData *configData = static_cast<TpConfigData *>(data_);
    configData->status = NoError;
    configData->fileName = fileName;
    configData->data.clear();
    configData->groupStack.clear();

    // 使用TpFile检查文件是否存在
    if (!TpFile::exists(fileName))
    {
        configData->status = AccessError;
        return false;
    }

    // 使用TpFile对象打开文件
    TpFile file(fileName);
    if (!file.open(TpFile::ReadOnly))
    {
        configData->status = AccessError;
        return false;
    }

    // 读取文件全部内容
    TpString content = file.readAll();
    file.close();

    // 检测并跳过UTF-8 BOM（0xEF, 0xBB, 0xBF）
    if (content.size() >= 3 &&
        static_cast<unsigned char>(content[0]) == 0xEF &&
        static_cast<unsigned char>(content[1]) == 0xBB &&
        static_cast<unsigned char>(content[2]) == 0xBF)
    {
        content = content.mid(3);
    }

    // 按行分割内容
    TpVector<TpString> lines;
    size_t start = 0;
    size_t end = 0;

    while ((end = content.find('\n', start)) != TpString::npos)
    {
        TpString line = content.mid(start, end - start);
        line = line.simplified();
        line = line.replace(" ", "");

        // 移除可能的回车符
        if (!line.empty() && line.back() == '\r')
            line.pop_back();

        lines.push_back(line);
        start = end + 1;
    }
    // 添加最后一行
    if (start < content.size())
        lines.push_back(content.mid(start));

    // 解析INI内容
    TpString currentSection;
    int lineNumber = 0;

    for (const TpString &line : lines)
    {
        lineNumber++;
        TpString trimmedLine = line.trimmed();

        // 跳过空行和注释行
        if (trimmedLine.empty() || trimmedLine[0] == ';' || trimmedLine[0] == '#')
            continue;

        // 处理节声明
        if (trimmedLine[0] == '[' && trimmedLine.back() == ']')
        {
            currentSection = trimmedLine.mid(1, trimmedLine.length() - 2).trimmed();
            continue;
        }

        // 处理键值对
        size_t equalsPos = trimmedLine.find('=');
        if (equalsPos != TpString::npos)
        {
            TpString key = trimmedLine.mid(0, equalsPos).trimmed();
            TpString value = trimmedLine.mid(equalsPos + 1).trimmed();

            if (key.empty())
            {
                configData->status = FormatError;
                continue;
            }

            // 处理引号包围的值
            if (value.size() >= 2 &&
                ((value[0] == '"' && value.back() == '"') ||
                 (value[0] == '\'' && value.back() == '\'')))
            {
                value = value.mid(1, value.size() - 2);
            }

            // 使用当前节（如果没有节，则使用空字符串作为全局节）
            TpString effectiveSection = currentSection.empty() ? "" : currentSection;
            configData->data[effectiveSection][key] = value;
        }
        else
        {
            // 没有等号的行，视为格式错误
            configData->status = FormatError;
        }
    }

    return configData->status == NoError;
}

TpString TpConfig::fileName() const
{
    TpConfigData *configData = static_cast<TpConfigData *>(data_);
    return configData->fileName;
}

void TpConfig::clear()
{
    TpConfigData *configData = static_cast<TpConfigData *>(data_);
    configData->data.clear();
    configData->groupStack.clear();
}

void TpConfig::sync()
{
    TpConfigData *configData = static_cast<TpConfigData *>(data_);

    if (configData->fileName.empty())
        return;

    TpString outputFileName = configData->fileName;
    if (configData->atomicSync)
    {
        // 原子写入：先写入临时文件，然后重命名
        outputFileName += ".tmp";
    }

    // 使用TpFile进行文件写入
    TpFile file(outputFileName);
    if (!file.open(TpFile::WriteOnly)) // WriteOnly模式会覆盖原有文件
    {
        configData->status = AccessError;
        return;
    }

    // 构建INI格式内容
    TpString content;

    // 首先写入全局节（空节）的键值对
    if (configData->data.find("") != configData->data.end())
    {
        for (const auto &keyValue : configData->data.at(""))
        {
            TpString value = keyValue.second;
            // 对包含特殊字符的值添加引号
            if (value.contains(" ") || value.contains(";") || value.contains("#") || value.contains("="))
            {
                value = "\"" + value + "\"";
            }
            content += keyValue.first + "=" + value + "\n";
        }
        content += "\n";
    }

    // 写入其他节
    for (const auto &sectionPair : configData->data)
    {
        if (sectionPair.first.empty()) // 跳过全局节
            continue;

        content += "[" + sectionPair.first + "]\n";

        for (const auto &keyValue : sectionPair.second)
        {
            TpString value = keyValue.second;
            // 对包含特殊字符的值添加引号
            if (value.contains(" ") || value.contains(";") || value.contains("#") || value.contains("="))
            {
                value = "\"" + value + "\"";
            }
            content += keyValue.first + "=" + value + "\n";
        }
        content += "\n";
    }

    // 写入内容到文件
    uint64_t bytesWritten = file.write(content);
    if (bytesWritten != content.size())
    {
        configData->status = AccessError;
        file.close();
        return;
    }

    file.flush();
    file.close();

    // 原子同步处理
    if (configData->atomicSync)
    {
        // 删除原文件
        if (!TpFile::remove(configData->fileName))
        {
            configData->status = AccessError;
            return;
        }

        // 重命名临时文件
        if (!TpFile::rename(outputFileName, configData->fileName))
        {
            configData->status = AccessError;
            return;
        }
    }

    configData->status = NoError;
}

TpConfig::Status TpConfig::status() const
{
    TpConfigData *configData = static_cast<TpConfigData *>(data_);
    return configData->status;
}

void TpConfig::beginGroup(const TpString &prefix)
{
    TpConfigData *configData = static_cast<TpConfigData *>(data_);
    configData->groupStack.push_back(prefix);
}

void TpConfig::endGroup()
{
    TpConfigData *configData = static_cast<TpConfigData *>(data_);
    if (!configData->groupStack.empty())
    {
        configData->groupStack.pop_back();
    }
}

TpString TpConfig::group() const
{
    TpConfigData *configData = static_cast<TpConfigData *>(data_);
    return configData->currentGroupPrefix();
}

TpVector<TpString> TpConfig::allKeys() const
{
    TpConfigData *configData = static_cast<TpConfigData *>(data_);
    TpVector<TpString> keys;
    TpString prefix = configData->currentGroupPrefix();

    for (const auto &sectionPair : configData->data)
    {
        for (const auto &keyValuePair : sectionPair.second)
        {
            if (prefix.empty() || sectionPair.first.startsWith(prefix))
            {
                TpString fullKey = sectionPair.first;
                if (!prefix.empty())
                {
                    fullKey = fullKey.mid(prefix.length() + 1);
                }
                fullKey += "/" + keyValuePair.first;
                keys.push_back(fullKey);
            }
        }
    }

    return keys;
}

TpVector<TpString> TpConfig::childKeys() const
{
    TpConfigData *configData = static_cast<TpConfigData *>(data_);
    TpVector<TpString> keys;
    TpString currentGroup = configData->currentGroupPrefix();

    if (configData->data.find(currentGroup) != configData->data.end())
    {
        for (const auto &keyValuePair : configData->data.at(currentGroup))
        {
            keys.push_back(keyValuePair.first);
        }
    }

    return keys;
}

TpVector<TpString> TpConfig::childGroups() const
{
    TpConfigData *configData = static_cast<TpConfigData *>(data_);
    TpVector<TpString> groups;
    TpString currentGroup = configData->currentGroupPrefix();
    TpString prefix = currentGroup.empty() ? "" : currentGroup + "/";

    for (const auto &sectionPair : configData->data)
    {
        if (sectionPair.first.startsWith(prefix) &&
            sectionPair.first != currentGroup)
        {
            TpString groupName = sectionPair.first.mid(prefix.length());
            if (groupName.contains("/"))
            {
                groupName = groupName.left(groupName.indexOf("/"));
            }

            if (std::find(groups.begin(), groups.end(), groupName) == groups.end())
            {
                groups.push_back(groupName);
            }
        }
    }

    return groups;
}

bool TpConfig::isWritable() const
{
    TpConfigData *configData = static_cast<TpConfigData *>(data_);
    if (configData->fileName.empty())
        return false;

    TpFile file(configData->fileName);
    return file.isWritable();
}

void TpConfig::setValue(const TpString &key, const TpString &value)
{
    TpConfigData *configData = static_cast<TpConfigData *>(data_);
    TpString fullKey = configData->currentGroupPrefix();

    if (!fullKey.empty())
    {
        fullKey += "/";
    }
    fullKey += key;

    // 解析节和键
    int slashPos = fullKey.lastIndexOf("/");
    if (slashPos <= 0)
    {
        configData->status = FormatError;
        return;
    }

    TpString section = fullKey.left(slashPos);
    TpString realKey = fullKey.mid(slashPos + 1);

    configData->data[section][realKey] = value;
    configData->status = NoError;
}

TpString TpConfig::value(const TpString &key) const
{
    TpConfigData *configData = static_cast<TpConfigData *>(data_);
    TpString fullKey = configData->currentGroupPrefix();

    if (!fullKey.empty())
    {
        fullKey += "/";
    }
    fullKey += key;

    // 解析节和键
    int slashPos = fullKey.lastIndexOf("/");
    if (slashPos <= 0)
    {
        return TpString();
    }

    TpString section = fullKey.left(slashPos);
    TpString realKey = fullKey.mid(slashPos + 1);

    if (configData->data.find(section) != configData->data.end() &&
        configData->data.at(section).find(realKey) != configData->data.at(section).end())
    {
        return configData->data.at(section).at(realKey);
    }

    return TpString();
}

void TpConfig::remove(const TpString &key)
{
    TpConfigData *configData = static_cast<TpConfigData *>(data_);
    TpString fullKey = configData->currentGroupPrefix();

    if (!fullKey.empty())
    {
        fullKey += "/";
    }
    fullKey += key;

    // 解析节和键
    int slashPos = fullKey.lastIndexOf("/");
    if (slashPos <= 0)
    {
        return;
    }

    TpString section = fullKey.left(slashPos);
    TpString realKey = fullKey.mid(slashPos + 1);

    if (configData->data.find(section) != configData->data.end())
    {
        configData->data[section].erase(realKey);

        // 如果节为空，删除整个节
        if (configData->data[section].empty())
        {
            configData->data.erase(section);
        }
    }
}

bool TpConfig::contains(const TpString &key) const
{
    TpConfigData *configData = static_cast<TpConfigData *>(data_);
    TpString fullKey = configData->currentGroupPrefix();

    if (!fullKey.empty())
    {
        fullKey += "/";
    }
    fullKey += key;

    // 解析节和键
    int slashPos = fullKey.lastIndexOf("/");
    if (slashPos <= 0)
    {
        return false;
    }

    TpString section = fullKey.left(slashPos);
    TpString realKey = fullKey.mid(slashPos + 1);

    return (configData->data.find(section) != configData->data.end() &&
            configData->data.at(section).find(realKey) != configData->data.at(section).end());
}