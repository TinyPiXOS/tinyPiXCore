#include "TpNotificationItem.h"
#include "TpLabel.h"
#include "TpFont.h"

struct TpNotificationItemData
{
    // 核心数据
    TpNotificationItem::NotificationItemData itemInfo;

    TpLabel *iconLabel;
    TpLabel *mainTitleLabel;
    TpLabel *subTitleLabel;
    TpLabel *timeLabel;

    TpNotificationItemData()
    {
    }
    ~TpNotificationItemData()
    {
        iconLabel->setParent(nullptr);
        mainTitleLabel->setParent(nullptr);
        subTitleLabel->setParent(nullptr);
        timeLabel->setParent(nullptr);

        delete iconLabel;
        delete mainTitleLabel;
        delete subTitleLabel;
        delete timeLabel;
    }
};

TpNotificationItem::TpNotificationItem(TpChildWidget *parent)
    : TpChildWidget(parent)
{
    TpNotificationItemData *itemData = new TpNotificationItemData();
    data_ = itemData;

    itemData->iconLabel = new TpLabel(this);
    itemData->mainTitleLabel = new TpLabel(this);
    itemData->subTitleLabel = new TpLabel(this);
    itemData->timeLabel = new TpLabel(this);
}

TpNotificationItem::~TpNotificationItem()
{
    TpNotificationItemData *itemData = static_cast<TpNotificationItemData *>(data_);
    if (itemData)
    {
        delete itemData;
        itemData = nullptr;
        data_ = nullptr;
    }
}

void TpNotificationItem::setItemData(const NotificationItemData &data)
{
    TpNotificationItemData *itemData = static_cast<TpNotificationItemData *>(data_);
    itemData->itemInfo = data;
}

const TpNotificationItem::NotificationItemData &TpNotificationItem::itemData()
{
    TpNotificationItemData *itemData = static_cast<TpNotificationItemData *>(data_);
    return itemData->itemInfo;
}

bool TpNotificationItem::onPaintEvent(TpPaintEvent *event)
{
    return true;
}

bool TpNotificationItem::onMousePressEvent(TpMouseEvent *event)
{
    return true;
}

bool TpNotificationItem::onMouseRleaseEvent(TpMouseEvent *event)
{
    return true;
}

bool TpNotificationItem::onResizeEvent(TpResizeEvent *event)
{
    return true;
}
