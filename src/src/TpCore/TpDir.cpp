#include "TpDir.h"
#include "TpVector.h"

#include <fstream>
#include <filesystem>
#include <iostream>
#include <algorithm>
#include <dirent.h>
#include "TpFile.h"

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
		if(part.empty())
		{
			currentPath = pathJoin("/", part);
			continue;
		}
        currentPath = pathJoin(currentPath, part);
        if (::mkdir(currentPath.c_str(), 0755) != 0 && errno != EEXIST)
            return false;
    }
    return true;
}

bool TpDir::removeRecursively(const TpString &dirPath)
{
    if (dirPath.empty())
        return false;

    if (!TpDir::exists(dirPath))
        return false;

    return system(("rm -rf " + dirPath).c_str()) == 0;
}

bool TpDir::exists(const TpString &dirPath)
{
    if (dirPath.empty())
        return false;

    struct stat info;
    return stat(dirPath.c_str(), &info) == 0 && S_ISDIR(info.st_mode);
}

uint64_t TpDir::size(const TpString &dirPath)
{
    uint64_t totalSize = 0;

    if (dirPath.empty())
        return totalSize;

    TpFileInfo inputDirInfo(dirPath);
    if (!inputDirInfo.isDir())
        return totalSize;

    DIR *dir;
    struct dirent *entry;
    struct stat statbuf;

    // 打开目录
    if ((dir = opendir(dirPath.c_str())) == nullptr)
        return 0;

    // 遍历目录项
    while ((entry = readdir(dir)) != nullptr)
    {
        // 跳过 "." 和 ".."
        if (strcmp(entry->d_name, ".") == 0 || strcmp(entry->d_name, "..") == 0)
        {
            continue;
        }

        TpString fullPath = dirPath + "/" + entry->d_name;

        // 获取文件/目录状态
        if (lstat(fullPath.c_str(), &statbuf) == -1)
        {
            // 无法获取状态，跳过
            continue;
        }

        if (S_ISDIR(statbuf.st_mode))
        {
            // 如果是目录，递归计算大小
            totalSize += TpDir::size(TpString(fullPath.c_str()));
        }
        else if (S_ISREG(statbuf.st_mode))
        {
            // 如果是普通文件，累加大小
            totalSize += statbuf.st_size;
        }
        else
        {
        }
        // 忽略符号链接和其他特殊文件
    }

    closedir(dir);
    return totalSize;
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



/// @brief 检查路径是文件还是目录
/// @param path 路径
/// @return 0:不存在, 1:文件, 2:目录
static int getPathType(const TpString &path)
{
    struct stat statbuf;
    if (lstat(path.c_str(), &statbuf) == -1) {
        return 0; // 不存在
    }
    
    if (S_ISDIR(statbuf.st_mode)) {
        return 2; // 目录
    } else if (S_ISREG(statbuf.st_mode)) {
        return 1; // 文件
    }
    
    return 0; // 其他类型视为不存在
}

/// @brief 专门为copy函数设计的路径处理函数（处理不存在的路径）
/// @param path 输入路径
/// @return 处理后的绝对路径
static TpString getPathForCopy(const TpString &path)
{
    if (path.empty()) {
        return "";
    }
    
    TpString result = path;
    std::replace(result.begin(), result.end(), '\\', '/');
    
    // 如果是绝对路径，直接返回
    if (!result.empty() && result[0] == '/') {
        return result;
    }
    
    // 对于相对路径，添加当前工作目录
    char cwd[PATH_MAX];
    if (getcwd(cwd, sizeof(cwd)) == nullptr) {
        // 如果获取当前目录失败，返回原始路径
        return result;
    }
    
    TpString currentDir(cwd);
    if (!currentDir.empty() && currentDir.back() != '/') {
        currentDir += '/';
    }
    
    // 组合路径
    result = currentDir + result;
    
    // 规范化路径：处理 . 和 ..
    TpVector<TpString> parts;
    size_t start = 0, end = 0;
    
    // 分割路径
    while ((end = result.find('/', start)) != TpString::npos) {
        if (end != start) {
            TpString part = result.substr(start, end - start);
            parts.push_back(part);
        }
        start = end + 1;
    }
    if (start < result.size()) {
        parts.push_back(result.substr(start));
    }
    
    // 处理 . 和 ..
    TpVector<TpString> normalizedParts;
    for (const auto &part : parts) {
        if (part == TpString(".")) {
            // 忽略当前目录
            continue;
        } else if (part == TpString("..")) {
            // 回退到上级目录
            if (!normalizedParts.empty()) {
                normalizedParts.pop_back();
            }
        } else if (!part.empty()) {
            normalizedParts.push_back(part);
        }
    }
    
    // 重建路径
    if (normalizedParts.empty()) {
        return "/";
    }
    
    TpString absolutePath;
    for (const auto &part : normalizedParts) {
        absolutePath += "/" + part;
    }
    
    // 如果原始路径以斜杠结尾，确保结果也以斜杠结尾
    if (!path.empty() && path.back() == '/' && !absolutePath.empty() && absolutePath.back() != '/') {
        absolutePath += '/';
    }
    
    return absolutePath;
}

/// @brief 获取最终的目标路径
/// @param sourcePath 源路径
/// @param destinationPath 目标路径
/// @param sourceIsDir 源路径是否是目录
/// @return 最终的目标路径
static TpString getFinalDestinationPath(const TpString &sourcePath, const TpString &destinationPath, bool sourceIsDir)
{
    TpString dest = destinationPath;
    
    // 如果目标路径以斜杠结尾，表示目标是一个目录
    if (!dest.empty() && dest.back() == '/') {
        // 获取源文件名或目录名
        TpString srcName;
        size_t lastSlash = sourcePath.find_last_of('/');
        if (lastSlash == TpString::npos) {
            srcName = sourcePath;
        } else {
            srcName = sourcePath.substr(lastSlash + 1);
        }
        
        if (srcName.empty()) {
            srcName = sourceIsDir ? "copied_directory" : "copied_file";
        }
        
        dest += srcName;
    }
    // 如果目标路径已存在且是目录，将源文件/目录放入该目录
    else if (TpDir::exists(dest)) {
        // 获取源文件名或目录名
        TpString srcName;
        size_t lastSlash = sourcePath.find_last_of('/');
        if (lastSlash == TpString::npos) {
            srcName = sourcePath;
        } else {
            srcName = sourcePath.substr(lastSlash + 1);
        }
        
        if (!srcName.empty()) {
            dest = pathJoin(dest, srcName);
        }
    }
    
    return dest;
}

/// @brief 递归拷贝目录的实现
/// @param sourcePath 源路径
/// @param destinationPath 目标路径
/// @return 拷贝成功返回true，否则返回false
static bool copyDirImpl(const TpString &sourcePath, const TpString &destinationPath)
{
    // 检查源目录是否存在
    if (!TpDir::exists(sourcePath)) {
        std::cerr << "Source directory does not exist: " << sourcePath << std::endl;
        return false;
    }
    
    // 创建目标目录（包括所有父级目录）
    if (!TpDir::mkpath(destinationPath)) {
        std::cerr << "Failed to create destination directory: " << destinationPath << std::endl;
        return false;
    }
    
    // 打开源目录
    DIR *dir = opendir(sourcePath.c_str());
    if (!dir) {
        std::cerr << "Failed to open source directory: " << sourcePath << std::endl;
        return false;
    }
    
    struct dirent *entry;
    bool success = true;
    
    // 遍历源目录中的所有条目
    while ((entry = readdir(dir)) != nullptr && success) {
        TpString name = entry->d_name;
        
        // 跳过 "." 和 ".."
        if (name == TpString(".") || name == TpString("..")) {
            continue;
        }
        
        TpString sourceEntryPath = pathJoin(sourcePath, name);
        TpString destinationEntryPath = pathJoin(destinationPath, name);
        
        // 获取文件信息
        struct stat statbuf;
        if (lstat(sourceEntryPath.c_str(), &statbuf) == -1) {
            std::cerr << "Failed to get file info: " << sourceEntryPath << std::endl;
            continue;
        }
        
        if (S_ISDIR(statbuf.st_mode)) {
            // 如果是目录，递归拷贝
            if (!copyDirImpl(sourceEntryPath, destinationEntryPath)) {
                success = false;
            }
        } else if (S_ISREG(statbuf.st_mode)) {
            // 如果是普通文件，使用TpFile::copy方法拷贝
            if (!TpFile::copy(sourceEntryPath, destinationEntryPath)) {
                std::cerr << "Failed to copy file: " << sourceEntryPath << std::endl;
                success = false;
            }
        } else if (S_ISLNK(statbuf.st_mode)) {
            // 如果是符号链接，跳过
            std::cout << "Skipping symlink: " << sourceEntryPath << std::endl;
        }
    }
    
    closedir(dir);
    return success;
}

/// @brief 拷贝单个文件
/// @param sourcePath 源文件路径
/// @param destinationPath 目标文件路径
/// @return 拷贝成功返回true，否则返回false
static bool copyFileImpl(const TpString &sourcePath, const TpString &destinationPath)
{
    // 检查源文件是否存在
    if (!TpFile::exists(sourcePath)) {
        std::cerr << "Source file does not exist: " << sourcePath << std::endl;
        return false;
    }
    
    // 创建目标文件的目录（如果不存在）
    TpString destDir = TpFileInfo(destinationPath).path();
    if (!TpDir::exists(destDir)) {
        if (!TpDir::mkpath(destDir)) {
            std::cerr << "Failed to create destination directory: " << destDir << std::endl;
            return false;
        }
    }
    
    // 使用TpFile::copy拷贝文件
    if (!TpFile::copy(sourcePath, destinationPath)) {
        std::cerr << "Failed to copy file: " << sourcePath << " to " << destinationPath << std::endl;
        return false;
    }
    
    return true;
}

bool TpDir::copy(const TpString &sourcePath, const TpString &destinationPath)
{
    if (sourcePath.empty() || destinationPath.empty()) {
        std::cerr << "Source or destination path is empty" << std::endl;
        return false;
    }
    
    // 使用专门的路径处理函数
    TpString absSourcePath = getPathForCopy(sourcePath);
    TpString absDestPath = getPathForCopy(destinationPath);
    
    if (absSourcePath.empty() || absDestPath.empty()) {
        std::cerr << "Failed to process paths" << std::endl;
        return false;
    }
    
    // 检查源路径是否存在
    int sourceType = getPathType(absSourcePath);
    if (sourceType == 0) {
        std::cerr << "Source path does not exist: " << absSourcePath << std::endl;
        return false;
    }
    
    bool sourceIsDir = (sourceType == 2);
    
    // 获取最终的目标路径
    TpString finalDestPath = getFinalDestinationPath(absSourcePath, absDestPath, sourceIsDir);
    
    // 检查目标路径是否是源路径的子目录（避免循环拷贝，仅对目录有效）
    if (sourceIsDir) {
        TpString tempSource = absSourcePath;
        if (tempSource.back() != '/') {
            tempSource += '/';
        }
        
        TpString tempDest = finalDestPath;
        if (tempDest.back() != '/') {
            tempDest += '/';
        }
        
        if (tempDest.find(tempSource) == 0) {
            std::cerr << "Destination is a subdirectory of source - would cause infinite loop" << std::endl;
            return false;
        }
    }
    
    // 检查源路径和目标路径是否相同
    if (absSourcePath == finalDestPath) {
        std::cerr << "Source and destination paths are the same" << std::endl;
        return false;
    }
    
    // 根据源路径类型选择拷贝方式
    if (sourceIsDir) {
        return copyDirImpl(absSourcePath, finalDestPath);
    } else {
        return copyFileImpl(absSourcePath, finalDestPath);
    }
}