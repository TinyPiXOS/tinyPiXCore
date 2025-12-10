#ifndef __TP_PANEL_SWITCH_BUTTON_H
#define __TP_PANEL_SWITCH_BUTTON_H

#include "TpWidget.h"
#include "TpSignalSlot.h"
#include "TpEvent.h"

TP_DEF_VOID_TYPE_VAR(ITpPanelSwitchButtonData);

/// @brief 面板开关按钮
class TpPanelSwitchButton : public TpWidget
{
public:
    TpPanelSwitchButton(TpWidget *parent);

    virtual ~TpPanelSwitchButton();

    /// @brief 设置图标全路径
    /// @param iconPath icon路径
    void setIcon(const TpString &iconPath);

    /// @brief 设置文本内容
    /// @param text 文本
    void setText(const TpString &text);
    /// @brief 获取当前按钮文本
    /// @return 文本字符串
    TpString text();

public:
    virtual TpString pluginType() { return TO_STRING(TpPanelSwitchButton); }

public
signals:
    declare_signal(onClicked, bool);

protected:
    virtual bool onResizeEvent(TpResizeEvent *event) override;

    virtual bool onMousePressEvent(TpMouseEvent *event) override;
    virtual bool onMouseRleaseEvent(TpMouseEvent *event) override;
    virtual bool onPaintEvent(TpPaintEvent *event) override;
    virtual bool onLeaveEvent(TpLeaveEvent *event) override;
    virtual bool eventFilter(TpObject *watched, TpEvent *event) override;

private:
    ITpPanelSwitchButtonData *data_;
};

#endif // __TP_COMBOX_H
