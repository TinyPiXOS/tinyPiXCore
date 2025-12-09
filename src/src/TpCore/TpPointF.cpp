#include "TpPointF.h"
#include <cmath>

TpPointF::TpPointF() : x_(0.0), y_(0.0)
{
}

TpPointF::TpPointF(const TpPointF &other)
{
    this->x_ = other.x();
    this->y_ = other.y();
}

TpPointF::TpPointF(double xpos, double ypos)
{
    this->x_ = xpos;
    this->y_ = ypos;
}

TpPointF::~TpPointF()
{
}

bool TpPointF::isNull() const
{
    return (tpFuzzyIsNull(this->x_) && tpFuzzyIsNull(this->y_));
}

double TpPointF::x() const
{
    return this->x_;
}

double TpPointF::y() const
{
    return this->y_;
}

void TpPointF::setX(double x)
{
    this->x_ = x;
}

void TpPointF::setY(double y)
{
    this->y_ = y;
}

double TpPointF::manhattanLength() const
{
    return std::fabs(this->x_) + std::fabs(this->y_);
}

TpPointF TpPointF::transposed() const noexcept
{
    return TpPointF(this->y_, this->x_);
}

double &TpPointF::rx()
{
    return this->x_;
}

double &TpPointF::ry()
{
    return this->y_;
}

const TpPointF &TpPointF::operator=(const TpPointF &p)
{
    this->x_ = p.x_;
    this->y_ = p.y_;
    return *this;
}

TpPointF &TpPointF::operator+=(const TpPointF &p)
{
    this->x_ += p.x_;
    this->y_ += p.y_;
    return *this;
}

TpPointF &TpPointF::operator-=(const TpPointF &p)
{
    this->x_ -= p.x_;
    this->y_ -= p.y_;
    return *this;
}

TpPointF &TpPointF::operator*=(float factor)
{
    this->x_ = this->x_ * factor;
    this->y_ = this->y_ * factor;
    return *this;
}

TpPointF &TpPointF::operator*=(double factor)
{
    this->x_ = this->x_ * factor;
    this->y_ = this->y_ * factor;
    return *this;
}

TpPointF &TpPointF::operator*=(int32_t factor)
{
    this->x_ *= factor;
    this->y_ *= factor;
    return *this;
}

TpPointF &TpPointF::operator/=(float divisor)
{
    if (divisor == 0.0f)
        return *this;
    this->x_ = this->x_ / divisor;
    this->y_ = this->y_ / divisor;
    return *this;
}

TpPointF &TpPointF::operator/=(double divisor)
{
    if (divisor == 0.0)
        return *this;
    this->x_ = this->x_ / divisor;
    this->y_ = this->y_ / divisor;
    return *this;
}

TpPointF &TpPointF::operator/=(int32_t divisor)
{
    if (divisor == 0)
        return *this;
    this->x_ /= divisor;
    this->y_ /= divisor;
    return *this;
}

double TpPointF::dotProduct(const TpPointF &p1, const TpPointF &p2)
{
    return p1.x_ * p2.x_ + p1.y_ * p2.y_;
}

inline bool operator==(const TpPointF &p1, const TpPointF &p2)
{
    return tpFuzzyCompare(p1.x_, p2.x_) && tpFuzzyCompare(p1.y_, p2.y_);
}

inline bool operator!=(const TpPointF &p1, const TpPointF &p2)
{
    return !(p1 == p2);
}

inline const TpPointF operator+(const TpPointF &p1, const TpPointF &p2)
{
    return TpPointF(p1.x_ + p2.x_, p1.y_ + p2.y_);
}

inline const TpPointF operator-(const TpPointF &p1, const TpPointF &p2)
{
    return TpPointF(p1.x_ - p2.x_, p1.y_ - p2.y_);
}

inline const TpPointF operator*(const TpPointF &p, float factor)
{
    return TpPointF(static_cast<int32_t>(p.x_ * factor), static_cast<int32_t>(p.y_ * factor));
}

inline const TpPointF operator*(float factor, const TpPointF &p)
{
    return p * factor;
}

inline const TpPointF operator*(const TpPointF &p, double factor)
{
    return TpPointF(static_cast<int32_t>(p.x_ * factor), static_cast<int32_t>(p.y_ * factor));
}

inline const TpPointF operator*(double factor, const TpPointF &p)
{
    return p * factor;
}

inline const TpPointF operator*(const TpPointF &p, int32_t factor)
{
    return TpPointF(p.x_ * factor, p.y_ * factor);
}

inline const TpPointF operator*(int32_t factor, const TpPointF &p)
{
    return p * factor;
}

inline const TpPointF operator+(const TpPointF &p)
{
    return p;
}

inline const TpPointF operator-(const TpPointF &p)
{
    return TpPointF(-p.x_, -p.y_);
}

inline const TpPointF operator/(const TpPointF &p, float divisor)
{
    if (divisor == 0.0f)
        return p;
    return TpPointF(static_cast<int32_t>(p.x_ / divisor), static_cast<int32_t>(p.y_ / divisor));
}

inline const TpPointF operator/(const TpPointF &p, double divisor)
{
    if (divisor == 0.0)
        return p;
    return TpPointF(static_cast<int32_t>(p.x_ / divisor), static_cast<int32_t>(p.y_ / divisor));
}

inline const TpPointF operator/(const TpPointF &p, int32_t divisor)
{
    if (divisor == 0)
        return p;
    return TpPointF(p.x_ / divisor, p.y_ / divisor);
}