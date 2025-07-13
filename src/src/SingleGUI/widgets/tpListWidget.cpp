/***
 * @Author: hywang
 * @Date: 2024-05-31 09:39:18
 * @LastEditors: hywang
 * @LastEditTime: 2024-06-03 15:08:47
 * @FilePath: /pix-singlegui/PixSingleGUI/src/SingleGUI/widgets/tpListWidget.cpp
 * @Description: listWidget的实现类
 * @
 * @PiXOS
 */
#include "tpListWidget.h"
#include "tpList.h"
#include "tpVector.h"
#include "tpFont.h"
#include "tpCanvas.h"

#include <cmath>
#include <iterator>

#ifndef ITEM_V_PIX
#define ITEM_V_PIX 3
#endif

struct PiListWidgetPrivData
{
    // 默认item大小
    ItpSize defaultSize;

    // 显示区域，从哪个item的下标开始显示
    uint32_t showMinIndex;

    bool isHover;
    // 当前选中的下标
    tpVector<uint32_t> selectIndexList;

    uint32_t maxCount;

    // 是否绘制进度条
    bool isDrawScrollBar;

    // 是否按住ctrl
    bool isPressCtrl;
    bool isPressShift;

    // 选择模式
    tinyPiX::ItemSelectionMode selectMode;

    tpList<tpListWidgetItem *> allItemList;

    PiListWidgetPrivData()
        : showMinIndex(0), isDrawScrollBar(false), isPressCtrl(false), selectMode(tinyPiX::SingleSelection), isPressShift(false)
    {
        tpFont defaultFont;
        defaultFont.setText("Test Text");

        defaultSize.w = -1;
        defaultSize.h = defaultFont.pixelHeight();
    }

    virtual ~PiListWidgetPrivData()
    {
        for (auto &itemPtr : allItemList)
        {
            if (itemPtr)
            {
                delete itemPtr;
                itemPtr = nullptr;
            }
        }
        allItemList.clear();
    }
};

int32_t partitionFunc(tinyPiX::SortOrder order, tpList<tpListWidgetItem *> &arr, int32_t low, int32_t high)
{
    tpListWidgetItem *pivot = arr.at(high);
    int32_t i = (low - 1);
    for (int32_t j = low; j <= high - 1; j++)
    {
        tpListWidgetItem *firstItem = arr.at(j);
        tpListWidgetItem *secondItem = pivot;

        if (order == tinyPiX::DescendingOrder)
        {
            firstItem = pivot;
            secondItem = arr.at(j);
        }

        if (firstItem->text() < secondItem->text())
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

void quickSortFunc(tinyPiX::SortOrder order, tpList<tpListWidgetItem *> &arr, int32_t low, int32_t high)
{
    if (low < high)
    {
        int32_t pi = partitionFunc(order, arr, low, high);

        quickSortFunc(order, arr, low, pi - 1);
        quickSortFunc(order, arr, pi + 1, high);
    }
}

tpListWidget::tpListWidget(tpChildWidget *parent)
    : tpChildWidget(parent)
{
    this->privData = new PiListWidgetPrivData();
}

tpListWidget::~tpListWidget()
{
    PiListWidgetPrivData *privData = (PiListWidgetPrivData *)this->privData;
    if (!privData)
        return;

    if (privData)
    {
        delete privData;
        privData = nullptr;
    }
}

tpListWidgetItem *tpListWidget::item(int32_t row) const
{
    PiListWidgetPrivData *privData = (PiListWidgetPrivData *)this->privData;
    if (!privData)
        return nullptr;

    if (row >= privData->allItemList.size())
        return nullptr;

    return privData->allItemList.at(row);
}

int32_t tpListWidget::row(const tpListWidgetItem *item) const
{
    if (item == nullptr)
        return -1;

    PiListWidgetPrivData *privData = (PiListWidgetPrivData *)this->privData;
    if (!privData)
        return -1;

    for (int32_t i = 0; i < privData->allItemList.size(); ++i)
    {
        tpListWidgetItem *tempItem = privData->allItemList.at(i);
        if (tempItem == item)
            return i;
    }

    return -1;
}

void tpListWidget::insertItem(int32_t row, tpListWidgetItem *item)
{
    PiListWidgetPrivData *privData = (PiListWidgetPrivData *)this->privData;
    if (!privData)
        return;

    if (row >= privData->allItemList.size())
        return;

    privData->allItemList.insertData(row, item);

    RefreshShowIndex();
}

void tpListWidget::insertItem(int32_t row, const tpString &label)
{
    PiListWidgetPrivData *privData = (PiListWidgetPrivData *)this->privData;
    if (!privData)
        return;

    if (row >= privData->allItemList.size())
        return;

    tpListWidgetItem *newItem = new tpListWidgetItem(label);
    privData->allItemList.insertData(row, newItem);

    RefreshShowIndex();
}

void tpListWidget::insertItems(int32_t row, const tpList<tpString> &labels)
{
    PiListWidgetPrivData *privData = (PiListWidgetPrivData *)this->privData;
    if (!privData)
        return;

    if (row >= privData->allItemList.size())
        return;

    for (const auto &text : labels)
    {
        tpListWidgetItem *newItem = new tpListWidgetItem(text);
        privData->allItemList.insertData(row, newItem);
    }

    RefreshShowIndex();
}

void tpListWidget::addItem(tpListWidgetItem *item)
{
    PiListWidgetPrivData *privData = (PiListWidgetPrivData *)this->privData;
    if (!privData)
        return;

    privData->allItemList.append(item);

    RefreshShowIndex();
}

tpListWidgetItem *tpListWidget::takeItem(int32_t row)
{
    PiListWidgetPrivData *privData = (PiListWidgetPrivData *)this->privData;
    if (!privData)
        return nullptr;

    if (row >= privData->allItemList.size())
        return nullptr;

    tpListWidgetItem *resItem = privData->allItemList.at(row);
    privData->allItemList.remove(row);

    return resItem;
}

int32_t tpListWidget::count() const
{
    PiListWidgetPrivData *privData = (PiListWidgetPrivData *)this->privData;
    if (!privData)
        return 0;

    return privData->allItemList.size();
}

tpListWidgetItem *tpListWidget::currentItem() const
{
    PiListWidgetPrivData *privData = (PiListWidgetPrivData *)this->privData;
    if (!privData)
        return nullptr;

    if (privData->selectIndexList.size() == 0)
        return nullptr;

    return privData->allItemList.at(privData->selectIndexList.back());
}

void tpListWidget::setCurrentItem(tpListWidgetItem *item)
{
    PiListWidgetPrivData *privData = (PiListWidgetPrivData *)this->privData;
    if (!privData)
        return;

    for (int32_t i = 0; i < privData->allItemList.size(); ++i)
    {
        tpListWidgetItem *tempItem = privData->allItemList.at(i);
        if (tempItem == item)
        {
            privData->selectIndexList.clear();
            privData->selectIndexList.emplace_back(i);
            break;
        }
    }
}

int32_t tpListWidget::currentRow() const
{
    PiListWidgetPrivData *privData = (PiListWidgetPrivData *)this->privData;
    if (!privData)
        return -1;

    return privData->selectIndexList.back();
}

void tpListWidget::setCurrentRow(int32_t row)
{
    PiListWidgetPrivData *privData = (PiListWidgetPrivData *)this->privData;
    if (!privData)
        return;

    privData->selectIndexList.clear();
    privData->selectIndexList.emplace_back(row);
    update();
}

tpListWidgetItem *tpListWidget::itemAt(const ItpPoint &p)
{
    PiListWidgetPrivData *privData = (PiListWidgetPrivData *)this->privData;
    if (!privData)
        return nullptr;

    uint32_t pointIndex = getItemIndex(p.x, p.y);
    return privData->allItemList.at(pointIndex);
}

tpListWidgetItem *tpListWidget::itemAt(int32_t x, int32_t y)
{
    PiListWidgetPrivData *privData = (PiListWidgetPrivData *)this->privData;
    if (!privData)
        return nullptr;

    uint32_t pointIndex = getItemIndex(x, y);
    return privData->allItemList.at(pointIndex);
}

void tpListWidget::sortItems(tinyPiX::SortOrder order)
{
    PiListWidgetPrivData *privData = (PiListWidgetPrivData *)this->privData;
    if (!privData)
        return;

    quickSortFunc(order, privData->allItemList, 0, privData->allItemList.size() - 1);

    update();
}

void tpListWidget::setSortingEnabled(bool enable)
{
}

bool tpListWidget::isSortingEnabled() const
{
    return false;
}

tpChildWidget *tpListWidget::itemWidget(tpListWidgetItem *item) const
{
    return nullptr;
}

void tpListWidget::setItemWidget(tpListWidgetItem *item, tpChildWidget *widget)
{
}

inline void tpListWidget::removeItemWidget(tpListWidgetItem *item)
{
}

bool tpListWidget::isItemSelected(const tpListWidgetItem *item) const
{
    PiListWidgetPrivData *privData = (PiListWidgetPrivData *)this->privData;
    if (!privData)
        return false;

    for (int32_t i = 0; i < privData->allItemList.size(); ++i)
    {
        if (privData->allItemList.at(i) != item)
            continue;

        if (privData->selectIndexList.contains(i))
            return true;

        return false;
    }

    return false;
}

void tpListWidget::setItemSelected(const tpListWidgetItem *item, bool select)
{
    PiListWidgetPrivData *privData = (PiListWidgetPrivData *)this->privData;
    if (!privData)
        return;

    for (int32_t i = 0; i < privData->allItemList.size(); ++i)
    {
        if (privData->allItemList.at(i) != item)
            continue;

        if (select)
        {
            if (!privData->selectIndexList.contains(i))
                privData->selectIndexList.emplace_back(i);
        }
        else
        {
            if (privData->selectIndexList.contains(i))
                privData->selectIndexList.remove(i);
        }

        update();
    }
}

tpList<tpListWidgetItem *> tpListWidget::selectedItems() const
{
    PiListWidgetPrivData *privData = (PiListWidgetPrivData *)this->privData;
    if (!privData)
        return tpList<tpListWidgetItem *>();

    tpList<tpListWidgetItem *> selectItemList;
    for (const auto &selectIndex : privData->selectIndexList)
    {
        selectItemList.emplace_back(privData->allItemList.at(selectIndex));
    }

    return selectItemList;
}

tpList<tpListWidgetItem *> tpListWidget::findItems(const tpString &text, tinyPiX::MatchFlags flags) const
{
    PiListWidgetPrivData *privData = (PiListWidgetPrivData *)this->privData;
    if (!privData)
        return tpList<tpListWidgetItem *>();

    tpList<tpListWidgetItem *> findItemList;
    for (const auto &itemPtr : privData->allItemList)
    {
        if (itemPtr->text().compare(text) == 0)
            findItemList.emplace_back(itemPtr);
    }

    return findItemList;
}

// bool tpListWidget::isItemHidden(const tpListWidgetItem *item) const
// {
//     return false;
// }

// void tpListWidget::setItemHidden(const tpListWidgetItem *item, bool hide)
// {
// }

void tpListWidget::clear()
{
    PiListWidgetPrivData *privData = (PiListWidgetPrivData *)this->privData;
    if (!privData)
        return;

    for (auto &itemPtr : privData->allItemList)
    {
        if (itemPtr)
        {
            delete itemPtr;
            itemPtr = nullptr;
        }
    }

    privData->allItemList.clear();
    privData->selectIndexList.clear();
    privData->showMinIndex = 0;
}

inline ItpSize tpListWidget::sizeHint() const
{
    return ItpSize();
}

inline void tpListWidget::setSizeHint(const ItpSize &size)
{
}

void tpListWidget::setSelectionModel(tinyPiX::ItemSelectionMode selectMode)
{
    PiListWidgetPrivData *privData = (PiListWidgetPrivData *)this->privData;
    if (!privData)
        return;

    privData->selectMode = selectMode;
}

bool tpListWidget::onMoveEvent(tpObjectMoveEvent *event)
{
    return false;
}

bool tpListWidget::onResizeEvent(tpObjectResizeEvent *event)
{
    PiListWidgetPrivData *privData = (PiListWidgetPrivData *)this->privData;
    if (!privData)
        return false;

    ItpRect editBound = this->rect();

    // std::cout << editBound.h << "--" << privData->defaultSize.h << std::endl;
    privData->maxCount = std::ceil(editBound.h / (privData->defaultSize.h + 2 * ITEM_V_PIX));

    return true;
}

bool tpListWidget::onKeyPressEvent(tpKeyboardEvent *event)
{
    PiListWidgetPrivData *privData = (PiListWidgetPrivData *)this->privData;
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

bool tpListWidget::onKeyReleaseEvent(tpKeyboardEvent *event)
{
    PiListWidgetPrivData *privData = (PiListWidgetPrivData *)this->privData;
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

bool tpListWidget::onMousePressEvent(tpMouseEvent *event)
{
    PiListWidgetPrivData *privData = (PiListWidgetPrivData *)this->privData;
    if (!privData)
        return true;

    if (event->button() == BUTTON_LEFT)
    {
        // 鼠标左键
        // 根据点击下标，获取当前item
        uint32_t curIndex = getItemIndex(event->pos().x, event->pos().y);

        if (curIndex >= privData->allItemList.size())
            return true;

        tpListWidgetItem *curItem = privData->allItemList.at(curIndex);

        // 按下
        itemPressed.emit(curItem);
    }
    else
    {
        // 右键或滚轮键，清空所有选择项，选中当前项
        uint32_t curIndex = getItemIndex(event->pos().x, event->pos().y);
        if (curIndex < privData->allItemList.size())
        {
            privData->selectIndexList.clear();
            privData->selectIndexList.emplace_back(curIndex);
        }

        update();
    }

    return true;
}

bool tpListWidget::onMouseRleaseEvent(tpMouseEvent *event)
{
    PiListWidgetPrivData *privData = (PiListWidgetPrivData *)this->privData;
    if (!privData)
        return true;

    if (event->button() == BUTTON_LEFT)
    {
        uint32_t curIndex = getItemIndex(event->pos().x, event->pos().y);
        if (curIndex >= privData->allItemList.size())
            return true;

        tpListWidgetItem *curItem = privData->allItemList.at(curIndex);

        // 释放
        static uint32_t lastIndex = -1;
        if (curIndex != lastIndex)
        {
            // 发送选中item改变信号
            tpListWidgetItem *preItem = nullptr;
            if (lastIndex != -1)
                preItem = privData->allItemList.at(lastIndex);

            currentItemChanged.emit(curItem, preItem);

            if (preItem && (preItem->text().compare(curItem->text()) != 0))
                currentTextChanged.emit(curItem->text());

            currentRowChanged.emit(curIndex);

            // 根据选择模式不同，操作不同
            if (privData->selectMode == tinyPiX::SingleSelection)
            {
                privData->selectIndexList.clear();
                privData->selectIndexList.emplace_back(curIndex);
            }
            else if (privData->selectMode == tinyPiX::MultiSelection)
            {
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
                                    privData->selectIndexList.emplace_back(i);
                            }
                        }
                        else
                        {
                            if (!privData->selectIndexList.contains(curIndex))
                                privData->selectIndexList.emplace_back(curIndex);
                        }
                    }
                    else
                    {
                        if (!privData->selectIndexList.contains(curIndex))
                            privData->selectIndexList.emplace_back(curIndex);
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

        itemClicked.emit(curItem);
    }

    return true;
}

bool tpListWidget::onMouseMoveEvent(tpMouseEvent *event)
{
    PiListWidgetPrivData *privData = (PiListWidgetPrivData *)this->privData;
    if (!privData)
        return false;

    return true;
}

bool tpListWidget::onWheelEvent(tpWheelEvent *event)
{
    PiListWidgetPrivData *privData = (PiListWidgetPrivData *)this->privData;
    if (!privData)
        return true;

    if (event->angleDelta() > 0)
    {
        if (privData->allItemList.size() <= privData->maxCount)
            return true;

        if (privData->showMinIndex == 0)
            return true;

        privData->showMinIndex--;
        // std::cout << "-- " << privData->showMinIndex << std::endl;

        update();
    }
    else
    {
        if (privData->allItemList.size() <= privData->maxCount)
            return true;

        if ((privData->showMinIndex + privData->maxCount) >= privData->allItemList.size())
            return true;

        privData->showMinIndex++;
        // std::cout << "++ " << privData->showMinIndex << std::endl;

        update();
    }

    return true;
}

bool tpListWidget::onFocusEvent(tpObjectFocusEvent *event)
{
    return false;
}

bool tpListWidget::onLeaveEvent(tpObjectLeaveEvent *event)
{
    PiListWidgetPrivData *privData = (PiListWidgetPrivData *)this->privData;

    if (!privData)
        return false;

    if (event->eventType() == tpEvent::EVENT_OBJECT_LEAVE_TYPE)
    {
        privData->isHover = event->leave();
        // update();
    }

    return true;
}

bool tpListWidget::onPaintEvent(tpObjectPaintEvent *event)
{
    uint8_t alpha = 100;

    PiListWidgetPrivData *privData = (PiListWidgetPrivData *)this->privData;
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
    //  最大显示数量
    // for (int32_t i = privData->showMinIndex; i <= privData->showMaxIndex; ++i)
    uint32_t indexMax = (privData->showMinIndex + privData->maxCount) > privData->allItemList.size() ? privData->allItemList.size() : (privData->showMinIndex + privData->maxCount);
    for (int32_t i = privData->showMinIndex; i < indexMax; ++i)
    {
        uint32_t relativeIndex = i - privData->showMinIndex;

        tpListWidgetItem *tempItem = privData->allItemList.at(i);
        if (!tempItem)
            continue;

        tpString itemText = tempItem->text();

        // tpFont itemFont = tempItem->font();
        tpFont *testItemFont = new tpFont();
        testItemFont->setText(itemText.c_str());

        // itemFont->setText(itemText.c_str());

        // std::cout << "itemText ：" << itemText << std::endl;
        paint->renderText(*testItemFont, 5, ITEM_V_PIX + relativeIndex * (2 * ITEM_V_PIX + testItemFont->pixelHeight()), itemText);

        tempItem->setSelected(false);

        // 鼠标在当前选项悬停，绘制背景
        if (privData->selectIndexList.contains(i))
        {
            tempItem->setSelected(true);

            // 鼠标悬停项绘制选中背景
            uint32_t startY = relativeIndex * (2 * ITEM_V_PIX + testItemFont->pixelHeight());
            uint32_t endX = editBound.w - 1;
            uint32_t endY = relativeIndex * (2 * ITEM_V_PIX + testItemFont->pixelHeight()) + (2 * ITEM_V_PIX + testItemFont->pixelHeight());

            paint->rectangle(0, startY, endX, endY, _RGB(255, 10, 10));
        }

        delete testItemFont;
        testItemFont = nullptr;
    }

    // 数据超过最大显示条数时，要根据当前进度，绘制进度条
    if (privData->isDrawScrollBar)
    {
        // 以最小索引为基准，最小索引的最小值为0，最大值为数据总数量-最大显示数量
        uint32_t showMinIndexMax = privData->allItemList.size() - privData->maxCount;
        if (showMinIndexMax > 0)
        {
            // 先计算进度条应该绘制多长，根据 最大显示条数 / 数据总条数，得出百分比
            double scrollBarHeightPersent = 1.0 * privData->maxCount / privData->allItemList.size();
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

uint32_t tpListWidget::getItemIndex(const uint32_t &_x, const uint32_t &_y)
{
    PiListWidgetPrivData *privData = (PiListWidgetPrivData *)this->privData;
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
        return tmpIndex + privData->showMinIndex;
    }

    return 0;
}

void tpListWidget::RefreshShowIndex()
{
    PiListWidgetPrivData *privData = (PiListWidgetPrivData *)this->privData;
    if (!privData)
        return;

    ItpRect editBound = this->rect();
    privData->maxCount = std::ceil(editBound.h / (privData->defaultSize.h + 2 * ITEM_V_PIX));

    if (privData->allItemList.size() <= privData->maxCount)
    {
        // privData->showMaxIndex = privData->allItemList.size() - 1;
        privData->isDrawScrollBar = false;
    }
    else
    {
        // showMaxIndex_ = maxCount_ - 1;
        privData->isDrawScrollBar = true;
    }
}
