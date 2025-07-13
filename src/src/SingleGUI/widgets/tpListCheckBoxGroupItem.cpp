#include "tpListCheckBoxGroupItem.h"
#include "tpLabel.h"
#include "tpFont.h"
#include "tpSurface.h"
#include "tpEvent.h"

struct tpListCheckBoxGroupItemData
{
    tpLabel *titleLabel;
    tpLabel *selectIconLabel;
};

tpListCheckBoxGroupItem::tpListCheckBoxGroupItem(tpChildWidget *parent)
    : tpChildWidget(parent)
{
    tpListCheckBoxGroupItemData *itemData = new tpListCheckBoxGroupItemData();

    itemData->titleLabel = new tpLabel(this);
    itemData->titleLabel->installEventFilter(this);

    itemData->selectIconLabel = new tpLabel(this);
    itemData->selectIconLabel->installEventFilter(this);

    tpShared<tpSurface> selectIcon = tpMakeShared<tpSurface>();
    selectIcon->fromFile("/usr/res/tinyPiX/勾选.png");
    itemData->selectIconLabel->setBackGroundImage(selectIcon);

    tpShared<tpCssData> normalCss = currentStatusCss();
    itemData->titleLabel->font()->setText("tpMenu");
    itemData->titleLabel->font()->setFontSize(normalCss->fontSize());
    itemData->titleLabel->font()->setFontColor(normalCss->color(), normalCss->color());

    itemData->selectIconLabel->setFixedSize(itemData->titleLabel->font()->pixelHeight(), itemData->titleLabel->font()->pixelHeight());

    data_ = itemData;

    refreshBaseCss();
    setCheckable(true);
}

tpListCheckBoxGroupItem::tpListCheckBoxGroupItem(const tpString &text, tpChildWidget *parent)
    : tpChildWidget(parent)
{
    tpListCheckBoxGroupItemData *itemData = new tpListCheckBoxGroupItemData();

    itemData->titleLabel = new tpLabel(this);
    itemData->titleLabel->installEventFilter(this);

    itemData->selectIconLabel = new tpLabel(this);
    itemData->selectIconLabel->installEventFilter(this);

    tpShared<tpSurface> selectIcon = tpMakeShared<tpSurface>();
    selectIcon->fromFile("/usr/res/tinyPiX/勾选.png");
    itemData->selectIconLabel->setBackGroundImage(selectIcon);

    tpShared<tpCssData> normalCss = currentStatusCss();
    itemData->titleLabel->font()->setText("tpMenu");
    itemData->titleLabel->font()->setFontSize(normalCss->fontSize());
    itemData->titleLabel->font()->setFontColor(normalCss->color(), normalCss->color());

    itemData->selectIconLabel->setFixedSize(itemData->titleLabel->font()->pixelHeight(), itemData->titleLabel->font()->pixelHeight());

    data_ = itemData;

    refreshBaseCss();
    setCheckable(true);

    setText(text);
}

tpListCheckBoxGroupItem::~tpListCheckBoxGroupItem()
{
    tpListCheckBoxGroupItemData *itemData = static_cast<tpListCheckBoxGroupItemData *>(data_);
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

tpString tpListCheckBoxGroupItem::text()
{
    tpListCheckBoxGroupItemData *itemData = static_cast<tpListCheckBoxGroupItemData *>(data_);

    return itemData->titleLabel->text();
}

void tpListCheckBoxGroupItem::setText(const tpString &text)
{
    tpListCheckBoxGroupItemData *itemData = static_cast<tpListCheckBoxGroupItemData *>(data_);
    itemData->titleLabel->setText(text);
}

tpVariant tpListCheckBoxGroupItem::data()
{
    return property("tpListCheckBoxGroupItemData");
}

void tpListCheckBoxGroupItem::setData(const tpVariant &data)
{
    setProperty("tpListCheckBoxGroupItemData", data);
}

bool tpListCheckBoxGroupItem::onPaintEvent(tpObjectPaintEvent *event)
{
    tpChildWidget::onPaintEvent(event);

    tpListCheckBoxGroupItemData *itemData = static_cast<tpListCheckBoxGroupItemData *>(data_);

    tpShared<tpCssData> normalCss = currentStatusCss();

    itemData->titleLabel->font()->setFontSize(normalCss->fontSize());
    itemData->titleLabel->font()->setFontColor(normalCss->color(), normalCss->color());

    itemData->selectIconLabel->setVisible(checked());

    return true;
}

bool tpListCheckBoxGroupItem::onMousePressEvent(tpMouseEvent *event)
{
    tpChildWidget::onMousePressEvent(event);

    return true;
}

bool tpListCheckBoxGroupItem::onMouseRleaseEvent(tpMouseEvent *event)
{
    tpChildWidget::onMouseRleaseEvent(event);

    onStatusChanged.emit(this);

    return true;
}

bool tpListCheckBoxGroupItem::onResizeEvent(tpObjectResizeEvent *event)
{
    tpChildWidget::onResizeEvent(event);

    tpListCheckBoxGroupItemData *itemData = static_cast<tpListCheckBoxGroupItemData *>(data_);
    tpShared<tpCssData> normalCss = currentStatusCss();

    uint32_t titleY = (height() - itemData->titleLabel->font()->pixelHeight()) / 2.0;
    itemData->titleLabel->move(normalCss->paddingLeft(), titleY);

    itemData->selectIconLabel->move(width() - normalCss->paddingRight() - itemData->titleLabel->font()->pixelHeight(), titleY);

    return true;
}

bool tpListCheckBoxGroupItem::eventFilter(tpObject *watched, tpEvent *event)
{
    if (event->eventType() == tpEvent::EVENT_MOUSE_PRESS_TYPE)
    {
        tpMouseEvent *mouseEvent = dynamic_cast<tpMouseEvent *>(event);
        onMousePressEvent(mouseEvent);
    }
    else if (event->eventType() == tpEvent::EVENT_MOUSE_RELEASE_TYPE)
    {
        tpMouseEvent *mouseEvent = dynamic_cast<tpMouseEvent *>(event);
        onMouseRleaseEvent(mouseEvent);
    }
    else
    {

    }

    return false;
}
