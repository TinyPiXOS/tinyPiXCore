#ifndef __TP_DIR_H
#define __TP_DIR_H

#include <tpUtils.h>
#include <tpString.h>
#include <tpFileInfo.h>

TP_DEF_VOID_TYPE_VAR(ItpDirData);
/// @brief 目录操作类，提供文件和目录操作功能
class tpDir
{
public:
    /// @brief 文件过滤枚举，用于指定entryInfoList返回的条目类型
    enum Filter
    {
        Dirs = 0x001,                       ///< @brief 包含目录
        Files = 0x002,                      ///< @brief 包含文件
        Drives = 0x004,                     ///< @brief 包含驱动器
        NoSymLinks = 0x008,                 ///< @brief 排除符号链接
        AllEntries = Dirs | Files | Drives, ///< @brief 包含所有条目（目录、文件、驱动器）
        TypeMask = 0x00f,                   ///< @brief 类型过滤掩码

        // Readable = 0x010,                ///< (已注释)可读文件
        // Writable = 0x020,                ///< (已注释)可写文件
        // Executable = 0x040,              ///< (已注释)可执行文件

        Hidden = 0x100,                     ///< @brief 包含隐藏文件和目录

        AccessMask = 0x3F0,                 ///< @brief 访问权限过滤掩码

        AllDirs = 0x400,                   ///< @brief 包含所有目录（包括.和..）
        CaseSensitive = 0x800,             ///< @brief 区分大小写的过滤
        NoDot = 0x2000,                    ///< @brief 排除当前目录(.)
        NoDotDot = 0x4000,                 ///< @brief 排除上级目录(..)
        NoDotAndDotDot = NoDot | NoDotDot, ///< @brief 排除当前和上级目录(./..)

        NoFilter = -1 ///< @brief 无过滤条件
    };
    typedef int32_t Filters; ///< @brief 过滤器类型（可组合使用）

    /// @brief 排序标志枚举，用于指定entryInfoList的排序方式
    enum SortFlag
    {
        Name = 0x00,     ///< @brief 按名称排序
        Time = 0x01,     ///< @brief 按修改时间排序
        Size = 0x02,     ///< @brief 按文件大小排序
        Unsorted = 0x03, ///< @brief 不排序（文件系统原始顺序）

        DirsFirst = 0x04, ///< @brief 目录排在文件前
        Reversed = 0x08,  ///< @brief 反向排序
        DirsLast = 0x20,  ///< @brief 目录排在文件后
        Type = 0x80,      ///< @brief 按文件类型排序

        NoSort = -1 ///< @brief 无排序
    };
    typedef int32_t SortFlags; ///< @brief 排序标志类型（可组合使用）

public:
    /// @brief 默认构造函数，创建空目录对象
    tpDir();

    /// @brief 构造函数，指定初始路径
    /// @param path 初始目录路径
    tpDir(const tpString &path);

    /// @brief 析构函数，清理资源
    ~tpDir();

public:
    /// @brief 静态方法：递归创建目录
    /// @param dirPath 要创建的完整目录路径
    /// @return 创建成功返回true，否则返回false
    static bool mkpath(const tpString &dirPath);

public:
    /// @brief 设置目录路径
    /// @param path 新的目录路径
    void setPath(const tpString &path);

    /// @brief 获取当前设置的目录路径
    /// @return 目录路径
    tpString path() const;

    /// @brief 获取目录的绝对路径
    /// @return 绝对路径
    tpString absolutePath() const;

    /// @brief 获取目录的规范路径（解析所有符号链接和相对路径）
    /// @return 规范路径
    tpString canonicalPath() const;

    /// @brief 获取目录名称
    /// @return 目录名称
    tpString dirName() const;

    /// @brief 获取指定文件在目录中的完整路径
    /// @param fileName 文件名
    /// @return 文件在目录中的完整路径
    tpString filePath(const tpString &fileName) const;

    /// @brief 获取指定文件的绝对路径
    /// @param fileName 文件名
    /// @return 文件的绝对路径
    tpString absoluteFilePath(const tpString &fileName) const;

    /// @brief 获取相对于当前目录的相对路径
    /// @param fileName 目标文件的绝对路径
    /// @return 相对于当前目录的相对路径
    tpString relativeFilePath(const tpString &fileName) const;

    /// @brief 获取目录下的文件信息列表
    /// @param filters 过滤条件（默认为无过滤）
    /// @param sort 排序标志（默认为不排序）
    /// @return 文件信息列表
    tpFileInfoList entryInfoList(Filters filters = NoFilter, SortFlags sort = NoSort) const;

    // tpFileInfoList entryInfoList(const tpList<tpString> &nameFilters, Filter filters = NoFilter,
    //                              SortFlag sort = NoSort) const;

    /// @brief 在当前目录下创建子目录
    /// @param dirName 要创建的子目录名称
    /// @return 创建成功返回true，否则返回false
    bool mkdir(const tpString &dirName) const;

    /// @brief 检查目录是否存在
    /// @return 存在返回true，否则返回false
    bool exists() const;

    /// @brief 删除目录下的文件
    /// @param fileName 要删除的文件名
    /// @return 删除成功返回true，否则返回false
    bool remove(const tpString &fileName);

    /// @brief 递归删除目录及其所有内容
    /// @return 删除成功返回true，否则返回false
    bool removeRecursively();

private:
    ItpDirData *data_;
};

#endif