#ifndef __TP_MENU_PANEL_WIDGET_H
#define __TP_MENU_PANEL_WIDGET_H

#include "tpChildWidget.h"
#include "tpSignalSlot.h"
#include "tpString.h"
#include "tpMenuPanelItem.h"

TP_DEF_VOID_TYPE_VAR(ItpMenuPanelWidgetData);

/// @brief 菜单面板窗体,item只能单选；失去焦点时取消所有选中；选中后不可取消选中
class tpMenuPanelWidget : public tpChildWidget
{
public:
    tpMenuPanelWidget(tpChildWidget *parent = nullptr);

    virtual ~tpMenuPanelWidget();

    /// @brief 设置选择模式;暂未实现
    /// @param selectionMode 选择模式枚举, 只有单选和多选生效
    void setSelectionMode(const tinyPiX::ItemSelectionMode &selectionMode);

    /// @brief 获取当前widget设置的选中模式
    /// @return 选中模式枚举
    tinyPiX::ItemSelectionMode selectionMode();

    /// @brief 添加一个面板item
    /// @param item 单元指针
    void addItem(tpMenuPanelItem *item);

    /// @brief 移除一个item
    /// @param item 单元指针
    void removeItem(tpMenuPanelItem *item);

    /// @brief 移除所有item的选中
    void clearSelection();

public
signals:
    /// @brief 点击信号，鼠标释放时触发
    /// @param tpMenuPanelItem* 点击的item
    declare_signal(onClicked, tpMenuPanelItem *);

protected:
    virtual bool onMousePressEvent(tpMouseEvent *event) override;
    virtual bool onMouseRleaseEvent(tpMouseEvent *event) override;
    virtual bool onPaintEvent(tpObjectPaintEvent *event) override;
    virtual bool onResizeEvent(tpObjectResizeEvent *event) override;
    virtual bool onFocusEvent(tpObjectFocusEvent *event) override;

    virtual bool eventFilter(tpObject *watched, tpEvent *event) override;

    /// @brief
    /// @param event
    virtual void onThemeChangeEvent(tpThemeChangeEvent *event) override;

protected:
    virtual tpString pluginType() override { return TO_STRING(tpMenuPanelWidget); }

private:
    ItpMenuPanelWidgetData *data_;
};

#endif
