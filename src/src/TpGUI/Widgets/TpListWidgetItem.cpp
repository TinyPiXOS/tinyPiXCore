#include "TpListWidgetItem.h"
#include "TpLabel.h"
#include "TpFont.h"
#include "TpImage.h"
#include "TpEvent.h"

struct TpListCheckBoxGroupItemData
{
    TpLabel *titleLabel;
    TpLabel *selectIconLabel;
};

TpListWidgetItem::TpListWidgetItem(TpChildWidget *parent)
    : TpChildWidget(parent)
{
    TpListCheckBoxGroupItemData *itemData = new TpListCheckBoxGroupItemData();

    itemData->titleLabel = new TpLabel(this);
    itemData->titleLabel->installEventFilter(this);

    itemData->selectIconLabel = new TpLabel(this);
    itemData->selectIconLabel->installEventFilter(this);

    itemData->selectIconLabel->setBackGroundImage(TpImage("/usr/res/TinyPiX/勾选.png"));

    tpShared<TpCssData> normalCss = currentStatusCss();
    itemData->titleLabel->font()->setText("TpMenu");
    itemData->titleLabel->font()->setFontSize(normalCss->fontSize());
    itemData->titleLabel->font()->setFontColor(normalCss->color(), normalCss->color());

    itemData->selectIconLabel->setFixedSize(itemData->titleLabel->font()->pixelHeight(), itemData->titleLabel->font()->pixelHeight());

    data_ = itemData;

    refreshBaseCss();
    setCheckable(true);
}

TpListWidgetItem::TpListWidgetItem(const TpString &text, TpChildWidget *parent)
    : TpChildWidget(parent)
{
    TpListCheckBoxGroupItemData *itemData = new TpListCheckBoxGroupItemData();

    itemData->titleLabel = new TpLabel(this);
    itemData->titleLabel->installEventFilter(this);

    itemData->selectIconLabel = new TpLabel(this);
    itemData->selectIconLabel->installEventFilter(this);

    itemData->selectIconLabel->setBackGroundImage(TpImage("/usr/res/TinyPiX/勾选.png"));

    tpShared<TpCssData> normalCss = currentStatusCss();
    itemData->titleLabel->font()->setText("TpMenu");
    itemData->titleLabel->font()->setFontSize(normalCss->fontSize());
    itemData->titleLabel->font()->setFontColor(normalCss->color(), normalCss->color());

    itemData->selectIconLabel->setFixedSize(itemData->titleLabel->font()->pixelHeight(), itemData->titleLabel->font()->pixelHeight());

    data_ = itemData;

    refreshBaseCss();
    setCheckable(true);

    setText(text);
}

TpListWidgetItem::~TpListWidgetItem()
{
    TpListCheckBoxGroupItemData *itemData = static_cast<TpListCheckBoxGroupItemData *>(data_);
    if (itemData)
    {
        itemData->titleLabel->setParent(nullptr);
        itemData->selectIconLabel->setParent(nullptr);

        delete itemData->titleLabel;
        itemData->titleLabel = nullptr;

        delete itemData->selectIconLabel;
        itemData->selectIconLabel = nullptr;

        delete itemData;
        itemData = nullptr;
        data_ = nullptr;
    }
}

TpString TpListWidgetItem::text()
{
    TpListCheckBoxGroupItemData *itemData = static_cast<TpListCheckBoxGroupItemData *>(data_);

    return itemData->titleLabel->text();
}

void TpListWidgetItem::setText(const TpString &text)
{
    TpListCheckBoxGroupItemData *itemData = static_cast<TpListCheckBoxGroupItemData *>(data_);
    itemData->titleLabel->setText(text);
}

TpVariant TpListWidgetItem::data()
{
    return property("TpListCheckBoxGroupItemData");
}

void TpListWidgetItem::setData(const TpVariant &data)
{
    setProperty("TpListCheckBoxGroupItemData", data);
}

bool TpListWidgetItem::onPaintEvent(TpPaintEvent *event)
{
    TpChildWidget::onPaintEvent(event);

    TpListCheckBoxGroupItemData *itemData = static_cast<TpListCheckBoxGroupItemData *>(data_);

    tpShared<TpCssData> normalCss = currentStatusCss();

    itemData->titleLabel->font()->setFontSize(normalCss->fontSize());
    itemData->titleLabel->font()->setFontColor(normalCss->color(), normalCss->color());

    itemData->selectIconLabel->setVisible(checked());

    return true;
}

bool TpListWidgetItem::onMousePressEvent(TpMouseEvent *event)
{
    TpChildWidget::onMousePressEvent(event);

    return true;
}

bool TpListWidgetItem::onMouseRleaseEvent(TpMouseEvent *event)
{
    TpChildWidget::onMouseRleaseEvent(event);

    onStatusChanged.emit(this);

    return true;
}

bool TpListWidgetItem::onResizeEvent(TpResizeEvent *event)
{
    TpChildWidget::onResizeEvent(event);

    TpListCheckBoxGroupItemData *itemData = static_cast<TpListCheckBoxGroupItemData *>(data_);
    tpShared<TpCssData> normalCss = currentStatusCss();

    uint32_t titleY = (height() - itemData->titleLabel->font()->pixelHeight()) / 2.0;
    itemData->titleLabel->move(normalCss->paddingLeft(), titleY);

    itemData->selectIconLabel->move(width() - normalCss->paddingRight() - itemData->titleLabel->font()->pixelHeight(), titleY);

    return true;
}

bool TpListWidgetItem::eventFilter(TpObject *watched, TpEvent *event)
{
    if (event->eventType() == TpEvent::EVENT_MOUSE_PRESS_TYPE)
    {
        TpMouseEvent *mouseEvent = dynamic_cast<TpMouseEvent *>(event);
        onMousePressEvent(mouseEvent);
    }
    else if (event->eventType() == TpEvent::EVENT_MOUSE_RELEASE_TYPE)
    {
        TpMouseEvent *mouseEvent = dynamic_cast<TpMouseEvent *>(event);
        onMouseRleaseEvent(mouseEvent);
    }
    else
    {

    }

    return false;
}
