#ifndef __TP_LAYOUT_ITEM_H
#define __TP_LAYOUT_ITEM_H

#include "tpChildWidget.h"

TP_DEF_VOID_TYPE_VAR(ItpLayoutItemData);

class tpLayout;
/// @brief 布局内成员item类，暂未启用
class tpLayoutItem
{
public:
    enum ItemType
    {
        Widget,
        Layout
    };

public:
    tpLayoutItem(const ItemType &type);
    ~tpLayoutItem();

    /// @brief 获取item中的widget指针，layout类型则返回逻辑widget
    /// @return 
    tpChildWidget* widget();

private:
    ItpLayoutItemData *data_;
};

#endif