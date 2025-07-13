#ifndef __TP_COLLAPSIBLE_FILE_WIDGET_H
#define __TP_COLLAPSIBLE_FILE_WIDGET_H

#include "tpChildWidget.h"
#include "tpEvent.h"
#include "tpCollapsibleFileItem.h"

TP_DEF_VOID_TYPE_VAR(ItpCollapsibleFileWidgetData);

/// @brief 可折叠文件窗口
class tpCollapsibleFileWidget
    : public tpChildWidget
{
public:
    enum SelectMode
    {
        Normal,        // 常态 显示
        MultiSelection // 多选状态
    };

public:
    tpCollapsibleFileWidget(tpChildWidget *parent = nullptr);
    ~tpCollapsibleFileWidget();

    /// @brief 设置选中状态
    /// @param mode 状态枚举
    void setSelectMode(const SelectMode &mode);

    /// @brief 设置主标题
    /// @param title 标题文本
    void setTitle(const tpString &title);

    /// @brief 设置副标题
    /// @param subTitle 标题文本
    void setSubTitle(const tpString &subTitle);

    /// @brief 添加一个文件item
    void addFileItem(tpCollapsibleFileItem *item);

public
signals:
    /// @brief 长按item触发信号
    declare_signal(onLongPress);

protected:
    virtual tpString pluginType() { return TO_STRING(tpCollapsibleFileWidget); }

    virtual bool onResizeEvent(tpObjectResizeEvent *event) override;

    virtual bool onMousePressEvent(tpMouseEvent *event) override;
    virtual bool onMouseRleaseEvent(tpMouseEvent *event) override;
    virtual bool onMouseLongPressEvent(tpMouseEvent *event) override;

    virtual bool onLeaveEvent(tpObjectLeaveEvent *event) override;

    virtual bool onPaintEvent(tpObjectPaintEvent *event) override;

    virtual bool eventFilter(tpObject *watched, tpEvent *event) override;

private:
    ItpCollapsibleFileWidgetData *data_;
};

#endif