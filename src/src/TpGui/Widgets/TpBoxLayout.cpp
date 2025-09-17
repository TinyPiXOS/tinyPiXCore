#include "TpBoxLayout.h"

struct TpBoxLayoutData
{
    TpBoxLayout::Direction direction;
};

TpBoxLayout::TpBoxLayout(TpChildWidget *parent, Direction direction)
    : TpLayout(parent)
{
    TpBoxLayoutData *layoutData = new TpBoxLayoutData();
    layoutData->direction = direction;

    data_ = layoutData;
}

TpBoxLayout::~TpBoxLayout()
{
    TpBoxLayoutData *layoutData = static_cast<TpBoxLayoutData *>(data_);
    if (layoutData)
    {
        delete layoutData;
        layoutData = nullptr;
        data_ = nullptr;
    }
}

void TpBoxLayout::setDirection(Direction direction)
{
    // 水平布局不能设置垂直布局的排列，反之同样
    TpBoxLayoutData *layoutData = static_cast<TpBoxLayoutData *>(data_);

    if ((layoutData->direction == LeftToRight || layoutData->direction == RightToLeft) && (direction == TopToBottom || direction == BottomToTop))
        return;

    if ((layoutData->direction == TopToBottom || layoutData->direction == BottomToTop) && (direction == LeftToRight || direction == RightToLeft))
        return;

    layoutData->direction = direction;
}

TpBoxLayout::Direction TpBoxLayout::direction() const
{
    TpBoxLayoutData *layoutData = static_cast<TpBoxLayoutData *>(data_);
    return layoutData->direction;
}

void TpBoxLayout::addStretch(int stretch)
{
}

uint32_t TpBoxLayout::rowCount()
{
    TpBoxLayoutData *layoutData = static_cast<TpBoxLayoutData *>(data_);
    if (layoutData->direction == TpBoxLayout::LeftToRight || layoutData->direction == TpBoxLayout::LeftToRight)
        return 1;
    return 0;
}

uint32_t TpBoxLayout::columnCount()
{
    TpBoxLayoutData *layoutData = static_cast<TpBoxLayoutData *>(data_);
    if (layoutData->direction == TpBoxLayout::TopToBottom || layoutData->direction == TpBoxLayout::BottomToTop)
        return 1;
    return 0;
}
