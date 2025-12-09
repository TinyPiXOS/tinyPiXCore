#include "TpSizeF.h"
#include <algorithm>

TpSizeF::TpSizeF() noexcept : w_(0.0), h_(0.0)
{
}

TpSizeF::TpSizeF(const TpSizeF &other) noexcept
{
    this->w_ = other.width();
    this->h_ = other.height();
}

TpSizeF::TpSizeF(double w, double h) noexcept
{
    this->w_ = w;
    this->h_ = h;
}

TpSizeF::~TpSizeF()
{
}

bool TpSizeF::isNull() const noexcept
{
    return (tpFuzzyIsNull(this->w_) == 0 && tpFuzzyIsNull(this->h_));
}

bool TpSizeF::isEmpty() const noexcept
{
    return (this->w_ < 0 || this->h_ < 0);
}

bool TpSizeF::isValid() const noexcept
{
    return (this->w_ >= 0 && this->h_ >= 0);
}

double TpSizeF::width() const noexcept
{
    return this->w_;
}

double TpSizeF::height() const noexcept
{
    return this->h_;
}

void TpSizeF::setWidth(double w) noexcept
{
    this->w_ = w;
}

void TpSizeF::setHeight(double h) noexcept
{
    this->h_ = h;
}

TpSizeF TpSizeF::transposed() const noexcept
{
    return TpSizeF(this->h_, this->w_);
}

TpSizeF TpSizeF::expandedTo(const TpSizeF &otherSize) const noexcept
{
    return TpSizeF(std::max(this->w_, otherSize.w_), std::max(this->h_, otherSize.h_));
}

TpSizeF TpSizeF::boundedTo(const TpSizeF &otherSize) const noexcept
{
    return TpSizeF(std::min(this->w_, otherSize.w_), std::min(this->h_, otherSize.h_));
}

double &TpSizeF::rwidth() noexcept
{
    return this->w_;
}

double &TpSizeF::rheight() noexcept
{
    return this->h_;
}

const TpSizeF &TpSizeF::operator=(const TpSizeF &other) noexcept
{
    this->w_ = other.w_;
    this->h_ = other.h_;
    return *this;
}

TpSizeF &TpSizeF::operator+=(const TpSizeF &other) noexcept
{
    this->w_ += other.w_;
    this->h_ += other.h_;
    return *this;
}

TpSizeF &TpSizeF::operator-=(const TpSizeF &other) noexcept
{
    this->w_ -= other.w_;
    this->h_ -= other.h_;
    return *this;
}

TpSizeF &TpSizeF::operator*=(float c) noexcept
{
    this->w_ = static_cast<tpDouble>(this->w_ * c);
    this->h_ = static_cast<tpDouble>(this->h_ * c);
    return *this;
}

TpSizeF &TpSizeF::operator/=(float c)
{
    if (c == 0.0f)
        return *this;
    this->w_ = static_cast<tpDouble>(this->w_ / c);
    this->h_ = static_cast<tpDouble>(this->h_ / c);
    return *this;
}

inline bool operator==(const TpSizeF &s1, const TpSizeF &s2) noexcept
{
    return (tpFuzzyCompare(s1.w_, s2.w_)) && (tpFuzzyCompare(s1.h_, s2.h_));
}

inline bool operator!=(const TpSizeF &s1, const TpSizeF &s2) noexcept
{
    return !(s1 == s2);
}

inline const TpSizeF operator+(const TpSizeF &s1, const TpSizeF &s2) noexcept
{
    return TpSizeF(s1.w_ + s2.w_, s1.h_ + s2.h_);
}

inline const TpSizeF operator-(const TpSizeF &s1, const TpSizeF &s2) noexcept
{
    return TpSizeF(s1.w_ - s2.w_, s1.h_ - s2.h_);
}

inline const TpSizeF operator*(const TpSizeF &s, float factor) noexcept
{
    return TpSizeF(static_cast<double>(s.w_ * factor), static_cast<double>(s.h_ * factor));
}

inline const TpSizeF operator*(float factor, const TpSizeF &s) noexcept
{
    return s * factor;
}

inline const TpSizeF operator/(const TpSizeF &s, float divisor)
{
    if (divisor == 0.0f)
        return s;
    return TpSizeF(static_cast<double>(s.w_ / divisor), static_cast<double>(s.h_ / divisor));
}