#include "tpRadioButton.h"
#include "tpEvent.h"
#include "tpColors.h"
#include "TpCanvas.h"
#include "tpRect.h"
#include "tpFont.h"
#include "tpString.h"
#include <cstring>
#include <list>

struct tpRadioButtonData
{
    bool enableFit;
    bool mouseActive;

    int32_t space;
    tpFont *font;

    tpRadioButtonData()
    {
    }
};

tpRadioButton::tpRadioButton(tpChildWidget *parent) : tpChildWidget(parent)
{
    tpRadioButtonData *set = new tpRadioButtonData();

    if (!set)
        return;

    set->enableFit = false;
    set->mouseActive = false;
    set->space = 1;
    set->font = new tpFont();

    setEnableBackGroundImage(false);
    setEnableBackGroundColor(false);
    setEnabledBorderColor(false);

    data_ = set;

    setCheckable(true);

    refreshBaseCss();
}

tpRadioButton::tpRadioButton(const tpString &text, tpChildWidget *parent)
    : tpChildWidget(parent)
{
    tpRadioButtonData *set = new tpRadioButtonData();

    if (!set)
        return;

    set->enableFit = false;
    set->mouseActive = false;
    set->space = 1;
    set->font = new tpFont();

    setEnableBackGroundImage(false);
    setEnableBackGroundColor(false);
    setEnabledBorderColor(false);

    data_ = set;

    setText(text);
    setCheckable(true);

    refreshBaseCss();
}

tpRadioButton::~tpRadioButton()
{
    tpRadioButtonData *set = (tpRadioButtonData *)this->data_;

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

void tpRadioButton::setAutoFit(bool enable)
{
    tpRadioButtonData *set = (tpRadioButtonData *)this->data_;

    if (!set)
        return;

    set->enableFit = enable;
    if (enable)
    {
        ItpSize size = set->font->pixelSize();
        this->setRect(this->rect().x, this->rect().y, size.w + size.h / 2 + set->space, size.h);
    }
}

void tpRadioButton::setSpacing(uint32_t space)
{
    tpRadioButtonData *set = (tpRadioButtonData *)this->data_;

    if (set)
    {
        set->space = space;
    }
}

void tpRadioButton::setRect(const int32_t &x, const int32_t &y, const uint32_t &w, const uint32_t &h)
{
    tpRadioButtonData *set = (tpRadioButtonData *)this->data_;

    if (!set)
        return;

    if (set->enableFit)
    {
        ItpSize size = set->font->pixelSize();
        tpChildWidget::setRect(x, y, size.w + size.h / 2 + set->space, size.h);
        return;
    }

    tpChildWidget::setRect(x, y, w, h);
}

void tpRadioButton::setText(const tpString &text)
{
    if (text.empty())
        return;

    tpChildWidget::setText(text);
    tpRadioButtonData *set = (tpRadioButtonData *)this->data_;

    if (!set)
        return;

    set->font->setText(text);
    if (set->enableFit)
    {
        ItpSize size = set->font->pixelSize();
        this->setRect(this->rect().x, this->rect().y, size.w + size.h / 4 + set->space, size.h);
    }
}

tpFont *tpRadioButton::font()
{
    tpRadioButtonData *set = (tpRadioButtonData *)this->data_;
    tpFont *font = nullptr;

    if (set)
    {
        font = set->font;
    }

    return font;
}

bool tpRadioButton::onMousePressEvent(tpMouseEvent *event)
{
    tpChildWidget::onMousePressEvent(event);

    tpRadioButtonData *set = (tpRadioButtonData *)this->data_;
    if (!set)
        return true;

    set->mouseActive = true;

    update();

    return true;
}

bool tpRadioButton::onMouseRleaseEvent(tpMouseEvent *event)
{
    tpChildWidget::onMouseRleaseEvent(event);

    tpRadioButtonData *set = (tpRadioButtonData *)this->data_;
    if (!set)
        return true;

    set->mouseActive = false;

    onClicked.emit(checked());

    return true;
}

bool tpRadioButton::onPaintEvent(tpObjectPaintEvent *event)
{
    tpRadioButtonData *set = (tpRadioButtonData *)this->data_;
    if (!set)
        return true;

    tpChildWidget::onPaintEvent(event);

    tpShared<tpCssData> curCssData = currentStatusCss();

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
