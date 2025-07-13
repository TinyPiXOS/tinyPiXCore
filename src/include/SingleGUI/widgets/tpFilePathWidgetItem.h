#ifndef __TP_FILE_PATH_WIDGET_ITEM_H
#define __TP_FILE_PATH_WIDGET_ITEM_H

#include "tpLabel.h"

TP_DEF_VOID_TYPE_VAR(ItpFilePathWidgetItemData);

/// @brief 路径显示按钮类
class tpFilePathWidgetItem : public tpLabel
{
public:
    tpFilePathWidgetItem(tpChildWidget *parent = nullptr);

    virtual ~tpFilePathWidgetItem();

    virtual void setText(const tpString &text) override;

protected:
    virtual bool onPaintEvent(tpObjectPaintEvent *event) override;

    virtual tpString pluginType() override { return TO_STRING(tpFilePathWidgetItem); }

private:
    ItpFilePathWidgetItemData *data_;
};

#endif
