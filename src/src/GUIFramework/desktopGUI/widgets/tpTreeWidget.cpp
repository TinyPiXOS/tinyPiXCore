#include "GUIFramework/desktopGUI/widgets/tpTreeWidget.h"
#include "tpList.h"
#include "tpLabel.h"
#include "tpSurface.h"
#include "tpCanvas.h"
#include "tpList.h"
#include "tpUtils.h"
#include <cmath>

DESKTOP_GUI_NAMESPACE_BEGIN

// 标题栏区域
static const uint32_t TitleHeight = 90;

// 一个item高度
static const uint32_t TreeItemHeight = 25;
// item之间 margin
static const uint32_t TreeItemMargin = 10;

// 一个层级item的后退量
static const uint32_t TreeItemXOffset = 10;

// 树上下边界距离
static const uint32_t UpDownMargin = 40;
static const uint32_t LeftRightMargin = 25;

// 图标与文本间距
static const uint32_t IconTextMargin = 10;

struct treeItemRectInfo
{
    ItpRect itemRect;
    tpTreeWidgetItem *itemPtr;
};

struct tpTreeWidgetData
{
    tpList<tpTreeWidgetItem *> topItemList;

    bool titleVisible;
    tpLabel *titleLabel;
    tpLabel *titleIconLabel;

    // 窗口显示不完的时候的偏移量
    // 滚动时偏移量，最小为0，最大为 当前展开显示的item数量 - 窗口可显示item数量 （为负值时则最大也为0）
    uint32_t offsetCount;

    // 窗口最大可显示item数量
    uint32_t maxShowItemCount;

    // 当前展开的可显示的item数量
    uint32_t curDisplayItemCount;

    // 当前在窗口区域内显示的item <item坐标，item>
    tpList<treeItemRectInfo> curShowItemList;

    bool isHover;
    ItpRect hoverRect;

    bool isSelect;
    ItpRect selectRect;

    // 当前选中的item
    tpTreeWidgetItem *curSelectItem;

    tpTreeWidgetData() : offsetCount(0), titleVisible(true), titleLabel(nullptr), titleIconLabel(titleIconLabel), maxShowItemCount(0), curDisplayItemCount(0), isHover(false), isSelect(false), curSelectItem(nullptr)
    {
    }

    ~tpTreeWidgetData()
    {
        if (titleLabel)
        {
            delete titleLabel;
            titleLabel = nullptr;
        }

        if (titleIconLabel)
        {
            delete titleIconLabel;
            titleIconLabel = nullptr;
        }
    }
};

void setItemSelected(const tpList<treeItemRectInfo> &itemList, const uint32_t &selectIndex)
{
    for (int32_t i = 0; i < itemList.size(); ++i)
    {
        auto itemData = itemList.at(i);
        if (i == selectIndex)
        {
            itemData.itemPtr->setSelected(true);
        }
        else
        {
            itemData.itemPtr->setSelected(false);
        }
    }
}

tpTreeWidget::tpTreeWidget(tpChildWidget *parent /* = nullptr*/)
    : tpChildWidget(parent)
{
    data_ = new tpTreeWidgetData();
    tpTreeWidgetData *widgetData = static_cast<tpTreeWidgetData *>(data_);

    widgetData->titleLabel = new tpLabel(this);
    widgetData->titleIconLabel = new tpLabel(this);

    widgetData->titleLabel->font()->setFontForeColor(_RGB(0, 0, 0));
    widgetData->titleLabel->font()->setFontSize(15);
    widgetData->titleLabel->font()->setFontStyle(TINY_FONT_BOLD);

    setVisible(true);
}

tpTreeWidget::~tpTreeWidget()
{
    tpTreeWidgetData *widgetData = static_cast<tpTreeWidgetData *>(data_);
    if (widgetData)
    {
        delete widgetData;
        widgetData = nullptr;
    }
}

void tpTreeWidget::setTitleVisible(const bool &visible)
{
    tpTreeWidgetData *widgetData = static_cast<tpTreeWidgetData *>(data_);
    if (!widgetData)
        return;

    widgetData->titleLabel->setVisible(visible);
    widgetData->titleIconLabel->setVisible(visible);

    onResizeEvent(nullptr);

    update();
}

void tpTreeWidget::setTitle(const tpString &title)
{
    tpTreeWidgetData *widgetData = static_cast<tpTreeWidgetData *>(data_);
    if (!widgetData)
        return;

    widgetData->titleLabel->setText(title);
    widgetData->titleLabel->update();
}

void tpTreeWidget::setTitleIcon(const tpString &titleIconPath)
{
    tpTreeWidgetData *widgetData = static_cast<tpTreeWidgetData *>(data_);
    if (!widgetData)
        return;

    tpShared<tpSurface> iconSurface = tpMakeShared<tpSurface>();
    iconSurface->fromFile(titleIconPath);

    widgetData->titleIconLabel->setBackGroundImage(iconSurface, true);
    widgetData->titleIconLabel->update();
}

tpTreeWidgetItem *tpTreeWidget::addTopItem(const tpString &text)
{
    tpTreeWidgetData *widgetData = static_cast<tpTreeWidgetData *>(data_);
    if (!widgetData)
        return nullptr;

    tpTreeWidgetItem *newTopItem = new tpTreeWidgetItem(text);
    widgetData->topItemList.emplace_back(newTopItem);

    widgetData->curDisplayItemCount = widgetData->topItemList.size();

    return newTopItem;
}

void tpTreeWidget::addTopItem(tpTreeWidgetItem *topItem)
{
    if (!topItem)
        return;

    tpTreeWidgetData *widgetData = static_cast<tpTreeWidgetData *>(data_);
    if (!widgetData)
        return;

    widgetData->curDisplayItemCount = widgetData->topItemList.size();

    widgetData->topItemList.emplace_back(topItem);
}

void tpTreeWidget::insertItem(int32_t index, tpTreeWidgetItem *item)
{
    if (!item)
        return;

    tpTreeWidgetData *widgetData = static_cast<tpTreeWidgetData *>(data_);
    if (!widgetData)
        return;

    widgetData->curDisplayItemCount = widgetData->topItemList.size();

    widgetData->topItemList.insertData(index, item);
}

int32_t tpTreeWidget::topLevelItemCount() const
{
    tpTreeWidgetData *widgetData = static_cast<tpTreeWidgetData *>(data_);
    if (!widgetData)
        return 0;

    return widgetData->topItemList.size();
}

tpTreeWidgetItem *tpTreeWidget::topLevelItem(int32_t index) const
{
    tpTreeWidgetData *widgetData = static_cast<tpTreeWidgetData *>(data_);
    if (!widgetData)
        return nullptr;

    for (int32_t i = 0; i < widgetData->topItemList.size(); ++i)
    {
        if (i == index)
        {
            return widgetData->topItemList.at(i);
        }
    }

    return nullptr;
}

bool tpTreeWidget::onPaintEvent(tpObjectPaintEvent *event)
{
    tpTreeWidgetData *widgetData = static_cast<tpTreeWidgetData *>(data_);
    if (!widgetData)
        return true;

    tpChildWidget::onPaintEvent(event);

    tpCanvas *paintCanvas = event->canvas();

    uint32_t drawItemStartY = UpDownMargin;

    if (widgetData->titleVisible)
    {
        // 配置标题栏, 文字居中向右偏移半个icon宽度
        uint32_t iconWidthHeight = widgetData->titleLabel->font()->pixelHeight();

        uint32_t titleTextX = (rect().w - widgetData->titleLabel->font()->pixelWidth()) / 2.0 + iconWidthHeight / 2.0;
        uint32_t titleTextY = (TitleHeight - widgetData->titleLabel->font()->pixelHeight()) / 2.0;

        widgetData->titleLabel->setRect(titleTextX, titleTextY, widgetData->titleLabel->font()->pixelWidth(), widgetData->titleLabel->font()->pixelHeight());

        widgetData->titleIconLabel->setRect(titleTextX - IconTextMargin - iconWidthHeight, titleTextY, iconWidthHeight, iconWidthHeight);

        drawItemStartY = TitleHeight;
    }

    // 绘制选中框
    if (widgetData->isSelect)
    {
        paintCanvas->box(widgetData->selectRect.x, widgetData->selectRect.y, widgetData->selectRect.x + widgetData->selectRect.w, widgetData->selectRect.y + widgetData->selectRect.h, _RGB(255, 255, 255));
    }

    // 绘制悬停框
    if (widgetData->isHover)
    {
        paintCanvas->box(widgetData->hoverRect.x, widgetData->hoverRect.y, widgetData->hoverRect.x + widgetData->hoverRect.w, widgetData->hoverRect.y + widgetData->hoverRect.h, _RGB(229, 243, 255));
    }

    widgetData->curShowItemList.clear();

    // 当前绘制item下标，包括子节点
    uint32_t curDrawIndex = 0;
    for (int32_t i = 0; i < widgetData->topItemList.size(); ++i)
    {
        tpTreeWidgetItem *curTopItem = widgetData->topItemList.at(i);
        if (!curTopItem)
            continue;

        drawItem(event, curTopItem, drawItemStartY, curDrawIndex);
    }

    return true;
}

bool tpTreeWidget::onMousePressEvent(tpMouseEvent *event)
{
    tpTreeWidgetData *widgetData = static_cast<tpTreeWidgetData *>(data_);
    if (!widgetData)
        return true;

    ItpPoint mousePoint = event->pos();

    // widgetData->curShowItemList
    int32_t pressIndex = queryPointIndex(mousePoint);
    if (pressIndex == -1)
    {
        widgetData->isSelect = false;
    }
    else
    {
        auto selectItemData = widgetData->curShowItemList.at(pressIndex);

        widgetData->isSelect = true;
        widgetData->selectRect = selectItemData.itemRect;
        setItemSelected(widgetData->curShowItemList, pressIndex);

        if (selectItemData.itemPtr->childCount() > 0)
        {
            selectItemData.itemPtr->setExpanded(!selectItemData.itemPtr->isExpanded());

            if (selectItemData.itemPtr->isExpanded())
                itemExpanded.emit(selectItemData.itemPtr);
            else
                itemCollapsed.emit(selectItemData.itemPtr);
        }

        if (widgetData->curSelectItem != selectItemData.itemPtr)
        {
            currentItemChanged.emit(widgetData->curSelectItem, selectItemData.itemPtr);
        }
        widgetData->curSelectItem = selectItemData.itemPtr;

        itemPressed.emit(selectItemData.itemPtr);
    }

    update();

    return true;
}

bool tpTreeWidget::onWheelEvent(tpWheelEvent *event)
{
    tpTreeWidgetData *widgetData = static_cast<tpTreeWidgetData *>(data_);
    if (!widgetData)
        return true;

    if (event->angleDelta() > 0)
    {
        widgetData->offsetCount++;
    }
    else
    {
        widgetData->offsetCount--;
        if (widgetData->offsetCount < 0)
            widgetData->offsetCount = 0;
    }
    return true;
}

bool tpTreeWidget::onLeaveEvent(tpObjectLeaveEvent *event)
{
    tpTreeWidgetData *widgetData = static_cast<tpTreeWidgetData *>(data_);
    if (!widgetData)
        return true;

    widgetData->isHover = false;

    return true;
}

bool tpTreeWidget::onMouseMoveEvent(tpMouseEvent *event)
{
    tpTreeWidgetData *widgetData = static_cast<tpTreeWidgetData *>(data_);
    if (!widgetData)
        return true;

    ItpPoint curMousePoint = event->pos();

    int32_t hoverIndex = queryPointIndex(curMousePoint);

    if (hoverIndex == -1)
    {
        widgetData->isHover = false;
    }
    else
    {
        widgetData->isHover = true;
        widgetData->hoverRect = widgetData->curShowItemList.at(hoverIndex).itemRect;
        update();
    }

    update();

    return true;
}

bool tpTreeWidget::onResizeEvent(tpObjectResizeEvent *event)
{
    tpTreeWidgetData *widgetData = static_cast<tpTreeWidgetData *>(data_);
    if (!widgetData)
        return true;

    // 窗口大小变化，计算可显示item数量
    uint32_t showHeight = rect().h - UpDownMargin * 2;

    if (widgetData->titleVisible)
        showHeight = rect().h - TitleHeight - UpDownMargin;

    widgetData->maxShowItemCount = std::ceil(1.0 * showHeight / TreeItemHeight);

    return true;
}

void tpTreeWidget::drawItem(tpObjectPaintEvent *event, tpTreeWidgetItem *topItem, const uint32_t &drawItemStartY, uint32_t &curIndex)
{
    tpTreeWidgetData *widgetData = static_cast<tpTreeWidgetData *>(data_);
    if (!widgetData)
        return;

    tpCanvas *paintCanvas = event->canvas();

    // curTopItem->setRect(LeftRightMargin, drawItemStartY + curDrawIndex * (TreeItemHeight + TreeItemMargin), rect().w, TreeItemHeight);

    uint32_t drawY = drawItemStartY + curIndex * (TreeItemHeight + TreeItemMargin);
    curIndex++;

    // 记录节点rect
    treeItemRectInfo itemRectInfo;
    itemRectInfo.itemPtr = topItem;
    itemRectInfo.itemRect.x = 0;
    itemRectInfo.itemRect.y = drawY;
    itemRectInfo.itemRect.w = rect().w;
    itemRectInfo.itemRect.h = TreeItemHeight;

    widgetData->curShowItemList.emplace_back(itemRectInfo);

    // 先绘制文字，文字要居中
    uint32_t textX = (rect().w - topItem->font()->pixelWidth()) / 2.0;

    // 根据节点层级添加偏移量
    textX += itemParentCount(topItem) * TreeItemXOffset;

    // uint32_t textY = (rect().h - topItem->font()->pixelHeight()) / 2.0;
    if (!topItem->text().empty())
    {
        paintCanvas->renderText(*topItem->font(), textX, drawY);
    }

    uint32_t fontTextHeight = topItem->font()->pixelHeight();

    // 绘制图标,图标在文字左侧
    tpShared<tpSurface> itemIcon = topItem->icon();
    if (itemIcon && itemIcon->hasSurface())
    {
        auto drawSurface = itemIcon->scaled(fontTextHeight * 0.7, fontTextHeight * 0.7);

        uint32_t iconX = textX - IconTextMargin - drawSurface->width();
        uint32_t iconY = (fontTextHeight - drawSurface->height()) / 2.0;

        paintCanvas->paintSurface(iconX, drawY + iconY, drawSurface);
    }

    // 如果子节点展开，需要绘制 展开符号
    if (topItem->childCount() > 0)
    {
        tpShared<tpSurface> expandSurface = tpMakeShared<tpSurface>();
        if (topItem->isExpanded())
        {
            expandSurface->fromFile("/usr/res/tinyPiX/desktopGUI/上箭头.png");
        }
        else
        {
            expandSurface->fromFile("/usr/res/tinyPiX/desktopGUI/下箭头.png");
        }

        auto drawSurface = expandSurface->scaled(fontTextHeight * 0.7, fontTextHeight * 0.7);

        uint32_t iconX = textX + topItem->font()->pixelWidth() + IconTextMargin;
        uint32_t iconY = (fontTextHeight - drawSurface->height()) / 2.0;

        paintCanvas->paintSurface(iconX, drawY + iconY, drawSurface);

        if (topItem->isExpanded())
        {
            for (int32_t i = 0; i < topItem->childCount(); ++i)
            {
                tpTreeWidgetItem *childItem = topItem->child(i);
                if (!childItem)
                    return;

                drawItem(event, childItem, drawItemStartY, curIndex);
            }
        }
    }
}

uint32_t tpTreeWidget::itemParentCount(tpTreeWidgetItem *item)
{
    // return 0;
    uint32_t resCount = 0;

    if (!item)
        return resCount;

    if (!item->parent())
        return resCount;

    std::function<void(tpTreeWidgetItem *)> findFunc = [&](tpTreeWidgetItem *findItem)
    {
        resCount++;

        if (!findItem)
            return;

        if (!findItem->parent())
            return;

        findFunc(findItem->parent());
    };

    findFunc(item->parent());

    return resCount;
}

int32_t tpTreeWidget::queryPointIndex(const ItpPoint &point)
{
    tpTreeWidgetData *widgetData = static_cast<tpTreeWidgetData *>(data_);
    if (!widgetData)
        return -1;

    int32_t left = 0;
    int32_t right = widgetData->curShowItemList.size() - 1;

    while (left <= right)
    {
        int32_t mid = left + (right - left) / 2;
        auto itemInfo = widgetData->curShowItemList.at(mid);

        if (itemInfo.itemRect.contains(point))
        {
            return mid;
        }
        else if (point.y < itemInfo.itemRect.y)
        {
            right = mid - 1;
        }
        else
        {
            left = mid + 1;
        }
    }

    return -1;
}

DESKTOP_GUI_NAMESPACE_END
