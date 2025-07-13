#ifndef __TP_SYSTEM_API_H
#define __TP_SYSTEM_API_H

#include "tpUtils.h"

TP_DEF_VOID_TYPE_VAR(ItpSystemApiData);
/// @brief 系统桌面级API功能接口类
class tpSystemApi
{
public:
    /// @brief 打开文件错误码
    enum OpenFileError
    {
        /// @brief 打开成功
        Succsssful,
        /// @brief 打开文件不存在
        FileNotExist,
        /// @brief 不支持文件类型
        NotSupport,
        /// @brief 系统文件损坏
        SystemFileDamage
    };

public:
    /// @brief 获取唯一单例
    /// @return 实例指针
    static tpSystemApi *Instance();

public:
    /// @brief 根据文件类型启动对应应用打开文件
    /// @param filePath 文件绝对路径
    /// @param appUuid 指定打开文件的应用UUID；为空则使用系统默认
    OpenFileError openFile(const tpString &filePath, const tpString &appUuid = "");

public:
    /// @brief 禁用拷贝构造
    tpSystemApi(const tpSystemApi &) = delete;
    /// @brief 禁用赋值构造
    tpSystemApi &operator=(const tpSystemApi &) = delete;

private:
    tpSystemApi();
    virtual ~tpSystemApi();

private:
    ItpSystemApiData *data_;
};

#endif
