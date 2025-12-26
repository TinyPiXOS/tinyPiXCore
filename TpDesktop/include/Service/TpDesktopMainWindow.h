#ifndef __TP_DESKTOPMAIN_WINDOW_H
#define __TP_DESKTOPMAIN_WINDOW_H

#include "TpMainWindow.h"

TP_DEF_VOID_TYPE_VAR(ITpDesktopMainWindowData);
/// @brief 应用主窗体，每个应用只能拥有一个TpDesktopMainWindow;开发在本桌面运行的应用必须使用本类作为主窗体
class TpDesktopMainWindow
    : public TpMainWindow
{
public:
    TpDesktopMainWindow();
    virtual ~TpDesktopMainWindow();

public:
    /// @brief 组件类名，子类实现，返回子类类名字符串，用于匹配CSS中对应样式
    /// @return 类名字符串
    virtual TpString pluginType() override { return TO_STRING(TpDesktopMainWindow); }

private:
    ITpDesktopMainWindowData *data_;
};

#endif
