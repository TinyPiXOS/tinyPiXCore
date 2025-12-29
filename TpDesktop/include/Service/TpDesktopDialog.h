#ifndef __TP_DESKTOP_DIALOG_H
#define __TP_DESKTOP_DIALOG_H

#include "TpDialog.h"

class TpDesktopDialog
    : public TpDialog
{
public:
    TpDesktopDialog();

    virtual ~TpDesktopDialog();

    virtual void move(int32_t x, int32_t y) override;

public:
    /// @brief 组件类名，子类实现，返回子类类名字符串，用于匹配CSS中对应样式
    /// @return 类名字符串
    virtual TpString pluginType() override { return TO_STRING(TpDesktopDialog); }
};

#endif
