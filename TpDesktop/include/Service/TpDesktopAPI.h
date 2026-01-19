#ifndef __TP_DESKTOP_API_H
#define __TP_DESKTOP_API_H

#include <TpCore.h>
#include "TpSize.h"
#include "TpVector.h"
#include "AppManage/TpAppConfigIO.h"

class TpImage;

TP_DEF_VOID_TYPE_VAR(ITpDesktopAPIData);
/// @brief 系统桌面提供的API功能接口类
class TpDesktopAPI
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

    /// @brief 正在运行的应用信息
    struct RunAppInfo
    {
        TpAppConfigIO appInfo;
        int32_t pid;

        RunAppInfo() : pid(0)
        {
        }

        bool isEmpty() { return appInfo.appUuid().empty(); }
    };

public:
    /// @brief 获取唯一单例
    /// @return 实例指针
    static TpDesktopAPI *Instance();

public:
    /// @brief 根据文件类型启动对应应用打开文件
    /// @param filePath 文件绝对路径
    /// @param appUuid 指定打开文件的应用UUID；为空则使用系统默认
    static OpenFileError openFile(const TpString &filePath, const TpString &appUuid = "");

    /// @brief 返回桌面；调用后会直接返回桌面显示
    /// @return 执行结果
    bool home();

    /// @brief 设置桌面状态栏样式;会自动将状态栏设置为显示状态
    /// @param rgba 状态栏背景色
    /// @return 设置结果
    bool setStatusBarStyle(int32_t rgba);

    /// @brief 设置状态栏显隐状态
    /// @param visible true=显示；false=隐藏
    /// @return 设置结果
    bool setStatusBarVisible(bool visible);

public:
    /// @brief 禁用拷贝构造
    TpDesktopAPI(const TpDesktopAPI &) = delete;
    /// @brief 禁用赋值构造
    TpDesktopAPI &operator=(const TpDesktopAPI &) = delete;

private:
    TpDesktopAPI();
    virtual ~TpDesktopAPI();

private:
    ITpDesktopAPIData *data_;
};

#endif
