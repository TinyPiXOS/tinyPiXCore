#include "TpSize.h"
#include <algorithm> // for std::max and std::min

// 定义尺寸数据结构体
struct TpSizeData
{
    int32_t w = 0;
    int32_t h = 0;
};

TpSize::TpSize() noexcept
{
    data_ = new TpSizeData();
}

TpSize::TpSize(const TpSize &other) noexcept
{
    TpSizeData *sizeData = new TpSizeData();
    sizeData->w = other.width();
    sizeData->h = other.height();
    data_ = sizeData;
}

TpSize::TpSize(int32_t w, int32_t h) noexcept
{
    TpSizeData *sizeData = new TpSizeData();
    sizeData->w = w;
    sizeData->h = h;
    data_ = sizeData;
}

TpSize::~TpSize()
{
    TpSizeData *sizeData = static_cast<TpSizeData *>(data_);
    if (sizeData)
    {
        delete sizeData;
        sizeData = nullptr;
        data_ = nullptr;
    }
}

bool TpSize::isNull() const noexcept
{
    TpSizeData *sizeData = static_cast<TpSizeData *>(data_);
    return (sizeData->w == 0 && sizeData->h == 0);
}

bool TpSize::isEmpty() const noexcept
{
    TpSizeData *sizeData = static_cast<TpSizeData *>(data_);
    return (sizeData->w < 0 || sizeData->h < 0);
}

bool TpSize::isValid() const noexcept
{
    TpSizeData *sizeData = static_cast<TpSizeData *>(data_);
    return (sizeData->w >= 0 && sizeData->h >= 0);
}

int32_t TpSize::width() const noexcept
{
    TpSizeData *sizeData = static_cast<TpSizeData *>(data_);
    return sizeData->w;
}

int32_t TpSize::height() const noexcept
{
    TpSizeData *sizeData = static_cast<TpSizeData *>(data_);
    return sizeData->h;
}

void TpSize::setWidth(int32_t w) noexcept
{
    TpSizeData *sizeData = static_cast<TpSizeData *>(data_);
    sizeData->w = w;
}

void TpSize::setHeight(int32_t h) noexcept
{
    TpSizeData *sizeData = static_cast<TpSizeData *>(data_);
    sizeData->h = h;
}

TpSize TpSize::transposed() const noexcept
{
    TpSizeData *sizeData = static_cast<TpSizeData *>(data_);
    return TpSize(sizeData->h, sizeData->w);
}

TpSize TpSize::expandedTo(const TpSize &otherSize) const noexcept
{
    TpSizeData *sizeData = static_cast<TpSizeData *>(data_);
    TpSizeData *otherData = static_cast<TpSizeData *>(otherSize.data_);
    return TpSize(std::max(sizeData->w, otherData->w), std::max(sizeData->h, otherData->h));
}

TpSize TpSize::boundedTo(const TpSize &otherSize) const noexcept
{
    TpSizeData *sizeData = static_cast<TpSizeData *>(data_);
    TpSizeData *otherData = static_cast<TpSizeData *>(otherSize.data_);
    return TpSize(std::min(sizeData->w, otherData->w), std::min(sizeData->h, otherData->h));
}

int32_t &TpSize::rwidth() noexcept
{
    TpSizeData *sizeData = static_cast<TpSizeData *>(data_);
    return sizeData->w;
}

int32_t &TpSize::rheight() noexcept
{
    TpSizeData *sizeData = static_cast<TpSizeData *>(data_);
    return sizeData->h;
}

const TpSize &TpSize::operator=(const TpSize &other) noexcept
{
    TpSizeData *sizeData = static_cast<TpSizeData *>(data_);
    TpSizeData *otherData = static_cast<TpSizeData *>(other.data_);
    sizeData->w = otherData->w;
    sizeData->h = otherData->h;
    return *this;
}

TpSize &TpSize::operator+=(const TpSize &other) noexcept
{
    TpSizeData *sizeData = static_cast<TpSizeData *>(data_);
    TpSizeData *otherData = static_cast<TpSizeData *>(other.data_);
    sizeData->w += otherData->w;
    sizeData->h += otherData->h;
    return *this;
}

TpSize &TpSize::operator-=(const TpSize &other) noexcept
{
    TpSizeData *sizeData = static_cast<TpSizeData *>(data_);
    TpSizeData *otherData = static_cast<TpSizeData *>(other.data_);
    sizeData->w -= otherData->w;
    sizeData->h -= otherData->h;
    return *this;
}

TpSize &TpSize::operator*=(float c) noexcept
{
    TpSizeData *sizeData = static_cast<TpSizeData *>(data_);
    sizeData->w = static_cast<int32_t>(sizeData->w * c);
    sizeData->h = static_cast<int32_t>(sizeData->h * c);
    return *this;
}

TpSize &TpSize::operator/=(float c)
{
    if (c == 0.0f)
        return *this;
    TpSizeData *sizeData = static_cast<TpSizeData *>(data_);
    sizeData->w = static_cast<int32_t>(sizeData->w / c);
    sizeData->h = static_cast<int32_t>(sizeData->h / c);
    return *this;
}

inline bool operator==(const TpSize &s1, const TpSize &s2) noexcept
{
    TpSizeData *s1Data = static_cast<TpSizeData *>(s1.data_);
    TpSizeData *s2Data = static_cast<TpSizeData *>(s2.data_);
    return (s1Data->w == s2Data->w) && (s1Data->h == s2Data->h);
}

inline bool operator!=(const TpSize &s1, const TpSize &s2) noexcept
{
    return !(s1 == s2);
}

inline const TpSize operator+(const TpSize &s1, const TpSize &s2) noexcept
{
    TpSizeData *s1Data = static_cast<TpSizeData *>(s1.data_);
    TpSizeData *s2Data = static_cast<TpSizeData *>(s2.data_);
    return TpSize(s1Data->w + s2Data->w, s1Data->h + s2Data->h);
}

inline const TpSize operator-(const TpSize &s1, const TpSize &s2) noexcept
{
    TpSizeData *s1Data = static_cast<TpSizeData *>(s1.data_);
    TpSizeData *s2Data = static_cast<TpSizeData *>(s2.data_);
    return TpSize(s1Data->w - s2Data->w, s1Data->h - s2Data->h);
}

inline const TpSize operator*(const TpSize &s, float factor) noexcept
{
    TpSizeData *sData = static_cast<TpSizeData *>(s.data_);
    return TpSize(static_cast<int32_t>(sData->w * factor), static_cast<int32_t>(sData->h * factor));
}

inline const TpSize operator*(float factor, const TpSize &s) noexcept
{
    return s * factor;
}

inline const TpSize operator/(const TpSize &s, float divisor)
{
    if (divisor == 0.0f)
        return s;
    TpSizeData *sData = static_cast<TpSizeData *>(s.data_);
    return TpSize(static_cast<int32_t>(sData->w / divisor), static_cast<int32_t>(sData->h / divisor));
}