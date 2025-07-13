/***
 * @Author: hywang
 * @Date: 2024-06-14 10:18:19
 * @LastEditors: hywang
 * @LastEditTime: 2024-06-14 15:06:23
 * @FilePath: /pix-singlegui/PixSingleGUI/src/SingleGUI/widgets/tpTreeWidget.cpp
 * @Description:
 * @
 * @PiXOS
 */

#include "tpTreeWidget.h"
#include "tpList.h"
#include "tpVector.h"
#include "tpFont.h"
#include "tpCanvas.h"
#include <cmath>

#ifndef ITEM_V_PIX
#define ITEM_V_PIX 3
#endif

// 子节点与父节点的X坐标偏移量
#ifndef ITEM_OFFSET
#define ITEM_OFFSET 10
#endif

// 顶层节点展开收缩字符绘制X坐标
#ifndef TOP_ITEM_EX_X
#define TOP_ITEM_EX_X 8
#endif

// 顶层节点文本绘制X坐标
#ifndef TOP_ITEM_TEXT_X
#define TOP_ITEM_TEXT_X 25
#endif

struct PiTreeWidgetPrivData
{
    ItpSize defaultSize;

    // 显示区域，从哪个item的下标开始显示
    uint32_t showMinIndex;

    bool isHover;
    // 当前选中的下标
    tpVector<uint32_t> selectIndexList;

    // 窗口最大显示条数
    uint32_t maxShowCount;

    // 当前需要显示的条数
    uint32_t curExpandItemCount;

    // 是否绘制进度条
    bool isDrawScrollBar;

    // 是否按住ctrl
    bool isPressCtrl;
    bool isPressShift;

    // 选择模式
    tinyPiX::ItemSelectionMode selectMode;

    // 排序模式
    // tpPair<int32_t, tinyPiX::SortOrder> sortMode;

    // 标题文本
    tpVector<tpString> headerLabels;
    uint32_t columnCount;

    // 所有顶层节点
    tpList<tpTreeWidgetItem *> topItemList;

    // 当前窗口显示的Item，可能包括任何一级节点
    tpList<tpTreeWidgetItem *> curShowItemList;

    PiTreeWidgetPrivData()
        : columnCount(1), curExpandItemCount(0), showMinIndex(0), selectMode(tinyPiX::SingleSelection)
    {
        // 默认无排序模式
        // sortMode.first = -1;

        tpFont defaultFont;
        defaultFont.setText("Test Text");

        defaultSize.w = -1;
        defaultSize.h = defaultFont.pixelHeight();
    }

    virtual ~PiTreeWidgetPrivData()
    {
    }
};

// 计算item有几级父节点，用于决定绘制的X偏移量
void CaculateItemLevel(tpTreeWidgetItem *item, uint32_t &parentCount)
{
    if (item->parent())
    {
        ++parentCount;
        CaculateItemLevel(item->parent(), parentCount);
    }
}

int32_t partitionFunc(int32_t column, tinyPiX::SortOrder order, tpList<tpTreeWidgetItem *> &arr, int32_t low, int32_t high)
{
    tpTreeWidgetItem *pivot = arr.at(high);
    int32_t i = (low - 1);
    for (int32_t j = low; j <= high - 1; j++)
    {
        tpTreeWidgetItem *firstItem = arr.at(j);
        tpTreeWidgetItem *secondItem = pivot;

        if (order == tinyPiX::DescendingOrder)
        {
            firstItem = pivot;
            secondItem = arr.at(j);
        }

        if (firstItem->text(column) < secondItem->text(column))
        {
            i++;

            auto it1 = std::next(arr.begin(), i);
            auto it2 = std::next(arr.begin(), j);
            std::swap(*it1, *it2);
        }
    }

    auto it1 = std::next(arr.begin(), i + 1);
    auto it2 = std::next(arr.begin(), high);
    std::swap(*it1, *it2);

    return (i + 1);
};

void quickSortFunc(int32_t column, tinyPiX::SortOrder order, tpList<tpTreeWidgetItem *> &arr, int32_t low, int32_t high)
{
    if (low < high)
    {
        int32_t pi = partitionFunc(column, order, arr, low, high);

        quickSortFunc(column, order, arr, low, pi - 1);
        quickSortFunc(column, order, arr, pi + 1, high);
    }
}

tpTreeWidget::tpTreeWidget(tpChildWidget *parent)
    : tpChildWidget(parent), privData(new PiTreeWidgetPrivData())
{
}

tpTreeWidget::~tpTreeWidget()
{
}

int32_t tpTreeWidget::columnCount()
{
    PiTreeWidgetPrivData *privData = (PiTreeWidgetPrivData *)this->privData;
    if (!privData)
        return 0;

    return privData->columnCount;
}

void tpTreeWidget::setColumnCount(int32_t columns)
{
    PiTreeWidgetPrivData *privData = (PiTreeWidgetPrivData *)this->privData;
    if (!privData)
        return;

    privData->columnCount = columns;
}

tpTreeWidgetItem *tpTreeWidget::topLevelItem(int32_t index) const
{
    PiTreeWidgetPrivData *privData = (PiTreeWidgetPrivData *)this->privData;
    if (!privData)
        return nullptr;

    if (index >= privData->topItemList.size())
        return nullptr;

    return privData->topItemList.at(index);
}

int32_t tpTreeWidget::topLevelItemCount() const
{
    PiTreeWidgetPrivData *privData = (PiTreeWidgetPrivData *)this->privData;
    if (!privData)
        return 0;

    return privData->topItemList.size();
}

void tpTreeWidget::insertTopLevelItem(int32_t index, tpTreeWidgetItem *item)
{
    PiTreeWidgetPrivData *privData = (PiTreeWidgetPrivData *)this->privData;
    if (!privData)
        return;

    privData->topItemList.insertData(index, item);
    item->setParent(nullptr);

    RefreshShowIndex();
    update();
}

void tpTreeWidget::addTopLevelItem(tpTreeWidgetItem *item)
{
    PiTreeWidgetPrivData *privData = (PiTreeWidgetPrivData *)this->privData;
    if (!privData)
        return;

    insertTopLevelItem(privData->topItemList.size(), item);

    // privData->topItemList.emplace_back(item);
    // item->setParent(nullptr);

    // RefreshShowIndex();
    // update();
}

tpTreeWidgetItem *tpTreeWidget::takeTopLevelItem(int32_t index)
{
    PiTreeWidgetPrivData *privData = (PiTreeWidgetPrivData *)this->privData;
    if (!privData)
        return nullptr;

    if (index >= privData->topItemList.size())
        return nullptr;

    tpTreeWidgetItem *resItem = privData->topItemList.at(index);
    if (resItem->parent())
    {
        resItem->parent()->removeChild(resItem);
        resItem->clear();
    }

    privData->topItemList.remove(index);

    RefreshShowIndex();
    update();

    return resItem;
}

int32_t tpTreeWidget::indexOfTopLevelItem(tpTreeWidgetItem *item) const
{
    PiTreeWidgetPrivData *privData = (PiTreeWidgetPrivData *)this->privData;
    if (!privData)
        return -1;

    for (int32_t i = 0; i < privData->topItemList.size(); ++i)
    {
        if (privData->topItemList.at(i) == item)
            return i;
    }
    return -1;
}

void tpTreeWidget::setHeaderLabels(const tpVector<tpString> &labels)
{
    PiTreeWidgetPrivData *privData = (PiTreeWidgetPrivData *)this->privData;
    if (!privData)
        return;

    privData->headerLabels = labels;
    update();
}

void tpTreeWidget::setHeaderLabel(const tpString &label)
{
    PiTreeWidgetPrivData *privData = (PiTreeWidgetPrivData *)this->privData;
    if (!privData)
        return;

    privData->headerLabels.clear();
    privData->headerLabels.emplace_back(label);
    update();
}

tpTreeWidgetItem *tpTreeWidget::currentItem() const
{
    PiTreeWidgetPrivData *privData = (PiTreeWidgetPrivData *)this->privData;
    if (!privData)
        return nullptr;

    if (privData->selectIndexList.size() == 0)
        return nullptr;

    return privData->topItemList.at(privData->selectIndexList.back());
}

int32_t tpTreeWidget::currentColumn() const
{
    return 0;
}

void tpTreeWidget::setCurrentItem(tpTreeWidgetItem *item)
{
    PiTreeWidgetPrivData *privData = (PiTreeWidgetPrivData *)this->privData;
    if (!privData)
        return;

    for (int32_t i = 0; i < privData->topItemList.size(); ++i)
    {
        if (privData->topItemList.at(i) == item)
        {
            privData->selectIndexList.clear();
            privData->selectIndexList.emplace_back(i);
            break;
        }
    }
}

void tpTreeWidget::setCurrentItem(tpTreeWidgetItem *item, int32_t column)
{
    PiTreeWidgetPrivData *privData = (PiTreeWidgetPrivData *)this->privData;
    if (!privData)
        return;

    for (int32_t i = 0; i < privData->topItemList.size(); ++i)
    {
        if (privData->topItemList.at(i) == item)
        {
            privData->selectIndexList.clear();
            privData->selectIndexList.emplace_back(i);
            break;
        }
    }
}

tpTreeWidgetItem *tpTreeWidget::itemAt(const ItpPoint &p) const
{
    return nullptr;
}

tpTreeWidgetItem *tpTreeWidget::itemAt(int32_t x, int32_t y) const
{
    return nullptr;
}

int32_t tpTreeWidget::sortColumn() const
{
    return 0;
}

void tpTreeWidget::sortItems(int32_t column, tinyPiX::SortOrder order)
{
    PiTreeWidgetPrivData *privData = (PiTreeWidgetPrivData *)this->privData;
    if (!privData)
        return;

    quickSortFunc(column, order, privData->topItemList, 0, privData->topItemList.size() - 1);

    update();
}

void tpTreeWidget::editItem(tpTreeWidgetItem *item, int32_t column)
{
}

tpChildWidget *tpTreeWidget::itemWidget(tpTreeWidgetItem *item, int32_t column) const
{
    return nullptr;
}

void tpTreeWidget::setItemWidget(tpTreeWidgetItem *item, int32_t column, tpChildWidget *widget)
{
}

void tpTreeWidget::removeItemWidget(tpTreeWidgetItem *item, int32_t column)
{
}

bool tpTreeWidget::isItemSelected(const tpTreeWidgetItem *item) const
{
    PiTreeWidgetPrivData *privData = (PiTreeWidgetPrivData *)this->privData;
    if (!privData)
        return false;

    for (auto &curItem : privData->topItemList)
    {
        if (item == curItem)
            return curItem->isSelected();
    }

    return false;
}

void tpTreeWidget::setItemSelected(const tpTreeWidgetItem *item, bool select)
{
    tpList<tpTreeWidgetItem *> resItemList;

    PiTreeWidgetPrivData *privData = (PiTreeWidgetPrivData *)this->privData;
    if (!privData)
        return;

    for (auto &curItem : privData->topItemList)
    {
        if (curItem == item)
            curItem->setSelected(true);
    }

    update();
}

tpList<tpTreeWidgetItem *> tpTreeWidget::selectedItems() const
{
    tpList<tpTreeWidgetItem *> resItemList;

    PiTreeWidgetPrivData *privData = (PiTreeWidgetPrivData *)this->privData;
    if (!privData)
        return resItemList;

    for (auto &curItem : privData->topItemList)
    {
        if (curItem->isSelected())
            resItemList.emplace_back(curItem);
    }

    return resItemList;
}

tpList<tpTreeWidgetItem *> tpTreeWidget::findItems(const tpString &text, tinyPiX::MatchFlags flags, int32_t column) const
{
    tpList<tpTreeWidgetItem *> resItemList;

    PiTreeWidgetPrivData *privData = (PiTreeWidgetPrivData *)this->privData;
    if (!privData)
        return resItemList;

    for (auto &curItem : privData->topItemList)
    {
        if (curItem->text(column).compare(text) == 0)
            resItemList.emplace_back(curItem);
    }

    return resItemList;
}

bool tpTreeWidget::isItemExpanded(const tpTreeWidgetItem *item) const
{
    PiTreeWidgetPrivData *privData = (PiTreeWidgetPrivData *)this->privData;
    if (!privData)
        return false;

    for (auto &curItem : privData->topItemList)
    {
        if (item == curItem)
            return curItem->isExpanded();
    }

    return false;
}

void tpTreeWidget::setItemExpanded(const tpTreeWidgetItem *item, bool expand)
{
    PiTreeWidgetPrivData *privData = (PiTreeWidgetPrivData *)this->privData;
    if (!privData)
        return;

    for (auto &curItem : privData->topItemList)
    {
        if (item == curItem)
        {
            curItem->setExpanded(expand);
            update();
            break;
        }
    }
}

void tpTreeWidget::setSelectionModel(tinyPiX::ItemSelectionMode selectMode)
{
    PiTreeWidgetPrivData *privData = (PiTreeWidgetPrivData *)this->privData;
    if (!privData)
        return;

    privData->selectMode = selectMode;
}

void tpTreeWidget::expandItem(const tpTreeWidgetItem *item)
{
    PiTreeWidgetPrivData *privData = (PiTreeWidgetPrivData *)this->privData;
    if (!privData)
        return;

    for (auto &curItem : privData->topItemList)
    {
        if (item == curItem)
        {
            curItem->setExpanded(true);
            update();
            break;
        }
    }
}

void tpTreeWidget::collapseItem(const tpTreeWidgetItem *item)
{
}

void tpTreeWidget::clear()
{
    PiTreeWidgetPrivData *privData = (PiTreeWidgetPrivData *)this->privData;
    if (!privData)
        return;

    for (auto &curItem : privData->topItemList)
    {
        if (curItem)
        {
            delete curItem;
            curItem = nullptr;
        }
    }
    privData->topItemList.clear();
}

bool tpTreeWidget::onMoveEvent(tpObjectMoveEvent *event)
{
    return false;
}

bool tpTreeWidget::onKeyPressEvent(tpKeyboardEvent *event)
{
    PiTreeWidgetPrivData *privData = (PiTreeWidgetPrivData *)this->privData;
    if (!privData)
        return false;

    if (event->scancode() == TP_SCANCODE_LCTRL || event->scancode() == TP_SCANCODE_RCTRL)
    {
        privData->isPressCtrl = event->state();
    }

    if (event->scancode() == TP_SCANCODE_LSHIFT || event->scancode() == TP_SCANCODE_RSHIFT)
    {
        privData->isPressShift = event->state();
    }

    return true;
}

bool tpTreeWidget::onKeyReleaseEvent(tpKeyboardEvent *event)
{
    PiTreeWidgetPrivData *privData = (PiTreeWidgetPrivData *)this->privData;
    if (!privData)
        return false;

    if (event->scancode() == TP_SCANCODE_LCTRL || event->scancode() == TP_SCANCODE_RCTRL)
    {
        privData->isPressCtrl = event->state();
    }

    if (event->scancode() == TP_SCANCODE_LSHIFT || event->scancode() == TP_SCANCODE_RSHIFT)
    {
        privData->isPressShift = event->state();
    }

    return true;
}

bool tpTreeWidget::onResizeEvent(tpObjectResizeEvent *event)
{
    RefreshShowIndex();
    update();

    return true;
}

bool tpTreeWidget::onMousePressEvent(tpMouseEvent *event)
{
    PiTreeWidgetPrivData *privData = (PiTreeWidgetPrivData *)this->privData;
    if (!privData)
        return false;

    if (event->button() != BUTTON_LEFT)
    {
        // 右键或滚轮键，清空所有选择项，选中当前项
        uint32_t curIndex = getItemIndex(event->pos().x, event->pos().y);
        if (curIndex < privData->curShowItemList.size())
        {
            privData->selectIndexList.clear();
            privData->selectIndexList.emplace_back(curIndex);
        }

        update();
        return true;
    }

    // 鼠标左键
    // 根据点击下标，获取当前item
    uint32_t curIndex = getItemIndex(event->pos().x, event->pos().y);

    if (curIndex >= privData->curShowItemList.size())
        return true;

    tpTreeWidgetItem *curItem = privData->curShowItemList.at(curIndex);

    // 如果点击的X坐标在箭头处，切换item的展开状态
    uint32_t parentCount = 0;
    CaculateItemLevel(curItem, parentCount);

    int32_t characterMin = TOP_ITEM_EX_X - 10 + parentCount * ITEM_OFFSET;
    int32_t characterMax = TOP_ITEM_EX_X + 10 + parentCount * ITEM_OFFSET;

    // 点击箭头位置，切换展开状态
    if (event->pos().x < characterMax && event->pos().x > characterMin)
    {
        curItem->setExpanded(!curItem->isExpanded());
        if (curItem->isExpanded())
            ++privData->curExpandItemCount;
        else
            --privData->curExpandItemCount;

        update();
    }

    // 按下
    itemPressed.emit(curItem, 0);

    return false;
}

bool tpTreeWidget::onMouseRleaseEvent(tpMouseEvent *event)
{
    PiTreeWidgetPrivData *privData = (PiTreeWidgetPrivData *)this->privData;
    if (!privData)
        return false;

    // 根据点击下标，获取当前item
    uint32_t curIndex = getItemIndex(event->pos().x, event->pos().y);

    if (curIndex >= privData->curShowItemList.size())
        return true;

    tpTreeWidgetItem *curItem = privData->curShowItemList.at(curIndex);

    // 如果点击的X坐标在箭头处，切换item的展开状态
    uint32_t parentCount = 0;
    CaculateItemLevel(curItem, parentCount);

    int32_t characterMin = TOP_ITEM_EX_X - 10 + parentCount * ITEM_OFFSET;
    int32_t characterMax = TOP_ITEM_EX_X + 10 + parentCount * ITEM_OFFSET;

    // 释放
    if (event->pos().x > characterMax)
    {
        static uint32_t lastIndex = -1;
        if (curIndex != lastIndex)
        {
            // 发送选中item改变信号
            tpTreeWidgetItem *preItem = nullptr;
            if (lastIndex != -1)
                preItem = privData->curShowItemList.at(lastIndex);

            currentItemChanged.emit(curItem, preItem);

            // 根据选择模式不同，操作不同
            if (privData->selectMode == tinyPiX::SingleSelection)
            {
                for (const auto &selectIndex : privData->selectIndexList)
                {
                    privData->curShowItemList.at(selectIndex)->setSelected(false);
                }
                curItem->setSelected(true);

                privData->selectIndexList.clear();
                privData->selectIndexList.emplace_back(curIndex);
            }
            else if (privData->selectMode == tinyPiX::MultiSelection)
            {
                curItem->setSelected(true);
                privData->selectIndexList.emplace_back(curIndex);
            }
            else if (privData->selectMode == tinyPiX::ExtendedSelection)
            {
                // 如果按下ctrl，选择项+1，否则，选择项修改为当前index
                if (privData->isPressCtrl)
                {
                    // 如果同时按下ctrl，shitf，选择上一个选择项到当前项中所有
                    if (privData->isPressShift)
                    {
                        if (lastIndex != -1)
                        {
                            uint32_t minIndex = lastIndex > curIndex ? curIndex : lastIndex;
                            uint32_t maxIndex = lastIndex > curIndex ? lastIndex : curIndex;
                            // std::cout << "minIndex " << minIndex << "  maxIndex " << maxIndex << std::endl;
                            for (int32_t i = minIndex; i <= maxIndex; ++i)
                            {
                                if (!privData->selectIndexList.contains(i))
                                {
                                    privData->curShowItemList.at(i)->setSelected(true);

                                    privData->selectIndexList.emplace_back(i);
                                }
                            }
                        }
                        else
                        {
                            if (!privData->selectIndexList.contains(curIndex))
                            {
                                privData->curShowItemList.at(curIndex)->setSelected(true);

                                privData->selectIndexList.emplace_back(curIndex);
                            }
                        }
                    }
                    else
                    {
                        if (!privData->selectIndexList.contains(curIndex))
                        {
                            privData->curShowItemList.at(curIndex)->setSelected(true);

                            privData->selectIndexList.emplace_back(curIndex);
                        }
                    }
                }
                else
                {
                    privData->selectIndexList.clear();
                    privData->selectIndexList.emplace_back(curIndex);
                }
            }
            else
            {
            }

            lastIndex = curIndex;

            update();
        }
    }

    itemClicked.emit(curItem, 0);

    return false;
}

bool tpTreeWidget::onWheelEvent(tpWheelEvent *event)
{
    PiTreeWidgetPrivData *privData = (PiTreeWidgetPrivData *)this->privData;
    if (!privData)
        return false;

    if (event->angleDelta() > 0)
    {
        if (privData->curExpandItemCount <= privData->maxShowCount)
            return true;

        if (privData->showMinIndex == 0)
            return true;

        privData->showMinIndex--;
        // std::cout << "-- " << privData->showMinIndex << std::endl;
    }
    else
    {
        if (privData->curExpandItemCount <= privData->maxShowCount)
            return true;

        if ((privData->showMinIndex + privData->maxShowCount) >= privData->curExpandItemCount)
            return true;

        privData->showMinIndex++;
        // std::cout << "++ " << privData->showMinIndex << std::endl;
    }

    update();

    return true;
}

bool tpTreeWidget::onMouseMoveEvent(tpMouseEvent *event)
{
    return false;
}

bool tpTreeWidget::onFocusEvent(tpObjectFocusEvent *event)
{
    return false;
}

bool tpTreeWidget::onLeaveEvent(tpObjectLeaveEvent *event)
{
    return false;
}

bool tpTreeWidget::onPaintEvent(tpObjectPaintEvent *event)
{
    uint8_t alpha = 100;

    PiTreeWidgetPrivData *privData = (PiTreeWidgetPrivData *)this->privData;
    if (!privData)
        return false;

    // 默认边框线颜色
    int32_t borderColor = _RGB(0, 0, 0) & 0xffffff00 | (alpha & 0xff);

    tpCanvas *paint = event->canvas();

    if (!paint)
        return false;

    ItpRect editBound = this->rect();

    paint->line(0, 0, editBound.w, 0, borderColor);
    paint->line(0, 0, 0, editBound.h, borderColor);
    paint->line(editBound.w - 1, 0, editBound.w - 1, editBound.h, borderColor);
    paint->line(0, editBound.h - 1, editBound.w, editBound.h - 1, borderColor);

    // 绘制所有item
    // 最大显示数量, i为所有层级Item索引,topIndex为顶层节点索引
    uint32_t indexMax = (privData->showMinIndex + privData->maxShowCount) > privData->curExpandItemCount ? privData->curExpandItemCount : (privData->showMinIndex + privData->maxShowCount);

    uint32_t globalDrawCount = 0;
    uint32_t findIndex = 0;
    privData->curShowItemList.clear();
    for (uint32_t i = 0; i < privData->topItemList.size(); ++i)
    {
        tpTreeWidgetItem *topItem = privData->topItemList.at(i);

        DrawItem(paint, topItem, findIndex, globalDrawCount);

        if ((findIndex >= privData->showMinIndex) && (findIndex - privData->showMinIndex) > indexMax)
            break;
    }

    // 数据超过最大显示条数时，要根据当前进度，绘制进度条
    if (privData->isDrawScrollBar)
    {
        // 以最小索引为基准，最小索引的最小值为0，最大值为数据总数量-最大显示数量
        uint32_t showMinIndexMax = privData->curExpandItemCount - privData->maxShowCount;
        if (showMinIndexMax > 0)
        {
            // 先计算进度条应该绘制多长，根据 最大显示条数 / 数据总条数，得出百分比
            double scrollBarHeightPersent = 1.0 * privData->maxShowCount / privData->curExpandItemCount;
            if (scrollBarHeightPersent > 1)
                scrollBarHeightPersent = 1;

            // 使用百分比 * 窗口显示高度，得出进度条长度
            uint32_t scrollBarHeight = scrollBarHeightPersent * editBound.h;

            // 根据当前的最小索引值，决定进度条绘制在什么位置
            uint32_t scrollBarY = (1.0 * privData->showMinIndex / showMinIndexMax) * (editBound.h - scrollBarHeight);

            int32_t scrollBarColor = _RGB(200, 0, 0);

            paint->roundedBox(editBound.w - 2, scrollBarY, editBound.w - 1, scrollBarY + scrollBarHeight, 0.5, scrollBarColor);
        }
    }

    return true;
}

uint32_t tpTreeWidget::getItemIndex(const uint32_t &_x, const uint32_t &_y)
{
    PiTreeWidgetPrivData *privData = (PiTreeWidgetPrivData *)this->privData;
    if (!privData)
        return 0;

    tpFont textFont;

    uint32_t oneRowHeight = 2 * ITEM_V_PIX + textFont.pixelHeight();

    if (_y < oneRowHeight)
    {
        return 0;
    }
    else
    {
        uint32_t tmpIndex = _y / oneRowHeight;
        return tmpIndex;
    }

    return 0;
}

// 递归计算当前显示的所有item的条数（包括父子节点）
void CaculateAllItemHeight(uint32_t &resShowCount, tpTreeWidgetItem *topItem)
{
    // 先累加当前节点
    resShowCount += 1;
    // 如果该节点展开，则累加下级节点长度
    if (topItem->isExpanded())
    {
        for (int32_t i = 0; i < topItem->childCount(); ++i)
        {
            CaculateAllItemHeight(resShowCount, topItem->child(i));
        }
    }
}

void tpTreeWidget::RefreshShowIndex()
{
    PiTreeWidgetPrivData *privData = (PiTreeWidgetPrivData *)this->privData;
    if (!privData)
        return;

    ItpRect editBound = this->rect();
    privData->maxShowCount = std::ceil(editBound.h / (privData->defaultSize.h + 2 * ITEM_V_PIX));

    // 顶层节点是必须显示的
    uint32_t resShowCount = 0;

    for (const auto &topItem : privData->topItemList)
    {
        CaculateAllItemHeight(resShowCount, topItem);
    }

    privData->curExpandItemCount = resShowCount;

    if (resShowCount < privData->maxShowCount)
    {
        privData->isDrawScrollBar = false;
    }
    else
    {
        privData->isDrawScrollBar = true;
    }
}

uint32_t tpTreeWidget::quetyChildCount(tpTreeWidgetItem *item)
{
    return 0;
}

void tpTreeWidget::DrawItem(tpCanvas *paint, tpTreeWidgetItem *item, uint32_t &findIndex, uint32_t &globalCount)
{
    PiTreeWidgetPrivData *privData = (PiTreeWidgetPrivData *)this->privData;
    if (!privData)
        return;

    uint32_t indexMax = (privData->showMinIndex + privData->maxShowCount) > privData->curExpandItemCount ? privData->curExpandItemCount : (privData->showMinIndex + privData->maxShowCount);
    if ((findIndex >= privData->showMinIndex) && (findIndex - privData->showMinIndex) > indexMax)
        return;

    ItpRect editBound = this->rect();

    auto DrawItemFunc = [&](tpTreeWidgetItem *drawItem)
    {
        tpString itemText = drawItem->text(0);

        tpFont *testItemFont = new tpFont();
        testItemFont->setText(itemText.c_str());

        uint32_t itemOffset = 0;

        CaculateItemLevel(drawItem, itemOffset);
        itemOffset *= ITEM_OFFSET;

        // 有子节点，要绘制展开标
        if (drawItem->childCount() > 0)
        {
            tpString expandCharacter = ">";
            if (drawItem->isExpanded())
                expandCharacter = tpString("v");

            tpFont *characterFont = new tpFont();
            characterFont->setText(expandCharacter.c_str());
            paint->renderText(*characterFont, TOP_ITEM_EX_X + itemOffset, ITEM_V_PIX + globalCount * (2 * ITEM_V_PIX + testItemFont->pixelHeight()), expandCharacter);
        }

        // 记录当前显示的item
        privData->curShowItemList.emplace_back(drawItem);

        paint->renderText(*testItemFont, TOP_ITEM_TEXT_X + itemOffset, ITEM_V_PIX + globalCount * (2 * ITEM_V_PIX + testItemFont->pixelHeight()), itemText);

        // 鼠标选中，绘制背景
        if (drawItem->isSelected())
        {
            // 鼠标选中项绘制选中背景
            uint32_t startY = globalCount * (2 * ITEM_V_PIX + testItemFont->pixelHeight());
            uint32_t endX = editBound.w - 1;
            uint32_t endY = globalCount * (2 * ITEM_V_PIX + testItemFont->pixelHeight()) + (2 * ITEM_V_PIX + testItemFont->pixelHeight());

            paint->rectangle(0, startY, endX, endY, _RGB(255, 10, 10));
        }

        ++globalCount;
        delete testItemFont;
        testItemFont = nullptr;
    };

    // 如果当前查询index比要显示的索引小，要递归找子节点是不是有符合条件
    if (findIndex < privData->showMinIndex)
    {
        ++findIndex;
        // 查询子节点是否要绘制绘制子节点
        if (item->isExpanded())
        {
            for (int32_t childIndex = 0; childIndex < item->childCount(); ++childIndex)
            {
                DrawItem(paint, item->child(childIndex), findIndex, globalCount);
            }
        }
    }
    else
    {
        if ((findIndex - privData->showMinIndex) <= privData->maxShowCount)
        {
            // 先绘制当前item
            DrawItemFunc(item);
            ++findIndex;

            // 如果有子节点，绘制子节点
            if (item->isExpanded())
            {
                for (int32_t i = 0; i < item->childCount(); ++i)
                {
                    DrawItem(paint, item->child(i), findIndex, globalCount);
                }
            }
        }
        else
        {
            return;
        }
    }
}
