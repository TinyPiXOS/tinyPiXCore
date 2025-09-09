#include "GUIFramework/desktopGUI/widgets/TpTreeWidgetItem.h"
#include "GUIFramework/desktopGUI/widgets/TpTreeWidget.h"
#include "TpImage.h"
#include "TpMap.h"
#include "TpCanvas.h"
#include "TpImage.h"

DESKTOP_GUI_NAMESPACE_BEGIN

// 图标与文本间距
static const uint32_t IconTextMargin = 15;

struct TpTreeWidgetItemData
{
    TpTreeWidgetItem *parentPtr;

    TpFont *font;

    TpString text;
    TpImage iconSurface;

    TpImage expandStatusSurface;

    // 自定义数据map
    TpMap<int32_t, TpVariant> dataMap;

    TpList<TpTreeWidgetItem *> childList;

    // 是否展开
    bool isExpand;

    bool isSelect;

    TpTreeWidgetItemData()
        : text(""), parentPtr(nullptr), font(new TpFont()), isExpand(false)
        ,isSelect(false)
    {
    }

    ~TpTreeWidgetItemData()
    {
        delete font;
        font = nullptr;
    }
};

TpTreeWidgetItem::TpTreeWidgetItem(TpTreeWidgetItem *parent)
{
    data_ = new TpTreeWidgetItemData();
    TpTreeWidgetItemData *itemData = static_cast<TpTreeWidgetItemData *>(data_);
    itemData->parentPtr = parent;
}

TpTreeWidgetItem::TpTreeWidgetItem(const TpString &text, TpTreeWidgetItem *parent)
{
    data_ = new TpTreeWidgetItemData();
    TpTreeWidgetItemData *itemData = static_cast<TpTreeWidgetItemData *>(data_);
    itemData->parentPtr = parent;

    setText(text);
}

TpTreeWidgetItem::TpTreeWidgetItem(const TpString &text, const TpString &iconPath, TpTreeWidgetItem *parent)
{
    data_ = new TpTreeWidgetItemData();
    TpTreeWidgetItemData *itemData = static_cast<TpTreeWidgetItemData *>(data_);
    itemData->parentPtr = parent;

    setText(text);
    setIcon(iconPath);
}

TpTreeWidgetItem::~TpTreeWidgetItem()
{
    TpTreeWidgetItemData *itemData = static_cast<TpTreeWidgetItemData *>(data_);
    if (itemData)
    {
        delete itemData;
        itemData = nullptr;
    }
}

TpFont *TpTreeWidgetItem::font()
{
    TpTreeWidgetItemData *itemData = static_cast<TpTreeWidgetItemData *>(data_);
    if (!itemData)
        return nullptr;

    return itemData->font;
}

void TpTreeWidgetItem::setText(const TpString &text)
{
    TpTreeWidgetItemData *itemData = static_cast<TpTreeWidgetItemData *>(data_);
    if (!itemData)
        return;

    itemData->font->setText(text);
    itemData->text = text;
    update();
}

TpString TpTreeWidgetItem::text()
{
    TpTreeWidgetItemData *itemData = static_cast<TpTreeWidgetItemData *>(data_);
    if (!itemData)
        return "";

    return itemData->text;
}

void TpTreeWidgetItem::setIcon(const TpString &iconPath)
{
    TpTreeWidgetItemData *itemData = static_cast<TpTreeWidgetItemData *>(data_);
    if (!itemData)
        return;

    itemData->iconSurface.load(iconPath);
    update();
}

TpImage TpTreeWidgetItem::icon()
{
    TpTreeWidgetItemData *itemData = static_cast<TpTreeWidgetItemData *>(data_);
    if (!itemData)
        return TpImage();

    return itemData->iconSurface;
}

void TpTreeWidgetItem::setExpanded(bool expand)
{
    TpTreeWidgetItemData *itemData = static_cast<TpTreeWidgetItemData *>(data_);
    if (!itemData)
        return;

    itemData->isExpand = expand;
}

bool TpTreeWidgetItem::isExpanded() const
{
    TpTreeWidgetItemData *itemData = static_cast<TpTreeWidgetItemData *>(data_);
    if (!itemData)
        return false;

    return itemData->isExpand;
}

void TpTreeWidgetItem::setSelected(bool selected)
{
    TpTreeWidgetItemData *itemData = static_cast<TpTreeWidgetItemData *>(data_);
    if (!itemData)
        return;

    itemData->isSelect = selected;
}

bool TpTreeWidgetItem::isSelected()
{
    TpTreeWidgetItemData *itemData = static_cast<TpTreeWidgetItemData *>(data_);
    if (!itemData)
        return false;

    return itemData->isSelect;
}

TpVariant TpTreeWidgetItem::data(int32_t role) const
{
    TpTreeWidgetItemData *itemData = static_cast<TpTreeWidgetItemData *>(data_);
    if (!itemData)
        return TpVariant();

    if (!itemData->dataMap.contains(role))
        return TpVariant();

    return itemData->dataMap[role];
}

void TpTreeWidgetItem::setData(int32_t role, const TpVariant &value)
{
    TpTreeWidgetItemData *itemData = static_cast<TpTreeWidgetItemData *>(data_);
    if (!itemData)
        return;

    itemData->dataMap[role] = value;
}

void TpTreeWidgetItem::setParent(TpTreeWidgetItem *parent)
{
    TpTreeWidgetItemData *itemData = static_cast<TpTreeWidgetItemData *>(data_);
    if (!itemData)
        return;

    parent->addChild(this);
    itemData->parentPtr = parent;
}

TpTreeWidgetItem *TpTreeWidgetItem::parent()
{
    TpTreeWidgetItemData *itemData = static_cast<TpTreeWidgetItemData *>(data_);
    if (!itemData)
        return nullptr;

    return itemData->parentPtr;
}

TpTreeWidgetItem *TpTreeWidgetItem::child(uint32_t index)
{
    TpTreeWidgetItemData *itemData = static_cast<TpTreeWidgetItemData *>(data_);
    if (!itemData)
        return nullptr;

    if (itemData->childList.size() <= index)
        return nullptr;

    return itemData->childList.at(index);
}

uint32_t TpTreeWidgetItem::childCount()
{
    TpTreeWidgetItemData *itemData = static_cast<TpTreeWidgetItemData *>(data_);
    if (!itemData)
        return 0;

    return itemData->childList.size();
}

uint32_t TpTreeWidgetItem::indexOfChild(TpTreeWidgetItem *child) const
{
    if (!child)
        return 0;

    TpTreeWidgetItemData *itemData = static_cast<TpTreeWidgetItemData *>(data_);
    if (!itemData)
        return 0;

    for (int32_t i = 0; i < itemData->childList.size(); ++i)
    {
        if (itemData->childList.at(i) == child)
            return i;
    }

    return 0;
}

void TpTreeWidgetItem::addChild(TpTreeWidgetItem *child)
{
    if (!child)
        return;

    TpTreeWidgetItemData *itemData = static_cast<TpTreeWidgetItemData *>(data_);
    if (!itemData)
        return;

    itemData->childList.emplace_back(child);
}

void TpTreeWidgetItem::insertChild(uint32_t index, TpTreeWidgetItem *child)
{
    if (!child)
        return;

    TpTreeWidgetItemData *itemData = static_cast<TpTreeWidgetItemData *>(data_);
    if (!itemData)
        return;

    itemData->childList.insertData(index, child);
}

void TpTreeWidgetItem::removeChild(TpTreeWidgetItem *child)
{
    if (!child)
        return;

    TpTreeWidgetItemData *itemData = static_cast<TpTreeWidgetItemData *>(data_);
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

TpTreeWidgetItem *TpTreeWidgetItem::takeChild(uint32_t index)
{
    TpTreeWidgetItemData *itemData = static_cast<TpTreeWidgetItemData *>(data_);
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

void TpTreeWidgetItem::addChildren(const TpList<TpTreeWidgetItem *> &children)
{
    TpTreeWidgetItemData *itemData = static_cast<TpTreeWidgetItemData *>(data_);
    if (!itemData)
        return;

    itemData->childList.insert(itemData->childList.end(), children.begin(), children.end());
}

void TpTreeWidgetItem::insertChildren(uint32_t index, const TpList<TpTreeWidgetItem *> &children)
{
    TpTreeWidgetItemData *itemData = static_cast<TpTreeWidgetItemData *>(data_);
    if (!itemData)
        return;

    for (int32_t i = 0, j = index; i < children.size(); ++i, ++j)
    {
        itemData->childList.insertData(j, children.at(i));
    }
}

TpList<TpTreeWidgetItem *> TpTreeWidgetItem::takeChildren()
{
    TpTreeWidgetItemData *itemData = static_cast<TpTreeWidgetItemData *>(data_);
    if (!itemData)
        return TpList<TpTreeWidgetItem *>();

    auto childList = itemData->childList;
    itemData->childList.clear();

    return childList;
}

bool TpTreeWidgetItem::onPaintEvent(TpPaintEvent *event)
{
    // TpTreeWidgetItemData *itemData = static_cast<TpTreeWidgetItemData *>(data_);
    // if (!itemData)
    //     return true;

    // TpCanvas *paintCanvas = event->canvas();

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
