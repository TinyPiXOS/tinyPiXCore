/***
 * @Author: hywang
 * @Date: 2024-05-31 09:55:00
 * @LastEditors: hywang
 * @LastEditTime: 2024-06-14 09:47:19
 * @FilePath: /pix-singlegui/PixSingleGUI/src/SingleGUI/Widgets/TpListWidgetItem.cpp
 * @Description:
 * @
 * @PiXOS
 */
#include "TpListWidgetItem.h"
#include "TpListWidget.h"
#include "TpVariant.h"
#include "TpString.h"
#include "TpFont.h"

TpListWidgetItem::TpListWidgetItem(TpListWidget *listview)
    : alignment_(tinyPiX::AlignLeft)
{
}

TpListWidgetItem::TpListWidgetItem(const TpString &text, TpListWidget *listview)
    : text_(text), alignment_(tinyPiX::AlignLeft)
{
    itemFont_.setText(text_.c_str());
}

TpListWidgetItem::TpListWidgetItem(const TpListWidgetItem &other)
    : alignment_(tinyPiX::AlignLeft)
{
}

void TpListWidgetItem::setSelected(bool select)
{
    select_ = select;
}

bool TpListWidgetItem::isSelected() const
{
    return select_;
}

TpString TpListWidgetItem::text() const
{
    return text_;
}

void TpListWidgetItem::setText(const TpString &text)
{
    text_ = text;
    itemFont_.setText(text_.c_str());
}

TpString TpListWidgetItem::statusTip() const
{
    return TpString();
}

void TpListWidgetItem::setStatusTip(const TpString &statusTip)
{
}

TpString TpListWidgetItem::toolTip() const
{
    return TpString();
}

void TpListWidgetItem::setToolTip(const TpString &toolTip)
{
}

TpFont TpListWidgetItem::font() const
{
    return itemFont_;
}

void TpListWidgetItem::setFont(const TpFont &font)
{
    itemFont_ = font;
}

tinyPiX::AlignmentFlag TpListWidgetItem::textAlignment()
{
    return alignment_;
}

void TpListWidgetItem::setTextAlignment(tinyPiX::AlignmentFlag alignment)
{
    alignment_ = alignment;
}

TpVariant TpListWidgetItem::data(int32_t role)
{
    if (itemDataMap_.contains(role))
        return itemDataMap_[role];
    return TpVariant();
}

void TpListWidgetItem::setData(int32_t role, const TpVariant &value)
{
    itemDataMap_[role] = value;
}

