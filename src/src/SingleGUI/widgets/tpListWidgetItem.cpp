/***
 * @Author: hywang
 * @Date: 2024-05-31 09:55:00
 * @LastEditors: hywang
 * @LastEditTime: 2024-06-14 09:47:19
 * @FilePath: /pix-singlegui/PixSingleGUI/src/SingleGUI/widgets/tpListWidgetItem.cpp
 * @Description:
 * @
 * @PiXOS
 */
#include "tpListWidgetItem.h"
#include "tpListWidget.h"
#include "tpVariant.h"
#include "tpString.h"
#include "tpFont.h"

tpListWidgetItem::tpListWidgetItem(tpListWidget *listview)
    : alignment_(tinyPiX::AlignLeft)
{
}

tpListWidgetItem::tpListWidgetItem(const tpString &text, tpListWidget *listview)
    : text_(text), alignment_(tinyPiX::AlignLeft)
{
    itemFont_.setText(text_.c_str());
}

tpListWidgetItem::tpListWidgetItem(const tpListWidgetItem &other)
    : alignment_(tinyPiX::AlignLeft)
{
}

void tpListWidgetItem::setSelected(bool select)
{
    select_ = select;
}

bool tpListWidgetItem::isSelected() const
{
    return select_;
}

tpString tpListWidgetItem::text() const
{
    return text_;
}

void tpListWidgetItem::setText(const tpString &text)
{
    text_ = text;
    itemFont_.setText(text_.c_str());
}

tpString tpListWidgetItem::statusTip() const
{
    return tpString();
}

void tpListWidgetItem::setStatusTip(const tpString &statusTip)
{
}

tpString tpListWidgetItem::toolTip() const
{
    return tpString();
}

void tpListWidgetItem::setToolTip(const tpString &toolTip)
{
}

tpFont tpListWidgetItem::font() const
{
    return itemFont_;
}

void tpListWidgetItem::setFont(const tpFont &font)
{
    itemFont_ = font;
}

tinyPiX::AlignmentFlag tpListWidgetItem::textAlignment()
{
    return alignment_;
}

void tpListWidgetItem::setTextAlignment(tinyPiX::AlignmentFlag alignment)
{
    alignment_ = alignment;
}

tpVariant tpListWidgetItem::data(int32_t role)
{
    if (itemDataMap_.contains(role))
        return itemDataMap_[role];
    return tpVariant();
}

void tpListWidgetItem::setData(int32_t role, const tpVariant &value)
{
    itemDataMap_[role] = value;
}

