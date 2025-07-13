#ifndef __TP_LIST_CHECKBOX_GROUP_ITEM_H
#define __TP_LIST_CHECKBOX_GROUP_ITEM_H

#include "tpChildWidget.h"
#include "tpSignalSlot.h"
#include "tpString.h"
#include "tpVariant.h"

TP_DEF_VOID_TYPE_VAR(ItpListCheckBoxGroupItemData);
/// @brief 列表模式下的单选复选按钮组
class tpListCheckBoxGroupItem : public tpChildWidget
{
public:
    tpListCheckBoxGroupItem(tpChildWidget *parent = nullptr);
    tpListCheckBoxGroupItem(const tpString &text, tpChildWidget *parent = nullptr);

    virtual ~tpListCheckBoxGroupItem();

public:
    /// @brief 获取当前文本
    /// @return 文本字符串
    tpString text();

    /// @brief 设置item的文本内容
    /// @param text 文本字符串
    void setText(const tpString &text);

    /// @brief 获取item设置的数据
    /// @return 数据对象
    tpVariant data();

    /// @brief 设置item数据
    /// @param data 数据
    void setData(const tpVariant &data);

public
signals:
    /// @brief 选中项切换事件
    /// @param tpListCheckBoxGroupItem* 当前item指针
    declare_signal(onStatusChanged, tpListCheckBoxGroupItem *);

protected:
    virtual bool onPaintEvent(tpObjectPaintEvent *event) override;
    virtual bool onMousePressEvent(tpMouseEvent *event) override;
    virtual bool onMouseRleaseEvent(tpMouseEvent *event) override;
    virtual bool onResizeEvent(tpObjectResizeEvent *event) override;
    virtual bool eventFilter(tpObject *watched, tpEvent *event) override;

protected:
    virtual tpString pluginType() override { return TO_STRING(tpListCheckBoxGroupItem); }

private:
    ItpListCheckBoxGroupItemData *data_;
};

#endif
