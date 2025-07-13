#ifndef __TP_NOTIFICATION_ITEM_H
#define __TP_NOTIFICATION_ITEM_H

#include "tpChildWidget.h"
#include "tpString.h"

TP_DEF_VOID_TYPE_VAR(ItpNotificationItemData);

class tpNotificationItem
    : public tpChildWidget
{
public:
    struct NotificationItemData
    {
        tpString messageId;   // 消息唯一ID，被点击后会回调给应用
        tpString appId;       // 应用uuid，唯一ID
        tpString appName;     // 应用名称（如"微信"）
        tpString timeStr;     // 时间（如"15:30"）
        tpString titleText;   // 标题文本
        tpString contentText; // 内容文本，过长会被自动截断

        NotificationItemData() : messageId(""), appId(""), appName(""), timeStr(""), titleText(""), contentText("")
        {
        }
        NotificationItemData(const tpString &_messageId, const tpString &_appId, const tpString &_appName, const tpString &_timeStr, const tpString &_titleText, const tpString &_contentText)
            : messageId(_messageId), appId(_appId), appName(_appName), timeStr(_timeStr), titleText(_titleText), contentText(_contentText)
        {
        }

        bool operator==(const NotificationItemData &others)
        {
            return (this->messageId.compare(others.messageId) == 0);
        }
    };

public:
    tpNotificationItem(tpChildWidget *parent = nullptr);
    ~tpNotificationItem();

    void setItemData(const NotificationItemData &data);
    const NotificationItemData &itemData();

public:
    virtual bool onPaintEvent(tpObjectPaintEvent *event) override;
    virtual bool onMousePressEvent(tpMouseEvent *event) override;
    virtual bool onMouseRleaseEvent(tpMouseEvent *event) override;
    virtual bool onResizeEvent(tpObjectResizeEvent *event) override;

private:
    ItpNotificationItemData *data_;
};

#endif