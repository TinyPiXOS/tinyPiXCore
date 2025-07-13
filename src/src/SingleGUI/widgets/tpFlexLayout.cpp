#include "tpFlexLayout.h"
#include "tpList.h"

struct ItemData
{
    tpChildWidget *widget;

    tpLayout *layout;
    // 子布局的容器widget
    tpChildWidget *layoutWidget;

    ItemData()
        : widget(nullptr), layout(nullptr), layoutWidget(nullptr)
    {
    }
};

struct tpFlexLayoutData
{
    tpList<ItemData> widgetList;

    uint32_t rowCount = 0;
    uint32_t columnCount = 0;

    tpFlexLayout::FlexDirection flexDirection = tpFlexLayout::Row;
    tpFlexLayout::JustifyContent justifyContent = tpFlexLayout::MainFlexStart;
    tpFlexLayout::AlignItems alignItems = tpFlexLayout::CrossCenter;
    tpFlexLayout::AlignContent alignContent = tpFlexLayout::Stretch;
};

tpFlexLayout::tpFlexLayout(tpChildWidget *parent)
    : tpLayout(parent)
{
    tpFlexLayoutData *layoutData = new tpFlexLayoutData();

    data_ = layoutData;
}

tpFlexLayout::~tpFlexLayout()
{
    tpFlexLayoutData *layoutData = static_cast<tpFlexLayoutData *>(data_);
    if (layoutData)
    {
        for (auto &itemData : layoutData->widgetList)
        {
            // 如果是子widget，不处理，指针由外部管理
            if (itemData.widget)
            {
                itemData.widget->setParent(nullptr);
                continue;
            }

            if (itemData.layout)
            {
                itemData.layout->setParent(nullptr);
                continue;
            }

            if (itemData.layoutWidget)
            {
                itemData.layoutWidget->setParent(nullptr);
                itemData.layoutWidget->deleteLater();
            }
        }
        layoutData->widgetList.clear();

        delete layoutData;
        layoutData = nullptr;
        data_ = nullptr;
    }
}

void tpFlexLayout::addWidget(tpChildWidget *childWidget)
{
    tpFlexLayoutData *flexData = static_cast<tpFlexLayoutData *>(data_);

    ItemData itemData;
    itemData.widget = childWidget;

    flexData->widgetList.emplace_back(itemData);

    update();
}

void tpFlexLayout::addLayout(tpLayout *layout)
{
    tpFlexLayoutData *flexData = static_cast<tpFlexLayoutData *>(data_);

    ItemData itemData;
    itemData.layout = layout;

    itemData.layoutWidget = new tpChildWidget();
    itemData.layoutWidget->setLayout(itemData.layout);

    flexData->widgetList.emplace_back(itemData);

    update();
}

void tpFlexLayout::insertWidget(uint32_t index, tpChildWidget *widget)
{
    tpFlexLayoutData *flexData = static_cast<tpFlexLayoutData *>(data_);

    ItemData itemData;
    itemData.widget = widget;

    flexData->widgetList.insertData(index, itemData);

    update();
}

void tpFlexLayout::insertLayout(uint32_t index, tpLayout *layout)
{
    tpFlexLayoutData *flexData = static_cast<tpFlexLayoutData *>(data_);

    ItemData itemData;
    itemData.layout = layout;

    itemData.layoutWidget = new tpChildWidget();
    itemData.layoutWidget->setLayout(itemData.layout);

    flexData->widgetList.insertData(index, itemData);

    update();
}

void tpFlexLayout::removeWidget(tpChildWidget *widget)
{
    tpFlexLayoutData *flexData = static_cast<tpFlexLayoutData *>(data_);
    for (int i = 0; i < flexData->widgetList.size(); ++i)
    {
        ItemData curItemData = flexData->widgetList.at(i);
        if (curItemData.widget == widget)
        {
            curItemData.widget->setParent(nullptr);
            flexData->widgetList.remove(i);

            update();
            break;
        }
    }
}

void tpFlexLayout::removeLayout(tpLayout *layout)
{
    tpFlexLayoutData *flexData = static_cast<tpFlexLayoutData *>(data_);
    for (int i = 0; i < flexData->widgetList.size(); ++i)
    {
        ItemData curItemData = flexData->widgetList.at(i);
        if (curItemData.layout == layout)
        {
            tpList<tpObject *> layoutChildObjList = curItemData.layoutWidget->objectList();
            for (const auto &childObj : layoutChildObjList)
            {
				childObj->uninstallEventFilter();
                childObj->setParent(nullptr);
            }

            layout->setParent(nullptr);
			layout->uninstallEventFilter();
            
            curItemData.layoutWidget->deleteLater();
            curItemData.layoutWidget = nullptr;

            flexData->widgetList.remove(i);

            update();

            break;
        }
    }
}

void tpFlexLayout::setFlexDirection(const FlexDirection &direction)
{
    tpFlexLayoutData *flexData = static_cast<tpFlexLayoutData *>(data_);
    flexData->flexDirection = direction;
    update();
}

void tpFlexLayout::setJustifyContent(const JustifyContent &justify)
{
    tpFlexLayoutData *flexData = static_cast<tpFlexLayoutData *>(data_);
    flexData->justifyContent = justify;
    update();
}

void tpFlexLayout::setAlignItems(const AlignItems &alignItems)
{
    tpFlexLayoutData *flexData = static_cast<tpFlexLayoutData *>(data_);
    flexData->alignItems = alignItems;
    update();
}

void tpFlexLayout::setAlignContent(const AlignContent &alignContent)
{
    tpFlexLayoutData *flexData = static_cast<tpFlexLayoutData *>(data_);
    flexData->alignContent = alignContent;
    update();
}

void tpFlexLayout::update()
{
    tpFlexLayoutData *flexData = static_cast<tpFlexLayoutData *>(data_);
    if (!flexData)
        return;

    // 获取父控件（通过tpChildWidget的parent()或存储的parent_）
    tpChildWidget *parentWidget = dynamic_cast<tpChildWidget *>(parent());
    if (!parentWidget)
        return;

    // 获取父控件尺寸和边距
    int parentWidth = parentWidget->width();
    int parentHeight = parentWidget->height();
    int left, top, right, bottom;
    contentsMargins(&left, &top, &right, &bottom);

    // 计算可用区域
    int availableWidth = std::max(0, parentWidth - left - right);
    int availableHeight = std::max(0, parentHeight - top - bottom);
    // int totalSpacing = (flexData->widgetList.size() - 1) * spacing();

    // 实际可用区域大小
    // uint32_t remainingWidth = std::max(0, availableWidth - totalSpacing);
    // uint32_t remainingHeight = std::max(0, availableHeight - totalSpacing);

    // 确定主轴参数
    bool isRow = (flexData->flexDirection == Row || flexData->flexDirection == RowReverse);
    bool isReverse = (flexData->flexDirection == RowReverse || flexData->flexDirection == ColumnReverse);
    int mainSign = isReverse ? -1 : 1;

    // 收集可见子项
    struct ItemInfo
    {
        tpChildWidget *widget;
        int mainSize;
        int crossSize;
        bool isSpacer;
    };
    tpVector<ItemInfo> visibleItems;

    for (auto &item : flexData->widgetList)
    {
        tpChildWidget *child = item.widget ? item.widget : item.layoutWidget;
        if (!child || !child->visible())
            continue;

        if (child->parent() != parentWidget)
            child->setParent(parentWidget);

        int mainSize = isRow ? child->width() : child->height();
        int crossSize = isRow ? child->height() : child->width();

        if (mainSize <= 0)
            continue; // 过滤无效尺寸
        visibleItems.push_back({child, mainSize, crossSize, false});
    }

    // 分行处理
    struct FlexLine
    {
        tpVector<ItemInfo> items;

        // 所有控件总宽度/高度 包括spacing
        int mainSize = 0;

        int crossSize = 0;
    };
    tpVector<FlexLine> lines;

    // 所有组件的主轴大小和交叉轴大小，用于限制layout的最小宽高
    // uint32_t allMainSize = 0;
    uint32_t allCrossSize = 0;

    if (!visibleItems.empty())
    {
        FlexLine currentLine;
        int currentMain = 0;

        for (auto &item : visibleItems)
        {
            int neededSpace = currentMain + (currentLine.items.empty() ? 0 : spacing()) + item.mainSize;

            if (!currentLine.items.empty() && neededSpace > (isRow ? availableWidth : availableHeight))
            {
                lines.push_back(currentLine);

                allCrossSize += currentLine.crossSize + spacing();

                currentLine = FlexLine();
                currentMain = 0;
            }

            if (currentLine.items.empty())
            {
                currentMain = item.mainSize;
            }
            else
            {
                currentMain += spacing() + item.mainSize;
            }

            currentLine.items.push_back(item);
            currentLine.mainSize = currentMain;

            if (item.crossSize > currentLine.crossSize)
            {
                currentLine.crossSize = item.crossSize;
            }
        }

        if (!currentLine.items.empty())
        {
            lines.push_back(currentLine);

            // 记录layout的最小宽高
            // allMainSize += currentMain;
            allCrossSize += currentLine.crossSize + spacing();
        }
    }

    // std::cout << " allCrossSize " << allCrossSize << std::endl
    //           << std::endl;
    // std::cout << " linesCount " << lines.size() << std::endl
    //           << std::endl;
    // std::cout << " FirstlinesCount " << (lines.size() > 0 ? lines.front().items.size() : -1) << std::endl
    //           << std::endl;

    flexData->rowCount = lines.size();
    flexData->columnCount = (lines.size() > 0 ? lines.front().items.size() : 0);

    if (isRow)
    {
        parentWidget->setMinumumHeight(allCrossSize + top + bottom);
    }
    else
    {
        parentWidget->setMinumumWidth(allCrossSize + left + right);
    }

    // 计算交叉轴布局
    int totalCrossSize = 0;
    for (auto &line : lines)
    {
        totalCrossSize += line.crossSize;
        if (&line != &lines.back())
            totalCrossSize += spacing();
    }

    // 交叉轴对齐
    int crossPos = top;
    int remainingCross = (isRow ? availableHeight : availableWidth) - totalCrossSize;

    // 分散对齐时，不再使用spacing，使用newSpace
    // int newSpace = 0;

    switch (flexData->alignContent)
    {
    case FlexStart:
        break;
    case FlexEnd:
        crossPos += remainingCross;
        break;
    case Center:
        crossPos += remainingCross / 2;
        break;
    case SpaceBetween:
        if (lines.size() > 1)
        {
            int space = remainingCross / (lines.size() - 1);
            crossPos = 0;
            // 在行间插入间距
            // crossPos += space;
        }
        break;
    case SpaceAround:
        if (lines.size() > 0)
        {
            int space = remainingCross / (lines.size() * 2);
            crossPos = space;
            // 每行前后加间距
        }
        break;
    case Stretch:
        // 需要调整行高
        break;
    }

    // 遍历每行布局
    for (auto &line : lines)
    {
        int remainingMain = (isRow ? availableWidth : availableHeight) - line.mainSize;
        int mainPos = left;

        // 主轴对齐
        switch (flexData->justifyContent)
        {
        case MainFlexStart:
            break;
        case MainFlexEnd:
            mainPos += remainingMain;
            break;
        case MainCenter:
            mainPos += remainingMain / 2;
            break;
        case MainSpaceBetween:
        {
            if (line.items.size() > 1)
            {
                int itemCross = crossPos;

                int availableMain = remainingMain + (line.items.size() - 1) * spacing();

                int space = availableMain / (line.items.size() - 1);

                // 如果间隔不能等分剩余空间，则在中间控件时以此补充1px，最终保证margin的准确
                int suppleSPace = availableMain - (space * (line.items.size() - 1));
                if (suppleSPace < 0)
                    suppleSPace = 0;

                // 调整主轴起始位置
                if (isReverse)
                {
                    mainPos = (isRow ? parentWidth - right : parentHeight - bottom) - line.mainSize;
                }
                else
                {
                    mainPos = isRow ? left : top;
                }
                // 记录当前主轴位置
                int currentMain = mainPos;

                // 遍历元素时应用间距
                for (int i = 0; i < line.items.size(); ++i)
                {
                    auto &item = line.items[i];
                    // 设置元素位置
                    if (isRow)
                    {
                        if (isReverse)
                        {
                            item.widget->move(currentMain - item.mainSize, itemCross);
                            if (i != line.items.size() - 1)
                            {
                                currentMain -= (item.mainSize + space);

                                if (suppleSPace > 0)
                                {
                                    currentMain -= 1;
                                    suppleSPace -= 1;
                                }
                            }
                        }
                        else
                        {
                            item.widget->move(currentMain, itemCross);
                            if (i != line.items.size() - 1)
                            {
                                currentMain += (item.mainSize + space);

                                if (suppleSPace > 0)
                                {
                                    currentMain += 1;
                                    suppleSPace -= 1;
                                }
                            }
                        }
                    }
                    else
                    {
                        if (isReverse)
                        {
                            item.widget->move(itemCross, currentMain - item.mainSize);
                            if (i != line.items.size() - 1)
                            {
                                currentMain -= (item.mainSize + space);

                                if (suppleSPace > 0)
                                {
                                    currentMain -= 1;
                                    suppleSPace -= 1;
                                }
                            }
                        }
                        else
                        {
                            item.widget->move(itemCross, currentMain);
                            if (i != line.items.size() - 1)
                            {
                                currentMain += (item.mainSize + space);

                                if (suppleSPace > 0)
                                {
                                    currentMain += 1;
                                    suppleSPace -= 1;
                                }
                            }
                        }
                    }
                }

                crossPos += line.crossSize + spacing();

                continue;
            }
            break;
        }
        case MainSpaceEvenly:
        {
            int itemCross = crossPos;

            // 均分模式不再使用spacing
            int availableMain = remainingMain + (line.items.size() - 1) * spacing();

            int space = availableMain / (2 + line.items.size() - 1);

            // 如果间隔不能等分剩余空间，则在中间控件时以此补充1px，最终保证margin的准确
            int suppleSPace = availableMain - (space * (2 + line.items.size() - 1));
            if (suppleSPace < 0)
                suppleSPace = 0;

            // 调整主轴起始位置
            mainPos = space;

            // 记录当前主轴位置
            int currentMain = mainPos;

            // 遍历元素时应用间距
            for (int i = 0; i < line.items.size(); ++i)
            {
                auto &item = line.items[i];
                // 设置元素位置
                if (isRow)
                {
                    if (isReverse)
                    {
                        item.widget->move(currentMain - item.mainSize, itemCross);
                        if (i != line.items.size() - 1)
                        {
                            currentMain -= (item.mainSize + space);

                            if (suppleSPace > 0)
                            {
                                currentMain -= 1;
                                suppleSPace -= 1;
                            }
                        }
                    }
                    else
                    {
                        item.widget->move(currentMain, itemCross);
                        if (i != line.items.size() - 1)
                        {
                            currentMain += (item.mainSize + space);

                            if (suppleSPace > 0)
                            {
                                currentMain += 1;
                                suppleSPace -= 1;
                            }
                        }
                    }
                }
                else
                {
                    if (isReverse)
                    {
                        item.widget->move(itemCross, currentMain - item.mainSize);
                        if (i != line.items.size() - 1)
                        {
                            currentMain -= (item.mainSize + space);

                            if (suppleSPace > 0)
                            {
                                currentMain -= 1;
                                suppleSPace -= 1;
                            }
                        }
                    }
                    else
                    {
                        item.widget->move(itemCross, currentMain);
                        if (i != line.items.size() - 1)
                        {
                            currentMain += (item.mainSize + space);

                            if (suppleSPace > 0)
                            {
                                currentMain += 1;
                                suppleSPace -= 1;
                            }
                        }
                    }
                }
            }

            crossPos += line.crossSize + spacing();

            continue;
        }
        case MainSpaceAround:
        {
            int itemCross = crossPos;

            // 边缘的间距是中间间距的一半，不再使用spacing
            int availableMain = remainingMain + (line.items.size() - 1) * spacing();

            int space = availableMain / (2 + (line.items.size() - 1) * 2);

            // 如果间隔不能等分剩余空间，则在中间控件时以此补充1px，最终保证margin的准确
            int suppleSPace = availableMain - (space * (2 + (line.items.size() - 1) * 2));
            if (suppleSPace < 0)
                suppleSPace = 0;

            // 调整主轴起始位置
            mainPos = space;

            // 记录当前主轴位置
            int currentMain = mainPos;

            // 遍历元素时应用间距
            for (int i = 0; i < line.items.size(); ++i)
            {
                auto &item = line.items[i];
                // 设置元素位置
                if (isRow)
                {
                    if (isReverse)
                    {
                        item.widget->move(currentMain - item.mainSize, itemCross);
                        if (i != line.items.size() - 1)
                        {
                            currentMain -= (item.mainSize + space * 2);

                            if (suppleSPace > 0)
                            {
                                currentMain -= 1;
                                suppleSPace -= 1;
                            }
                        }
                    }
                    else
                    {
                        item.widget->move(currentMain, itemCross);
                        if (i != line.items.size() - 1)
                        {
                            currentMain += (item.mainSize + space * 2);

                            if (suppleSPace > 0)
                            {
                                currentMain += 1;
                                suppleSPace -= 1;
                            }
                        }
                    }
                }
                else
                {
                    if (isReverse)
                    {
                        item.widget->move(itemCross, currentMain - item.mainSize);
                        if (i != line.items.size() - 1)
                        {
                            currentMain -= (item.mainSize + space * 2);

                            if (suppleSPace > 0)
                            {
                                currentMain -= 1;
                                suppleSPace -= 1;
                            }
                        }
                    }
                    else
                    {
                        item.widget->move(itemCross, currentMain);
                        if (i != line.items.size() - 1)
                        {
                            currentMain += (item.mainSize + space * 2);

                            if (suppleSPace > 0)
                            {
                                currentMain += 1;
                                suppleSPace -= 1;
                            }
                        }
                    }
                }
            }

            crossPos += line.crossSize + spacing();

            continue;
        }
        }

        // 反向处理
        if (isReverse)
        {
            mainPos = (isRow ? availableWidth : availableHeight) - mainPos;
        }

        // 遍历行内元素
        int currentMain = mainPos;
        for (auto &item : line.items)
        {
            // 交叉轴对齐
            int itemCross = crossPos;
            switch (flexData->alignItems)
            {
            case CrossFlexStart:
                break;
            case CrossFlexEnd:
                itemCross += remainingCross;
                // itemCross += line.crossSize - item.crossSize;
                break;
            case CrossCenter:
                // itemCross += (line.crossSize - item.crossSize) / 2;
                itemCross += remainingCross / 2;

                break;
            }

            // 设置坐标
            if (isRow)
            {
                if (isReverse)
                    currentMain -= (item.mainSize + spacing());

                item.widget->move(currentMain, itemCross);

                if (!isReverse)
                    currentMain += item.mainSize + spacing();
            }
            else
            {
                if (isReverse)
                    currentMain -= (item.mainSize + spacing());

                item.widget->move(itemCross, currentMain);

                if (!isReverse)
                    currentMain += item.mainSize + spacing();
            }
        
            // item.widget->update();
        }

        // 更新交叉轴位置
        crossPos += line.crossSize + spacing();
    }
}

uint32_t tpFlexLayout::rowCount()
{
    tpFlexLayoutData *layoutData = static_cast<tpFlexLayoutData *>(data_);
    return layoutData->rowCount;
}

uint32_t tpFlexLayout::columnCount()
{
    tpFlexLayoutData *layoutData = static_cast<tpFlexLayoutData *>(data_);
    return layoutData->columnCount;
}

void tpFlexLayout::clear()
{
    tpFlexLayoutData *flexData = static_cast<tpFlexLayoutData *>(data_);
    for (auto &item : flexData->widgetList)
    {
        if (item.layoutWidget)
        {
            tpList<tpObject *> layoutChildObjList = item.layoutWidget->objectList();
            for (const auto &childObj : layoutChildObjList)
            {
                childObj->setParent(nullptr);
            }

            item.layoutWidget->deleteLater();
            item.layoutWidget = nullptr;
        }
        else if (item.widget)
        {
            item.widget->setParent(nullptr);
        }
        else
        {
        }
    }

    flexData->widgetList.clear();
}

tpVector<tpObject *> tpFlexLayout::children()
{
    tpVector<tpObject *> childList;

    tpFlexLayoutData *layoutData = static_cast<tpFlexLayoutData *>(data_);
    if (!layoutData || layoutData->widgetList.empty())
        return childList;

    for (const auto &childData : layoutData->widgetList)
    {
        if (childData.widget)
        {
            childList.emplace_back(childData.widget);
        }
        else if (childData.layout)
        {
            childList.emplace_back(childData.layout);
        }
        else
        {
        }
    }

    return childList;
}
