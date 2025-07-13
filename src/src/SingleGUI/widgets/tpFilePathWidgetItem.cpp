#include "tpFilePathWidgetItem.h"
#include "tpFont.h"

struct tpFilePathWidgetItemData
{
};

tpFilePathWidgetItem::tpFilePathWidgetItem(tpChildWidget *parent)
    : tpLabel(parent)
{
    data_ = nullptr;

    refreshBaseCss();

    setCheckable(true);

    tpShared<tpCssData> curCssData = currentStatusCss();
    font()->setFontColor(curCssData->color(), curCssData->color());
    font()->setFontSize(curCssData->fontSize());
}

tpFilePathWidgetItem::~tpFilePathWidgetItem()
{
}

void tpFilePathWidgetItem::setText(const tpString &text)
{
    tpLabel::setText(text);

    // 根据文本长度，自动调整宽高
    uint32_t fontWidth = font()->pixelWidth();

    tpShared<tpCssData> curCssData = currentStatusCss();
    setWidth(fontWidth + curCssData->paddingLeft() + curCssData->paddingRight());
}

bool tpFilePathWidgetItem::onPaintEvent(tpObjectPaintEvent *event)
{
    tpLabel::onPaintEvent(event);

    tpShared<tpCssData> curCssData = currentStatusCss();
    font()->setFontColor(curCssData->color(), curCssData->color());

    return true;
}
