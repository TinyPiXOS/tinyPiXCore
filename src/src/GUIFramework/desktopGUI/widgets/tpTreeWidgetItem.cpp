#include "GUIFramework/desktopGUI/widgets/tpTreeWidgetItem.h"
#include "GUIFramework/desktopGUI/widgets/tpTreeWidget.h"
#include "tpSurface.h"
#include "tpMap.h"
#include "tpCanvas.h"

DESKTOP_GUI_NAMESPACE_BEGIN

// 图标与文本间距
static const uint32_t IconTextMargin = 15;

struct tpTreeWidgetItemData
{
    tpTreeWidgetItem *parentPtr;

    tpFont *font;

    tpString text;
    tpShared<tpSurface> iconSurface;

    tpShared<tpSurface> expandStatusSurface;

    // 自定义数据map
    tpMap<int32_t, tpVariant> dataMap;

    tpList<tpTreeWidgetItem *> childList;

    // 是否展开
    bool isExpand;

    bool isSelect;

    tpTreeWidgetItemData()
        : text(""), iconSurface(tpMakeShared<tpSurface>()), parentPtr(nullptr), font(new tpFont()), isExpand(false), expandStatusSurface(tpMakeShared<tpSurface>())
        ,isSelect(false)
    {
    }

    ~tpTreeWidgetItemData()
    {
        delete font;
        font = nullptr;
    }
};

tpTreeWidgetItem::tpTreeWidgetItem(tpTreeWidgetItem *parent)
{
    data_ = new tpTreeWidgetItemData();
    tpTreeWidgetItemData *itemData = static_cast<tpTreeWidgetItemData *>(data_);
    itemData->parentPtr = parent;
}

tpTreeWidgetItem::tpTreeWidgetItem(const tpString &text, tpTreeWidgetItem *parent)
{
    data_ = new tpTreeWidgetItemData();
    tpTreeWidgetItemData *itemData = static_cast<tpTreeWidgetItemData *>(data_);
    itemData->parentPtr = parent;

    setText(text);
}

tpTreeWidgetItem::tpTreeWidgetItem(const tpString &text, const tpString &iconPath, tpTreeWidgetItem *parent)
{
    data_ = new tpTreeWidgetItemData();
    tpTreeWidgetItemData *itemData = static_cast<tpTreeWidgetItemData *>(data_);
    itemData->parentPtr = parent;

    setText(text);
    setIcon(iconPath);
}

tpTreeWidgetItem::~tpTreeWidgetItem()
{
    tpTreeWidgetItemData *itemData = static_cast<tpTreeWidgetItemData *>(data_);
    if (itemData)
    {
        delete itemData;
        itemData = nullptr;
    }
}

tpFont *tpTreeWidgetItem::font()
{
    tpTreeWidgetItemData *itemData = static_cast<tpTreeWidgetItemData *>(data_);
    if (!itemData)
        return nullptr;

    return itemData->font;
}

void tpTreeWidgetItem::setText(const tpString &text)
{
    tpTreeWidgetItemData *itemData = static_cast<tpTreeWidgetItemData *>(data_);
    if (!itemData)
        return;

    itemData->font->setText(text);
    itemData->text = text;
    update();
}

tpString tpTreeWidgetItem::text()
{
    tpTreeWidgetItemData *itemData = static_cast<tpTreeWidgetItemData *>(data_);
    if (!itemData)
        return "";

    return itemData->text;
}

void tpTreeWidgetItem::setIcon(const tpString &iconPath)
{
    tpTreeWidgetItemData *itemData = static_cast<tpTreeWidgetItemData *>(data_);
    if (!itemData)
        return;

    itemData->iconSurface->fromFile(iconPath);
    update();
}

tpShared<tpSurface> tpTreeWidgetItem::icon()
{
    tpTreeWidgetItemData *itemData = static_cast<tpTreeWidgetItemData *>(data_);
    if (!itemData)
        return nullptr;

    return itemData->iconSurface;
}

void tpTreeWidgetItem::setExpanded(bool expand)
{
    tpTreeWidgetItemData *itemData = static_cast<tpTreeWidgetItemData *>(data_);
    if (!itemData)
        return;

    itemData->isExpand = expand;
}

bool tpTreeWidgetItem::isExpanded() const
{
    tpTreeWidgetItemData *itemData = static_cast<tpTreeWidgetItemData *>(data_);
    if (!itemData)
        return false;

    return itemData->isExpand;
}

void tpTreeWidgetItem::setSelected(bool selected)
{
    tpTreeWidgetItemData *itemData = static_cast<tpTreeWidgetItemData *>(data_);
    if (!itemData)
        return;

    itemData->isSelect = selected;
}

bool tpTreeWidgetItem::isSelected()
{
    tpTreeWidgetItemData *itemData = static_cast<tpTreeWidgetItemData *>(data_);
    if (!itemData)
        return false;

    return itemData->isSelect;
}

tpVariant tpTreeWidgetItem::data(int32_t role) const
{
    tpTreeWidgetItemData *itemData = static_cast<tpTreeWidgetItemData *>(data_);
    if (!itemData)
        return tpVariant();

    if (!itemData->dataMap.contains(role))
        return tpVariant();

    return itemData->dataMap[role];
}

void tpTreeWidgetItem::setData(int32_t role, const tpVariant &value)
{
    tpTreeWidgetItemData *itemData = static_cast<tpTreeWidgetItemData *>(data_);
    if (!itemData)
        return;

    itemData->dataMap[role] = value;
}

void tpTreeWidgetItem::setParent(tpTreeWidgetItem *parent)
{
    tpTreeWidgetItemData *itemData = static_cast<tpTreeWidgetItemData *>(data_);
    if (!itemData)
        return;

    parent->addChild(this);
    itemData->parentPtr = parent;
}

tpTreeWidgetItem *tpTreeWidgetItem::parent()
{
    tpTreeWidgetItemData *itemData = static_cast<tpTreeWidgetItemData *>(data_);
    if (!itemData)
        return nullptr;

    return itemData->parentPtr;
}

tpTreeWidgetItem *tpTreeWidgetItem::child(uint32_t index)
{
    tpTreeWidgetItemData *itemData = static_cast<tpTreeWidgetItemData *>(data_);
    if (!itemData)
        return nullptr;

    if (itemData->childList.size() <= index)
        return nullptr;

    return itemData->childList.at(index);
}

uint32_t tpTreeWidgetItem::childCount()
{
    tpTreeWidgetItemData *itemData = static_cast<tpTreeWidgetItemData *>(data_);
    if (!itemData)
        return 0;

    return itemData->childList.size();
}

uint32_t tpTreeWidgetItem::indexOfChild(tpTreeWidgetItem *child) const
{
    if (!child)
        return 0;

    tpTreeWidgetItemData *itemData = static_cast<tpTreeWidgetItemData *>(data_);
    if (!itemData)
        return 0;

    for (int32_t i = 0; i < itemData->childList.size(); ++i)
    {
        if (itemData->childList.at(i) == child)
            return i;
    }

    return 0;
}

void tpTreeWidgetItem::addChild(tpTreeWidgetItem *child)
{
    if (!child)
        return;

    tpTreeWidgetItemData *itemData = static_cast<tpTreeWidgetItemData *>(data_);
    if (!itemData)
        return;

    itemData->childList.emplace_back(child);
}

void tpTreeWidgetItem::insertChild(uint32_t index, tpTreeWidgetItem *child)
{
    if (!child)
        return;

    tpTreeWidgetItemData *itemData = static_cast<tpTreeWidgetItemData *>(data_);
    if (!itemData)
        return;

    itemData->childList.insertData(index, child);
}

void tpTreeWidgetItem::removeChild(tpTreeWidgetItem *child)
{
    if (!child)
        return;

    tpTreeWidgetItemData *itemData = static_cast<tpTreeWidgetItemData *>(data_);
    if (!itemData)
        return;

    for (int32_t i = 0; i < itemData->childList.size(); ++i)
    {
        if (itemData->childList.at(i) == child)
        {
            itemData->childList.remove(child);
            break;
        }
    }
}

tpTreeWidgetItem *tpTreeWidgetItem::takeChild(uint32_t index)
{
    tpTreeWidgetItemData *itemData = static_cast<tpTreeWidgetItemData *>(data_);
    if (!itemData)
        return nullptr;

    for (int32_t i = 0; i < itemData->childList.size(); ++i)
    {
        if (i == index)
        {
            auto takeChild = itemData->childList.at(i);
            itemData->childList.remove(takeChild);
            return takeChild;
        }
    }

    return nullptr;
}

void tpTreeWidgetItem::addChildren(const tpList<tpTreeWidgetItem *> &children)
{
    tpTreeWidgetItemData *itemData = static_cast<tpTreeWidgetItemData *>(data_);
    if (!itemData)
        return;

    itemData->childList.insert(itemData->childList.end(), children.begin(), children.end());
}

void tpTreeWidgetItem::insertChildren(uint32_t index, const tpList<tpTreeWidgetItem *> &children)
{
    tpTreeWidgetItemData *itemData = static_cast<tpTreeWidgetItemData *>(data_);
    if (!itemData)
        return;

    for (int32_t i = 0, j = index; i < children.size(); ++i, ++j)
    {
        itemData->childList.insertData(j, children.at(i));
    }
}

tpList<tpTreeWidgetItem *> tpTreeWidgetItem::takeChildren()
{
    tpTreeWidgetItemData *itemData = static_cast<tpTreeWidgetItemData *>(data_);
    if (!itemData)
        return tpList<tpTreeWidgetItem *>();

    auto childList = itemData->childList;
    itemData->childList.clear();

    return childList;
}

bool tpTreeWidgetItem::onPaintEvent(tpObjectPaintEvent *event)
{
    // tpTreeWidgetItemData *itemData = static_cast<tpTreeWidgetItemData *>(data_);
    // if (!itemData)
    //     return true;

    // tpCanvas *paintCanvas = event->canvas();

    // // 先绘制文字，文字要居中
    // uint32_t textX = (rect().w - itemData->font->pixelWidth()) / 2.0;
    // uint32_t textY = (rect().h - itemData->font->pixelHeight()) / 2.0;
    // if (!itemData->text.empty())
    // {
    //     paintCanvas->renderText(*itemData->font, textX, textY);
    // }

    // // 绘制图标,图标在文字左侧
    // if (itemData->iconSurface->hasSurface())
    // {
    //     uint32_t iconX = textX - IconTextMargin - itemData->iconSurface->width();
    //     uint32_t iconY = (rect().h - itemData->iconSurface->height()) / 2.0;

    //     auto drawSurface = itemData->iconSurface->scaled(rect().h * 0.7, rect().h * 0.7);
    //     paintCanvas->paintSurface(iconX, iconY, drawSurface);
    // }

    // // 如果子节点展开，需要绘制 展开符号
    // if (itemData->childList.size() > 0)
    // {
    //     if (itemData->isExpand)
    //     {
    //         itemData->expandStatusSurface->fromFile("/usr/res/desktopGUI/上箭头");
    //     }
    //     else
    //     {
    //         itemData->expandStatusSurface->fromFile("/usr/res/desktopGUI/下箭头");
    //     }

    //     uint32_t iconX = textX + IconTextMargin;
    //     uint32_t iconY = (rect().h - itemData->expandStatusSurface->height()) / 2.0;

    //     auto drawSurface = itemData->expandStatusSurface->scaled(rect().h * 0.6, rect().h * 0.6);
    //     paintCanvas->paintSurface(iconX, iconY, drawSurface);
    // }

    return true;
}

DESKTOP_GUI_NAMESPACE_END
