#include "TpSizeF.h"
#include <algorithm>

// 定义尺寸数据结构体
struct TpSizeData
{
    double w = 0;
    double h = 0;
};

TpSizeF::TpSizeF() noexcept
{
    data_ = new TpSizeData();
}

TpSizeF::TpSizeF(const TpSizeF &other) noexcept
{
    TpSizeData *sizeData = new TpSizeData();
    sizeData->w = other.width();
    sizeData->h = other.height();
    data_ = sizeData;
}

TpSizeF::TpSizeF(double w, double h) noexcept
{
    TpSizeData *sizeData = new TpSizeData();
    sizeData->w = w;
    sizeData->h = h;
    data_ = sizeData;
}

TpSizeF::~TpSizeF()
{
    TpSizeData *sizeData = static_cast<TpSizeData *>(data_);
    if (sizeData)
    {
        delete sizeData;
        sizeData = nullptr;
        data_ = nullptr;
    }
}

bool TpSizeF::isNull() const noexcept
{
    TpSizeData *sizeData = static_cast<TpSizeData *>(data_);
    return (tpFuzzyIsNull(sizeData->w) == 0 && tpFuzzyIsNull(sizeData->h));
}

bool TpSizeF::isEmpty() const noexcept
{
    TpSizeData *sizeData = static_cast<TpSizeData *>(data_);
    return (sizeData->w < 0 || sizeData->h < 0);
}

bool TpSizeF::isValid() const noexcept
{
    TpSizeData *sizeData = static_cast<TpSizeData *>(data_);
    return (sizeData->w >= 0 && sizeData->h >= 0);
}

double TpSizeF::width() const noexcept
{
    TpSizeData *sizeData = static_cast<TpSizeData *>(data_);
    return sizeData->w;
}

double TpSizeF::height() const noexcept
{
    TpSizeData *sizeData = static_cast<TpSizeData *>(data_);
    return sizeData->h;
}

void TpSizeF::setWidth(double w) noexcept
{
    TpSizeData *sizeData = static_cast<TpSizeData *>(data_);
    sizeData->w = w;
}

void TpSizeF::setHeight(double h) noexcept
{
    TpSizeData *sizeData = static_cast<TpSizeData *>(data_);
    sizeData->h = h;
}

TpSizeF TpSizeF::transposed() const noexcept
{
    TpSizeData *sizeData = static_cast<TpSizeData *>(data_);
    return TpSizeF(sizeData->h, sizeData->w);
}

TpSizeF TpSizeF::expandedTo(const TpSizeF &otherSize) const noexcept
{
    TpSizeData *sizeData = static_cast<TpSizeData *>(data_);
    TpSizeData *otherData = static_cast<TpSizeData *>(otherSize.data_);
    return TpSizeF(std::max(sizeData->w, otherData->w), std::max(sizeData->h, otherData->h));
}

TpSizeF TpSizeF::boundedTo(const TpSizeF &otherSize) const noexcept
{
    TpSizeData *sizeData = static_cast<TpSizeData *>(data_);
    TpSizeData *otherData = static_cast<TpSizeData *>(otherSize.data_);
    return TpSizeF(std::min(sizeData->w, otherData->w), std::min(sizeData->h, otherData->h));
}

double &TpSizeF::rwidth() noexcept
{
    TpSizeData *sizeData = static_cast<TpSizeData *>(data_);
    return sizeData->w;
}

double &TpSizeF::rheight() noexcept
{
    TpSizeData *sizeData = static_cast<TpSizeData *>(data_);
    return sizeData->h;
}

const TpSizeF &TpSizeF::operator=(const TpSizeF &other) noexcept
{
    TpSizeData *sizeData = static_cast<TpSizeData *>(data_);
    TpSizeData *otherData = static_cast<TpSizeData *>(other.data_);
    sizeData->w = otherData->w;
    sizeData->h = otherData->h;
    return *this;
}

TpSizeF &TpSizeF::operator+=(const TpSizeF &other) noexcept
{
    TpSizeData *sizeData = static_cast<TpSizeData *>(data_);
    TpSizeData *otherData = static_cast<TpSizeData *>(other.data_);
    sizeData->w += otherData->w;
    sizeData->h += otherData->h;
    return *this;
}

TpSizeF &TpSizeF::operator-=(const TpSizeF &other) noexcept
{
    TpSizeData *sizeData = static_cast<TpSizeData *>(data_);
    TpSizeData *otherData = static_cast<TpSizeData *>(other.data_);
    sizeData->w -= otherData->w;
    sizeData->h -= otherData->h;
    return *this;
}

TpSizeF &TpSizeF::operator*=(float c) noexcept
{
    TpSizeData *sizeData = static_cast<TpSizeData *>(data_);
    sizeData->w = static_cast<double>(sizeData->w * c);
    sizeData->h = static_cast<double>(sizeData->h * c);
    return *this;
}

TpSizeF &TpSizeF::operator/=(float c)
{
    if (c == 0.0f)
        return *this;
    TpSizeData *sizeData = static_cast<TpSizeData *>(data_);
    sizeData->w = static_cast<double>(sizeData->w / c);
    sizeData->h = static_cast<double>(sizeData->h / c);
    return *this;
}

inline bool operator==(const TpSizeF &s1, const TpSizeF &s2) noexcept
{
    TpSizeData *s1Data = static_cast<TpSizeData *>(s1.data_);
    TpSizeData *s2Data = static_cast<TpSizeData *>(s2.data_);
    return (tpFuzzyCompare(s1Data->w, s2Data->w)) && (tpFuzzyCompare(s1Data->h, s2Data->h));
}

inline bool operator!=(const TpSizeF &s1, const TpSizeF &s2) noexcept
{
    return !(s1 == s2);
}

inline const TpSizeF operator+(const TpSizeF &s1, const TpSizeF &s2) noexcept
{
    TpSizeData *s1Data = static_cast<TpSizeData *>(s1.data_);
    TpSizeData *s2Data = static_cast<TpSizeData *>(s2.data_);
    return TpSizeF(s1Data->w + s2Data->w, s1Data->h + s2Data->h);
}

inline const TpSizeF operator-(const TpSizeF &s1, const TpSizeF &s2) noexcept
{
    TpSizeData *s1Data = static_cast<TpSizeData *>(s1.data_);
    TpSizeData *s2Data = static_cast<TpSizeData *>(s2.data_);
    return TpSizeF(s1Data->w - s2Data->w, s1Data->h - s2Data->h);
}

inline const TpSizeF operator*(const TpSizeF &s, float factor) noexcept
{
    TpSizeData *sData = static_cast<TpSizeData *>(s.data_);
    return TpSizeF(static_cast<double>(sData->w * factor), static_cast<double>(sData->h * factor));
}

inline const TpSizeF operator*(float factor, const TpSizeF &s) noexcept
{
    return s * factor;
}

inline const TpSizeF operator/(const TpSizeF &s, float divisor)
{
    if (divisor == 0.0f)
        return s;
    TpSizeData *sData = static_cast<TpSizeData *>(s.data_);
    return TpSizeF(static_cast<double>(sData->w / divisor), static_cast<double>(sData->h / divisor));
}