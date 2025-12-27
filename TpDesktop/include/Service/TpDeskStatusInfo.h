#ifndef __TP_DESKTOP_STATUS_INFO_H
#define __TP_DESKTOP_STATUS_INFO_H

#include <TpCore.h>
#include <InteractData/TpDesktopData.h>

TP_DEF_VOID_TYPE_VAR(ITpDeskStatusInfoData);
/// @brief 系统桌面状态栏信息
class TpDeskStatusInfo
{
    friend class TpDesktopMainWindow;
    
public:
    /// @brief 获取唯一单例
    /// @return 实例指针
    static TpDeskStatusInfo *Instance();

    /// @brief 获取状态栏信息
    /// @return 状态栏信息
    const TpDeskStatusBarInfo &statusInfo();

protected:
    /// @brief 更新状态栏信息；仅桌面调用有效
    /// @param info 状态栏信息
    void setStatusInfo(const TpDeskStatusBarInfo &info);

public:
    /// @brief 禁用拷贝构造
    TpDeskStatusInfo(const TpDeskStatusInfo &) = delete;
    /// @brief 禁用赋值构造
    TpDeskStatusInfo &operator=(const TpDeskStatusInfo &) = delete;

private:
    TpDeskStatusInfo();
    virtual ~TpDeskStatusInfo();

private:
    ITpDeskStatusInfoData *data_;
};

#endif
