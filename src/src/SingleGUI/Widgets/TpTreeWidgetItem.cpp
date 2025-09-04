/***
 * @Author: hywang
 * @Date: 2024-06-14 10:32:51
 * @LastEditors: hywang
 * @LastEditTime: 2024-06-15 14:48:26
 * @FilePath: /pix-singlegui/PixSingleGUI/src/SingleGUI/Widgets/TpTreeWidgetItem.cpp
 * @Description:
 * @
 * @PiXOS
 */

#include "TpTreeWidgetItem.h"
#include "TpVector.h"

struct PiTreeWidgetItemPrivData
{
    TpTreeWidget *view;

    // 是否选中
    bool isSelect;

    // 是否隐藏
    bool isHidden;

    // 是否展开子节点
    bool isExpand;

    // 所有列的文本，下标即列号
    TpVector<TpString> textList;

    TpTreeWidgetItem *parent;

    // 所有子节点
    TpVector<TpTreeWidgetItem *> childItemList;

    PiTreeWidgetItemPrivData()
        : parent(nullptr)
    {
    }

    ~PiTreeWidgetItemPrivData()
    {
    }
};

TpTreeWidgetItem::TpTreeWidgetItem(const TpString &text)
    : privData(new PiTreeWidgetItemPrivData())
{
    PiTreeWidgetItemPrivData *privData = (PiTreeWidgetItemPrivData *)this->privData;
    if (!privData)
        return;

    privData->textList.emplace_back(text);
    // privData->textList[0] = text;
}

TpTreeWidgetItem::TpTreeWidgetItem(TpTreeWidgetItem *parent, const TpString &text)
    : privData(new PiTreeWidgetItemPrivData())

{
    PiTreeWidgetItemPrivData *privData = (PiTreeWidgetItemPrivData *)this->privData;
    if (!privData)
        return;

    privData->textList.emplace_back(text);
    // privData->textList[0] = text;
}

TpTreeWidgetItem::TpTreeWidgetItem(TpTreeWidget *treeview)
    : privData(new PiTreeWidgetItemPrivData())

{
    PiTreeWidgetItemPrivData *privData = (PiTreeWidgetItemPrivData *)this->privData;
    if (!privData)
        return;
}

TpTreeWidget *TpTreeWidgetItem::treeWidget()
{
    PiTreeWidgetItemPrivData *privData = (PiTreeWidgetItemPrivData *)this->privData;
    if (!privData)
        return nullptr;

    return privData->view;
}

void TpTreeWidgetItem::setSelected(bool select)
{
    PiTreeWidgetItemPrivData *privData = (PiTreeWidgetItemPrivData *)this->privData;
    if (!privData)
        return;

    privData->isSelect = select;
}

bool TpTreeWidgetItem::isSelected() const
{
    PiTreeWidgetItemPrivData *privData = (PiTreeWidgetItemPrivData *)this->privData;
    if (!privData)
        return false;

    return privData->isSelect;
}

void TpTreeWidgetItem::setHidden(bool hide)
{
    PiTreeWidgetItemPrivData *privData = (PiTreeWidgetItemPrivData *)this->privData;
    if (!privData)
        return;

    privData->isHidden = hide;
}

bool TpTreeWidgetItem::isHidden() const
{
    PiTreeWidgetItemPrivData *privData = (PiTreeWidgetItemPrivData *)this->privData;
    if (!privData)
        return false;

    return privData->isHidden;
}

void TpTreeWidgetItem::setExpanded(bool expand)
{
    PiTreeWidgetItemPrivData *privData = (PiTreeWidgetItemPrivData *)this->privData;
    if (!privData)
        return;

    privData->isExpand = expand;
}

bool TpTreeWidgetItem::isExpanded() const
{
    PiTreeWidgetItemPrivData *privData = (PiTreeWidgetItemPrivData *)this->privData;
    if (!privData)
        return false;

    return privData->isExpand;
}

void TpTreeWidgetItem::setFirstColumnSpanned(bool span)
{
}

bool TpTreeWidgetItem::isFirstColumnSpanned() const
{
    return false;
}

void TpTreeWidgetItem::setDisabled(bool disabled)
{
}

bool TpTreeWidgetItem::isDisabled() const
{
    return false;
}

TpString TpTreeWidgetItem::text(int32_t column)
{
    PiTreeWidgetItemPrivData *privData = (PiTreeWidgetItemPrivData *)this->privData;
    if (!privData)
        return TpString();

    if (privData->textList.size() == 0)
        return TpString();

    return privData->textList.at(0);
}

void TpTreeWidgetItem::setText(int32_t column, const TpString &text)
{
    PiTreeWidgetItemPrivData *privData = (PiTreeWidgetItemPrivData *)this->privData;
    if (!privData)
        return;

    privData->textList[0] = text;
}

TpString TpTreeWidgetItem::statusTip(int32_t column)
{
    return TpString();
}

void TpTreeWidgetItem::setStatusTip(int32_t column, const TpString &statusTip)
{
}

TpString TpTreeWidgetItem::toolTip(int32_t column)
{
    return TpString();
}

void TpTreeWidgetItem::setToolTip(int32_t column, const TpString &toolTip)
{
}

TpFont TpTreeWidgetItem::font(int32_t column)
{
    return TpFont();
}

void TpTreeWidgetItem::setFont(int32_t column, const TpFont &font)
{
}

tinyPiX::AlignmentFlag TpTreeWidgetItem::textAlignment(int32_t column)
{
    return tinyPiX::AlignmentFlag();
}

void TpTreeWidgetItem::setTextAlignment(int32_t column, tinyPiX::AlignmentFlag alignment)
{
}

TpColors TpTreeWidgetItem::textColor(int32_t column)
{
    return TpColors(0, 0, 0, 255);
}

void TpTreeWidgetItem::setTextColor(int32_t column, const TpColors &color)
{
}

tinyPiX::CheckState TpTreeWidgetItem::checkState(int32_t column)
{
    return tinyPiX::CheckState();
}

void TpTreeWidgetItem::setCheckState(int32_t column, tinyPiX::CheckState state)
{
}

TpSize TpTreeWidgetItem::sizeHint(int32_t column)
{
    return TpSize();
}

void TpTreeWidgetItem::setSizeHint(int32_t column, const TpSize &size)
{
}

TpVariant TpTreeWidgetItem::data(int32_t column, int32_t role) const
{
    return TpVariant();
}

void TpTreeWidgetItem::setData(int32_t column, int32_t role, const TpVariant &value)
{
}

bool TpTreeWidgetItem::operator<(const TpTreeWidgetItem &other) const
{
    return false;
}

void TpTreeWidgetItem::setParent(TpTreeWidgetItem *parent)
{
    PiTreeWidgetItemPrivData *privData = (PiTreeWidgetItemPrivData *)this->privData;
    if (!privData)
        return;

    privData->parent = parent;
}

TpTreeWidgetItem *TpTreeWidgetItem::parent()
{
    PiTreeWidgetItemPrivData *privData = (PiTreeWidgetItemPrivData *)this->privData;
    if (!privData)
        return nullptr;

    return privData->parent;
}

TpTreeWidgetItem *TpTreeWidgetItem::child(int32_t index)
{
    PiTreeWidgetItemPrivData *privData = (PiTreeWidgetItemPrivData *)this->privData;
    if (!privData)
        return nullptr;

    if (index >= privData->childItemList.size())
        return nullptr;

    return privData->childItemList.at(index);
}

int32_t TpTreeWidgetItem::childCount()
{
    PiTreeWidgetItemPrivData *privData = (PiTreeWidgetItemPrivData *)this->privData;
    if (!privData)
        return 0;
    return privData->childItemList.size();
}

int32_t TpTreeWidgetItem::columnCount()
{
    return 0;
}

int32_t TpTreeWidgetItem::indexOfChild(TpTreeWidgetItem *child) const
{
    return 0;
}

void TpTreeWidgetItem::addChild(TpTreeWidgetItem *child)
{
    PiTreeWidgetItemPrivData *privData = (PiTreeWidgetItemPrivData *)this->privData;
    if (!privData)
        return;

    insertChild(privData->childItemList.size(), child);
}

void TpTreeWidgetItem::insertChild(int32_t index, TpTreeWidgetItem *child)
{
    PiTreeWidgetItemPrivData *privData = (PiTreeWidgetItemPrivData *)this->privData;
    if (!privData)
        return;

    child->setParent(this);

    privData->childItemList.insertData(index, child);
}

void TpTreeWidgetItem::removeChild(TpTreeWidgetItem *child)
{
    PiTreeWidgetItemPrivData *privData = (PiTreeWidgetItemPrivData *)this->privData;
    if (!privData)
        return;

    for (int32_t i = 0; i < privData->childItemList.size(); ++i)
    {
        TpTreeWidgetItem *curChildItem = privData->childItemList.at(i);
        if (curChildItem == child)
        {
            privData->childItemList.remove(i);
            curChildItem->clear();

            delete curChildItem;
            curChildItem = nullptr;
        }
    }
}

TpTreeWidgetItem *TpTreeWidgetItem::takeChild(int32_t index)
{
    return nullptr;
}

void TpTreeWidgetItem::addChildren(const TpList<TpTreeWidgetItem *> &children)
{
}

void TpTreeWidgetItem::insertChildren(int32_t index, const TpList<TpTreeWidgetItem *> &children)
{
}

TpList<TpTreeWidgetItem *> TpTreeWidgetItem::takeChildren()
{
    return TpList<TpTreeWidgetItem *>();
}

void TpTreeWidgetItem::clear()
{
}

void TpTreeWidgetItem::sortChildren(int32_t column, tinyPiX::SortOrder order)
{
}
