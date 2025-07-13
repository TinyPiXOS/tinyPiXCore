#include "tpBoxLayout.h"

struct tpBoxLayoutData
{
    tpBoxLayout::Direction direction;
};

tpBoxLayout::tpBoxLayout(tpChildWidget *parent, Direction direction)
    : tpLayout(parent)
{
    tpBoxLayoutData *layoutData = new tpBoxLayoutData();
    layoutData->direction = direction;

    data_ = layoutData;
}

tpBoxLayout::~tpBoxLayout()
{
    tpBoxLayoutData *layoutData = static_cast<tpBoxLayoutData *>(data_);
    if (layoutData)
    {
        delete layoutData;
        layoutData = nullptr;
        data_ = nullptr;
    }
}

void tpBoxLayout::setDirection(Direction direction)
{
    // 水平布局不能设置垂直布局的排列，反之同样
    tpBoxLayoutData *layoutData = static_cast<tpBoxLayoutData *>(data_);

    if ((layoutData->direction == LeftToRight || layoutData->direction == RightToLeft) && (direction == TopToBottom || direction == BottomToTop))
        return;

    if ((layoutData->direction == TopToBottom || layoutData->direction == BottomToTop) && (direction == LeftToRight || direction == RightToLeft))
        return;

    layoutData->direction = direction;
}

tpBoxLayout::Direction tpBoxLayout::direction() const
{
    tpBoxLayoutData *layoutData = static_cast<tpBoxLayoutData *>(data_);
    return layoutData->direction;
}

void tpBoxLayout::addStretch(int stretch)
{
}

uint32_t tpBoxLayout::rowCount()
{
    tpBoxLayoutData *layoutData = static_cast<tpBoxLayoutData *>(data_);
    if (layoutData->direction == tpBoxLayout::LeftToRight || layoutData->direction == tpBoxLayout::LeftToRight)
        return 1;
    return 0;
}

uint32_t tpBoxLayout::columnCount()
{
    tpBoxLayoutData *layoutData = static_cast<tpBoxLayoutData *>(data_);
    if (layoutData->direction == tpBoxLayout::TopToBottom || layoutData->direction == tpBoxLayout::BottomToTop)
        return 1;
    return 0;
}
