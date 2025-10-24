#ifndef __TP_MENU_PANEL_ITEM_H
#define __TP_MENU_PANEL_ITEM_H

#include "TpWidget.h"
#include "TpSignalSlot.h"
#include "TpString.h"

TP_DEF_VOID_TYPE_VAR(ItpMenuPanelItemData);

/// @brief 菜单面板Item单元
class TpMenuPanelItem : public TpWidget
{
public:
    TpMenuPanelItem(TpWidget *parent = nullptr);

    virtual ~TpMenuPanelItem();

    /// @brief 设置icon绝对路径，不设置则不显示
    /// @param iconPath 图标的绝对路径
    void setIcon(const TpString &iconPath);

    /// @brief 设置主标题,不设置则不显示
    /// @param title 标题的文本
    void setTitle(const TpString &text);

    /// @brief 设置子标题，不设置则不显示
    /// @param text 子标题文本
    void setSubTitle(const TpString &text);

    /// @brief 设置自定义窗体，窗体会被添加到最右侧，会覆盖子标题和末尾箭头，给入空则会清除自定义窗体
    /// @param widget 窗体指针
    void setCustomizeWidget(TpWidget *widget);

    /// @brief 获取自定义窗体；无则返回nullptr
    /// @return 自定义窗体指针
    TpWidget* customizeWidget();

public
signals:
    /// @brief 按钮点击信号槽，鼠标释放时触发
    /// @param bool 按钮选中状态
    declare_signal(onClicked, bool);

protected:
	virtual bool onMousePressEvent(TpMouseEvent *event) override;
    virtual bool onMouseRleaseEvent(TpMouseEvent *event) override;
	virtual bool onMouseMoveEvent(TpMouseEvent *event) override;
    virtual bool eventFilter(TpObject *watched, TpEvent *event) override;

protected:
    virtual TpString pluginType() override { return TO_STRING(TpMenuPanelItem); }

private:
    ItpMenuPanelItemData *data_;
};

#endif
