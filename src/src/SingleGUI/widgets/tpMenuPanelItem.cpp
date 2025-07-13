#include "tpMenuPanelItem.h"
#include "tpLabel.h"
#include "tpFont.h"
#include "tpHBoxLayout.h"
#include "tpSpacerItem.h"
#include "tpSurface.h"
#include "tpFile.h"
#include "tpEvent.h"

struct tpMenuPanelItemData
{
    tpLabel *iconLabel;
    tpLabel *titleLabel;
    tpLabel *subTitleLabel;
    tpLabel *expandLabel;

    tpChildWidget *customWidget = nullptr;

    tpHBoxLayout *mainLayout;
};

tpMenuPanelItem::tpMenuPanelItem(tpChildWidget *parent)
    : tpChildWidget(parent)
{
    tpShared<tpCssData> curCssData = currentStatusCss();

    tpMenuPanelItemData *itemData = new tpMenuPanelItemData();

    itemData->iconLabel = new tpLabel(this);
    itemData->iconLabel->setVisible(false);
    itemData->iconLabel->setFixedSize(curCssData->iconSize(), curCssData->iconSize());
    itemData->iconLabel->installEventFilter(this);

    itemData->titleLabel = new tpLabel(this);
    itemData->titleLabel->font()->setFontSize(curCssData->fontSize());
    itemData->titleLabel->font()->setFontColor(curCssData->color(), curCssData->color());
    itemData->titleLabel->installEventFilter(this);

    itemData->subTitleLabel = new tpLabel(this);
    itemData->subTitleLabel->font()->setFontSize(curCssData->fontSize());
    itemData->subTitleLabel->font()->setFontColor(_RGB(89, 89, 89), _RGB(89, 89, 89));
    itemData->subTitleLabel->installEventFilter(this);
    itemData->subTitleLabel->setVisible(false);

    itemData->expandLabel = new tpLabel(this);
    itemData->expandLabel->setFixedSize(19, 19);
    itemData->expandLabel->installEventFilter(this);

    tpShared<tpSurface> expandIcon = tpMakeShared<tpSurface>();
    expandIcon->fromFile("/usr/res/tinyPiX/菜单面板单元-展开.png");
    itemData->expandLabel->setBackGroundImage(expandIcon);

    itemData->mainLayout = new tpHBoxLayout();
    itemData->mainLayout->setContentsMargins(curCssData->paddingLeft(), curCssData->paddingTop(), curCssData->paddingRight(), curCssData->paddingBottom());
    itemData->mainLayout->setSpacing(curCssData->gap());

    tpSpacerItem *spacerItem = new tpSpacerItem(20, 20, tpSpacerItem::Expanding, tpSpacerItem::Minimum);

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

tpMenuPanelItem::~tpMenuPanelItem()
{
    tpMenuPanelItemData *itemData = static_cast<tpMenuPanelItemData *>(data_);
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

void tpMenuPanelItem::setIcon(const tpString &iconPath)
{
    tpFile iconFile(iconPath);
    if (!iconFile.exists())
        return;

    tpMenuPanelItemData *itemData = static_cast<tpMenuPanelItemData *>(data_);
    if (!itemData)
        return;

    itemData->iconLabel->setVisible(true);

    tpShared<tpSurface> expandIcon = tpMakeShared<tpSurface>();
    expandIcon->fromFile(iconPath);
    itemData->iconLabel->setBackGroundImage(expandIcon);
}

void tpMenuPanelItem::setTitle(const tpString &text)
{
    tpMenuPanelItemData *itemData = static_cast<tpMenuPanelItemData *>(data_);
    if (!itemData)
        return;

    itemData->titleLabel->setText(text);
}

void tpMenuPanelItem::setSubTitle(const tpString &text)
{
    tpMenuPanelItemData *itemData = static_cast<tpMenuPanelItemData *>(data_);
    if (!itemData)
        return;

    itemData->subTitleLabel->setVisible(true);
    itemData->subTitleLabel->setText(text);
}

void tpMenuPanelItem::setCustomizeWidget(tpChildWidget *widget)
{
    tpMenuPanelItemData *itemData = static_cast<tpMenuPanelItemData *>(data_);
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

tpChildWidget *tpMenuPanelItem::customizeWidget()
{
    tpMenuPanelItemData *itemData = static_cast<tpMenuPanelItemData *>(data_);
    if (!itemData)
        return nullptr;
    return itemData->customWidget;
}

bool tpMenuPanelItem::onMouseRleaseEvent(tpMouseEvent *event)
{
    tpChildWidget::onMouseRleaseEvent(event);

    onClicked.emit(checked());

    return true;
}

bool tpMenuPanelItem::onPaintEvent(tpObjectPaintEvent *event)
{
    tpChildWidget::onPaintEvent(event);

    return true;
}

bool tpMenuPanelItem::onResizeEvent(tpObjectResizeEvent *event)
{
    tpChildWidget::onResizeEvent(event);

    return true;
}

bool tpMenuPanelItem::eventFilter(tpObject *watched, tpEvent *event)
{
    if (event->eventType() == tpEvent::EVENT_MOUSE_RELEASE_TYPE)
    {
        tpMouseEvent *mouseEvent = dynamic_cast<tpMouseEvent *>(event);
        onMouseRleaseEvent(mouseEvent);
    }
    return false;
}

void tpMenuPanelItem::onThemeChangeEvent(tpThemeChangeEvent *event)
{
}
