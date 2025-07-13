#ifndef __TP_PANEL_SWITCH_BUTTON_H
#define __TP_PANEL_SWITCH_BUTTON_H

#include "tpChildWidget.h"
#include "tpSignalSlot.h"
#include "tpEvent.h"

TP_DEF_VOID_TYPE_VAR(ItpPanelSwitchButtonData);

class tpPanelSwitchButton : public tpChildWidget
{
public:
    tpPanelSwitchButton(tpChildWidget *parent);

    virtual ~tpPanelSwitchButton();

    /// @brief 设置图标全路径
    /// @param iconPath icon路径
    void setIcon(const tpString &iconPath);

    /// @brief 设置文本内容
    /// @param text 文本
    void setText(const tpString &text);
    /// @brief 获取当前按钮文本
    /// @return 文本字符串
    tpString text();

public:
    virtual tpString pluginType() { return TO_STRING(tpPanelSwitchButton); }

public
signals:
    declare_signal(onClicked, bool);

protected:
    virtual bool onResizeEvent(tpObjectResizeEvent *event) override;

    virtual bool onMousePressEvent(tpMouseEvent *event) override;
    virtual bool onMouseRleaseEvent(tpMouseEvent *event) override;
    virtual bool onPaintEvent(tpObjectPaintEvent *event) override;
    virtual bool onLeaveEvent(tpObjectLeaveEvent *event) override;
    virtual bool eventFilter(tpObject *watched, tpEvent *event) override;

private:
    ItpPanelSwitchButtonData *data_;
};

#endif // __TP_COMBOX_H
