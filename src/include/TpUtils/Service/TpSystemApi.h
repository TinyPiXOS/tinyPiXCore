#ifndef __TP_SYSTEM_API_H
#define __TP_SYSTEM_API_H

#include <TpCore.h>
#include "TpSize.h"
#include "TpVector.h"
#include "Service/TpAppConfigIO.h"

class TpImage;

TP_DEF_VOID_TYPE_VAR(ITpSystemApiData);
/// @brief 系统桌面级API功能接口类
class TpSystemApi
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
    static TpSystemApi *Instance();

public:
    /// @brief 根据文件类型启动对应应用打开文件
    /// @param filePath 文件绝对路径
    /// @param appUuid 指定打开文件的应用UUID；为空则使用系统默认
    static OpenFileError openFile(const TpString &filePath, const TpString &appUuid = "");

    /// @brief 指定UUID通知小组件绘制画布尺寸变化；小组件更新画布并重绘；暂未实现
    /// @param widgetUuid 小组件UUID
    void notifyWidgetsResize(const TpString &widgetUuid, const TpSize &widgetSize);

    /// @brief 指定UUID通知小组件进行重绘；暂未实现
    /// @param widgetUuid 小组件UUID
    void notifyWidgetsPaint(const TpString &widgetUuid);

    /// @brief 指定应用UUID获取该应用缩略图；若应用未运行则返回空对象
    /// @param uuid 应用UUID
    /// @return 应用缩略图；若应用未运行则返回空对象
    TpImage appImage(const TpString &uuid);

    /// @brief 返回桌面；调用后会直接返回桌面显示
    /// @return 执行结果
    bool home();

    /// @brief 指定应用UUID启动该应用；若应用已启动则将应用重新置顶显示
    /// @param uuid 应用UUID
    /// @param argList 应用启动参数
    /// @return 启动应用结果
    bool startApp(const TpString &uuid, const TpVector<TpString> &argList = TpVector<TpString>());

    /// @brief 指定应用UUID停止应用进程
    /// @param uuid 应用UUID
    /// @return 停止结果；应用进程未运行同样返回false
    bool killApp(const TpString &uuid);

    /// @brief 停止当前运行的所有应用进程
    /// @return 停止结果
    bool killAllApp();

    /// @brief 获取当前运行应用信息列表
    /// @return 应用信息列表
    TpVector<RunAppInfo> runAppInfoList();

    /// @brief 指定应用UUID获取应用运行信息
    /// @param uuid 应用UUID
    /// @return 该应用的进程信息；若应用未运行则返回空
    RunAppInfo runAppInfo(const TpString &uuid);

public:
    /// @brief 禁用拷贝构造
    TpSystemApi(const TpSystemApi &) = delete;
    /// @brief 禁用赋值构造
    TpSystemApi &operator=(const TpSystemApi &) = delete;

private:
    TpSystemApi();
    virtual ~TpSystemApi();

private:
    ITpSystemApiData *data_;
};

#endif
