#include "TpLabel.h"
#include "TpPainter.h"
#include "TpFont.h"
#include "TpEvent.h"
#include "TpRect.h"
#include "SystemInfo/TpDisplay.h"

#include <cstring>
#include <iostream>

struct TpLabelData
{
    TpString text = "";

    TpFont *font = nullptr;
    bool enableFit;

    int textSpacing = 2;

    bool wrap = false;
};

TpLabel::TpLabel(TpWidget *parent)
    : TpWidget(parent)
{
    TpLabelData *labelData = new TpLabelData();

    labelData->font = new TpFont();
    labelData->font->setAlign(Tp::AlignLeft);
    labelData->enableFit = false;

    setEnableBackGroundImage(false);
    setEnableBackGroundColor(false);
    data_ = labelData;
}

TpLabel::TpLabel(const TpString &text, TpWidget *parent)
    : TpWidget(parent)
{
    TpLabelData *labelData = new TpLabelData();

    labelData->font = new TpFont();
    labelData->font->setAlign(Tp::AlignLeft);
    labelData->enableFit = false;

    setEnableBackGroundImage(false);
    setEnableBackGroundColor(false);
    data_ = labelData;

    setText(text);
}

TpLabel::~TpLabel()
{
    TpLabelData *labelData = static_cast<TpLabelData *>(data_);

    if (labelData->font)
        delete labelData->font;

    delete labelData;
    data_ = nullptr;
}

void TpLabel::setAutoFit(bool enable)
{
    TpLabelData *labelData = static_cast<TpLabelData *>(data_);

    labelData->enableFit = enable;
    if (enable)
    {
        TpSize size = labelData->font->pixelSize();
        this->setRect(this->rect().x(), this->rect().y(), size.width(), size.height());
    }
}

void TpLabel::setText(const TpString &text)
{
    if (text.empty())
        return;

    TpLabelData *labelData = static_cast<TpLabelData *>(data_);
    if (!labelData)
        return;

    labelData->text = text;
    labelData->font->setText(text);

    if (labelData->enableFit)
    {
        TpSize size = labelData->font->pixelSize();
        setRect(rect().x(), this->rect().y(), size.width(), size.height());
    }

    // 根据文本宽度调整最小宽度,只有没有设置固定宽度情况下才动态调整
    if (!isFixedWidth())
    {
        setMinumumWidth(labelData->font->pixelWidth());

        // if (labelData->font->pixelWidth() > TpDisplay::dp2Px(131))
        // {
        //     setMinumumWidth(TpDisplay::dp2Px(131));
        // }
        // else
        // {
        //     setMinumumWidth(labelData->font->pixelWidth());
        // }
    }
    if (!isFixedHeight())
    {
        setMinumumHeight(labelData->font->pixelHeight());
    }

    update();
}

TpString TpLabel::text() const
{
    TpLabelData *labelData = static_cast<TpLabelData *>(data_);
    if (!labelData)
        return TpString();
    return labelData->text;
}

void TpLabel::setWordWrap(bool wrap)
{
    TpLabelData *labelData = static_cast<TpLabelData *>(data_);
    labelData->wrap = wrap;
    labelData->font->setWrap(wrap);
}

void TpLabel::setRect(const TpRect &rect)
{
    this->setRect(rect.x(), rect.y(), rect.width(), rect.height());
}

void TpLabel::setRect(int32_t x, int32_t y, int32_t w, int32_t h)
{
    TpLabelData *set = (TpLabelData *)this->data_;

    if (set)
    {
        if (set->enableFit)
        {
            TpSize size = set->font->pixelSize();
            TpWidget::setRect(x, y, size.width(), size.height());
            return;
        }

        TpWidget::setRect(x, y, w, h);
    }
}

TpFont *TpLabel::font()
{
    TpLabelData *set = (TpLabelData *)this->data_;
    TpFont *font = nullptr;

    if (set)
    {
        font = set->font;
    }

    return font;
}

void TpLabel::setAlign(const Tp::Alignment align)
{
    TpLabelData *labelData = static_cast<TpLabelData *>(data_);
    labelData->font->setAlign(align);
}

bool TpLabel::onPaintEvent(TpPaintEvent *event)
{
    TpLabelData *labelData = static_cast<TpLabelData *>(data_);

    TpWidget::onPaintEvent(event);

    if (labelData->text.empty())
        return true;

    // // 设置CSS
    // tpShared<TpCssData> curCssData = currentStatusCss();
    // set->font->setFontColor(curCssData->color());
    // set->font->setFontSize(curCssData->fontSize());

    labelData->font->setText(text());

    // 根据文本宽度调整最小宽度,只有没有设置固定宽度情况下才动态调整
    if (!isFixedWidth())
    {
        setMinumumWidth(labelData->font->pixelWidth());

        // if (labelData->font->pixelWidth() > TpDisplay::dp2Px(131))
        // {
        //     setMinumumWidth(TpDisplay::dp2Px(131));
        // }
        // else
        // {
        //     setMinumumWidth(labelData->font->pixelWidth());
        // }
    }
    // 下边计算完行数，设置最小高度
    if (!isFixedHeight())
    {
        setMinumumHeight(labelData->font->pixelHeight());
    }

    TpPainter *canvas = event->painter();

    TpSize size = labelData->font->pixelSize();
    canvas->drawText(*labelData->font, 0, 0);

    return true;
}

bool TpLabel::onResizeEvent(TpResizeEvent *event)
{
    TpWidget::onResizeEvent(event);

    TpLabelData *labelData = static_cast<TpLabelData *>(data_);
    labelData->font->setLayout(width(), height());

    return true;
}
