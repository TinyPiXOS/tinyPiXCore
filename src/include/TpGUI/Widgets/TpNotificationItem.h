#ifndef __TP_NOTIFICATION_ITEM_H
#define __TP_NOTIFICATION_ITEM_H

#include "TpWidget.h"
#include "TpString.h"

TP_DEF_VOID_TYPE_VAR(ITpNotificationItemData);

class TpNotificationItem
    : public TpWidget
{
public:
    struct NotificationItemData
    {
        TpString messageId;   // 消息唯一ID，被点击后会回调给应用
        TpString appId;       // 应用uuid，唯一ID
        TpString appName;     // 应用名称（如"微信"）
        TpString timeStr;     // 时间（如"15:30"）
        TpString titleText;   // 标题文本
        TpString contentText; // 内容文本，过长会被自动截断

        NotificationItemData() : messageId(""), appId(""), appName(""), timeStr(""), titleText(""), contentText("")
        {
        }
        NotificationItemData(const TpString &_messageId, const TpString &_appId, const TpString &_appName, const TpString &_timeStr, const TpString &_titleText, const TpString &_contentText)
            : messageId(_messageId), appId(_appId), appName(_appName), timeStr(_timeStr), titleText(_titleText), contentText(_contentText)
        {
        }

        bool operator==(const NotificationItemData &others)
        {
            return (this->messageId.compare(others.messageId) == 0);
        }
    };

public:
    TpNotificationItem(TpWidget *parent = nullptr);
    ~TpNotificationItem();

    void setItemData(const NotificationItemData &data);
    const NotificationItemData &itemData();

public:
    virtual bool onPaintEvent(TpPaintEvent *event) override;
    virtual bool onMousePressEvent(TpMouseEvent *event) override;
    virtual bool onMouseRleaseEvent(TpMouseEvent *event) override;
    virtual bool onResizeEvent(TpResizeEvent *event) override;

private:
    ITpNotificationItemData *data_;
};

#endif