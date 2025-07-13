
#include "GUIFramework/desktopGUI/widgets/tpFileListWidget.h"
#include "tpCanvas.h"
#include "tpMap.h"
#include "tpFile.h"
#include "tpSurface.h"

#include <cmath>
#include <iterator>
#include "tpFileListWidget.h"

DESKTOP_GUI_NAMESPACE_BEGIN

// item之间间距
static const uint32_t RowMargin = 8;

// item内部距离左右边界距离
static const uint32_t ItemContenMargin = 20;

// 距左右边界距离
static const uint32_t LeftRightMargin = 15;

struct ListItemData
{
    tpList<tpString> dataStrList;
    tpVariant data;

    // <列号，icon>
    tpMap<uint32_t, tpShared<tpSurface>> iconMap;
};

// 当前绘制的item信息
struct ItemRectInfo
{
    ItpRect itemRect;
    ListItemData itemData;
};

struct FileListWidgetData
{
    int32_t headerBgColor;

    int32_t itemBgColor;
    int32_t itemHoverColor;
    int32_t itemPressColor;

    tpFont *titleFont;
    tpFont *itemFont;

    // 标题栏高度和item高度
    uint32_t titleHeight = 36;
    uint32_t itemHeight = 36;

    // 显示区域，从哪个item的下标开始显示
    uint32_t showMinIndex;
    // 窗口最大显示条数
    uint32_t maxShowCount;

    // 是否绘制进度条
    bool isDrawScrollBar;

    // 是否按住ctrl
    bool isPressCtrl;
    bool isPressShift;

    // 选择模式
    tinyPiX::ItemSelectionMode selectMode;

    // 标题栏文本
    tpList<tpString> headerList;

    // 当前显示的item列表
    tpList<ItemRectInfo> curShowItemList;

    // 所有item数据
    tpList<ListItemData> allItemList;

    // 上一个选中的item
    tpVariant lastSelectItem;
    // 所有被选中的item
    tpList<tpVariant> selectItemList;

    // 当前鼠标悬停item
    tpVariant hoverItem;

    // 列占比
    tpList<uint32_t> columnStrech;

    FileListWidgetData()
        : headerBgColor(_RGB(255, 255, 255)), itemBgColor(_RGB(255, 255, 255)), itemHoverColor(_RGB(229, 243, 255)), itemPressColor(_RGB(204, 232, 255)), showMinIndex(0), isDrawScrollBar(false), isPressCtrl(false), selectMode(tinyPiX::ExtendedSelection), isPressShift(false), maxShowCount(0)
    {
        titleFont = new tpFont();
        titleFont->setFontStyle(TINY_FONT_BOLD);
        titleFont->setFontSize(12);
        titleFont->setFontForeColor(_RGB(0, 0, 0));

        itemFont = new tpFont();
        itemFont->setFontSize(11);
        itemFont->setFontForeColor(_RGB(0, 0, 0));
    }

    virtual ~FileListWidgetData()
    {
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

tpFileListWidget::tpFileListWidget(tpChildWidget *parent)
    : tpChildWidget(parent)
{
    this->privData = new FileListWidgetData();
}

tpFileListWidget::~tpFileListWidget()
{
    FileListWidgetData *privData = (FileListWidgetData *)this->privData;
    if (!privData)
        return;

    if (privData)
    {
        delete privData;
        privData = nullptr;
    }
}

void tpFileListWidget::setHeaderList(const tpList<tpString> &headerList)
{
    FileListWidgetData *widgetData = static_cast<FileListWidgetData *>(privData);
    if (!widgetData)
        return;

    widgetData->headerList = headerList;

    // 默认比例 1：1
    widgetData->columnStrech.clear();
    std::fill_n(std::back_inserter(widgetData->columnStrech), widgetData->headerList.size(), 1);

    update();
}

void tpFileListWidget::setHeaderBackColor(const int32_t &color)
{
    FileListWidgetData *widgetData = static_cast<FileListWidgetData *>(privData);
    if (!widgetData)
        return;

    widgetData->headerBgColor = color;

    update();
}

void tpFileListWidget::setColumnStrech(const tpList<uint32_t> &strechList)
{
    FileListWidgetData *widgetData = static_cast<FileListWidgetData *>(privData);
    if (!widgetData)
        return;

    widgetData->columnStrech = strechList;
}

void tpFileListWidget::addRow(const tpList<tpString> &rowData, const tpVariant &data)
{
    FileListWidgetData *widgetData = static_cast<FileListWidgetData *>(privData);
    if (!widgetData)
        return;

    ListItemData newItemData;
    newItemData.data = data;
    newItemData.dataStrList = rowData;

    widgetData->allItemList.emplace_back(newItemData);
}

void tpFileListWidget::setColumnIcon(const tpVariant &data, const uint32_t &column, const tpString &iconPath)
{
    FileListWidgetData *widgetData = static_cast<FileListWidgetData *>(privData);
    if (!widgetData)
        return;

    if (!tpFile::exists(iconPath))
        return;

    for (auto &itemInfo : widgetData->allItemList)
    {
        if (itemInfo.data == data)
        // if (static_cast<const tpVariant &>(itemInfo.data) == static_cast<const tpVariant &>(data))
        {
            if (itemInfo.iconMap.contains(column))
            {
                itemInfo.iconMap[column]->fromFile(iconPath);
            }
            else
            {
                tpShared<tpSurface> newSurface = tpMakeShared<tpSurface>();
                newSurface->fromFile(iconPath);
                itemInfo.iconMap[column] = newSurface;
            }
        }
    }
}

void tpFileListWidget::setRowBackColor(const int32_t &color)
{
    FileListWidgetData *widgetData = static_cast<FileListWidgetData *>(privData);
    if (!widgetData)
        return;

    widgetData->itemBgColor = color;
}

void tpFileListWidget::setRowHoverColor(const int32_t &color)
{
    FileListWidgetData *widgetData = static_cast<FileListWidgetData *>(privData);
    if (!widgetData)
        return;

    widgetData->itemHoverColor = color;
}

void tpFileListWidget::setRowPressColor(const int32_t &color)
{
    FileListWidgetData *widgetData = static_cast<FileListWidgetData *>(privData);
    if (!widgetData)
        return;

    widgetData->itemPressColor = color;
}

tpFont *tpFileListWidget::titleFont()
{
    FileListWidgetData *widgetData = static_cast<FileListWidgetData *>(privData);
    if (!widgetData)
        return nullptr;

    return widgetData->titleFont;
}

tpFont *tpFileListWidget::itemFont()
{
    FileListWidgetData *widgetData = static_cast<FileListWidgetData *>(privData);
    if (!widgetData)
        return nullptr;

    return widgetData->itemFont;
}

tpList<tpVariant> tpFileListWidget::selectedList()
{
    FileListWidgetData *widgetData = static_cast<FileListWidgetData *>(privData);
    if (!widgetData)
        return tpList<tpVariant>();

    return widgetData->selectItemList;
}

// void tpFileListWidget::setSelectionModel(tinyPiX::ItemSelectionMode selectMode)
// {
//     FileListWidgetData *widgetData = static_cast<FileListWidgetData *>(privData);
//     if (!widgetData)
//         return;

//     widgetData->selectMode = selectMode;
// }

void tpFileListWidget::setTitleHeight(const uint32_t &height)
{
    FileListWidgetData *widgetData = static_cast<FileListWidgetData *>(privData);
    if (!widgetData)
        return;

    widgetData->titleHeight = height;
}

void tpFileListWidget::setItemHeight(const uint32_t &height)
{
    FileListWidgetData *widgetData = static_cast<FileListWidgetData *>(privData);
    if (!widgetData)
        return;

    widgetData->itemHeight = height;
}

void tpFileListWidget::clear()
{
    FileListWidgetData *widgetData = static_cast<FileListWidgetData *>(privData);
    if (!widgetData)
        return;

    widgetData->curShowItemList.clear();
    widgetData->selectItemList.clear();
    widgetData->allItemList.clear();
    widgetData->hoverItem = tpVariant();
    widgetData->lastSelectItem = tpVariant();
    widgetData->showMinIndex = 0;
}

bool tpFileListWidget::onResizeEvent(tpObjectResizeEvent *event)
{
    FileListWidgetData *widgetData = static_cast<FileListWidgetData *>(privData);
    if (!widgetData)
        return true;

    // 计算窗口内一次最多能显示多少条数据
    widgetData->maxShowCount = (rect().h - widgetData->titleHeight) / (widgetData->itemHeight + RowMargin);

    return true;
}

bool tpFileListWidget::onKeyPressEvent(tpKeyboardEvent *event)
{
    FileListWidgetData *widgetData = static_cast<FileListWidgetData *>(privData);
    if (!widgetData)
        return true;

    if (event->scancode() == TP_SCANCODE_LCTRL || event->scancode() == TP_SCANCODE_RCTRL)
    {
        widgetData->isPressCtrl = true;
    }

    if (event->scancode() == TP_SCANCODE_LSHIFT || event->scancode() == TP_SCANCODE_RSHIFT)
    {
        widgetData->isPressShift = true;
    }

    return true;
}

bool tpFileListWidget::onKeyReleaseEvent(tpKeyboardEvent *event)
{
    FileListWidgetData *widgetData = static_cast<FileListWidgetData *>(privData);
    if (!widgetData)
        return true;

    if (event->scancode() == TP_SCANCODE_LCTRL || event->scancode() == TP_SCANCODE_RCTRL)
    {
        widgetData->isPressCtrl = false;
    }

    if (event->scancode() == TP_SCANCODE_LSHIFT || event->scancode() == TP_SCANCODE_RSHIFT)
    {
        widgetData->isPressShift = false;
    }

    return true;
}

bool tpFileListWidget::onMousePressEvent(tpMouseEvent *event)
{
    FileListWidgetData *widgetData = static_cast<FileListWidgetData *>(privData);
    if (!widgetData)
        return true;

    std::cout << "BUTTON_LEFT" << std::endl;
    tpVariant pressData = queryPointIndex(event->pos());
    if (pressData.isNull())
    {
        // 如果点击空白位置，所有选中清空
        widgetData->selectItemList.clear();
    }
    else
    {
        // 发送点击事件
        itemPressed.emit(pressData);

        // 如果按住ctrl键，追加选中
        if (widgetData->isPressCtrl)
        {
            // 同时按住ctrl和shift，上次选中到当前选中之间的全部选中
            if (widgetData->isPressShift)
            {
                if (widgetData->lastSelectItem.isNull())
                {
                    widgetData->selectItemList.emplace_back(pressData);
                }
                else
                {
                    // 当前item是否再选中区间内
                    bool isInRange = false;
                    for (auto &itemInfo : widgetData->allItemList)
                    {
                        if (itemInfo.data == widgetData->lastSelectItem || itemInfo.data == pressData)
                        {
                            auto findIter = std::find(widgetData->selectItemList.begin(), widgetData->selectItemList.end(), itemInfo.data);

                            if (findIter == widgetData->selectItemList.end())
                                widgetData->selectItemList.emplace_back(itemInfo.data);

                            isInRange = !isInRange;
                            if (isInRange == false)
                                break;
                            else
                                continue;
                        }

                        if (!isInRange)
                            continue;

                        auto findIter = std::find(widgetData->selectItemList.begin(), widgetData->selectItemList.end(), itemInfo.data);

                        if (findIter == widgetData->selectItemList.end())
                            widgetData->selectItemList.emplace_back(itemInfo.data);
                    }
                }
            }
            else
            {
                // 按住ctrl状态下，点击已选中节点，取消选中，否则继续选中当前节点
                auto findIter = std::find(widgetData->selectItemList.begin(), widgetData->selectItemList.end(), pressData);

                if (findIter == widgetData->selectItemList.end())
                {
                    widgetData->selectItemList.emplace_back(pressData);
                }
                else
                {
                    widgetData->selectItemList.remove(pressData);
                }
            }
        }
        else
        {
            widgetData->selectItemList.clear();
            widgetData->selectItemList.emplace_back(pressData);
        }
    }

    widgetData->lastSelectItem = pressData;

    update();

    return false;
}

bool tpFileListWidget::onMouseDoubleClickEvent(tpMouseEvent *event)
{
    std::cout << "BUTTON_DOUBLE_CLICK" << std::endl;

    tpVariant pressData = queryPointIndex(event->pos());
    if (!pressData.isNull())
        itemDoubleClicked.emit(pressData);

    return true;
}

bool tpFileListWidget::onMouseRleaseEvent(tpMouseEvent *event)
{
    return true;
}

bool tpFileListWidget::onMouseMoveEvent(tpMouseEvent *event)
{
    FileListWidgetData *widgetData = static_cast<FileListWidgetData *>(privData);
    if (!widgetData)
        return true;

    tpVariant pressData = queryPointIndex(event->pos());
    widgetData->hoverItem = pressData;

    update();

    return true;
}

bool tpFileListWidget::onWheelEvent(tpWheelEvent *event)
{
    FileListWidgetData *widgetData = static_cast<FileListWidgetData *>(privData);
    if (!widgetData)
        return true;

    if (event->angleDelta() > 0)
    {
        if (widgetData->showMinIndex > 0)
        {
            widgetData->showMinIndex--;
            update();
        }
    }
    else
    {
        if ((widgetData->showMinIndex + widgetData->maxShowCount) < widgetData->allItemList.size())
        {
            widgetData->showMinIndex++;

            update();
        }
    }

    return true;
}

bool tpFileListWidget::onLeaveEvent(tpObjectLeaveEvent *event)
{
    return true;
}

bool tpFileListWidget::onPaintEvent(tpObjectPaintEvent *event)
{
    FileListWidgetData *widgetData = static_cast<FileListWidgetData *>(privData);
    if (!widgetData)
        return true;

    tpChildWidget::onPaintEvent(event);

    tpCanvas *paintCanvas = event->canvas();

    // 计算所有列总份数
    uint32_t columnStrechTotal = 0;
    if (widgetData->columnStrech.size() == 0)
    {
        columnStrechTotal = widgetData->headerList.size();
    }
    else
    {
        for (const auto &strech : widgetData->columnStrech)
        {
            columnStrechTotal += strech;
        }
    }
    if (columnStrechTotal == 0)
        return true;

    // 绘制标题栏
    paintCanvas->box(LeftRightMargin, 0, rect().w - LeftRightMargin, widgetData->titleHeight, widgetData->headerBgColor);

    // 绘制标题文本
    if (widgetData->headerList.size() > 0)
    {
        int32_t titleY = (widgetData->titleHeight - widgetData->titleFont->pixelHeight()) / 2.0;
        if (titleY < 0)
            titleY = 0;

        uint32_t forwardWidth = 0;
        for (int32_t i = 0; i < widgetData->headerList.size(); ++i)
        {
            if (widgetData->columnStrech.size() <= i)
                break;

            // 当前列占比宽度
            uint32_t curColumnWidth = 1.0 * widgetData->columnStrech.at(i) / columnStrechTotal * (rect().w - ItemContenMargin * 2);

            auto headText = widgetData->headerList.at(i);
            widgetData->titleFont->setText(headText);

            paintCanvas->renderText(*widgetData->titleFont, LeftRightMargin + ItemContenMargin + forwardWidth, titleY);

            forwardWidth += curColumnWidth;
        }
    }

    // 绘制行数据
    widgetData->curShowItemList.clear();
    for (int32_t i = 0; i < widgetData->maxShowCount; ++i)
    {
        uint32_t actualDataIndex = i + widgetData->showMinIndex;
        if (actualDataIndex >= widgetData->allItemList.size())
            break;

        ListItemData curItemData = widgetData->allItemList.at(actualDataIndex);

        // 绘制数据底纹
        uint32_t curItemStartY = widgetData->titleHeight + RowMargin + i * (widgetData->itemHeight + RowMargin);

        // 如果鼠标悬停绘制悬停色
        if (widgetData->hoverItem == curItemData.data)
        {
            paintCanvas->box(LeftRightMargin, curItemStartY, rect().w - LeftRightMargin, curItemStartY + widgetData->itemHeight, widgetData->itemHoverColor);
        }
        else
        {
            // 鼠标没有悬停的，如果选中绘制选中色，否则绘制默认背景色
            auto selectFind = std::find(widgetData->selectItemList.begin(), widgetData->selectItemList.end(), curItemData.data);
            if (selectFind == widgetData->selectItemList.end())
                paintCanvas->box(LeftRightMargin, curItemStartY, rect().w - LeftRightMargin, curItemStartY + widgetData->itemHeight, widgetData->itemBgColor);
            else
                paintCanvas->box(LeftRightMargin, curItemStartY, rect().w - LeftRightMargin, curItemStartY + widgetData->itemHeight, widgetData->itemPressColor);
        }

        // 记录当前item的rect数据和item数据
        // !!这个地方因为box使用的是两个xy坐标，rect使用的一个坐标+宽高，所以赋值不太一样
        ItemRectInfo itemRectInfo;
        itemRectInfo.itemRect.x = LeftRightMargin;
        itemRectInfo.itemRect.y = curItemStartY;
        itemRectInfo.itemRect.w = rect().w - LeftRightMargin * 2;
        itemRectInfo.itemRect.h = widgetData->itemHeight;

        itemRectInfo.itemData = curItemData;
        widgetData->curShowItemList.emplace_back(itemRectInfo);

        // 依次绘制所有列数据
        uint32_t forwardWidth = 0;

        for (int32_t j = 0; j < curItemData.dataStrList.size(); ++j)
        {
            // 如果数据索引比表头长，多余部分舍弃
            if (j >= widgetData->headerList.size())
                break;

            tpString curColumnText = curItemData.dataStrList.at(j);
            widgetData->itemFont->setText(curColumnText);

            uint32_t curColumnWidth = 1.0 * widgetData->columnStrech.at(j) / columnStrechTotal * (rect().w - ItemContenMargin * 2);

            uint32_t curStartX = LeftRightMargin + ItemContenMargin + forwardWidth;

            // 如果当前列有图标先绘制图标
            if (curItemData.iconMap.contains(j))
            {
                auto drawSurface = curItemData.iconMap[j]->scaled(widgetData->itemHeight * 0.6, widgetData->itemHeight * 0.6);
                uint32_t iconY = curItemStartY + (widgetData->itemHeight - drawSurface->height()) / 2.0;
                paintCanvas->paintSurface(curStartX, iconY, drawSurface);

                curStartX += drawSurface->width() + 10;
            }

            // 绘制当前列文本
            int32_t textY = (widgetData->itemHeight - widgetData->itemFont->pixelHeight()) / 2.0;
            if (textY < 0)
                textY = 0;
            textY += curItemStartY;

            paintCanvas->renderText(*widgetData->itemFont, curStartX, textY);

            // 记录上一列宽度
            forwardWidth += curColumnWidth;
        }
    }

    return true;
}

tpVariant tpFileListWidget::queryPointIndex(const ItpPoint &point)
{
    FileListWidgetData *widgetData = static_cast<FileListWidgetData *>(privData);
    if (!widgetData)
        return true;

    int32_t left = 0;
    int32_t right = widgetData->curShowItemList.size() - 1;

    while (left <= right)
    {
        int32_t mid = left + (right - left) / 2;
        auto itemInfo = widgetData->curShowItemList.at(mid);

        if (itemInfo.itemRect.contains(point))
        {
            return itemInfo.itemData.data;
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

    return tpVariant();
}
DESKTOP_GUI_NAMESPACE_END
