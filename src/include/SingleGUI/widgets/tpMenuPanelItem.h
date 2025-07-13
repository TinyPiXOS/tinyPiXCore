#ifndef __TP_MENU_PANEL_ITEM_H
#define __TP_MENU_PANEL_ITEM_H

#include "tpChildWidget.h"
#include "tpSignalSlot.h"
#include "tpString.h"

TP_DEF_VOID_TYPE_VAR(ItpMenuPanelItemData);

/// @brief 菜单面板Item单元
class tpMenuPanelItem : public tpChildWidget
{
public:
    tpMenuPanelItem(tpChildWidget *parent = nullptr);

    virtual ~tpMenuPanelItem();

    /// @brief 设置icon绝对路径，不设置则不显示
    /// @param iconPath 图标的绝对路径
    void setIcon(const tpString &iconPath);

    /// @brief 设置主标题,不设置则不显示
    /// @param title 标题的文本
    void setTitle(const tpString &text);

    /// @brief 设置子标题，不设置则不显示
    /// @param text 子标题文本
    void setSubTitle(const tpString &text);

    /// @brief 设置自定义窗体，窗体会被添加到最右侧，会覆盖子标题和末尾箭头，给入空则会清除自定义窗体
    /// @param widget 窗体指针
    void setCustomizeWidget(tpChildWidget *widget);

    /// @brief 获取自定义窗体；无则返回nullptr
    /// @return 自定义窗体指针
    tpChildWidget* customizeWidget();

public
signals:
    /// @brief 按钮点击信号槽，鼠标释放时触发
    /// @param bool 按钮选中状态
    declare_signal(onClicked, bool);

protected:
    virtual bool onMouseRleaseEvent(tpMouseEvent *event) override;
    virtual bool onPaintEvent(tpObjectPaintEvent *event) override;
    virtual bool onResizeEvent(tpObjectResizeEvent *event) override;

    virtual bool eventFilter(tpObject *watched, tpEvent *event) override;

    /// @brief
    /// @param event
    virtual void onThemeChangeEvent(tpThemeChangeEvent *event) override;

protected:
    virtual tpString pluginType() override { return TO_STRING(tpMenuPanelItem); }

private:
    ItpMenuPanelItemData *data_;
};

#endif
