#include "tpMenuPanelWidget.h"
#include "tpVector.h"
#include "tpVBoxLayout.h"
#include "tpEvent.h"
#include "tpLine.h"

struct tpMenuPanelWidgetData
{
    tpVector<tpMenuPanelItem *> itemList;

    tpVBoxLayout *mainLayout;

    tinyPiX::ItemSelectionMode selectionMode = tinyPiX::SingleSelection;
};

tpMenuPanelWidget::tpMenuPanelWidget(tpChildWidget *parent)
    : tpChildWidget(parent)
{
    tpMenuPanelWidgetData *widgetData = new tpMenuPanelWidgetData();

    widgetData->mainLayout = new tpVBoxLayout();
    widgetData->mainLayout->setSpacing(0);
    widgetData->mainLayout->setContentsMargins(0, 0, 0, 0);

    data_ = widgetData;

    setLayout(widgetData->mainLayout);
    refreshBaseCss();
}

tpMenuPanelWidget::~tpMenuPanelWidget()
{
    tpMenuPanelWidgetData *widgetData = static_cast<tpMenuPanelWidgetData *>(data_);
    if (widgetData)
    {
        delete widgetData;
        widgetData = nullptr;
    }
}

void tpMenuPanelWidget::setSelectionMode(const tinyPiX::ItemSelectionMode &selectionMode)
{
    tpMenuPanelWidgetData *widgetData = static_cast<tpMenuPanelWidgetData *>(data_);
    widgetData->selectionMode = selectionMode;
}

tinyPiX::ItemSelectionMode tpMenuPanelWidget::selectionMode()
{
    tpMenuPanelWidgetData *widgetData = static_cast<tpMenuPanelWidgetData *>(data_);
    return widgetData->selectionMode;
}

void tpMenuPanelWidget::addItem(tpMenuPanelItem *item)
{
    tpMenuPanelWidgetData *widgetData = static_cast<tpMenuPanelWidgetData *>(data_);

    // item->setParent(this);

    item->installEventFilter(this);

    if (widgetData->itemList.size() != 0)
    {
        // 添加一个分割线
        tpLine *newLine = new tpLine();
        newLine->setLineType(tpLine::HLine);
        newLine->setColor(_RGB(230, 235, 241));
        newLine->setLinePercentLength(0.85);
        newLine->setAlign(tinyPiX::AlignCenter);
        newLine->setFixedHeight(1);

        widgetData->mainLayout->addWidget(newLine);
    }

    widgetData->mainLayout->addWidget(item);
    widgetData->itemList.emplace_back(item);

    // 绑定选中信号槽
    connect(item, onClicked, [=](bool checked)
            {
        if (checked == false)
        {
            item->setChecked(true);
            return;
        }

        for (const auto& curItem : widgetData->itemList)
        {
            if (curItem != item)
            {
                curItem->setChecked(false);
            }
        }

        onClicked.emit(item);

        update(); });

    // widget永远固定大小
    setFixedHeight((item->height() + 1) * widgetData->itemList.size());

    update();
}

void tpMenuPanelWidget::removeItem(tpMenuPanelItem *item)
{
    if (!item)
        return;

    tpMenuPanelWidgetData *widgetData = static_cast<tpMenuPanelWidgetData *>(data_);
    for (int i = 0; i < widgetData->itemList.size(); ++i)
    {
        tpMenuPanelItem *curItem = widgetData->itemList.at(i);
        if (curItem != item)
            continue;

        // curItem->setParent(nullptr);
        // delete curItem;
        // curItem = nullptr;

        // 移除分割线
        if (i != 0)
        {
            // 遍历找到要移除的item，分割线就是item的前一个item
            tpVector<tpObject *> childList = widgetData->mainLayout->children();
            tpObject *lastChild = childList.size() > 0 ? childList.front() : nullptr;
            for (const auto &layoutChild : childList)
            {
                if (layoutChild == curItem)
                {
                    if (lastChild)
                    {
                        widgetData->mainLayout->removeWidget((tpChildWidget*)lastChild);
                        lastChild->deleteLater();
                    }
                    break;
                }

                lastChild = layoutChild;
            }
        }

        // 从布局移除item并释放指针
        widgetData->mainLayout->removeWidget(curItem);
        curItem->deleteLater();
        widgetData->itemList.remove(i);

        break;
    }
}

void tpMenuPanelWidget::clearSelection()
{
    tpMenuPanelWidgetData *widgetData = static_cast<tpMenuPanelWidgetData *>(data_);
    for (int i = 0; i < widgetData->itemList.size(); ++i)
    {
        tpMenuPanelItem *curItem = widgetData->itemList.at(i);
        curItem->setChecked(false);
    }

    update();
}

bool tpMenuPanelWidget::onMousePressEvent(tpMouseEvent *event)
{
    tpChildWidget::onMousePressEvent(event);

    return true;
}

bool tpMenuPanelWidget::onMouseRleaseEvent(tpMouseEvent *event)
{
    tpChildWidget::onMouseRleaseEvent(event);

    return true;
}

bool tpMenuPanelWidget::onPaintEvent(tpObjectPaintEvent *event)
{
    tpChildWidget::onPaintEvent(event);

    return true;
}

bool tpMenuPanelWidget::onResizeEvent(tpObjectResizeEvent *event)
{
    tpChildWidget::onResizeEvent(event);

    return true;
}

bool tpMenuPanelWidget::onFocusEvent(tpObjectFocusEvent *event)
{
    tpChildWidget::onFocusEvent(event);

    if (!event->focused())
    {
        clearSelection();
    }

    return true;
}

bool tpMenuPanelWidget::eventFilter(tpObject *watched, tpEvent *event)
{
    if (event->eventType() == tpEvent::EVENT_MOUSE_PRESS_TYPE)
    {
        tpMouseEvent *mouseEvent = dynamic_cast<tpMouseEvent *>(event);
        onMousePressEvent(mouseEvent);
    }
    else if (event->eventType() == tpEvent::EVENT_MOUSE_RELEASE_TYPE)
    {
        tpMouseEvent *mouseEvent = dynamic_cast<tpMouseEvent *>(event);
        onMouseRleaseEvent(mouseEvent);
    }
    else if (event->eventType() == tpEvent::EVENT_OBJECT_FOCUS_TYPE)
    {
        tpObjectFocusEvent *focusEvent = dynamic_cast<tpObjectFocusEvent *>(event);
        onFocusEvent(focusEvent);
    }
    else
    {
    }

    return false;
}

void tpMenuPanelWidget::onThemeChangeEvent(tpThemeChangeEvent *event)
{
}
