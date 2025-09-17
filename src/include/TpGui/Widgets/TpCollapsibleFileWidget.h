#ifndef __TP_COLLAPSIBLE_FILE_WIDGET_H
#define __TP_COLLAPSIBLE_FILE_WIDGET_H

#include "TpChildWidget.h"
#include "TpEvent.h"
#include "TpCollapsibleFileItem.h"

TP_DEF_VOID_TYPE_VAR(ItpCollapsibleFileWidgetData);

/// @brief 可折叠文件窗口
class TpCollapsibleFileWidget
    : public TpChildWidget
{
public:
    enum SelectMode
    {
        Normal,        // 常态 显示
        MultiSelection // 多选状态
    };

public:
    TpCollapsibleFileWidget(TpChildWidget *parent = nullptr);
    ~TpCollapsibleFileWidget();

    /// @brief 设置选中状态
    /// @param mode 状态枚举
    void setSelectMode(const SelectMode &mode);

    /// @brief 设置主标题
    /// @param title 标题文本
    void setTitle(const TpString &title);

    /// @brief 设置副标题
    /// @param subTitle 标题文本
    void setSubTitle(const TpString &subTitle);

    /// @brief 添加一个文件item
    void addFileItem(TpCollapsibleFileItem *item);

public
signals:
    /// @brief 长按item触发信号
    declare_signal(onLongPress);

protected:
    virtual TpString pluginType() { return TO_STRING(TpCollapsibleFileWidget); }

    virtual bool onResizeEvent(TpResizeEvent *event) override;

    virtual bool onMousePressEvent(TpMouseEvent *event) override;
    virtual bool onMouseRleaseEvent(TpMouseEvent *event) override;
    virtual bool onMouseLongPressEvent(TpMouseEvent *event) override;

    virtual bool onLeaveEvent(TpLeaveEvent *event) override;

    virtual bool onPaintEvent(TpPaintEvent *event) override;

    virtual bool eventFilter(TpObject *watched, TpEvent *event) override;

private:
    ItpCollapsibleFileWidgetData *data_;
};

#endif