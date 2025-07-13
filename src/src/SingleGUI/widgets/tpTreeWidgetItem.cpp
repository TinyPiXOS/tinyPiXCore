/***
 * @Author: hywang
 * @Date: 2024-06-14 10:32:51
 * @LastEditors: hywang
 * @LastEditTime: 2024-06-15 14:48:26
 * @FilePath: /pix-singlegui/PixSingleGUI/src/SingleGUI/widgets/tpTreeWidgetItem.cpp
 * @Description:
 * @
 * @PiXOS
 */

#include "tpTreeWidgetItem.h"
#include "tpVector.h"

struct PiTreeWidgetItemPrivData
{
    tpTreeWidget *view;

    // 是否选中
    bool isSelect;

    // 是否隐藏
    bool isHidden;

    // 是否展开子节点
    bool isExpand;

    // 所有列的文本，下标即列号
    tpVector<tpString> textList;

    tpTreeWidgetItem *parent;

    // 所有子节点
    tpVector<tpTreeWidgetItem *> childItemList;

    PiTreeWidgetItemPrivData()
        : parent(nullptr)
    {
    }

    ~PiTreeWidgetItemPrivData()
    {
    }
};

tpTreeWidgetItem::tpTreeWidgetItem(const tpString &text)
    : privData(new PiTreeWidgetItemPrivData())
{
    PiTreeWidgetItemPrivData *privData = (PiTreeWidgetItemPrivData *)this->privData;
    if (!privData)
        return;

    privData->textList.emplace_back(text);
    // privData->textList[0] = text;
}

tpTreeWidgetItem::tpTreeWidgetItem(tpTreeWidgetItem *parent, const tpString &text)
    : privData(new PiTreeWidgetItemPrivData())

{
    PiTreeWidgetItemPrivData *privData = (PiTreeWidgetItemPrivData *)this->privData;
    if (!privData)
        return;

    privData->textList.emplace_back(text);
    // privData->textList[0] = text;
}

tpTreeWidgetItem::tpTreeWidgetItem(tpTreeWidget *treeview)
    : privData(new PiTreeWidgetItemPrivData())

{
    PiTreeWidgetItemPrivData *privData = (PiTreeWidgetItemPrivData *)this->privData;
    if (!privData)
        return;
}

tpTreeWidget *tpTreeWidgetItem::treeWidget()
{
    PiTreeWidgetItemPrivData *privData = (PiTreeWidgetItemPrivData *)this->privData;
    if (!privData)
        return nullptr;

    return privData->view;
}

void tpTreeWidgetItem::setSelected(bool select)
{
    PiTreeWidgetItemPrivData *privData = (PiTreeWidgetItemPrivData *)this->privData;
    if (!privData)
        return;

    privData->isSelect = select;
}

bool tpTreeWidgetItem::isSelected() const
{
    PiTreeWidgetItemPrivData *privData = (PiTreeWidgetItemPrivData *)this->privData;
    if (!privData)
        return false;

    return privData->isSelect;
}

void tpTreeWidgetItem::setHidden(bool hide)
{
    PiTreeWidgetItemPrivData *privData = (PiTreeWidgetItemPrivData *)this->privData;
    if (!privData)
        return;

    privData->isHidden = hide;
}

bool tpTreeWidgetItem::isHidden() const
{
    PiTreeWidgetItemPrivData *privData = (PiTreeWidgetItemPrivData *)this->privData;
    if (!privData)
        return false;

    return privData->isHidden;
}

void tpTreeWidgetItem::setExpanded(bool expand)
{
    PiTreeWidgetItemPrivData *privData = (PiTreeWidgetItemPrivData *)this->privData;
    if (!privData)
        return;

    privData->isExpand = expand;
}

bool tpTreeWidgetItem::isExpanded() const
{
    PiTreeWidgetItemPrivData *privData = (PiTreeWidgetItemPrivData *)this->privData;
    if (!privData)
        return false;

    return privData->isExpand;
}

void tpTreeWidgetItem::setFirstColumnSpanned(bool span)
{
}

bool tpTreeWidgetItem::isFirstColumnSpanned() const
{
    return false;
}

void tpTreeWidgetItem::setDisabled(bool disabled)
{
}

bool tpTreeWidgetItem::isDisabled() const
{
    return false;
}

tpString tpTreeWidgetItem::text(int32_t column)
{
    PiTreeWidgetItemPrivData *privData = (PiTreeWidgetItemPrivData *)this->privData;
    if (!privData)
        return tpString();

    if (privData->textList.size() == 0)
        return tpString();

    return privData->textList.at(0);
}

void tpTreeWidgetItem::setText(int32_t column, const tpString &text)
{
    PiTreeWidgetItemPrivData *privData = (PiTreeWidgetItemPrivData *)this->privData;
    if (!privData)
        return;

    privData->textList[0] = text;
}

tpString tpTreeWidgetItem::statusTip(int32_t column)
{
    return tpString();
}

void tpTreeWidgetItem::setStatusTip(int32_t column, const tpString &statusTip)
{
}

tpString tpTreeWidgetItem::toolTip(int32_t column)
{
    return tpString();
}

void tpTreeWidgetItem::setToolTip(int32_t column, const tpString &toolTip)
{
}

tpFont tpTreeWidgetItem::font(int32_t column)
{
    return tpFont();
}

void tpTreeWidgetItem::setFont(int32_t column, const tpFont &font)
{
}

tinyPiX::AlignmentFlag tpTreeWidgetItem::textAlignment(int32_t column)
{
    return tinyPiX::AlignmentFlag();
}

void tpTreeWidgetItem::setTextAlignment(int32_t column, tinyPiX::AlignmentFlag alignment)
{
}

tpColors tpTreeWidgetItem::textColor(int32_t column)
{
    return tpColors(0, 0, 0, 255);
}

void tpTreeWidgetItem::setTextColor(int32_t column, const tpColors &color)
{
}

tinyPiX::CheckState tpTreeWidgetItem::checkState(int32_t column)
{
    return tinyPiX::CheckState();
}

void tpTreeWidgetItem::setCheckState(int32_t column, tinyPiX::CheckState state)
{
}

ItpSize tpTreeWidgetItem::sizeHint(int32_t column)
{
    return ItpSize();
}

void tpTreeWidgetItem::setSizeHint(int32_t column, const ItpSize &size)
{
}

tpVariant tpTreeWidgetItem::data(int32_t column, int32_t role) const
{
    return tpVariant();
}

void tpTreeWidgetItem::setData(int32_t column, int32_t role, const tpVariant &value)
{
}

bool tpTreeWidgetItem::operator<(const tpTreeWidgetItem &other) const
{
    return false;
}

void tpTreeWidgetItem::setParent(tpTreeWidgetItem *parent)
{
    PiTreeWidgetItemPrivData *privData = (PiTreeWidgetItemPrivData *)this->privData;
    if (!privData)
        return;

    privData->parent = parent;
}

tpTreeWidgetItem *tpTreeWidgetItem::parent()
{
    PiTreeWidgetItemPrivData *privData = (PiTreeWidgetItemPrivData *)this->privData;
    if (!privData)
        return nullptr;

    return privData->parent;
}

tpTreeWidgetItem *tpTreeWidgetItem::child(int32_t index)
{
    PiTreeWidgetItemPrivData *privData = (PiTreeWidgetItemPrivData *)this->privData;
    if (!privData)
        return nullptr;

    if (index >= privData->childItemList.size())
        return nullptr;

    return privData->childItemList.at(index);
}

int32_t tpTreeWidgetItem::childCount()
{
    PiTreeWidgetItemPrivData *privData = (PiTreeWidgetItemPrivData *)this->privData;
    if (!privData)
        return 0;
    return privData->childItemList.size();
}

int32_t tpTreeWidgetItem::columnCount()
{
    return 0;
}

int32_t tpTreeWidgetItem::indexOfChild(tpTreeWidgetItem *child) const
{
    return 0;
}

void tpTreeWidgetItem::addChild(tpTreeWidgetItem *child)
{
    PiTreeWidgetItemPrivData *privData = (PiTreeWidgetItemPrivData *)this->privData;
    if (!privData)
        return;

    insertChild(privData->childItemList.size(), child);
}

void tpTreeWidgetItem::insertChild(int32_t index, tpTreeWidgetItem *child)
{
    PiTreeWidgetItemPrivData *privData = (PiTreeWidgetItemPrivData *)this->privData;
    if (!privData)
        return;

    child->setParent(this);

    privData->childItemList.insertData(index, child);
}

void tpTreeWidgetItem::removeChild(tpTreeWidgetItem *child)
{
    PiTreeWidgetItemPrivData *privData = (PiTreeWidgetItemPrivData *)this->privData;
    if (!privData)
        return;

    for (int32_t i = 0; i < privData->childItemList.size(); ++i)
    {
        tpTreeWidgetItem *curChildItem = privData->childItemList.at(i);
        if (curChildItem == child)
        {
            privData->childItemList.remove(i);
            curChildItem->clear();

            delete curChildItem;
            curChildItem = nullptr;
        }
    }
}

tpTreeWidgetItem *tpTreeWidgetItem::takeChild(int32_t index)
{
    return nullptr;
}

void tpTreeWidgetItem::addChildren(const tpList<tpTreeWidgetItem *> &children)
{
}

void tpTreeWidgetItem::insertChildren(int32_t index, const tpList<tpTreeWidgetItem *> &children)
{
}

tpList<tpTreeWidgetItem *> tpTreeWidgetItem::takeChildren()
{
    return tpList<tpTreeWidgetItem *>();
}

void tpTreeWidgetItem::clear()
{
}

void tpTreeWidgetItem::sortChildren(int32_t column, tinyPiX::SortOrder order)
{
}
