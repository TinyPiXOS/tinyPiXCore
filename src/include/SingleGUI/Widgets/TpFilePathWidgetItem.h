#ifndef __TP_FILE_PATH_WIDGET_ITEM_H
#define __TP_FILE_PATH_WIDGET_ITEM_H

#include "TpLabel.h"

TP_DEF_VOID_TYPE_VAR(ItpFilePathWidgetItemData);

/// @brief 路径显示按钮类
class TpFilePathWidgetItem : public TpLabel
{
public:
    TpFilePathWidgetItem(TpChildWidget *parent = nullptr);

    virtual ~TpFilePathWidgetItem();

    virtual void setText(const TpString &text) override;

protected:
    virtual bool onPaintEvent(TpPaintEvent *event) override;

    virtual TpString pluginType() override { return TO_STRING(TpFilePathWidgetItem); }

private:
    ItpFilePathWidgetItemData *data_;
};

#endif
