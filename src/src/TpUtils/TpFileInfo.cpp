#include "TpFileInfo.h"
#include "TpDir.h"
#include <chrono>
#include <ctime>
#include "TpFile.h"
#include "TpString.h"
#include "TpVector.h"

#ifdef _WIN32
#include <windows.h>
#else
#include <sys/stat.h>
#endif

struct TpFileInfoData
{
    TpString path;        // 存储路径字符串
    struct stat fileStat; // 存储文件状态信息
    bool statValid;       // 标记 stat 是否有效

    TpFileInfoData() : statValid(false)
    {
    }

    void refreshStat()
    {
        statValid = (stat(path.c_str(), &fileStat) == 0);
    }
};

// 辅助函数，用于将时间点格式化为字符串
// TpString formatDateTime(const std::filesystem::file_time_type &time, const TpString &format)
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
//     return TpString(buffer);
// }

TpString formatDateTime(time_t time, const TpString &format)
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
        return TpString(formattedTime.c_str());
    }

    // 如果格式化失败，返回空字符串
    return TpString("");
}

TpFileInfo::TpFileInfo()
{
    this->data_ = new TpFileInfoData();
}

TpFileInfo::TpFileInfo(const TpString &file)
{
    this->data_ = new TpFileInfoData();

    setFile(file);
}

TpFileInfo::~TpFileInfo()
{
    TpFileInfoData *fileInfoData = static_cast<TpFileInfoData *>(data_);
    if (fileInfoData)
    {
        delete fileInfoData;
        fileInfoData = nullptr;
    }
}

TpFileInfo TpFileInfo::operator=(const TpFileInfo &other)
{
    this->setFile(other.filePath());
    return *this;
}

bool TpFileInfo::exists(const TpString &file)
{
    TpFile fileObj(file);
    return fileObj.exists();
}

void TpFileInfo::setFile(const TpString &file)
{
    TpFileInfoData *fileInfoData = static_cast<TpFileInfoData *>(data_);
    if (!fileInfoData)
        return;

    fileInfoData->path = file;
    fileInfoData->refreshStat();
}

bool TpFileInfo::exists() const
{
    TpFileInfoData *fileInfoData = static_cast<TpFileInfoData *>(data_);
    TpFile fileObj(fileInfoData->path);
    return fileObj.exists();
}

TpString TpFileInfo::filePath() const
{
    TpFileInfoData *fileInfoData = static_cast<TpFileInfoData *>(data_);
    if (!fileInfoData)
        return "";

    return fileInfoData->path;
}

TpString TpFileInfo::absoluteFilePath() const
{
    TpFileInfoData *fileInfoData = static_cast<TpFileInfoData *>(data_);
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
    TpString absPath = TpString(cwd) + "/" + fileInfoData->path;
    return resolvePath(absPath); 
}

TpString TpFileInfo::canonicalFilePath() const
{
    TpFileInfoData *fileInfoData = static_cast<TpFileInfoData *>(data_);
    if (!fileInfoData)
        return "";

    if (!fileInfoData || fileInfoData->path.empty())
        return "";

    char resolved[PATH_MAX];
    // 使用 realpath 解析符号链接和绝对路径
    if (realpath(fileInfoData->path.c_str(), resolved) != nullptr)
    {
        return TpString(resolved);
    }
    return ""; // 路径不存在或无法解析
}

TpString TpFileInfo::fileName() const
{
    TpFileInfoData *fileInfoData = static_cast<TpFileInfoData *>(data_);
    if (!fileInfoData)
        return "";

    size_t pos = fileInfoData->path.find_last_of('/');
    return (pos != TpString::npos) ? fileInfoData->path.substr(pos + 1) : fileInfoData->path;
}

TpString TpFileInfo::baseName() const
{
    TpFileInfoData *fileInfoData = static_cast<TpFileInfoData *>(data_);
    if (!fileInfoData)
        return "";

    TpString name = fileName();
    size_t dotPos = name.find_last_of('.');
    return (dotPos != TpString::npos) ? name.substr(0, dotPos) : name;
}

TpString TpFileInfo::suffix() const
{
    TpFileInfoData *fileInfoData = static_cast<TpFileInfoData *>(data_);
    if (!fileInfoData)
        return "";

    TpString name = fileName();
    size_t dotPos = name.find_last_of('.');
    return (dotPos != TpString::npos) ? name.substr(dotPos + 1) : "";
}

TpString TpFileInfo::path() const
{
    TpFileInfoData *fileInfoData = static_cast<TpFileInfoData *>(data_);
    if (!fileInfoData)
        return "";

    size_t pos = fileInfoData->path.find_last_of('/');
    return (pos != TpString::npos) ? fileInfoData->path.substr(0, pos) : "";
}

TpString TpFileInfo::absolutePath() const
{
    TpFileInfoData *fileInfoData = static_cast<TpFileInfoData *>(data_);
    if (!fileInfoData)
        return "";

    char absPath[PATH_MAX];
    if (realpath(fileInfoData->path.c_str(), absPath))
    {
        return TpString(absPath);
    }
    return ""; // 错误处理
}

TpString TpFileInfo::canonicalPath() const
{
    TpFileInfoData *fileInfoData = static_cast<TpFileInfoData *>(data_);
    if (!fileInfoData)
        return "";

    if (!fileInfoData || fileInfoData->path.empty())
        return "";

    char resolved[PATH_MAX];
    // 使用 realpath 解析符号链接和绝对路径
    if (realpath(fileInfoData->path.c_str(), resolved) != nullptr)
    {
        return TpString(resolved);
    }
    return ""; // 路径不存在或无法解析
}

TpDir TpFileInfo::dir() const
{
    TpFileInfoData *fileInfoData = static_cast<TpFileInfoData *>(data_);
    if (!fileInfoData)
        return TpDir();

    return TpDir(fileInfoData->path);
}

TpDir TpFileInfo::absoluteDir() const
{
    TpFileInfoData *fileInfoData = static_cast<TpFileInfoData *>(data_);
    if (!fileInfoData)
        return TpDir();

    // 获取绝对路径
    const TpString absPath = absoluteFilePath();
    if (absPath.empty())
        return TpDir();

    // 如果是目录，直接返回自身路径
    if (isDir())
    {
        return TpDir(absPath);
    }

    // 否则提取父目录路径
    size_t lastSlash = absPath.find_last_of('/');
    if (lastSlash == TpString::npos)
    {
        return TpDir(); // 无效路径
    }

    // 处理根目录特殊情况
    if (lastSlash == 0)
    {
        return TpDir("/"); // 根目录
    }

    // 截取父目录路径
    TpString parentPath = absPath.substr(0, lastSlash);

    // 确保路径有效性（例如处理 "/home/user///test"）
    return TpDir(parentPath.c_str());
}

bool TpFileInfo::isReadable() const
{
    TpFileInfoData *fileInfoData = static_cast<TpFileInfoData *>(data_);
    if (!fileInfoData)
        return false;

    return access(fileInfoData->path.c_str(), R_OK) == 0;
}

bool TpFileInfo::isWritable() const
{
    TpFileInfoData *fileInfoData = static_cast<TpFileInfoData *>(data_);
    if (!fileInfoData)
        return false;

    return access(fileInfoData->path.c_str(), W_OK) == 0;
}

bool TpFileInfo::isExecutable() const
{
    TpFileInfoData *fileInfoData = static_cast<TpFileInfoData *>(data_);
    if (!fileInfoData)
        return false;

    return access(fileInfoData->path.c_str(), X_OK) == 0;
}

bool TpFileInfo::isHidden() const
{
    TpFileInfoData *fileInfoData = static_cast<TpFileInfoData *>(data_);
    if (!fileInfoData)
        return false;

    TpString name = fileName();
    return !name.empty() && name[0] == '.';
}

bool TpFileInfo::isNativePath() const
{
    TpFileInfoData *fileInfoData = static_cast<TpFileInfoData *>(data_);
    if (!fileInfoData)
        return false;

#ifdef _WIN32
    return !fileInfoData->pathObj.is_absolute() || fileInfoData->pathObj.root_name() != "\\";
#else
    return true; // Unix-like系统中，所有路径都是本地路径
#endif
}

bool TpFileInfo::isRelative() const
{
    TpFileInfoData *fileInfoData = static_cast<TpFileInfoData *>(data_);

    if (!fileInfoData || fileInfoData->path.empty())
        return false;

    // Unix 绝对路径以 '/' 开头，否则为相对路径
    return fileInfoData->path[0] != '/';
}

bool TpFileInfo::isFile() const
{
    TpFileInfoData *fileInfoData = static_cast<TpFileInfoData *>(data_);
    if (!fileInfoData)
        return false;

    return fileInfoData->statValid && S_ISREG(fileInfoData->fileStat.st_mode);
}

bool TpFileInfo::isDir() const
{
    TpFileInfoData *fileInfoData = static_cast<TpFileInfoData *>(data_);
    if (!fileInfoData)
        return false;

    return fileInfoData->statValid && S_ISDIR(fileInfoData->fileStat.st_mode);
}

bool TpFileInfo::isSymLink() const
{
    TpFileInfoData *fileInfoData = static_cast<TpFileInfoData *>(data_);
    if (!fileInfoData)
        return false;

    struct stat lst;
    return lstat(fileInfoData->path.c_str(), &lst) == 0 && S_ISLNK(lst.st_mode);
}

bool TpFileInfo::isRoot() const
{
    TpFileInfoData *fileInfoData = static_cast<TpFileInfoData *>(data_);

    if (!fileInfoData || fileInfoData->path.empty())
        return false;

    // 获取绝对路径并规范化
    TpString absPath = absoluteFilePath();
    if (absPath.empty())
        return false;

    // 解析路径中的 "." 和 ".."（使用之前实现的 resolvePath）
    TpString resolved = resolvePath(absPath);

    // 根目录的规范化路径为 "/"
    return resolved == TpString("/");
}

uint64_t TpFileInfo::size() const
{
    if (!exists())
        return 0;

    if (isDir())
        return 0;

    TpFileInfoData *fileInfoData = static_cast<TpFileInfoData *>(data_);
    if (!fileInfoData)
        return 0;

    return fileInfoData->statValid ? fileInfoData->fileStat.st_size : 0;
}

TpString TpFileInfo::lastModified(const TpString &format) const
{
    TpFileInfoData *fileInfoData = static_cast<TpFileInfoData *>(data_);
    if (!fileInfoData)
        return "";

    return fileInfoData->statValid ? formatDateTime(fileInfoData->fileStat.st_mtime, format) : "";
}

// 解析路径中的 . 和 ..
TpString TpFileInfo::resolvePath(const TpString &path) const
{
    std::vector<TpString> parts;
    TpString currentToken;

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
    TpVector<TpString> resolvedParts;
    for (const auto &part : parts)
    {
        if (part == TpString("."))
        {
            continue;
        }
        else if (part == TpString(".."))
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
    TpString resolved = "/";
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
