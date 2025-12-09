#include "TpSize.h"
#include <algorithm>

TpSize::TpSize() noexcept : w_(0), h_(0)
{
}

TpSize::TpSize(const TpSize &other) noexcept
{
    this->w_ = other.width();
    this->h_ = other.height();
}

TpSize::TpSize(int32_t w, int32_t h) noexcept
{
    this->w_ = w;
    this->h_ = h;
}

TpSize::~TpSize()
{
}

bool TpSize::isNull() const noexcept
{
    return (this->w_ == 0 && this->h_ == 0);
}

bool TpSize::isEmpty() const noexcept
{
    return (this->w_ < 0 || this->h_ < 0);
}

bool TpSize::isValid() const noexcept
{
    return (this->w_ >= 0 && this->h_ >= 0);
}

int32_t TpSize::width() const noexcept
{
    return this->w_;
}

int32_t TpSize::height() const noexcept
{
    return this->h_;
}

void TpSize::setWidth(int32_t w) noexcept
{
    this->w_ = w;
}

void TpSize::setHeight(int32_t h) noexcept
{
    this->h_ = h;
}

TpSize TpSize::transposed() const noexcept
{
    return TpSize(this->h_, this->w_);
}

TpSize TpSize::expandedTo(const TpSize &otherSize) const noexcept
{
    return TpSize(std::max(this->w_, otherSize.w_), std::max(this->h_, otherSize.h_));
}

TpSize TpSize::boundedTo(const TpSize &otherSize) const noexcept
{
    return TpSize(std::min(this->w_, otherSize.w_), std::min(this->h_, otherSize.h_));
}

int32_t &TpSize::rwidth() noexcept
{
    return this->w_;
}

int32_t &TpSize::rheight() noexcept
{
    return this->h_;
}

const TpSize &TpSize::operator=(const TpSize &other) noexcept
{
    this->w_ = other.w_;
    this->h_ = other.h_;
    return *this;
}

TpSize &TpSize::operator+=(const TpSize &other) noexcept
{
    this->w_ += other.w_;
    this->h_ += other.h_;
    return *this;
}

TpSize &TpSize::operator-=(const TpSize &other) noexcept
{
    this->w_ -= other.w_;
    this->h_ -= other.h_;
    return *this;
}

TpSize &TpSize::operator*=(float c) noexcept
{
    this->w_ = static_cast<tpInt32>(this->w_ * c);
    this->h_ = static_cast<tpInt32>(this->h_ * c);
    return *this;
}

TpSize &TpSize::operator/=(float c)
{
    if (c == 0.0f)
        return *this;
    this->w_ = static_cast<tpInt32>(this->w_ / c);
    this->h_ = static_cast<tpInt32>(this->h_ / c);
    return *this;
}

inline bool operator==(const TpSize &s1, const TpSize &s2) noexcept
{
    return (s1.w_ == s2.w_) && (s1.h_ == s2.h_);
}

inline bool operator!=(const TpSize &s1, const TpSize &s2) noexcept
{
    return !(s1 == s2);
}

inline const TpSize operator+(const TpSize &s1, const TpSize &s2) noexcept
{
    return TpSize(s1.w_ + s2.w_, s1.h_ + s2.h_);
}

inline const TpSize operator-(const TpSize &s1, const TpSize &s2) noexcept
{
    return TpSize(s1.w_ - s2.w_, s1.h_ - s2.h_);
}

inline const TpSize operator*(const TpSize &s, float factor) noexcept
{
    return TpSize(static_cast<int32_t>(s.w_ * factor), static_cast<int32_t>(s.h_ * factor));
}

inline const TpSize operator*(float factor, const TpSize &s) noexcept
{
    return s * factor;
}

inline const TpSize operator/(const TpSize &s, float divisor)
{
    if (divisor == 0.0f)
        return s;
    return TpSize(static_cast<int32_t>(s.w_ / divisor), static_cast<int32_t>(s.h_ / divisor));
}