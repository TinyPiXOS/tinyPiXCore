#include "GUIFramework/desktopGUI/widgets/TpTreeWidget.h"
#include "TpList.h"
#include "TpLabel.h"
#include "TpImage.h"
#include "TpCanvas.h"
#include "TpList.h"
#include "TpUtils.h"
#include "TpImage.h"

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
    TpRect itemRect;
    TpTreeWidgetItem *itemPtr;
};

struct TpTreeWidgetData
{
    TpList<TpTreeWidgetItem *> topItemList;

    bool titleVisible;
    TpLabel *titleLabel;
    TpLabel *titleIconLabel;

    // 窗口显示不完的时候的偏移量
    // 滚动时偏移量，最小为0，最大为 当前展开显示的item数量 - 窗口可显示item数量 （为负值时则最大也为0）
    uint32_t offsetCount;

    // 窗口最大可显示item数量
    uint32_t maxShowItemCount;

    // 当前展开的可显示的item数量
    uint32_t curDisplayItemCount;

    // 当前在窗口区域内显示的item <item坐标，item>
    TpList<treeItemRectInfo> curShowItemList;

    bool isHover;
    TpRect hoverRect;

    bool isSelect;
    TpRect selectRect;

    // 当前选中的item
    TpTreeWidgetItem *curSelectItem;

    TpTreeWidgetData() : offsetCount(0), titleVisible(true), titleLabel(nullptr), titleIconLabel(titleIconLabel), maxShowItemCount(0), curDisplayItemCount(0), isHover(false), isSelect(false), curSelectItem(nullptr)
    {
    }

    ~TpTreeWidgetData()
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

void setItemSelected(const TpList<treeItemRectInfo> &itemList, const uint32_t &selectIndex)
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

TpTreeWidget::TpTreeWidget(TpChildWidget *parent /* = nullptr*/)
    : TpChildWidget(parent)
{
    data_ = new TpTreeWidgetData();
    TpTreeWidgetData *widgetData = static_cast<TpTreeWidgetData *>(data_);

    widgetData->titleLabel = new TpLabel(this);
    widgetData->titleIconLabel = new TpLabel(this);

    widgetData->titleLabel->font()->setFontForeColor(_RGB(0, 0, 0));
    widgetData->titleLabel->font()->setFontSize(15);
    widgetData->titleLabel->font()->setFontStyle(TINY_FONT_BOLD);

    setVisible(true);
}

TpTreeWidget::~TpTreeWidget()
{
    TpTreeWidgetData *widgetData = static_cast<TpTreeWidgetData *>(data_);
    if (widgetData)
    {
        delete widgetData;
        widgetData = nullptr;
    }
}

void TpTreeWidget::setTitleVisible(const bool &visible)
{
    TpTreeWidgetData *widgetData = static_cast<TpTreeWidgetData *>(data_);
    if (!widgetData)
        return;

    widgetData->titleLabel->setVisible(visible);
    widgetData->titleIconLabel->setVisible(visible);

    onResizeEvent(nullptr);

    update();
}

void TpTreeWidget::setTitle(const TpString &title)
{
    TpTreeWidgetData *widgetData = static_cast<TpTreeWidgetData *>(data_);
    if (!widgetData)
        return;

    widgetData->titleLabel->setText(title);
    widgetData->titleLabel->update();
}

void TpTreeWidget::setTitleIcon(const TpString &titleIconPath)
{
    TpTreeWidgetData *widgetData = static_cast<TpTreeWidgetData *>(data_);
    if (!widgetData)
        return;

    widgetData->titleIconLabel->setBackGroundImage(TpImage(titleIconPath));
    widgetData->titleIconLabel->update();
}

TpTreeWidgetItem *TpTreeWidget::addTopItem(const TpString &text)
{
    TpTreeWidgetData *widgetData = static_cast<TpTreeWidgetData *>(data_);
    if (!widgetData)
        return nullptr;

    TpTreeWidgetItem *newTopItem = new TpTreeWidgetItem(text);
    widgetData->topItemList.emplace_back(newTopItem);

    widgetData->curDisplayItemCount = widgetData->topItemList.size();

    return newTopItem;
}

void TpTreeWidget::addTopItem(TpTreeWidgetItem *topItem)
{
    if (!topItem)
        return;

    TpTreeWidgetData *widgetData = static_cast<TpTreeWidgetData *>(data_);
    if (!widgetData)
        return;

    widgetData->curDisplayItemCount = widgetData->topItemList.size();

    widgetData->topItemList.emplace_back(topItem);
}

void TpTreeWidget::insertItem(int32_t index, TpTreeWidgetItem *item)
{
    if (!item)
        return;

    TpTreeWidgetData *widgetData = static_cast<TpTreeWidgetData *>(data_);
    if (!widgetData)
        return;

    widgetData->curDisplayItemCount = widgetData->topItemList.size();

    widgetData->topItemList.insertData(index, item);
}

int32_t TpTreeWidget::topLevelItemCount() const
{
    TpTreeWidgetData *widgetData = static_cast<TpTreeWidgetData *>(data_);
    if (!widgetData)
        return 0;

    return widgetData->topItemList.size();
}

TpTreeWidgetItem *TpTreeWidget::topLevelItem(int32_t index) const
{
    TpTreeWidgetData *widgetData = static_cast<TpTreeWidgetData *>(data_);
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

bool TpTreeWidget::onPaintEvent(TpObjectPaintEvent *event)
{
    TpTreeWidgetData *widgetData = static_cast<TpTreeWidgetData *>(data_);
    if (!widgetData)
        return true;

    TpChildWidget::onPaintEvent(event);

    TpCanvas *paintCanvas = event->canvas();

    uint32_t drawItemStartY = UpDownMargin;

    if (widgetData->titleVisible)
    {
        // 配置标题栏, 文字居中向右偏移半个icon宽度
        uint32_t iconWidthHeight = widgetData->titleLabel->font()->pixelHeight();

        uint32_t titleTextX = (rect().width() - widgetData->titleLabel->font()->pixelWidth()) / 2.0 + iconWidthHeight / 2.0;
        uint32_t titleTextY = (TitleHeight - widgetData->titleLabel->font()->pixelHeight()) / 2.0;

        widgetData->titleLabel->setRect(titleTextX, titleTextY, widgetData->titleLabel->font()->pixelWidth(), widgetData->titleLabel->font()->pixelHeight());

        widgetData->titleIconLabel->setRect(titleTextX - IconTextMargin - iconWidthHeight, titleTextY, iconWidthHeight, iconWidthHeight);

        drawItemStartY = TitleHeight;
    }

    // 绘制选中框
    if (widgetData->isSelect)
    {
        paintCanvas->box(widgetData->selectRect.x(), widgetData->selectRect.y(),
                         widgetData->selectRect.x() + widgetData->selectRect.width(), widgetData->selectRect.y() + widgetData->selectRect.height(), _RGB(255, 255, 255));
    }

    // 绘制悬停框
    if (widgetData->isHover)
    {
        paintCanvas->box(widgetData->hoverRect.x(), widgetData->hoverRect.y(),
                         widgetData->hoverRect.x() + widgetData->hoverRect.width(), widgetData->hoverRect.y() + widgetData->hoverRect.height(), _RGB(229, 243, 255));
    }

    widgetData->curShowItemList.clear();

    // 当前绘制item下标，包括子节点
    uint32_t curDrawIndex = 0;
    for (int32_t i = 0; i < widgetData->topItemList.size(); ++i)
    {
        TpTreeWidgetItem *curTopItem = widgetData->topItemList.at(i);
        if (!curTopItem)
            continue;

        drawItem(event, curTopItem, drawItemStartY, curDrawIndex);
    }

    return true;
}

bool TpTreeWidget::onMousePressEvent(TpMouseEvent *event)
{
    TpTreeWidgetData *widgetData = static_cast<TpTreeWidgetData *>(data_);
    if (!widgetData)
        return true;

    TpPoint mousePoint = event->pos();

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

bool TpTreeWidget::onWheelEvent(TpWheelEvent *event)
{
    TpTreeWidgetData *widgetData = static_cast<TpTreeWidgetData *>(data_);
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

bool TpTreeWidget::onLeaveEvent(TpObjectLeaveEvent *event)
{
    TpTreeWidgetData *widgetData = static_cast<TpTreeWidgetData *>(data_);
    if (!widgetData)
        return true;

    widgetData->isHover = false;

    return true;
}

bool TpTreeWidget::onMouseMoveEvent(TpMouseEvent *event)
{
    TpTreeWidgetData *widgetData = static_cast<TpTreeWidgetData *>(data_);
    if (!widgetData)
        return true;

    TpPoint curMousePoint = event->pos();

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

bool TpTreeWidget::onResizeEvent(TpObjectResizeEvent *event)
{
    TpTreeWidgetData *widgetData = static_cast<TpTreeWidgetData *>(data_);
    if (!widgetData)
        return true;

    // 窗口大小变化，计算可显示item数量
    uint32_t showHeight = rect().height() - UpDownMargin * 2;

    if (widgetData->titleVisible)
        showHeight = rect().height() - TitleHeight - UpDownMargin;

    widgetData->maxShowItemCount = std::ceil(1.0 * showHeight / TreeItemHeight);

    return true;
}

void TpTreeWidget::drawItem(TpObjectPaintEvent *event, TpTreeWidgetItem *topItem, const uint32_t &drawItemStartY, uint32_t &curIndex)
{
    TpTreeWidgetData *widgetData = static_cast<TpTreeWidgetData *>(data_);
    if (!widgetData)
        return;

    TpCanvas *paintCanvas = event->canvas();

    // curTopItem->setRect(LeftRightMargin, drawItemStartY + curDrawIndex * (TreeItemHeight + TreeItemMargin), rect().w, TreeItemHeight);

    uint32_t drawY = drawItemStartY + curIndex * (TreeItemHeight + TreeItemMargin);
    curIndex++;

    // 记录节点rect
    treeItemRectInfo itemRectInfo;
    itemRectInfo.itemPtr = topItem;

    itemRectInfo.itemRect.setRect(0, drawY, rect().width(), TreeItemHeight);

    widgetData->curShowItemList.emplace_back(itemRectInfo);

    // 先绘制文字，文字要居中
    uint32_t textX = (rect().width() - topItem->font()->pixelWidth()) / 2.0;

    // 根据节点层级添加偏移量
    textX += itemParentCount(topItem) * TreeItemXOffset;

    // uint32_t textY = (rect().h - topItem->font()->pixelHeight()) / 2.0;
    if (!topItem->text().empty())
    {
        paintCanvas->renderText(*topItem->font(), textX, drawY);
    }

    uint32_t fontTextHeight = topItem->font()->pixelHeight();

    // 绘制图标,图标在文字左侧
    TpImage itemIcon = topItem->icon();
    if (!itemIcon.isNull())
    {
        auto drawSurface = itemIcon.scaled(fontTextHeight * 0.7, fontTextHeight * 0.7);

        uint32_t iconX = textX - IconTextMargin - drawSurface.width();
        uint32_t iconY = (fontTextHeight - drawSurface.height()) / 2.0;

        paintCanvas->paintImage(iconX, drawY + iconY, drawSurface);
    }

    // 如果子节点展开，需要绘制 展开符号
    if (topItem->childCount() > 0)
    {
        TpImage expandSurface;
        if (topItem->isExpanded())
        {
            expandSurface.load("/usr/res/tinyPiX/desktopGUI/上箭头.png");
        }
        else
        {
            expandSurface.load("/usr/res/tinyPiX/desktopGUI/下箭头.png");
        }

        auto drawSurface = expandSurface.scaled(fontTextHeight * 0.7, fontTextHeight * 0.7);

        uint32_t iconX = textX + topItem->font()->pixelWidth() + IconTextMargin;
        uint32_t iconY = (fontTextHeight - drawSurface.height()) / 2.0;

        paintCanvas->paintImage(iconX, drawY + iconY, drawSurface);

        if (topItem->isExpanded())
        {
            for (int32_t i = 0; i < topItem->childCount(); ++i)
            {
                TpTreeWidgetItem *childItem = topItem->child(i);
                if (!childItem)
                    return;

                drawItem(event, childItem, drawItemStartY, curIndex);
            }
        }
    }
}

uint32_t TpTreeWidget::itemParentCount(TpTreeWidgetItem *item)
{
    // return 0;
    uint32_t resCount = 0;

    if (!item)
        return resCount;

    if (!item->parent())
        return resCount;

    std::function<void(TpTreeWidgetItem *)> findFunc = [&](TpTreeWidgetItem *findItem)
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

int32_t TpTreeWidget::queryPointIndex(const TpPoint &point)
{
    TpTreeWidgetData *widgetData = static_cast<TpTreeWidgetData *>(data_);
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
        else if (point.y() < itemInfo.itemRect.y())
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
