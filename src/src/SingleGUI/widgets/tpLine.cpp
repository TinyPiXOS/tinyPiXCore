#include "tpLine.h"
#include "tpCanvas.h"
#include "tpEvent.h"

struct tpLineData
{
    tinyPiX::AlignmentFlag alignmentType = tinyPiX::AlignCenter;
    tpLine::LineType lineType = tpLine::HLine;

    uint32_t length = 0;
    float lengthPercent = 0;
    bool isPercent = false;

    int32_t color = _RGB(190, 196, 202);

    // 线宽
    int32_t width = 1;

    bool isAuto = true;
};

tpLine::tpLine(tpChildWidget *parent)
    : tpChildWidget(parent)
{
    data_ = new tpLineData();
}

tpLine::~tpLine()
{
    tpLineData *lineData = static_cast<tpLineData *>(data_);
    if (lineData)
    {
        delete lineData;
        lineData = nullptr;
        data_ = nullptr;
    }
}

void tpLine::setLineType(const LineType &type)
{
    tpLineData *lineData = static_cast<tpLineData *>(data_);
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

void tpLine::setLineLength(const uint32_t &length)
{
    tpLineData *lineData = static_cast<tpLineData *>(data_);
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

void tpLine::setLinePercentLength(const float &percent)
{
    tpLineData *lineData = static_cast<tpLineData *>(data_);
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

void tpLine::setLineLengthMode(const bool &isAuto)
{
    tpLineData *lineData = static_cast<tpLineData *>(data_);
    lineData->isAuto = isAuto;

    if (isAuto)
    {
        lineData->isPercent = false;
    }

    update();
}

void tpLine::setAlign(const tinyPiX::AlignmentFlag &alignment)
{
    tpLineData *lineData = static_cast<tpLineData *>(data_);
    lineData->alignmentType = alignment;
}

void tpLine::setColor(const int32_t &color)
{
    tpLineData *lineData = static_cast<tpLineData *>(data_);
    lineData->color = color;
}

void tpLine::setLineWidth(const uint32_t &width)
{
    tpLineData *lineData = static_cast<tpLineData *>(data_);
    lineData->width = width;
}

bool tpLine::onPaintEvent(tpObjectPaintEvent *event)
{
    tpChildWidget::onPaintEvent(event);

    tpLineData *lineData = static_cast<tpLineData *>(data_);

    // 默认绘制在水平垂直居中位置
    int32_t startX = 0;
    int32_t startY = 0;

    uint32_t length = lineData->length;

    if (lineData->lineType == tpLine::HLine)
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
        if (lineData->lineType == tpLine::HLine)
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
        if (lineData->alignmentType == tinyPiX::AlignCenter)
        {
            if (lineData->lineType == tpLine::HLine)
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
            if (lineData->alignmentType & tinyPiX::AlignLeft)
            {
                startX = 0;
            }
            else if (lineData->alignmentType & tinyPiX::AlignRight)
            {
                if (lineData->lineType == tpLine::HLine)
                {
                    startX = width() - lineData->length;
                }
                else
                {
                    startX = width() - lineData->width;
                }
            }
            else if (lineData->alignmentType & tinyPiX::AlignHCenter)
            {
                if (lineData->lineType == tpLine::HLine)
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
                if (lineData->lineType == tpLine::HLine)
                {
                    startX = (width() - lineData->length) / 2.0;
                }
                else
                {
                    startX = (width() - lineData->width) / 2.0;
                }
            }

            if (lineData->alignmentType & tinyPiX::AlignTop)
            {
                startY = 0;
            }
            else if (lineData->alignmentType & tinyPiX::AlignBottom)
            {
                if (lineData->lineType == tpLine::HLine)
                {
                    startY = height() - lineData->width;
                }
                else
                {
                    startY = height() - lineData->length;
                }
            }
            else if (lineData->alignmentType & tinyPiX::AlignVCenter)
            {
                if (lineData->lineType == tpLine::HLine)
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
                if (lineData->lineType == tpLine::HLine)
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

    tpCanvas *paintCanvas = event->canvas();
    // std::cout << "lineData->width " << lineData->width << std::endl;
    if (lineData->lineType == tpLine::HLine)
    {
        paintCanvas->hline(startX, startX + length, startY, lineData->color, lineData->width);
    }
    else
    {
        paintCanvas->vline(startX, startY, startY + length, lineData->color, lineData->width);
    }

    return true;
}

bool tpLine::onResizeEvent(tpObjectResizeEvent *event)
{
    tpChildWidget::onResizeEvent(event);
    tpLineData *lineData = static_cast<tpLineData *>(data_);

    if (lineData->isPercent)
    {
        if (lineData->lineType == tpLine::HLine)
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

void tpLine::onThemeChangeEvent(tpThemeChangeEvent *event)
{
}
