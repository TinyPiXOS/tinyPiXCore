#include "TpRadioButton.h"
#include "TpEvent.h"
#include "TpColors.h"
#include "TpPainter.h"
#include "TpRect.h"
#include "TpFont.h"
#include "TpString.h"
#include <cstring>
#include <list>

struct TpRadioButtonData
{
    bool enableFit;
    bool mouseActive;

    int32_t space;
    TpFont *font;

    TpRadioButtonData()
    {
    }
};

TpRadioButton::TpRadioButton(TpWidget *parent) : TpWidget(parent)
{
    TpRadioButtonData *set = new TpRadioButtonData();

    if (!set)
        return;

    set->enableFit = false;
    set->mouseActive = false;
    set->space = 1;
    set->font = new TpFont();

    setEnableBackGroundImage(false);
    setEnableBackGroundColor(false);
    setEnabledBorderColor(false);

    data_ = set;

    setCheckable(true);

    refreshBaseCss();
}

TpRadioButton::TpRadioButton(const TpString &text, TpWidget *parent)
    : TpWidget(parent)
{
    TpRadioButtonData *set = new TpRadioButtonData();

    if (!set)
        return;

    set->enableFit = false;
    set->mouseActive = false;
    set->space = 1;
    set->font = new TpFont();

    setEnableBackGroundImage(false);
    setEnableBackGroundColor(false);
    setEnabledBorderColor(false);

    data_ = set;

    setText(text);
    setCheckable(true);

    refreshBaseCss();
}

TpRadioButton::~TpRadioButton()
{
    TpRadioButtonData *set = (TpRadioButtonData *)this->data_;

    if (set)
    {
        if (set->font)
        {
            delete set->font;
        }

        delete set;
        set = nullptr;
    }
}

void TpRadioButton::setAutoFit(bool enable)
{
    TpRadioButtonData *set = (TpRadioButtonData *)this->data_;

    if (!set)
        return;

    set->enableFit = enable;
    if (enable)
    {
        TpSize size = set->font->pixelSize();
        this->setRect(this->rect().x(), this->rect().y(), size.width() + size.height() / 2.0 + set->space, size.height());
    }
}

void TpRadioButton::setSpacing(uint32_t space)
{
    TpRadioButtonData *set = (TpRadioButtonData *)this->data_;

    if (set)
    {
        set->space = space;
    }
}

void TpRadioButton::setRect(const int32_t &x, const int32_t &y, const int32_t &w, const int32_t &h)
{
    TpRadioButtonData *set = (TpRadioButtonData *)this->data_;

    if (!set)
        return;

    if (set->enableFit)
    {
        TpSize size = set->font->pixelSize();
        TpWidget::setRect(x, y, size.width() + size.height() / 2.0 + set->space, size.height());
        return;
    }

    TpWidget::setRect(x, y, w, h);
}

void TpRadioButton::setText(const TpString &text)
{
    if (text.empty())
        return;

    TpWidget::setText(text);
    TpRadioButtonData *set = (TpRadioButtonData *)this->data_;

    if (!set)
        return;

    set->font->setText(text);
    if (set->enableFit)
    {
        TpSize size = set->font->pixelSize();
        this->setRect(this->rect().x(), this->rect().y(), size.width() + size.height() / 4.0 + set->space, size.height());
    }
}

TpFont *TpRadioButton::font()
{
    TpRadioButtonData *set = (TpRadioButtonData *)this->data_;
    TpFont *font = nullptr;

    if (set)
    {
        font = set->font;
    }

    return font;
}

bool TpRadioButton::onMousePressEvent(TpMouseEvent *event)
{
    TpWidget::onMousePressEvent(event);

    TpRadioButtonData *set = (TpRadioButtonData *)this->data_;
    if (!set)
        return true;

    set->mouseActive = true;

    update();

    return true;
}

bool TpRadioButton::onMouseRleaseEvent(TpMouseEvent *event)
{
    TpWidget::onMouseRleaseEvent(event);

    TpRadioButtonData *set = (TpRadioButtonData *)this->data_;
    if (!set)
        return true;

    set->mouseActive = false;

    onClicked.emit(checked());

    return true;
}

bool TpRadioButton::onPaintEvent(TpPaintEvent *event)
{
    TpRadioButtonData *set = (TpRadioButtonData *)this->data_;
    if (!set)
        return true;

    TpWidget::onPaintEvent(event);

    tpShared<TpCssData> curCssData = currentStatusCss();

    TpPainter *canvas = event->painter();
    TpSize size = set->font->pixelSize();
    double rad = size.height() / 4.0;

    double cx = (width() - size.width()) / 2.0;
    double cy = (height() - size.height()) / 2.0;
    cx = TP_MAX(cx, rad);

    int32_t lineWidth = TP_MAX(1, rad / 8);

    canvas->setBrush(TpBrush(curCssData->backgroundColor()));
    canvas->pen().setWidth(lineWidth);
    canvas->pen().setColor(curCssData->borderColor());

    canvas->drawEllipse(cx, cy + 9 * size.height() / 16.0, rad, rad);

    if (checked())
    {
        canvas->drawEllipse(cx, cy + 9 * size.height() / 16.0, rad / 2.0, rad / 2.0);
    }

    canvas->drawText(*set->font, cx + rad + set->space, cy);

    return true;
}
