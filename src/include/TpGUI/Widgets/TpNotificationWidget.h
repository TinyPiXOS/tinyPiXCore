#ifndef __TP_NOTIFICATION_WIDGET_H
#define __TP_NOTIFICATION_WIDGET_H

#include "TpChildWidget.h"
#include "TpString.h"
#include "TpFont.h"
#include "TpNotificationItem.h"
#include "TpEvent.h"

TP_DEF_VOID_TYPE_VAR(ItpNotificationWidgetData);
class TpNotificationWidget : public TpChildWidget
{
public:
    TpNotificationWidget(TpChildWidget *parent = nullptr);
    ~TpNotificationWidget();

    void addItem(TpNotificationItem *item);

public:
    virtual bool onPaintEvent(TpPaintEvent *event) override;
    virtual bool onMousePressEvent(TpMouseEvent *event) override;
    virtual bool onMouseRleaseEvent(TpMouseEvent *event) override;
    virtual bool onResizeEvent(TpResizeEvent *event) override;

private:
    ItpNotificationWidgetData *data_;
};

#endif