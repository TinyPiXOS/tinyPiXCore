/* liucy has been here，but nothing to see and nothing left ^_^!*/

/*
** Copyright (c) 2007-2021 By Alexander.King.
**
** Permission is hereby granted, free of charge, to any person obtaining a
** copy of this software and/or associated documentation files (the
** "Materials"), to deal in the Materials without restriction, including
** without limitation the rights to use, copy, modify, merge, publish,
** distribute, sublicense, and/or sell copies of the Materials, and to
** permit persons to whom the Materials are furnished to do so, subject to
** the following conditions:
**
** The above copyright notice and this permission notice shall be included
** in all copies or substantial portions of the Materials.
**
** THE MATERIALS ARE PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
** EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
** MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.
** IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY
** CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT,
** TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE
** MATERIALS OR THE USE OR OTHER DEALINGS IN THE MATERIALS.
*/
#include "TpProgressBar.h"
#include "TpEvent.h"
#include "TpPainter.h"
#include "TpColors.h"
#include "TpRange.h"
#include "TpFont.h"
#include "TpRect.h"
#include "TpApp.h"

typedef struct
{
    TpRange range;
    TpRange rectRange;
    TpFont *font;
    int32_t bkColor;
    int32_t topLeftColor;
    int32_t bottomRightColor;
} ItpProgressSet;

TpProgressBar::TpProgressBar(TpChildWidget *parent) : TpChildWidget(parent)
{
    ItpProgressSet *set = new ItpProgressSet();

    if (set)
    {
        set->range.setRange(0, 100);
        set->range.setPercent(0);
        set->rectRange.setRange(0, 100);
        set->rectRange.setPercent(0);
        set->bkColor = TpColors::Green;
        set->topLeftColor = TpColors::Black;
        set->bottomRightColor = TpColors::LightSlateGray;

        set->font = new TpFont();

        if (set->font)
        {
            set->font->setFontForeColor(TpColors::Black);
        }

        this->progressSet = set;
    }
}

TpProgressBar::~TpProgressBar()
{
    ItpProgressSet *set = (ItpProgressSet *)this->progressSet;

    if (set)
    {
        if (set->font)
        {
            delete set->font;
        }

        delete set;
    }
}

void TpProgressBar::setRangeFrom(int32_t min, int32_t max)
{
    ItpProgressSet *set = (ItpProgressSet *)this->progressSet;

    if (set)
    {
        set->range.setRange(min, max);
    }
}

void TpProgressBar::setPosition(int32_t pos)
{
    ItpProgressSet *set = (ItpProgressSet *)this->progressSet;

    if (set)
    {
        set->range.setPosition(pos);
        double percent = set->range.percent();
        set->rectRange.setPercent(percent);
        this->update();
    }
}

void TpProgressBar::setPercent(double percent)
{
    ItpProgressSet *set = (ItpProgressSet *)this->progressSet;

    if (set)
    {
        set->range.setPercent(percent);
        double percent = set->range.percent();
        set->rectRange.setPercent(percent);
        this->update();
    }
}

int32_t TpProgressBar::position()
{
    ItpProgressSet *set = (ItpProgressSet *)this->progressSet;
    int32_t pos = 0;

    if (set)
    {
        pos = set->range.position();
    }

    return pos;
}

double TpProgressBar::percent()
{
    ItpProgressSet *set = (ItpProgressSet *)this->progressSet;
    double percent = 0;

    if (set)
    {
        percent = set->range.percent();
    }

    return percent;
}

TpFont *TpProgressBar::font()
{
    ItpProgressSet *set = (ItpProgressSet *)this->progressSet;
    TpFont *font = nullptr;

    if (set)
    {
        font = set->font;
    }

    return font;
}

void TpProgressBar::setProgressBackColor(uint32_t bkColor)
{
    ItpProgressSet *set = (ItpProgressSet *)this->progressSet;

    if (set)
    {
        set->bkColor = bkColor;
    }
}

void TpProgressBar::setProgressBackColor(TpColors &color)
{
    this->setProgressBackColor(color.rgba());
}

void TpProgressBar::setTopLeftLineColor(uint32_t color)
{
    ItpProgressSet *set = (ItpProgressSet *)this->progressSet;

    if (set)
    {
        set->topLeftColor = color;
    }
}

void TpProgressBar::setTopLeftLineColor(TpColors &color)
{
    this->setTopLeftLineColor(color.rgba());
}

void TpProgressBar::setRightBottomLineColor(uint32_t color)
{
    ItpProgressSet *set = (ItpProgressSet *)this->progressSet;

    if (set)
    {
        set->bottomRightColor = color;
    }
}

void TpProgressBar::setRightBottomLineColor(TpColors &color)
{
    this->setRightBottomLineColor(color.rgba());
}

void TpProgressBar::setRect(const TpRect &rect)
{
    this->setRect(rect.x(), rect.y(), rect.width(), rect.height());
}

void TpProgressBar::setRect(int32_t x, int32_t y, uint32_t w, uint32_t h)
{
    ItpProgressSet *set = (ItpProgressSet *)this->progressSet;

    if (set)
    {
        double percent = set->rectRange.percent();
        set->rectRange.setRange(0, w - 4);
        set->rectRange.setPercent(percent);
        TpChildWidget::setRect(x, y, w, h);

        int32_t nWH = TP_MIN(w, h);
        // int32_t fontSize = (int32_t)(nWH * TpApp::Inst()->appConfigSet()->ratio);
        int32_t fontSize = 15;

        fontSize = TP_MAX(1, fontSize);
        set->font->setFontSize(fontSize);
    }
}

bool TpProgressBar::onPaintEvent(TpPaintEvent *event)
{
    ItpProgressSet *set = (ItpProgressSet *)this->progressSet;
    bool ret = false;

    if (set)
    {
        ret = TpChildWidget::onPaintEvent(event);

        if (ret)
        {
            TpRect rect = event->rect();
            TpPainter *canvas = event->canvas();

            uint8_t alpha1 = mapAlpha((uint8_t)(set->topLeftColor & 0x000000ff), this->alpha());
            uint8_t alpha2 = mapAlpha((uint8_t)(set->bottomRightColor & 0x000000ff), this->alpha());
            uint8_t alpha3 = mapAlpha((uint8_t)(set->bkColor & 0x000000ff), this->alpha());

            int32_t x0 = 0, y0 = 0, x1 = rect.width() - 1, y1 = rect.height() - 1;

            canvas->setPen((set->topLeftColor & 0xffffff00) | alpha1);
            canvas->drawLine(x0, y0, x0, y1);
            canvas->drawLine(x0, y0, x1, y0);

            canvas->setPen((set->bottomRightColor & 0xffffff00) | alpha2);
            canvas->drawLine(x0, y1, x1, y1);
            canvas->drawLine(x1, y0, x1, y1);

            int32_t position = set->rectRange.position();

            if (position)
            {
                canvas->setBrush(TpBrush((set->bkColor & 0xffffff00) | alpha3));
                canvas->drawRect(x0 + 1, y0 + 1, position + 1, y1 - 1 - y0 - 1);
            }

            int32_t percent = (int32_t)(set->rectRange.percent() * 100);
            TpString perString = TpString::number(percent);
            perString += "%";
            set->font->setText(perString.c_str());
            TpSize size = set->font->pixelSize();

            int32_t cx = (rect.width() - size.width()) / 2.0, cy = (rect.height() - size.height()) / 2.0;
            canvas->drawText(*set->font, cx, cy);
        }
    }

    return ret;
}
