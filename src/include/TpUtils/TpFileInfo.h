#ifndef __TP_FILEINFO_H
#define __TP_FILEINFO_H

#include <TpCore.h>
#include <TpString.h>
#include <TpList.h>

/// @brief tpFileInfo内部数据的不透明类型定义
TP_DEF_VOID_TYPE_VAR(ItpFileInfoData);

class TpDir;
/// @brief 文件信息类，提供文件和目录的元数据查询
class TpFileInfo
{
public:
    /// @brief 默认构造函数
    TpFileInfo();
    
    /// @brief 构造函数，使用指定文件路径初始化
    /// @param file 目标文件或目录的路径
    TpFileInfo(const TpString &file);

    /// @brief 析构函数
    ~TpFileInfo();

    /// @brief 赋值运算符重载
    /// @param other 要复制的文件信息对象
    TpFileInfo operator=(const TpFileInfo &other);

public:
    /// @brief 检查指定文件是否存在
    /// @param file 要检查的文件路径
    /// @return 存在返回true，否则返回false
    static bool exists(const TpString &file);

public:
    /// @brief 设置要查询的文件路径
    /// @param file 新的文件路径
    void setFile(const TpString &file);

    /// @brief 检查当前文件是否存在
    /// @return 存在返回true，否则返回false
    bool exists() const;

    /// @brief 获取完整文件路径（包括文件名）
    /// @return 完整的文件路径
    TpString filePath() const;
    
    /// @brief 获取绝对文件路径
    /// @return 绝对路径表示的文件完整路径
    TpString absoluteFilePath() const;
    
    /// @brief 获取规范化路径（解析符号链接）
    /// @return 解析所有符号链接后的规范路径
    TpString canonicalFilePath() const;
    
    /// @brief 获取文件名部分（不含目录）
    /// @return 文件名部分
    TpString fileName() const;
    
    /// @brief 获取基本名称（不含扩展名）
    /// @return 文件基本名称
    TpString baseName() const;
    
    /// @brief 获取文件扩展名
    /// @return 文件扩展名（不含点号）
    TpString suffix() const;

    /// @brief 获取文件所在目录路径
    /// @return 文件所在的目录路径
    TpString path() const;
    
    /// @brief 获取文件所在的绝对目录路径
    /// @return 文件所在的绝对目录路径
    TpString absolutePath() const;
    
    /// @brief 获取规范化目录路径（解析符号链接）
    /// @return 解析所有符号链接后的规范目录路径
    TpString canonicalPath() const;
    
    /// @brief 获取文件所在目录对象
    /// @return 表示文件所在目录的tpDir对象
    TpDir dir() const;
    
    /// @brief 获取文件所在的绝对目录对象
    /// @return 表示文件绝对路径目录的tpDir对象
    TpDir absoluteDir() const;

    /// @brief 检查文件是否可读
    /// @return 可读返回true，否则返回false
    bool isReadable() const;
    
    /// @brief 检查文件是否可写
    /// @return 可写返回true，否则返回false
    bool isWritable() const;
    
    /// @brief 检查文件是否可执行
    /// @return 可执行返回true，否则返回false
    bool isExecutable() const;
    
    /// @brief 检查文件是否为隐藏文件
    /// @return 隐藏文件返回true，否则返回false
    bool isHidden() const;
    
    /// @brief 检查路径是否为本地路径
    /// @return 本地路径返回true，否则返回false
    bool isNativePath() const;

    /// @brief 检查路径是否为相对路径
    /// @return 相对路径返回true，否则返回false
    bool isRelative() const;
    
    /// @brief 检查路径是否为绝对路径
    /// @return 绝对路径返回true，否则返回false
    inline bool isAbsolute() const { return !isRelative(); }

    /// @brief 检查是否为普通文件
    /// @return 普通文件返回true，否则返回false
    bool isFile() const;
    
    /// @brief 检查是否为目录
    /// @return 目录返回true，否则返回false
    bool isDir() const;
    
    /// @brief 检查是否为符号链接
    /// @return 符号链接返回true，否则返回false
    bool isSymLink() const;
    
    /// @brief 检查是否为根目录
    /// @return 根目录返回true，否则返回false
    bool isRoot() const;

    /// @brief 获取文件大小（字节数）
    /// @return 文件大小（字节数）
    uint64_t size() const;
    
    /// @brief 获取最后修改时间
    /// @param format 时间格式字符串（默认为"年-月-日 时:分:秒"）
    /// @return 格式化后的最后修改时间字符串
    TpString lastModified(const TpString &format = "%Y-%m-%d %H:%M:%S") const;

private:
    /// @brief 解析路径的辅助方法
    /// @param path 需要解析的路径
    TpString resolvePath(const TpString &path) const;

private:
    ItpFileInfoData *data_; 
};

/// @brief 文件信息列表类型定义
typedef TpList<TpFileInfo> TpFileInfoList;

#endif