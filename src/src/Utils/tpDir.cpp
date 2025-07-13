#include "tpDir.h"
#include "tpVector.h"

#include <iostream>
#include <algorithm>
#include <dirent.h>

struct tpDirData
{
    tpString dirPath;

    tpDirData() : dirPath("")
    {
    }
};

// 辅助函数：拼接路径
static tpString pathJoin(const tpString &a, const tpString &b)
{
    if (a.empty())
        return b;
    if (a.back() == '/')
        return a + b;
    return a + "/" + b;
}

// 辅助函数：解析绝对路径
static tpString getAbsolutePath(const tpString &path)
{
    char resolved[PATH_MAX];
    return (realpath(path.c_str(), resolved)) ? resolved : "";
}

bool isHidden(const tpString &filename)
{
    // 根据操作系统实现隐藏文件检查
    // 例如，在Unix中，隐藏文件通常以点开头
    return !filename.empty() && filename[0] == '.';
}

bool filterAccepts(const tpFileInfo &info, tpDir::Filters filters)
{
    if (filters == tpDir::NoFilter)
        return true;

    // 排除符号链接
    if ((filters & tpDir::NoSymLinks) && info.isSymLink())
        return false;

    // 类型过滤
    bool isDir = info.isDir();
    bool isFile = info.isFile();
    if ((filters & tpDir::AllEntries) != tpDir::AllEntries)
    {
        if (isDir && !(filters & tpDir::Dirs))
            return false;
        if (isFile && !(filters & tpDir::Files))
            return false;
    }

    // 隐藏文件过滤
    if ((filters & tpDir::Hidden) && !isHidden(info.fileName()))
        return false;

    // 排除 "." 和 ".."
    if ((filters & tpDir::NoDotAndDotDot) &&
        (info.fileName() == tpString(".") || info.fileName() == tpString("..")))
        return false;

    return true;
}

void sortEntries(tpFileInfoList &entries, tpDir::SortFlags sort)
{
    // 根据SortFlag定义的排序规则进行排序

    // 不进行排序
    if (sort == tpDir::SortFlag::NoSort)
        return;

    // 定义比较函数
    auto compareFunc = [=](const tpFileInfo &a, const tpFileInfo &b) -> bool
    {
        // 默认比较文件名
        if ((sort & tpDir::SortFlag::Name) == tpDir::SortFlag::Name)
        {
            return a.fileName() < b.fileName();
        }
        // 比较文件修改时间
        else if ((sort & tpDir::SortFlag::Time) == tpDir::SortFlag::Time)
        {
            return a.lastModified() < b.lastModified();
        }
        // 比较文件大小
        else if ((sort & tpDir::SortFlag::Size) == tpDir::SortFlag::Size)
        {
            return a.size() < b.size();
        }
        // 比较文件类型（目录在前或文件在前）
        else if ((sort & tpDir::SortFlag::Type) == tpDir::SortFlag::Type)
        {
            if ((sort & tpDir::SortFlag::DirsFirst) == tpDir::SortFlag::DirsFirst)
            {
                if (a.isDir() && !b.isDir())
                    return true;
                if (!a.isDir() && b.isDir())
                    return false;
            }
            else if ((sort & tpDir::SortFlag::DirsLast) == tpDir::SortFlag::DirsLast)
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
    if ((sort & tpDir::SortFlag::Reversed) == tpDir::SortFlag::Reversed)
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

tpDir::tpDir()
{
    this->data_ = new tpDirData();
}

tpDir::tpDir(const tpString &path)
{
    this->data_ = new tpDirData();

    setPath(path);
}

tpDir::~tpDir()
{
    tpDirData *dirData = (tpDirData *)this->data_;

    if (dirData)
    {
        delete dirData;
        dirData = nullptr;
    }
}

bool tpDir::mkpath(const tpString &dirPath)
{
    // 创建任意路径下的多级目录
    tpString currentPath;
    for (const auto &part : dirPath.split('/'))
    {
        currentPath = pathJoin(currentPath, part);
        if (::mkdir(currentPath.c_str(), 0755) != 0 && errno != EEXIST)
            return false;
    }
    return true;
}

void tpDir::setPath(const tpString &path)
{
    tpDirData *dirData = (tpDirData *)this->data_;
    if (!dirData)
        return;

    dirData->dirPath = path;
}

tpString tpDir::path() const
{
    tpDirData *dirData = (tpDirData *)this->data_;
    if (!dirData)
        return "";

    return dirData->dirPath;
}

tpString tpDir::absolutePath() const
{
    // 返回绝对路径
    tpDirData *dirData = (tpDirData *)this->data_;
    if (!dirData)
        return "";

    return getAbsolutePath(dirData->dirPath);
}

tpString tpDir::canonicalPath() const
{
    // 返回规范路径，它会解析符号链接。
    tpDirData *dirData = (tpDirData *)this->data_;
    if (!dirData)
        return "";

    return getAbsolutePath(dirData->dirPath);
}

tpString tpDir::dirName() const
{
    // 返回路径的最后一部分，即目录名。
    tpDirData *dirData = (tpDirData *)this->data_;
    if (!dirData)
        return "";

    size_t pos = dirData->dirPath.find_last_of('/');
    if (pos == tpString::npos)
        return dirData->dirPath;
    return dirData->dirPath.substr(0, pos);
}

tpString tpDir::filePath(const tpString &fileName) const
{
    // 返回给定文件名在当前目录下的完整路径。
    tpDirData *dirData = (tpDirData *)this->data_;
    if (!dirData)
        return "";

    return pathJoin(dirData->dirPath, fileName);
}

tpString tpDir::absoluteFilePath(const tpString &fileName) const
{
    // 返回给定文件名的绝对路径。
    tpDirData *dirData = (tpDirData *)this->data_;
    if (!dirData)
        return "";

    return pathJoin(absolutePath(), fileName);
}

tpString tpDir::relativeFilePath(const tpString &fileName) const
{
    // 返回给定文件名的相对路径。
    tpDirData *dirData = (tpDirData *)this->data_;

    if (!dirData || dirData->dirPath.empty())
        return "";

    // 获取当前目录和目标文件的绝对路径
    tpString baseAbs = getAbsolutePath(dirData->dirPath);
    tpString targetAbs = getAbsolutePath(pathJoin(dirData->dirPath, fileName));

    // 规范化路径（统一使用 "/" 分隔符）
    auto normalize = [](tpString path)
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
    auto splitPath = [](const tpString &path) -> tpVector<tpString>
    {
        tpVector<tpString> parts;
        size_t start = 0, end = 0;
        while ((end = path.find('/', start)) != tpString::npos)
        {
            if (end != start) // 跳过空组件（如开头的 "/"）
                parts.push_back(path.substr(start, end - start));
            start = end + 1;
        }
        if (start < path.size())
            parts.push_back(path.substr(start));
        return parts;
    };

    tpVector<tpString> baseParts = splitPath(baseAbs);
    tpVector<tpString> targetParts = splitPath(targetAbs);

    // 寻找共同前缀
    size_t commonLen = 0;
    while (commonLen < baseParts.size() &&
           commonLen < targetParts.size() &&
           baseParts[commonLen] == targetParts[commonLen])
    {
        commonLen++;
    }

    // 构建相对路径
    tpString relativePath;
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

tpFileInfoList tpDir::entryInfoList(Filters filters, SortFlags sort) const
{
    tpFileInfoList entries;

    tpDirData *dirData = (tpDirData *)this->data_;
    if (!dirData)
        return entries;

    DIR *dir = opendir(dirData->dirPath.c_str());
    if (!dir)
        return entries;

    struct dirent *entry;
    while ((entry = readdir(dir)) != nullptr)
    {
        tpString name = entry->d_name;
        if (name == tpString(".") || name == tpString(".."))
            continue;

        tpString filePath = pathJoin(dirData->dirPath, name);
        tpFileInfo info(filePath);
        if (filterAccepts(info, filters))
            entries.emplace_back(filePath);
    }
    closedir(dir);

    sortEntries(entries, sort);
    return entries;
}

bool tpDir::mkdir(const tpString &dirName) const
{
    tpDirData *dirData = (tpDirData *)this->data_;
    if (!dirData || dirData->dirPath.empty())
        return false;

    tpString fullPath = pathJoin(dirData->dirPath, dirName);
    return ::mkdir(dirName.c_str(), 0755) == 0;
}

bool tpDir::exists() const
{
    tpDirData *dirData = (tpDirData *)this->data_;
    if (!dirData || dirData->dirPath.empty())
        return false;

    struct stat info;
    return stat(dirData->dirPath.c_str(), &info) == 0 && S_ISDIR(info.st_mode);
}

bool tpDir::remove(const tpString &fileName)
{
    tpDirData *dirData = (tpDirData *)this->data_;
    if (!dirData || dirData->dirPath.empty())
        return false;

    tpString fullPath = pathJoin(dirData->dirPath, fileName);
    return ::unlink(fullPath.c_str()) == 0;
}

bool tpDir::removeRecursively()
{
    tpDirData *dirData = (tpDirData *)this->data_;
    if (!dirData || dirData->dirPath.empty())
        return false;

    return system(("rm -rf " + dirData->dirPath).c_str()) == 0;
}
