#ifndef __TP_MENU_PANEL_WIDGET_H
#define __TP_MENU_PANEL_WIDGET_H

#include "TpWidget.h"
#include "TpSignalSlot.h"
#include "TpString.h"
#include "TpMenuPanelItem.h"

TP_DEF_VOID_TYPE_VAR(ITpMenuPanelWidgetData);

/// @brief 菜单面板窗体,item只能单选；失去焦点时取消所有选中；选中后不可取消选中
class TpMenuPanelWidget : public TpWidget
{
public:
    TpMenuPanelWidget(TpWidget *parent = nullptr);

    virtual ~TpMenuPanelWidget();

    /// @brief 设置选择模式;暂未实现
    /// @param selectionMode 选择模式枚举, 只有单选和多选生效
    void setSelectionMode(const Tp::ItemSelectionMode &selectionMode);

    /// @brief 获取当前widget设置的选中模式
    /// @return 选中模式枚举
    Tp::ItemSelectionMode selectionMode();

    /// @brief 添加一个面板item
    /// @param item 单元指针
    void addItem(TpMenuPanelItem *item);

    /// @brief 移除一个item
    /// @param item 单元指针
    void removeItem(TpMenuPanelItem *item);

    /// @brief 移除所有item的选中
    void clearSelection();

public
signals:
    /// @brief 点击信号，鼠标释放时触发
    /// @param TpMenuPanelItem* 点击的item
    declare_signal(onClicked, TpMenuPanelItem *);

protected:
    virtual bool onMousePressEvent(TpMouseEvent *event) override;
    virtual bool onMouseRleaseEvent(TpMouseEvent *event) override;
    virtual bool onPaintEvent(TpPaintEvent *event) override;
    virtual bool onResizeEvent(TpResizeEvent *event) override;
    virtual bool onFocusEvent(TpFocusEvent *event) override;

    virtual bool eventFilter(TpObject *watched, TpEvent *event) override;

    /// @brief
    /// @param event
    virtual void onThemeChangeEvent(TpThemeChangeEvent *event) override;

protected:
    virtual TpString pluginType() override { return TO_STRING(TpMenuPanelWidget); }

private:
    ITpMenuPanelWidgetData *data_;
};

#endif
