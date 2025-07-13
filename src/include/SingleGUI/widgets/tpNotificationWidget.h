#ifndef __TP_NOTIFICATION_WIDGET_H
#define __TP_NOTIFICATION_WIDGET_H

#include "tpChildWidget.h"
#include "tpString.h"
#include "tpFont.h"
#include "tpNotificationItem.h"
#include "tpEvent.h"

TP_DEF_VOID_TYPE_VAR(ItpNotificationWidgetData);
class tpNotificationWidget : public tpChildWidget
{
public:
    tpNotificationWidget(tpChildWidget *parent = nullptr);
    ~tpNotificationWidget();

    void addItem(tpNotificationItem *item);

public:
    virtual bool onPaintEvent(tpObjectPaintEvent *event) override;
    virtual bool onMousePressEvent(tpMouseEvent *event) override;
    virtual bool onMouseRleaseEvent(tpMouseEvent *event) override;
    virtual bool onResizeEvent(tpObjectResizeEvent *event) override;

private:
    ItpNotificationWidgetData *data_;
};

#endif