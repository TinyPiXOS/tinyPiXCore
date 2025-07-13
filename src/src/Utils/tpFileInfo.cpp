#include "tpFileInfo.h"
#include "tpDir.h"
#include <chrono>
#include <ctime>
#include "tpFile.h"
#include "tpString.h"
#include "tpVector.h"

#ifdef _WIN32
#include <windows.h>
#else
#include <sys/stat.h>
#endif

struct tpFileInfoData
{
    tpString path;        // 存储路径字符串
    struct stat fileStat; // 存储文件状态信息
    bool statValid;       // 标记 stat 是否有效

    tpFileInfoData() : statValid(false)
    {
    }

    void refreshStat()
    {
        statValid = (stat(path.c_str(), &fileStat) == 0);
    }
};

// 辅助函数，用于将时间点格式化为字符串
// tpString formatDateTime(const std::filesystem::file_time_type &time, const tpString &format)
// {
//     // 将 file_time_type 转换为 time_t
//     auto sys_time_point = std::chrono::system_clock::from_time_t(
//         std::chrono::duration_cast<std::chrono::seconds>(
//             time.time_since_epoch())
//             .count());

//     // 然后使用 std::chrono::system_clock::to_time_t() 方法转换为 time_t
//     std::time_t cftime = std::chrono::system_clock::to_time_t(sys_time_point);

//     // 使用 localtime 将 time_t 转换为 tm 结构
//     std::tm tm = *std::localtime(&cftime);

//     char buffer[100];
//     std::strftime(buffer, sizeof(buffer), format.c_str(), &tm);
//     return tpString(buffer);
// }

tpString formatDateTime(time_t time, const tpString &format)
{
    // 将 time_t 转换为 tm 结构
    std::tm tm = *std::localtime(&time);

    // 使用用户提供的格式来格式化日期和时间
    std::string formattedTime;
    formattedTime.resize(64); // 预分配足够的空间

    // 使用 strftime 来格式化时间
    if (std::strftime(&formattedTime[0], formattedTime.size(), format.c_str(), &tm))
    {
        // 返回格式化后的字符串
        return tpString(formattedTime.c_str());
    }

    // 如果格式化失败，返回空字符串
    return tpString("");
}

tpFileInfo::tpFileInfo()
{
    this->data_ = new tpFileInfoData();
}

tpFileInfo::tpFileInfo(const tpString &file)
{
    this->data_ = new tpFileInfoData();

    setFile(file);
}

tpFileInfo::~tpFileInfo()
{
    tpFileInfoData *fileInfoData = static_cast<tpFileInfoData *>(data_);
    if (fileInfoData)
    {
        delete fileInfoData;
        fileInfoData = nullptr;
    }
}

tpFileInfo tpFileInfo::operator=(const tpFileInfo &other)
{
    this->setFile(other.filePath());
    return *this;
}

bool tpFileInfo::exists(const tpString &file)
{
    tpFile fileObj(file);
    return fileObj.exists();
}

void tpFileInfo::setFile(const tpString &file)
{
    tpFileInfoData *fileInfoData = static_cast<tpFileInfoData *>(data_);
    if (!fileInfoData)
        return;

    fileInfoData->path = file;
    fileInfoData->refreshStat();
}

bool tpFileInfo::exists() const
{
    tpFileInfoData *fileInfoData = static_cast<tpFileInfoData *>(data_);
    tpFile fileObj(fileInfoData->path);
    return fileObj.exists();
}

tpString tpFileInfo::filePath() const
{
    tpFileInfoData *fileInfoData = static_cast<tpFileInfoData *>(data_);
    if (!fileInfoData)
        return "";

    return fileInfoData->path;
}

tpString tpFileInfo::absoluteFilePath() const
{
    tpFileInfoData *fileInfoData = static_cast<tpFileInfoData *>(data_);
    if (!fileInfoData)
        return "";

    if (!fileInfoData || fileInfoData->path.empty())
        return "";

    // 如果已经是绝对路径，直接返回
    if (!fileInfoData->path.empty() && fileInfoData->path[0] == '/')
    {
        return fileInfoData->path;
    }

    // 获取当前工作目录并拼接相对路径
    char cwd[PATH_MAX];
    if (getcwd(cwd, sizeof(cwd)) == nullptr)
    {
        return ""; // 获取当前目录失败
    }

    // 解析路径中的 "." 和 ".."
    tpString absPath = tpString(cwd) + "/" + fileInfoData->path;
    return resolvePath(absPath); 
}

tpString tpFileInfo::canonicalFilePath() const
{
    tpFileInfoData *fileInfoData = static_cast<tpFileInfoData *>(data_);
    if (!fileInfoData)
        return "";

    if (!fileInfoData || fileInfoData->path.empty())
        return "";

    char resolved[PATH_MAX];
    // 使用 realpath 解析符号链接和绝对路径
    if (realpath(fileInfoData->path.c_str(), resolved) != nullptr)
    {
        return tpString(resolved);
    }
    return ""; // 路径不存在或无法解析
}

tpString tpFileInfo::fileName() const
{
    tpFileInfoData *fileInfoData = static_cast<tpFileInfoData *>(data_);
    if (!fileInfoData)
        return "";

    size_t pos = fileInfoData->path.find_last_of('/');
    return (pos != tpString::npos) ? fileInfoData->path.substr(pos + 1) : fileInfoData->path;
}

tpString tpFileInfo::baseName() const
{
    tpFileInfoData *fileInfoData = static_cast<tpFileInfoData *>(data_);
    if (!fileInfoData)
        return "";

    tpString name = fileName();
    size_t dotPos = name.find_last_of('.');
    return (dotPos != tpString::npos) ? name.substr(0, dotPos) : name;
}

tpString tpFileInfo::suffix() const
{
    tpFileInfoData *fileInfoData = static_cast<tpFileInfoData *>(data_);
    if (!fileInfoData)
        return "";

    tpString name = fileName();
    size_t dotPos = name.find_last_of('.');
    return (dotPos != tpString::npos) ? name.substr(dotPos + 1) : "";
}

tpString tpFileInfo::path() const
{
    tpFileInfoData *fileInfoData = static_cast<tpFileInfoData *>(data_);
    if (!fileInfoData)
        return "";

    size_t pos = fileInfoData->path.find_last_of('/');
    return (pos != tpString::npos) ? fileInfoData->path.substr(0, pos) : "";
}

tpString tpFileInfo::absolutePath() const
{
    tpFileInfoData *fileInfoData = static_cast<tpFileInfoData *>(data_);
    if (!fileInfoData)
        return "";

    char absPath[PATH_MAX];
    if (realpath(fileInfoData->path.c_str(), absPath))
    {
        return tpString(absPath);
    }
    return ""; // 错误处理
}

tpString tpFileInfo::canonicalPath() const
{
    tpFileInfoData *fileInfoData = static_cast<tpFileInfoData *>(data_);
    if (!fileInfoData)
        return "";

    if (!fileInfoData || fileInfoData->path.empty())
        return "";

    char resolved[PATH_MAX];
    // 使用 realpath 解析符号链接和绝对路径
    if (realpath(fileInfoData->path.c_str(), resolved) != nullptr)
    {
        return tpString(resolved);
    }
    return ""; // 路径不存在或无法解析
}

tpDir tpFileInfo::dir() const
{
    tpFileInfoData *fileInfoData = static_cast<tpFileInfoData *>(data_);
    if (!fileInfoData)
        return tpDir();

    return tpDir(fileInfoData->path);
}

tpDir tpFileInfo::absoluteDir() const
{
    tpFileInfoData *fileInfoData = static_cast<tpFileInfoData *>(data_);
    if (!fileInfoData)
        return tpDir();

    // 获取绝对路径
    const tpString absPath = absoluteFilePath();
    if (absPath.empty())
        return tpDir();

    // 如果是目录，直接返回自身路径
    if (isDir())
    {
        return tpDir(absPath);
    }

    // 否则提取父目录路径
    size_t lastSlash = absPath.find_last_of('/');
    if (lastSlash == tpString::npos)
    {
        return tpDir(); // 无效路径
    }

    // 处理根目录特殊情况
    if (lastSlash == 0)
    {
        return tpDir("/"); // 根目录
    }

    // 截取父目录路径
    tpString parentPath = absPath.substr(0, lastSlash);

    // 确保路径有效性（例如处理 "/home/user///test"）
    return tpDir(parentPath.c_str());
}

bool tpFileInfo::isReadable() const
{
    tpFileInfoData *fileInfoData = static_cast<tpFileInfoData *>(data_);
    if (!fileInfoData)
        return false;

    return access(fileInfoData->path.c_str(), R_OK) == 0;
}

bool tpFileInfo::isWritable() const
{
    tpFileInfoData *fileInfoData = static_cast<tpFileInfoData *>(data_);
    if (!fileInfoData)
        return false;

    return access(fileInfoData->path.c_str(), W_OK) == 0;
}

bool tpFileInfo::isExecutable() const
{
    tpFileInfoData *fileInfoData = static_cast<tpFileInfoData *>(data_);
    if (!fileInfoData)
        return false;

    return access(fileInfoData->path.c_str(), X_OK) == 0;
}

bool tpFileInfo::isHidden() const
{
    tpFileInfoData *fileInfoData = static_cast<tpFileInfoData *>(data_);
    if (!fileInfoData)
        return false;

    tpString name = fileName();
    return !name.empty() && name[0] == '.';
}

bool tpFileInfo::isNativePath() const
{
    tpFileInfoData *fileInfoData = static_cast<tpFileInfoData *>(data_);
    if (!fileInfoData)
        return false;

#ifdef _WIN32
    return !fileInfoData->pathObj.is_absolute() || fileInfoData->pathObj.root_name() != "\\";
#else
    return true; // Unix-like系统中，所有路径都是本地路径
#endif
}

bool tpFileInfo::isRelative() const
{
    tpFileInfoData *fileInfoData = static_cast<tpFileInfoData *>(data_);

    if (!fileInfoData || fileInfoData->path.empty())
        return false;

    // Unix 绝对路径以 '/' 开头，否则为相对路径
    return fileInfoData->path[0] != '/';
}

bool tpFileInfo::isFile() const
{
    tpFileInfoData *fileInfoData = static_cast<tpFileInfoData *>(data_);
    if (!fileInfoData)
        return false;

    return fileInfoData->statValid && S_ISREG(fileInfoData->fileStat.st_mode);
}

bool tpFileInfo::isDir() const
{
    tpFileInfoData *fileInfoData = static_cast<tpFileInfoData *>(data_);
    if (!fileInfoData)
        return false;

    return fileInfoData->statValid && S_ISDIR(fileInfoData->fileStat.st_mode);
}

bool tpFileInfo::isSymLink() const
{
    tpFileInfoData *fileInfoData = static_cast<tpFileInfoData *>(data_);
    if (!fileInfoData)
        return false;

    struct stat lst;
    return lstat(fileInfoData->path.c_str(), &lst) == 0 && S_ISLNK(lst.st_mode);
}

bool tpFileInfo::isRoot() const
{
    tpFileInfoData *fileInfoData = static_cast<tpFileInfoData *>(data_);

    if (!fileInfoData || fileInfoData->path.empty())
        return false;

    // 获取绝对路径并规范化
    tpString absPath = absoluteFilePath();
    if (absPath.empty())
        return false;

    // 解析路径中的 "." 和 ".."（使用之前实现的 resolvePath）
    tpString resolved = resolvePath(absPath);

    // 根目录的规范化路径为 "/"
    return resolved == tpString("/");
}

uint64_t tpFileInfo::size() const
{
    if (!exists())
        return 0;

    if (isDir())
        return 0;

    tpFileInfoData *fileInfoData = static_cast<tpFileInfoData *>(data_);
    if (!fileInfoData)
        return 0;

    return fileInfoData->statValid ? fileInfoData->fileStat.st_size : 0;
}

tpString tpFileInfo::lastModified(const tpString &format) const
{
    tpFileInfoData *fileInfoData = static_cast<tpFileInfoData *>(data_);
    if (!fileInfoData)
        return "";

    return fileInfoData->statValid ? formatDateTime(fileInfoData->fileStat.st_mtime, format) : "";
}

// 解析路径中的 . 和 ..
tpString tpFileInfo::resolvePath(const tpString &path) const
{
    std::vector<tpString> parts;
    tpString currentToken;

    // 分解路径为各个部分
    for (char c : path)
    {
        if (c == '/')
        {
            if (!currentToken.empty())
            {
                parts.push_back(currentToken);
                currentToken.clear();
            }
        }
        else
        {
            currentToken += c;
        }
    }
    if (!currentToken.empty())
    {
        parts.push_back(currentToken);
    }

    // 处理 "." 和 ".."
    tpVector<tpString> resolvedParts;
    for (const auto &part : parts)
    {
        if (part == tpString("."))
        {
            continue;
        }
        else if (part == tpString(".."))
        {
            if (!resolvedParts.empty())
            {
                resolvedParts.pop_back();
            }
        }
        else
        {
            resolvedParts.push_back(part);
        }
    }

    // 重组路径
    tpString resolved = "/";
    for (const auto &part : resolvedParts)
    {
        resolved += part + "/";
    }
    if (!resolvedParts.empty())
    {
        resolved.pop_back(); // 移除末尾多余的 "/"
    }
    return resolved;
}
