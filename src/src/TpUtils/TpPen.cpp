#include "TpPen.h"
#include "TpBrush.h"

// 画笔内部数据结构
struct TpPenData
{
    Tp::PenStyle style = Tp::PenStyle::SolidLine; // 画笔样式，默认为实线
    float dashOffset = 0.0f;                      // 虚线偏移量
    int32_t width = 1;                            // 画笔宽度，默认1像素
    Tp::PenCapStyle capStyle = Tp::RoundCap;      // 线帽样式
    Tp::PenJoinStyle joinStyle = Tp::RoundJoin;   // 连接点样式

    TpBrush fillBrush;

    TpPenData()
    {
        fillBrush.setStyle(Tp::SolidPattern);
        fillBrush.setColor(_RGB(0, 0, 0));
    }
};

TpPen::TpPen()
{
    TpPenData *penData = new TpPenData();
    data_ = penData;
}

TpPen::TpPen(const TpColors &color)
{
    TpPenData *penData = new TpPenData();
    penData->fillBrush.setColor(color);
    data_ = penData;
}

TpPen::~TpPen()
{
    TpPenData *penData = static_cast<TpPenData *>(data_);
    if (penData)
    {
        delete penData;
        penData = nullptr;
        data_ = nullptr;
    }
}

Tp::PenStyle TpPen::style() const
{
    TpPenData *penData = static_cast<TpPenData *>(data_);
    return penData->style;
}

void TpPen::setStyle(Tp::PenStyle style)
{
    TpPenData *penData = static_cast<TpPenData *>(data_);
    penData->style = style;
}

float TpPen::dashOffset() const
{
    TpPenData *penData = static_cast<TpPenData *>(data_);
    return penData->dashOffset;
}

void TpPen::setDashOffset(float doffset)
{
    TpPenData *penData = static_cast<TpPenData *>(data_);
    penData->dashOffset = doffset;
}

int32_t TpPen::width() const
{
    TpPenData *penData = static_cast<TpPenData *>(data_);
    return penData->width;
}

void TpPen::setWidth(int32_t width)
{
    TpPenData *penData = static_cast<TpPenData *>(data_);
    if (width >= 0)
    {
        penData->width = width;
    }
}

TpColors TpPen::color() const
{
    TpPenData *penData = static_cast<TpPenData *>(data_);
    return penData->fillBrush.color();
}

void TpPen::setColor(const TpColors &color)
{
    TpPenData *penData = static_cast<TpPenData *>(data_);
    penData->fillBrush.setColor(color);
}

Tp::PenCapStyle TpPen::capStyle() const
{
    TpPenData *penData = static_cast<TpPenData *>(data_);
    return penData->capStyle;
}

void TpPen::setCapStyle(Tp::PenCapStyle pcs)
{
    TpPenData *penData = static_cast<TpPenData *>(data_);
    penData->capStyle = pcs;
}

Tp::PenJoinStyle TpPen::joinStyle() const
{
    TpPenData *penData = static_cast<TpPenData *>(data_);
    return penData->joinStyle;
}

void TpPen::setJoinStyle(Tp::PenJoinStyle pcs)
{
    TpPenData *penData = static_cast<TpPenData *>(data_);
    penData->joinStyle = pcs;
}

void TpPen::setBrush(const TpBrush &brush)
{
    TpPenData *penData = static_cast<TpPenData *>(data_);
    penData->fillBrush = brush;
}

TpBrush TpPen::brush()
{
    TpPenData *penData = static_cast<TpPenData *>(data_);
    return penData->fillBrush;
}