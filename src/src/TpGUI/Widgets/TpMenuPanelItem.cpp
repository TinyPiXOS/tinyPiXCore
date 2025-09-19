#include "TpMenuPanelItem.h"
#include "TpLabel.h"
#include "TpFont.h"
#include "TpHBoxLayout.h"
#include "TpSpacerItem.h"
#include "TpImage.h"
#include "TpFile.h"
#include "TpEvent.h"

struct TpMenuPanelItemData
{
    TpLabel *iconLabel;
    TpLabel *titleLabel;
    TpLabel *subTitleLabel;
    TpLabel *expandLabel;

    TpChildWidget *customWidget = nullptr;

    TpHBoxLayout *mainLayout;

    TpPoint pressPoint;
};

TpMenuPanelItem::TpMenuPanelItem(TpChildWidget *parent)
    : TpChildWidget(parent)
{
    tpShared<TpCssData> curCssData = currentStatusCss();

    TpMenuPanelItemData *itemData = new TpMenuPanelItemData();

    itemData->iconLabel = new TpLabel(this);
    itemData->iconLabel->setVisible(false);
    itemData->iconLabel->setFixedSize(curCssData->iconSize(), curCssData->iconSize());
    itemData->iconLabel->installEventFilter(this);

    itemData->titleLabel = new TpLabel(this);
    itemData->titleLabel->font()->setFontSize(curCssData->fontSize());
    itemData->titleLabel->font()->setFontColor(curCssData->color(), curCssData->color());
    itemData->titleLabel->installEventFilter(this);

    itemData->subTitleLabel = new TpLabel(this);
    itemData->subTitleLabel->font()->setFontSize(curCssData->fontSize());
    itemData->subTitleLabel->font()->setFontColor(_RGB(89, 89, 89), _RGB(89, 89, 89));
    itemData->subTitleLabel->installEventFilter(this);
    itemData->subTitleLabel->setVisible(false);

    itemData->expandLabel = new TpLabel(this);
    itemData->expandLabel->setFixedSize(19, 19);
    itemData->expandLabel->installEventFilter(this);

    itemData->expandLabel->setBackGroundImage(TpImage("/usr/res/TinyPiX/菜单面板单元-展开.png"));

    itemData->mainLayout = new TpHBoxLayout();
    itemData->mainLayout->setContentsMargins(curCssData->paddingLeft(), curCssData->paddingTop(), curCssData->paddingRight(), curCssData->paddingBottom());
    itemData->mainLayout->setSpacing(curCssData->gap());

    TpSpacerItem *spacerItem = new TpSpacerItem(20, 20, TpSpacerItem::Expanding, TpSpacerItem::Minimum);

    itemData->mainLayout->addWidget(itemData->iconLabel);
    itemData->mainLayout->addWidget(itemData->titleLabel);
    itemData->mainLayout->addSpacer(spacerItem);
    itemData->mainLayout->addWidget(itemData->subTitleLabel);
    itemData->mainLayout->addWidget(itemData->expandLabel);

    setLayout(itemData->mainLayout);

    data_ = itemData;

    setCheckable(true);
    refreshBaseCss();
}

TpMenuPanelItem::~TpMenuPanelItem()
{
    TpMenuPanelItemData *itemData = static_cast<TpMenuPanelItemData *>(data_);
    if (itemData)
    {
        itemData->iconLabel->setParent(nullptr);
        delete itemData->iconLabel;
        itemData->iconLabel = nullptr;

        itemData->titleLabel->setParent(nullptr);
        delete itemData->titleLabel;
        itemData->titleLabel = nullptr;

        itemData->subTitleLabel->setParent(nullptr);
        delete itemData->subTitleLabel;
        itemData->subTitleLabel = nullptr;

        itemData->expandLabel->setParent(nullptr);
        delete itemData->expandLabel;
        itemData->expandLabel = nullptr;

        delete itemData;
        itemData = nullptr;
        data_ = nullptr;
    }
}

void TpMenuPanelItem::setIcon(const TpString &iconPath)
{
    TpFile iconFile(iconPath);
    if (!iconFile.exists())
        return;

    TpMenuPanelItemData *itemData = static_cast<TpMenuPanelItemData *>(data_);
    if (!itemData)
        return;

    itemData->iconLabel->setVisible(true);

    itemData->iconLabel->setBackGroundImage(TpImage(iconPath));
}

void TpMenuPanelItem::setTitle(const TpString &text)
{
    TpMenuPanelItemData *itemData = static_cast<TpMenuPanelItemData *>(data_);
    if (!itemData)
        return;

    itemData->titleLabel->setText(text);
}

void TpMenuPanelItem::setSubTitle(const TpString &text)
{
    TpMenuPanelItemData *itemData = static_cast<TpMenuPanelItemData *>(data_);
    if (!itemData)
        return;

    itemData->subTitleLabel->setVisible(true);
    itemData->subTitleLabel->setText(text);
}

void TpMenuPanelItem::setCustomizeWidget(TpChildWidget *widget)
{
    TpMenuPanelItemData *itemData = static_cast<TpMenuPanelItemData *>(data_);
    if (!itemData)
        return;

    if (!widget)
    {
        if (itemData->customWidget)
        {
            itemData->mainLayout->removeWidget(itemData->customWidget);
            itemData->customWidget = nullptr;

            itemData->subTitleLabel->setVisible(true);
            itemData->expandLabel->setVisible(true);
        }
        return;
    }

    // 已经有一个widget了
    itemData->subTitleLabel->setVisible(false);
    itemData->expandLabel->setVisible(false);

    // 当前没有widget，需要向布局添加当前widget
    if (!itemData->customWidget)
    {
        itemData->mainLayout->addWidget(widget);
    }

    widget->installEventFilter(this);
    itemData->customWidget = widget;
}

TpChildWidget *TpMenuPanelItem::customizeWidget()
{
    TpMenuPanelItemData *itemData = static_cast<TpMenuPanelItemData *>(data_);
    if (!itemData)
        return nullptr;
    return itemData->customWidget;
}

bool TpMenuPanelItem::onMousePressEvent(TpMouseEvent *event)
{
    TpChildWidget::onMousePressEvent(event);

    TpMenuPanelItemData *itemData = static_cast<TpMenuPanelItemData *>(data_);
    itemData->pressPoint = event->globalPos();

    return true;
}

bool TpMenuPanelItem::onMouseRleaseEvent(TpMouseEvent *event)
{
    TpChildWidget::onMouseRleaseEvent(event);

    TpMenuPanelItemData *itemData = static_cast<TpMenuPanelItemData *>(data_);

    TpPoint mouseGlobalPos = event->globalPos();
    if ((std::abs(mouseGlobalPos.x() - itemData->pressPoint.x()) <= 5) && (std::abs(mouseGlobalPos.y() - itemData->pressPoint.y()) <= 5))
    {
        onClicked.emit(checked());
    }

    return true;
}

bool TpMenuPanelItem::onMouseMoveEvent(TpMouseEvent *event)
{
    TpChildWidget::onMouseMoveEvent(event);

    return true;
}

bool TpMenuPanelItem::eventFilter(TpObject *watched, TpEvent *event)
{
    if (event->eventType() == TpEvent::EVENT_MOUSE_RELEASE_TYPE)
    {
        TpMouseEvent *mouseEvent = dynamic_cast<TpMouseEvent *>(event);
        onMouseRleaseEvent(mouseEvent);
    }
    return false;
}
