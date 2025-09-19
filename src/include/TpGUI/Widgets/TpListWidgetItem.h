#ifndef __TP_LIST_WIDGET_ITEM_H
#define __TP_LIST_WIDGET_ITEM_H

#include "TpChildWidget.h"
#include "TpSignalSlot.h"
#include "TpString.h"
#include "TpVariant.h"

TP_DEF_VOID_TYPE_VAR(ITpListWidgetItemData);
/// @brief 列表下的item
class TpListWidgetItem : public TpChildWidget
{
public:
    TpListWidgetItem(TpChildWidget *parent = nullptr);
    TpListWidgetItem(const TpString &text, TpChildWidget *parent = nullptr);

    virtual ~TpListWidgetItem();

public:
    /// @brief 获取当前文本
    /// @return 文本字符串
    TpString text();

    /// @brief 设置item的文本内容
    /// @param text 文本字符串
    void setText(const TpString &text);

    /// @brief 获取item设置的数据
    /// @return 数据对象
    TpVariant data();

    /// @brief 设置item数据
    /// @param data 数据
    void setData(const TpVariant &data);

public
signals:
    /// @brief 选中项切换事件
    /// @param TpListWidgetItem* 当前item指针
    declare_signal(onStatusChanged, TpListWidgetItem *);

protected:
    virtual bool onPaintEvent(TpPaintEvent *event) override;
    virtual bool onMousePressEvent(TpMouseEvent *event) override;
    virtual bool onMouseRleaseEvent(TpMouseEvent *event) override;
    virtual bool onResizeEvent(TpResizeEvent *event) override;
    virtual bool eventFilter(TpObject *watched, TpEvent *event) override;

protected:
    virtual TpString pluginType() override { return TO_STRING(TpListWidgetItem); }

private:
    ITpListWidgetItemData *data_;
};

#endif
