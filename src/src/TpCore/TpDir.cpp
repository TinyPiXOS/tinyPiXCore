#include "TpDir.h"
#include "TpVector.h"

#include <iostream>
#include <algorithm>
#include <dirent.h>

struct TpDirData
{
    TpString dirPath;

    TpDirData() : dirPath("")
    {
    }
};

// 辅助函数：拼接路径
static TpString pathJoin(const TpString &a, const TpString &b)
{
    if (a.empty())
        return b;
    if (a.back() == '/')
        return a + b;
    return a + "/" + b;
}

// 辅助函数：解析绝对路径
static TpString getAbsolutePath(const TpString &path)
{
    char resolved[PATH_MAX];
    return (realpath(path.c_str(), resolved)) ? resolved : "";
}

bool isHidden(const TpString &filename)
{
    // 根据操作系统实现隐藏文件检查
    // 例如，在Unix中，隐藏文件通常以点开头
    return !filename.empty() && filename[0] == '.';
}

bool filterAccepts(const TpFileInfo &info, TpDir::Filters filters)
{
    if (filters == TpDir::NoFilter)
        return true;

    // 排除符号链接
    if ((filters & TpDir::NoSymLinks) && info.isSymLink())
        return false;

    // 类型过滤
    bool isDir = info.isDir();
    bool isFile = info.isFile();
    if ((filters & TpDir::AllEntries) != TpDir::AllEntries)
    {
        if (isDir && !(filters & TpDir::Dirs))
            return false;
        if (isFile && !(filters & TpDir::Files))
            return false;
    }

    // 隐藏文件过滤
    if ((filters & TpDir::Hidden) && !isHidden(info.fileName()))
        return false;

    // 排除 "." 和 ".."
    if ((filters & TpDir::NoDotAndDotDot) &&
        (info.fileName() == TpString(".") || info.fileName() == TpString("..")))
        return false;

    return true;
}

void sortEntries(TpFileInfoList &entries, TpDir::SortFlags sort)
{
    // 根据SortFlag定义的排序规则进行排序

    // 不进行排序
    if (sort == TpDir::SortFlag::NoSort)
        return;

    // 定义比较函数
    auto compareFunc = [=](const TpFileInfo &a, const TpFileInfo &b) -> bool
    {
        // 默认比较文件名
        if ((sort & TpDir::SortFlag::Name) == TpDir::SortFlag::Name)
        {
            return a.fileName() < b.fileName();
        }
        // 比较文件修改时间
        else if ((sort & TpDir::SortFlag::Time) == TpDir::SortFlag::Time)
        {
            return a.lastModified() < b.lastModified();
        }
        // 比较文件大小
        else if ((sort & TpDir::SortFlag::Size) == TpDir::SortFlag::Size)
        {
            return a.size() < b.size();
        }
        // 比较文件类型（目录在前或文件在前）
        else if ((sort & TpDir::SortFlag::Type) == TpDir::SortFlag::Type)
        {
            if ((sort & TpDir::SortFlag::DirsFirst) == TpDir::SortFlag::DirsFirst)
            {
                if (a.isDir() && !b.isDir())
                    return true;
                if (!a.isDir() && b.isDir())
                    return false;
            }
            else if ((sort & TpDir::SortFlag::DirsLast) == TpDir::SortFlag::DirsLast)
            {
                if (a.isDir() && !b.isDir())
                    return false;
                if (!a.isDir() && b.isDir())
                    return true;
            }
            // 如果是相同类型，则按名称排序
            return a.fileName() < b.fileName();
        }
        return false;
    };

    // 根据SortFlag设置是否反转排序
    if ((sort & TpDir::SortFlag::Reversed) == TpDir::SortFlag::Reversed)
    {
        // 使用比较函数进行排序，然后反转列表以得到正确的顺序
        entries.sort(compareFunc);
        entries.reverse();
    }
    else
    {
        entries.sort(compareFunc);
    }
}

TpDir::TpDir()
{
    this->data_ = new TpDirData();
}

TpDir::TpDir(const TpString &path)
{
    this->data_ = new TpDirData();

    setPath(path);
}

TpDir::~TpDir()
{
    TpDirData *dirData = (TpDirData *)this->data_;

    if (dirData)
    {
        delete dirData;
        dirData = nullptr;
    }
}

bool TpDir::mkpath(const TpString &dirPath)
{
    // 创建任意路径下的多级目录
    TpString currentPath;
    for (const auto &part : dirPath.split('/'))
    {
        currentPath = pathJoin(currentPath, part);
        if (::mkdir(currentPath.c_str(), 0755) != 0 && errno != EEXIST)
            return false;
    }
    return true;
}

void TpDir::setPath(const TpString &path)
{
    TpDirData *dirData = (TpDirData *)this->data_;
    if (!dirData)
        return;

    dirData->dirPath = path;
}

TpString TpDir::path() const
{
    TpDirData *dirData = (TpDirData *)this->data_;
    if (!dirData)
        return "";

    return dirData->dirPath;
}

TpString TpDir::absolutePath() const
{
    // 返回绝对路径
    TpDirData *dirData = (TpDirData *)this->data_;
    if (!dirData)
        return "";

    return getAbsolutePath(dirData->dirPath);
}

TpString TpDir::canonicalPath() const
{
    // 返回规范路径，它会解析符号链接。
    TpDirData *dirData = (TpDirData *)this->data_;
    if (!dirData)
        return "";

    return getAbsolutePath(dirData->dirPath);
}

TpString TpDir::dirName() const
{
    // 返回路径的最后一部分，即目录名。
    TpDirData *dirData = (TpDirData *)this->data_;
    if (!dirData)
        return "";

    size_t pos = dirData->dirPath.find_last_of('/');
    if (pos == TpString::npos)
        return dirData->dirPath;
    return dirData->dirPath.substr(0, pos);
}

TpString TpDir::filePath(const TpString &fileName) const
{
    // 返回给定文件名在当前目录下的完整路径。
    TpDirData *dirData = (TpDirData *)this->data_;
    if (!dirData)
        return "";

    return pathJoin(dirData->dirPath, fileName);
}

TpString TpDir::absoluteFilePath(const TpString &fileName) const
{
    // 返回给定文件名的绝对路径。
    TpDirData *dirData = (TpDirData *)this->data_;
    if (!dirData)
        return "";

    return pathJoin(absolutePath(), fileName);
}

TpString TpDir::relativeFilePath(const TpString &fileName) const
{
    // 返回给定文件名的相对路径。
    TpDirData *dirData = (TpDirData *)this->data_;

    if (!dirData || dirData->dirPath.empty())
        return "";

    // 获取当前目录和目标文件的绝对路径
    TpString baseAbs = getAbsolutePath(dirData->dirPath);
    TpString targetAbs = getAbsolutePath(pathJoin(dirData->dirPath, fileName));

    // 规范化路径（统一使用 "/" 分隔符）
    auto normalize = [](TpString path)
    {
        std::replace(path.begin(), path.end(), '\\', '/');
        while (path.endsWith("/") && path.size() > 1)
            path.pop_back();
        return path;
    };
    baseAbs = normalize(baseAbs);
    targetAbs = normalize(targetAbs);

    // 如果路径完全相同，直接返回文件名
    if (baseAbs == targetAbs)
        return fileName;

    // 分割路径为组件列表
    auto splitPath = [](const TpString &path) -> TpVector<TpString>
    {
        TpVector<TpString> parts;
        size_t start = 0, end = 0;
        while ((end = path.find('/', start)) != TpString::npos)
        {
            if (end != start) // 跳过空组件（如开头的 "/"）
                parts.push_back(path.substr(start, end - start));
            start = end + 1;
        }
        if (start < path.size())
            parts.push_back(path.substr(start));
        return parts;
    };

    TpVector<TpString> baseParts = splitPath(baseAbs);
    TpVector<TpString> targetParts = splitPath(targetAbs);

    // 寻找共同前缀
    size_t commonLen = 0;
    while (commonLen < baseParts.size() &&
           commonLen < targetParts.size() &&
           baseParts[commonLen] == targetParts[commonLen])
    {
        commonLen++;
    }

    // 构建相对路径
    TpString relativePath;
    // 回退到共同目录
    for (size_t i = commonLen; i < baseParts.size(); ++i)
    {
        if (!relativePath.empty())
            relativePath += "/";
        relativePath += "..";
    }
    // 添加目标路径剩余部分
    for (size_t i = commonLen; i < targetParts.size(); ++i)
    {
        if (!relativePath.empty())
            relativePath += "/";
        relativePath += targetParts[i];
    }

    // 如果无共同前缀（完全无关路径），返回绝对路径
    return (commonLen == 0) ? targetAbs : relativePath;
}

TpFileInfoList TpDir::entryInfoList(Filters filters, SortFlags sort) const
{
    TpFileInfoList entries;

    TpDirData *dirData = (TpDirData *)this->data_;
    if (!dirData)
        return entries;

    DIR *dir = opendir(dirData->dirPath.c_str());
    if (!dir)
        return entries;

    struct dirent *entry;
    while ((entry = readdir(dir)) != nullptr)
    {
        TpString name = entry->d_name;
        if (name == TpString(".") || name == TpString(".."))
            continue;

        TpString filePath = pathJoin(dirData->dirPath, name);
        TpFileInfo info(filePath);
        if (filterAccepts(info, filters))
            entries.emplace_back(filePath);
    }
    closedir(dir);

    sortEntries(entries, sort);
    return entries;
}

bool TpDir::mkdir(const TpString &dirName) const
{
    TpDirData *dirData = (TpDirData *)this->data_;
    if (!dirData || dirData->dirPath.empty())
        return false;

    TpString fullPath = pathJoin(dirData->dirPath, dirName);
    return ::mkdir(dirName.c_str(), 0755) == 0;
}

bool TpDir::exists() const
{
    TpDirData *dirData = (TpDirData *)this->data_;
    if (!dirData || dirData->dirPath.empty())
        return false;

    struct stat info;
    return stat(dirData->dirPath.c_str(), &info) == 0 && S_ISDIR(info.st_mode);
}

bool TpDir::remove(const TpString &fileName)
{
    TpDirData *dirData = (TpDirData *)this->data_;
    if (!dirData || dirData->dirPath.empty())
        return false;

    TpString fullPath = pathJoin(dirData->dirPath, fileName);
    return ::unlink(fullPath.c_str()) == 0;
}

bool TpDir::removeRecursively()
{
    TpDirData *dirData = (TpDirData *)this->data_;
    if (!dirData || dirData->dirPath.empty())
        return false;

    return system(("rm -rf " + dirData->dirPath).c_str()) == 0;
}
