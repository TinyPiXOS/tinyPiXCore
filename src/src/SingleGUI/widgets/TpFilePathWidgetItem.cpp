#include "TpFilePathWidgetItem.h"
#include "TpFont.h"

struct TpFilePathWidgetItemData
{
};

TpFilePathWidgetItem::TpFilePathWidgetItem(TpChildWidget *parent)
    : TpLabel(parent)
{
    data_ = nullptr;

    refreshBaseCss();

    setCheckable(true);

    tpShared<TpCssData> curCssData = currentStatusCss();
    font()->setFontColor(curCssData->color(), curCssData->color());
    font()->setFontSize(curCssData->fontSize());
}

TpFilePathWidgetItem::~TpFilePathWidgetItem()
{
}

void TpFilePathWidgetItem::setText(const TpString &text)
{
    TpLabel::setText(text);

    // 根据文本长度，自动调整宽高
    uint32_t fontWidth = font()->pixelWidth();

    tpShared<TpCssData> curCssData = currentStatusCss();
    setWidth(fontWidth + curCssData->paddingLeft() + curCssData->paddingRight());
}

bool TpFilePathWidgetItem::onPaintEvent(TpObjectPaintEvent *event)
{
    TpLabel::onPaintEvent(event);

    tpShared<TpCssData> curCssData = currentStatusCss();
    font()->setFontColor(curCssData->color(), curCssData->color());

    return true;
}
