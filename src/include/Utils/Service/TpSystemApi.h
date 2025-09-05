#ifndef __TP_SYSTEM_API_H
#define __TP_SYSTEM_API_H

#include "TpUtils.h"
#include "TpSize.h"

TP_DEF_VOID_TYPE_VAR(ItpSystemApiData);
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

public:
    /// @brief 获取唯一单例
    /// @return 实例指针
    static TpSystemApi *Instance();

public:
    /// @brief 根据文件类型启动对应应用打开文件
    /// @param filePath 文件绝对路径
    /// @param appUuid 指定打开文件的应用UUID；为空则使用系统默认
    OpenFileError openFile(const TpString &filePath, const TpString &appUuid = "");

    /// @brief 指定UUID通知小组件绘制画布尺寸变化；小组件更新画布并重绘
    /// @param widgetUuid 小组件UUID
    void notifyWidgetsResize(const TpString& widgetUuid, const TpSize& widgetSize);

    /// @brief 指定UUID通知小组件进行重绘
    /// @param widgetUuid 小组件UUID
    void notifyWidgetsPaint(const TpString& widgetUuid);

    // void setRecvUpdate();

public:
    /// @brief 禁用拷贝构造
    TpSystemApi(const TpSystemApi &) = delete;
    /// @brief 禁用赋值构造
    TpSystemApi &operator=(const TpSystemApi &) = delete;

private:
    TpSystemApi();
    virtual ~TpSystemApi();

private:
    ItpSystemApiData *data_;
};

#endif
