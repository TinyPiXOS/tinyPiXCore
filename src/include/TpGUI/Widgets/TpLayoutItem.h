#ifndef __TP_LAYOUT_ITEM_H
#define __TP_LAYOUT_ITEM_H

#include "TpWidget.h"

TP_DEF_VOID_TYPE_VAR(ItpLayoutItemData);

class TpLayout;
/// @brief 布局内成员item类，暂未启用
class TpLayoutItem
{
public:
    enum ItemType
    {
        Widget,
        Layout
    };

public:
    TpLayoutItem(const ItemType &type);
    ~TpLayoutItem();

    /// @brief 获取item中的widget指针，layout类型则返回逻辑widget
    /// @return 
    TpWidget* widget();

private:
    ItpLayoutItemData *data_;
};

#endif