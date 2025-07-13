#ifndef __TP_FILE_H
#define __TP_FILE_H

#include "tpUtils.h"
#include "tpString.h"
#include "tpFileInfo.h"

/// @brief tpFile内部数据的不透明类型定义
TP_DEF_VOID_TYPE_VAR(ItpFileData);
/// @brief 文件操作类，提供文件处理和I/O功能
class tpFile
{
public:
    /// @brief 文件打开模式标志枚举
    enum OpenModeFlag
    {
        /// @brief 文件未打开
        NotOpen = 0x0000,
        /// @brief 以只读模式打开文件
        ReadOnly = 0x0001,
        /// @brief 以只写模式打开文件
        WriteOnly = 0x0002,
        /// @brief 以读写模式打开文件
        ReadWrite = ReadOnly | WriteOnly,
        /// @brief 以追加模式打开文件（写入位置在文件末尾）
        Append = 0x0004,
		ReadWriteAppend =0x0008,
        /// @brief （保留用于将来）打开时清空文件
        // Truncate = 0x0008,
        /// @brief （保留用于将来）文本模式标志
        // Text = 0x0010,
    };

public:
    /// @brief 默认构造函数 - 创建未初始化的文件对象
    tpFile();

    /// @brief 构造函数 - 使用指定文件名初始化
    /// @param _fileName 目标文件的完整路径
    tpFile(const tpString &_fileName);

    /// @brief 析构函数 - 自动关闭已打开的文件
    ~tpFile();

public:
    /// @brief 检查文件系统中是否存在指定文件
    /// @param fileName 要检查的文件完整路径
    /// @return 存在返回true，否则返回false
    static bool exists(const tpString &fileName);

    /// @brief 删除指定文件
    /// @param fileName 要删除的文件完整路径
    /// @return 成功删除返回true，否则返回false
    static bool remove(const tpString &fileName);

    /// @brief 重命名/移动文件到新位置
    /// @param oldName 文件当前完整路径
    /// @param newName 文件新完整路径
    /// @return 重命名成功返回true，否则返回false
    static bool rename(const tpString &oldName, const tpString &newName);

    /// @brief 复制文件到新位置
    /// @param fileName 要复制的源文件路径
    /// @param newName 目标文件路径
    /// @return 复制成功返回true，否则返回false
    static bool copy(const tpString &fileName, const tpString &newName);

public:
    /// @brief 获取当前关联的文件名
    /// @return 当前文件名的tpString对象
    tpString fileName() const;

    /// @brief 设置新的文件名
    /// @param name 新关联的文件路径
    void setFileName(const tpString &name);

    /// @brief 获取文件的详细信息
    /// @return 包含文件元数据的tpFileInfo对象
    tpFileInfo fileInfo();

    /// @brief 检查当前关联的文件是否存在
    /// @return 存在返回true，否则返回false
    bool exists() const;

    /// @brief 删除当前关联的文件
    /// @return 成功删除返回true，否则返回false
    bool remove();

    /// @brief 重命名当前关联的文件
    /// @param newName 新的文件完整路径
    /// @return 重命名成功返回true，否则返回false
    bool rename(const tpString &newName);

    /// @brief 创建当前文件的副本
    /// @param newName 副本文件的完整路径
    /// @return 复制成功返回true，否则返回false
    bool copy(const tpString &newName);

    /// @brief 获取文件大小
    /// @return 文件大小（字节数），不可用则返回0
    uint64_t size() const;

    /// @brief 以指定模式打开文件
    /// @param mode 打开模式标志组合
    /// @return 成功打开返回true，否则返回false
    bool open(OpenModeFlag mode);

    /// @brief 检查文件是否已打开
    /// @return 已打开返回true，否则返回false
    bool isOpen() const;

    /// @brief 检查文件是否可读（以读模式打开）
    /// @return 可读返回true，否则返回false
    bool isReadable() const;

    /// @brief 检查文件是否可写（以写模式打开）
    /// @return 可写返回true，否则返回false
    bool isWritable() const;

    /// @brief 关闭当前打开的文件
    void close();

    /// @brief 获取当前读写位置
    /// @return 当前文件位置（字节偏移量）
    uint64_t pos() const;

    /// @brief 设置文件读写位置
    /// @param offset 目标位置字节偏移量
    /// @return 定位成功返回true，否则返回false
    bool seek(uint64_t offset);

    /// @brief 检查是否到达文件末尾
    /// @return 到达文件尾返回true，否则返回false
    bool atEnd() const;

    /// @brief 刷新写缓冲区到磁盘
    /// @return 刷新成功返回true，否则返回false
    bool flush();

    /// @brief 从文件读取数据到缓冲区
    /// @param[out] data 接收数据的缓冲区
    /// @param maxlen 最多读取的字节数
    /// @return 实际读取的字节数
    uint64_t read(char *data, uint64_t maxlen);

    /// @brief 读取最多指定字节数作为字符串
    /// @param maxlen 最多读取的字节数
    /// @return 包含读取数据的tpString对象
    tpString read(uint64_t maxlen);

    /// @brief 读取文件全部内容
    /// @return 包含文件完整内容的tpString对象
    tpString readAll();

    /// @brief 读取一行数据到缓冲区
    /// @param[out] data 接收行数据的缓冲区
    /// @param maxlen 最多读取的字节数
    /// @return 实际读取的字节数（包含行终止符）
    uint64_t readLine(char *data, uint64_t maxlen);

    /// @brief 读取一行数据作为字符串
    /// @param maxlen 最多读取的字节数（0表示无限制）
    /// @return 包含行数据的tpString对象
    tpString readLine(uint64_t maxlen = 0);

    /// @brief 写入二进制数据到文件
    /// @param data 要写入的数据缓冲区
    /// @param len 要写入的字节数
    /// @return 实际写入的字节数
    uint64_t write(const char *data, uint64_t len);

    /// @brief 写入C字符串到文件
    /// @param data 要写入的C字符串（空值结尾）
    /// @return 实际写入的字节数（不包含终止符）
    uint64_t write(const char *data);

    /// @brief 写入tpString数据到文件
    /// @param data 要写入的字符串数据
    /// @return 实际写入的字节数
    uint64_t write(const tpString &data);

private:
    ItpFileData *data_; 
};

#endif