#include "TpMenuPanelWidget.h"
#include "TpVector.h"
#include "TpVBoxLayout.h"
#include "TpEvent.h"
#include "TpLine.h"

struct TpMenuPanelWidgetData
{
    TpVector<TpMenuPanelItem *> itemList;

    TpVBoxLayout *mainLayout;

    Tp::ItemSelectionMode selectionMode = Tp::SingleSelection;
};

TpMenuPanelWidget::TpMenuPanelWidget(TpWidget *parent)
    : TpWidget(parent)
{
    TpMenuPanelWidgetData *widgetData = new TpMenuPanelWidgetData();

    widgetData->mainLayout = new TpVBoxLayout();
    widgetData->mainLayout->setSpacing(0);
    widgetData->mainLayout->setContentsMargins(0, 0, 0, 0);

    data_ = widgetData;

    setLayout(widgetData->mainLayout);
    refreshBaseCss();
}

TpMenuPanelWidget::~TpMenuPanelWidget()
{
    TpMenuPanelWidgetData *widgetData = static_cast<TpMenuPanelWidgetData *>(data_);
    if (widgetData)
    {
        delete widgetData;
        widgetData = nullptr;
    }
}

void TpMenuPanelWidget::setSelectionMode(const Tp::ItemSelectionMode &selectionMode)
{
    TpMenuPanelWidgetData *widgetData = static_cast<TpMenuPanelWidgetData *>(data_);
    widgetData->selectionMode = selectionMode;
}

Tp::ItemSelectionMode TpMenuPanelWidget::selectionMode()
{
    TpMenuPanelWidgetData *widgetData = static_cast<TpMenuPanelWidgetData *>(data_);
    return widgetData->selectionMode;
}

void TpMenuPanelWidget::addItem(TpMenuPanelItem *item)
{
    TpMenuPanelWidgetData *widgetData = static_cast<TpMenuPanelWidgetData *>(data_);

    // item->setParent(this);

    item->installEventFilter(this);

    if (widgetData->itemList.size() != 0)
    {
        // 添加一个分割线
        TpLine *newLine = new TpLine();
        newLine->setLineType(TpLine::HLine);
        newLine->setColor(_RGB(230, 235, 241));
        newLine->setLinePercentLength(0.85);
        newLine->setAlign(Tp::AlignCenter);
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

void TpMenuPanelWidget::removeItem(TpMenuPanelItem *item)
{
    if (!item)
        return;

    TpMenuPanelWidgetData *widgetData = static_cast<TpMenuPanelWidgetData *>(data_);
    for (int i = 0; i < widgetData->itemList.size(); ++i)
    {
        TpMenuPanelItem *curItem = widgetData->itemList.at(i);
        if (curItem != item)
            continue;

        // curItem->setParent(nullptr);
        // delete curItem;
        // curItem = nullptr;

        // 移除分割线
        if (i != 0)
        {
            // 遍历找到要移除的item，分割线就是item的前一个item
            TpVector<TpObject *> childList = widgetData->mainLayout->children();
            TpObject *lastChild = childList.size() > 0 ? childList.front() : nullptr;
            for (const auto &layoutChild : childList)
            {
                if (layoutChild == curItem)
                {
                    if (lastChild)
                    {
                        widgetData->mainLayout->removeWidget((TpWidget*)lastChild);
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

void TpMenuPanelWidget::clearSelection()
{
    TpMenuPanelWidgetData *widgetData = static_cast<TpMenuPanelWidgetData *>(data_);
    for (int i = 0; i < widgetData->itemList.size(); ++i)
    {
        TpMenuPanelItem *curItem = widgetData->itemList.at(i);
        curItem->setChecked(false);
    }

    update();
}

bool TpMenuPanelWidget::onMousePressEvent(TpMouseEvent *event)
{
    TpWidget::onMousePressEvent(event);

    return true;
}

bool TpMenuPanelWidget::onMouseRleaseEvent(TpMouseEvent *event)
{
    TpWidget::onMouseRleaseEvent(event);

    return true;
}

bool TpMenuPanelWidget::onPaintEvent(TpPaintEvent *event)
{
    TpWidget::onPaintEvent(event);

    return true;
}

bool TpMenuPanelWidget::onResizeEvent(TpResizeEvent *event)
{
    TpWidget::onResizeEvent(event);

    return true;
}

bool TpMenuPanelWidget::onFocusEvent(TpFocusEvent *event)
{
    TpWidget::onFocusEvent(event);

    if (!event->focused())
    {
        clearSelection();
    }

    return true;
}

bool TpMenuPanelWidget::eventFilter(TpObject *watched, TpEvent *event)
{
    if (event->eventType() == TpEvent::EVENT_MOUSE_PRESS_TYPE)
    {
        TpMouseEvent *mouseEvent = dynamic_cast<TpMouseEvent *>(event);
        onMousePressEvent(mouseEvent);
    }
    else if (event->eventType() == TpEvent::EVENT_MOUSE_RELEASE_TYPE)
    {
        TpMouseEvent *mouseEvent = dynamic_cast<TpMouseEvent *>(event);
        onMouseRleaseEvent(mouseEvent);
    }
    else if (event->eventType() == TpEvent::EVENT_OBJECT_FOCUS_TYPE)
    {
        TpFocusEvent *focusEvent = dynamic_cast<TpFocusEvent *>(event);
        onFocusEvent(focusEvent);
    }
    else
    {
    }

    return false;
}

void TpMenuPanelWidget::onThemeChangeEvent(TpThemeChangeEvent *event)
{
}
