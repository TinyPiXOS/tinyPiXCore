#ifndef __TP_CONFIG_H
#define __TP_CONFIG_H

#include <TpCore.h>
#include "TpVector.h"

TP_DEF_VOID_TYPE_VAR(ITpConfigData);
/// @brief INI配置文件读写类
/// 该类提供了读取、写入和操作INI格式配置文件的功能[1,2](@ref)。
/// INI文件由节(Section)、键(Key)和值(Value)组成，格式如下：
/// [SectionName]
/// Key1=Value1
/// Key2=Value2[1,2](@ref)
class TpConfig
{
public:
    /// @brief INI文件访问状态枚举
    enum Status
    {
        /// @brief 无异常；正常读写
        NoError = 0,
        /// @brief 权限错误；文件访问失败
        AccessError,
        /// @brief 格式化错误；文件格式非INI格式或格式有误
        FormatError
    };

public:
    /// @brief 默认构造函数
    TpConfig();

    /// @brief 构造函数，同时加载指定INI文件
    /// @param fileName 要加载的INI文件路径
    TpConfig(const TpString &fileName);

    /// @brief 析构函数
    virtual ~TpConfig();

public:
    /// @brief 加载指定的INI文件
    /// @param fileName 要加载的INI文件路径
    /// @return 成功加载返回true，否则返回false
    bool load(const TpString &fileName);

    /// @brief 获取当前关联的INI文件名
    /// @return 当前关联的文件名
    TpString fileName() const;

    /// @brief 清除所有配置数据
    void clear();

    /// @brief 将内存中的配置数据同步到文件
    void sync();

    /// @brief 获取当前操作状态
    /// @return 当前状态值
    Status status() const;

    /// @brief 开始一个配置组（节）
    /// @param prefix 组名前缀，实际节名为当前组路径+prefix
    void beginGroup(const TpString &prefix);

    /// @brief 结束当前配置组
    void endGroup();

    /// @brief 获取当前组路径
    /// @return 当前组路径字符串
    TpString group() const;

    /// @brief 获取所有键名（包含完整组路径）
    /// @return 包含所有键名的向量
    TpVector<TpString> allKeys() const;

    /// @brief 获取当前组下的所有键名
    /// @return 包含当前组下所有键名的向量
    TpVector<TpString> childKeys() const;

    /// @brief 获取当前组下的所有子组名
    /// @return 包含当前组下所有子组名的向量
    TpVector<TpString> childGroups() const;

    /// @brief 检查文件是否可写
    /// @return 文件可写返回true，否则返回false
    bool isWritable() const;

    /// @brief 设置指定键的值
    /// @param key 键名（支持组路径，如"Group/Key"）
    /// @param value 要设置的值
    void setValue(const TpString &key, const TpString &value);

    /// @brief 获取指定键的值
    /// @param key 键名（支持组路径，如"Group/Key"）
    /// @return 键对应的值，不存在时返回空
    TpString value(const TpString &key) const;

    /// @brief 移除指定键
    /// @param key 要移除的键名
    void remove(const TpString &key);

    /// @brief 检查是否包含指定键
    /// @param key 要检查的键名
    /// @return 存在返回true，否则返回false
    bool contains(const TpString &key) const;

private:
    ITpConfigData *data_; 
};

#endif