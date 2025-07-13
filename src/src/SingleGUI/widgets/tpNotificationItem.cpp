#include "tpNotificationItem.h"
#include "tpLabel.h"
#include "tpFont.h"

struct tpNotificationItemData
{
    // 核心数据
    tpNotificationItem::NotificationItemData itemInfo;

    tpLabel *iconLabel;
    tpLabel *mainTitleLabel;
    tpLabel *subTitleLabel;
    tpLabel *timeLabel;

    tpNotificationItemData()
    {
    }
    ~tpNotificationItemData()
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

tpNotificationItem::tpNotificationItem(tpChildWidget *parent)
    : tpChildWidget(parent)
{
    tpNotificationItemData *itemData = new tpNotificationItemData();
    data_ = itemData;

    itemData->iconLabel = new tpLabel(this);
    itemData->mainTitleLabel = new tpLabel(this);
    itemData->subTitleLabel = new tpLabel(this);
    itemData->timeLabel = new tpLabel(this);
}

tpNotificationItem::~tpNotificationItem()
{
    tpNotificationItemData *itemData = static_cast<tpNotificationItemData *>(data_);
    if (itemData)
    {
        delete itemData;
        itemData = nullptr;
        data_ = nullptr;
    }
}

void tpNotificationItem::setItemData(const NotificationItemData &data)
{
    tpNotificationItemData *itemData = static_cast<tpNotificationItemData *>(data_);
    itemData->itemInfo = data;
}

const tpNotificationItem::NotificationItemData &tpNotificationItem::itemData()
{
    tpNotificationItemData *itemData = static_cast<tpNotificationItemData *>(data_);
    return itemData->itemInfo;
}

bool tpNotificationItem::onPaintEvent(tpObjectPaintEvent *event)
{
    return true;
}

bool tpNotificationItem::onMousePressEvent(tpMouseEvent *event)
{
    return true;
}

bool tpNotificationItem::onMouseRleaseEvent(tpMouseEvent *event)
{
    return true;
}

bool tpNotificationItem::onResizeEvent(tpObjectResizeEvent *event)
{
    return true;
}
