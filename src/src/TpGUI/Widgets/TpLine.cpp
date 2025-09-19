#include "TpLine.h"
#include "TpPainter.h"
#include "TpEvent.h"

struct TpLineData
{
    Tp::AlignmentFlag alignmentType = Tp::AlignCenter;
    TpLine::LineType lineType = TpLine::HLine;

    uint32_t length = 0;
    float lengthPercent = 0;
    bool isPercent = false;

    int32_t color = _RGB(190, 196, 202);

    // 线宽
    int32_t width = 1;

    bool isAuto = true;
};

TpLine::TpLine(TpChildWidget *parent)
    : TpChildWidget(parent)
{
    data_ = new TpLineData();
}

TpLine::~TpLine()
{
    TpLineData *lineData = static_cast<TpLineData *>(data_);
    if (lineData)
    {
        delete lineData;
        lineData = nullptr;
        data_ = nullptr;
    }
}

void TpLine::setLineType(const LineType &type)
{
    TpLineData *lineData = static_cast<TpLineData *>(data_);
    lineData->lineType = type;

    if (lineData->lineType == HLine)
    {
        setMinumumWidth(lineData->length);
        setMinumumHeight(30);
    }
    else
    {
        setMinumumWidth(30);
        setMinumumHeight(lineData->length);
    }
}

void TpLine::setLineLength(const uint32_t &length)
{
    TpLineData *lineData = static_cast<TpLineData *>(data_);
    lineData->length = length;

    if (lineData->lineType == HLine)
    {
        setMinumumHeight(lineData->width);
    }
    else
    {
        setMinumumWidth(lineData->width);
    }

    lineData->isAuto = false;
    lineData->isPercent = false;

    update();
}

void TpLine::setLinePercentLength(const float &percent)
{
    TpLineData *lineData = static_cast<TpLineData *>(data_);
    lineData->lengthPercent = percent;

    if (lineData->lineType == HLine)
    {
        setMinumumHeight(lineData->width);
    }
    else
    {
        setMinumumWidth(lineData->width);
    }

    lineData->isAuto = false;
    lineData->isPercent = true;

    update();
}

void TpLine::setLineLengthMode(const bool &isAuto)
{
    TpLineData *lineData = static_cast<TpLineData *>(data_);
    lineData->isAuto = isAuto;

    if (isAuto)
    {
        lineData->isPercent = false;
    }

    update();
}

void TpLine::setAlign(const Tp::AlignmentFlag &alignment)
{
    TpLineData *lineData = static_cast<TpLineData *>(data_);
    lineData->alignmentType = alignment;
}

void TpLine::setColor(const int32_t &color)
{
    TpLineData *lineData = static_cast<TpLineData *>(data_);
    lineData->color = color;
}

void TpLine::setLineWidth(const uint32_t &width)
{
    TpLineData *lineData = static_cast<TpLineData *>(data_);
    lineData->width = width;
}

bool TpLine::onPaintEvent(TpPaintEvent *event)
{
    TpChildWidget::onPaintEvent(event);

    TpLineData *lineData = static_cast<TpLineData *>(data_);

    // 默认绘制在水平垂直居中位置
    int32_t startX = 0;
    int32_t startY = 0;

    uint32_t length = lineData->length;

    if (lineData->lineType == TpLine::HLine)
    {
        if (length > width())
            length = width();
    }
    else
    {
        if (length > height())
            length = height();
    }

    if (lineData->isAuto)
    {
        if (lineData->lineType == TpLine::HLine)
        {
            startX = 0;
            startY = (height() - lineData->width) / 2.0;
            length = width();
        }
        else
        {
            startX = (width() - lineData->width) / 2.0;
            startY = 0;
            length = height();
        }
    }
    else
    {
        if (lineData->alignmentType == Tp::AlignCenter)
        {
            if (lineData->lineType == TpLine::HLine)
            {
                startX = (width() - lineData->length) / 2.0;
                startY = (height() - lineData->width) / 2.0;
            }
            else
            {
                startX = (width() - lineData->width) / 2.0;
                startY = (height() - lineData->length) / 2.0;
            }
        }
        else
        {
            if (lineData->alignmentType & Tp::AlignLeft)
            {
                startX = 0;
            }
            else if (lineData->alignmentType & Tp::AlignRight)
            {
                if (lineData->lineType == TpLine::HLine)
                {
                    startX = width() - lineData->length;
                }
                else
                {
                    startX = width() - lineData->width;
                }
            }
            else if (lineData->alignmentType & Tp::AlignHCenter)
            {
                if (lineData->lineType == TpLine::HLine)
                {
                    startX = (width() - lineData->length) / 2.0;
                }
                else
                {
                    startX = (width() - lineData->width) / 2.0;
                }
            }
            else
            {
                if (lineData->lineType == TpLine::HLine)
                {
                    startX = (width() - lineData->length) / 2.0;
                }
                else
                {
                    startX = (width() - lineData->width) / 2.0;
                }
            }

            if (lineData->alignmentType & Tp::AlignTop)
            {
                startY = 0;
            }
            else if (lineData->alignmentType & Tp::AlignBottom)
            {
                if (lineData->lineType == TpLine::HLine)
                {
                    startY = height() - lineData->width;
                }
                else
                {
                    startY = height() - lineData->length;
                }
            }
            else if (lineData->alignmentType & Tp::AlignVCenter)
            {
                if (lineData->lineType == TpLine::HLine)
                {
                    startY = (height() - lineData->width) / 2.0;
                }
                else
                {
                    startY = (height() - lineData->length) / 2.0;
                }
            }
            else
            {
                if (lineData->lineType == TpLine::HLine)
                {
                    startY = (height() - lineData->width) / 2.0;
                }
                else
                {
                    startY = (height() - lineData->length) / 2.0;
                }
            }
        }
    }

    TpPainter *paintCanvas = event->painter();
    paintCanvas->pen().setColor(lineData->color);
    paintCanvas->pen().setWidth(lineData->width);

    if (lineData->lineType == TpLine::HLine)
    {
        paintCanvas->drawHLine(startX, startX + length, startY);
    }
    else
    {
        paintCanvas->drawVLine(startX, startY, startY + length);
    }

    return true;
}

bool TpLine::onResizeEvent(TpResizeEvent *event)
{
    TpChildWidget::onResizeEvent(event);
    TpLineData *lineData = static_cast<TpLineData *>(data_);

    if (lineData->isPercent)
    {
        if (lineData->lineType == TpLine::HLine)
        {
            lineData->length = width() * lineData->lengthPercent;
        }
        else
        {
            lineData->length = height() * lineData->lengthPercent;
        }
    }

    return true;
}

void TpLine::onThemeChangeEvent(TpThemeChangeEvent *event)
{
}
