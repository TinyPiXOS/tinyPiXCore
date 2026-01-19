#include "TpRange.h"

struct TpRangeData
{
    int64_t min = 0;
    int64_t max = 0;
    double percent = 0;
    int64_t value = 0;

    TpRangeData()
    {
    }
};

TpRange::TpRange(int64_t min, int64_t max)
{
    TpRangeData *rangeData = new TpRangeData();
    data_ = rangeData;
    setRange(min, max);
}

TpRange::~TpRange()
{
    TpRangeData *rangeData = static_cast<TpRangeData *>(data_);
    if (rangeData)
    {
        delete rangeData;
        rangeData = nullptr;
        data_ = nullptr;
    }
}

void TpRange::setRange(int64_t min, int64_t max)
{
    TpRangeData *rangeData = static_cast<TpRangeData *>(data_);
    rangeData->min = min;
    rangeData->max = max;
    rangeData->percent = 0;
    rangeData->value = rangeData->min;
}

void TpRange::setValue(int64_t value)
{
    TpRangeData *rangeData = static_cast<TpRangeData *>(data_);
    if (value < rangeData->min)
        value = rangeData->min;
    if (value > rangeData->max)
        value = rangeData->max;

    rangeData->value = value;
    rangeData->percent = 1.0 * (rangeData->value - rangeData->min) / (rangeData->max - rangeData->min);
}

int64_t TpRange::value() const noexcept
{
    TpRangeData *rangeData = static_cast<TpRangeData *>(data_);
    return rangeData->value;
}

void TpRange::setPercent(double percent)
{
    if (percent < 0)
        percent = 0;
    if (percent > 1)
        percent = 1;

    TpRangeData *rangeData = static_cast<TpRangeData *>(data_);
    rangeData->percent = percent;
    rangeData->value = rangeData->min + rangeData->percent * (rangeData->max - rangeData->min);
}

double TpRange::percent() const noexcept
{
    TpRangeData *rangeData = static_cast<TpRangeData *>(data_);
    return rangeData->percent;
}

int64_t TpRange::min() const noexcept
{
    TpRangeData *rangeData = static_cast<TpRangeData *>(data_);
    return rangeData->min;
}

int64_t TpRange::max() const noexcept
{
    TpRangeData *rangeData = static_cast<TpRangeData *>(data_);
    return rangeData->max;
}