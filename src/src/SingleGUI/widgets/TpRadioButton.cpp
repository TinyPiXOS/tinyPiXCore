#include "TpRadioButton.h"
#include "TpEvent.h"
#include "TpColors.h"
#include "TpCanvas.h"
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

TpRadioButton::TpRadioButton(TpChildWidget *parent) : TpChildWidget(parent)
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

TpRadioButton::TpRadioButton(const TpString &text, TpChildWidget *parent)
    : TpChildWidget(parent)
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
        ItpSize size = set->font->pixelSize();
        this->setRect(this->rect().x, this->rect().y, size.w + size.h / 2 + set->space, size.h);
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

void TpRadioButton::setRect(const int32_t &x, const int32_t &y, const uint32_t &w, const uint32_t &h)
{
    TpRadioButtonData *set = (TpRadioButtonData *)this->data_;

    if (!set)
        return;

    if (set->enableFit)
    {
        ItpSize size = set->font->pixelSize();
        TpChildWidget::setRect(x, y, size.w + size.h / 2 + set->space, size.h);
        return;
    }

    TpChildWidget::setRect(x, y, w, h);
}

void TpRadioButton::setText(const TpString &text)
{
    if (text.empty())
        return;

    TpChildWidget::setText(text);
    TpRadioButtonData *set = (TpRadioButtonData *)this->data_;

    if (!set)
        return;

    set->font->setText(text);
    if (set->enableFit)
    {
        ItpSize size = set->font->pixelSize();
        this->setRect(this->rect().x, this->rect().y, size.w + size.h / 4 + set->space, size.h);
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
    TpChildWidget::onMousePressEvent(event);

    TpRadioButtonData *set = (TpRadioButtonData *)this->data_;
    if (!set)
        return true;

    set->mouseActive = true;

    update();

    return true;
}

bool TpRadioButton::onMouseRleaseEvent(TpMouseEvent *event)
{
    TpChildWidget::onMouseRleaseEvent(event);

    TpRadioButtonData *set = (TpRadioButtonData *)this->data_;
    if (!set)
        return true;

    set->mouseActive = false;

    onClicked.emit(checked());

    return true;
}

bool TpRadioButton::onPaintEvent(TpObjectPaintEvent *event)
{
    TpRadioButtonData *set = (TpRadioButtonData *)this->data_;
    if (!set)
        return true;

    TpChildWidget::onPaintEvent(event);

    tpShared<TpCssData> curCssData = currentStatusCss();

    TpCanvas *canvas = event->canvas();
    ItpSize size = set->font->pixelSize();
    double rad = size.h / 4.0;

    double cx = (width() - size.w) / 2.0;
    double cy = (height() - size.h) / 2.0;
    cx = TP_MAX(cx, rad);

    int32_t lineWidth = TP_MAX(1, rad / 8);

    canvas->circle(cx, cy + 9 * size.h / 16.0, rad, curCssData->borderColor(), lineWidth);

    if (checked())
    {
        canvas->filledCircle(cx, cy + 9 * size.h / 16.0, rad / 2.0, curCssData->backgroundColor());
    }

    canvas->renderText(*set->font, cx + rad + set->space, cy);

    return true;
}
