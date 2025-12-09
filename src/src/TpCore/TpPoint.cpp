#include "TpPoint.h"
#include <cmath>

TpPoint::TpPoint() : x_(0), y_(0)
{
}

TpPoint::TpPoint(const TpPoint &other)
{
    this->x_ = other.x();
    this->y_ = other.y();
}

TpPoint::TpPoint(int32_t xpos, int32_t ypos)
{
    this->x_ = xpos;
    this->y_ = ypos;
}

TpPoint::~TpPoint()
{
}

bool TpPoint::isNull() const
{
    return (this->x_ == 0 && this->y_ == 0);
}

int32_t TpPoint::x() const
{
    return this->x_;
}

int32_t TpPoint::y() const
{
    return this->y_;
}

void TpPoint::setX(int32_t x)
{
    this->x_ = x;
}

void TpPoint::setY(int32_t y)
{
    this->y_ = y;
}

int32_t TpPoint::manhattanLength() const
{
    return std::abs(this->x_) + std::abs(this->y_);
}

TpPoint TpPoint::transposed() const noexcept
{
    return TpPoint(this->y_, this->x_);
}

int32_t &TpPoint::rx()
{
    return this->x_;
}

int32_t &TpPoint::ry()
{
    return this->y_;
}

const TpPoint &TpPoint::operator=(const TpPoint &p)
{
    this->x_ = p.x_;
    this->y_ = p.y_;
    return *this;
}

TpPoint &TpPoint::operator+=(const TpPoint &p)
{
    this->x_ += p.x_;
    this->y_ += p.y_;
    return *this;
}

TpPoint &TpPoint::operator-=(const TpPoint &p)
{
    this->x_ -= p.x_;
    this->y_ -= p.y_;
    return *this;
}

TpPoint &TpPoint::operator*=(float factor)
{
    this->x_ = static_cast<tpInt32>(this->x_ * factor);
    this->y_ = static_cast<tpInt32>(this->y_ * factor);
    return *this;
}

TpPoint &TpPoint::operator*=(double factor)
{
    this->x_ = static_cast<tpInt32>(this->x_ * factor);
    this->y_ = static_cast<tpInt32>(this->y_ * factor);
    return *this;
}

TpPoint &TpPoint::operator*=(int32_t factor)
{
    this->x_ *= factor;
    this->y_ *= factor;
    return *this;
}

TpPoint &TpPoint::operator/=(float divisor)
{
    if (divisor == 0.0f)
        return *this;
    this->x_ = static_cast<int32_t>(this->x_ / divisor);
    this->y_ = static_cast<int32_t>(this->y_ / divisor);
    return *this;
}

TpPoint &TpPoint::operator/=(double divisor)
{
    if (divisor == 0.0)
        return *this;
    this->x_ = static_cast<int32_t>(this->x_ / divisor);
    this->y_ = static_cast<int32_t>(this->y_ / divisor);
    return *this;
}

TpPoint &TpPoint::operator/=(int32_t divisor)
{
    if (divisor == 0)
        return *this;
    this->x_ /= divisor;
    this->y_ /= divisor;
    return *this;
}

int32_t TpPoint::dotProduct(const TpPoint &p1, const TpPoint &p2)
{
    return p1.x_ * p2.x_ + p1.y_ * p2.y_;
}

bool operator==(const TpPoint &p1, const TpPoint &p2)
{
    return (p1.x_ == p2.x_) && (p1.y_ == p2.y_);
}

bool operator!=(const TpPoint &p1, const TpPoint &p2)
{
    return !(p1 == p2);
}

const TpPoint operator+(const TpPoint &p1, const TpPoint &p2)
{
    return TpPoint(p1.x_ + p2.x_, p1.y_ + p2.y_);
}

const TpPoint operator-(const TpPoint &p1, const TpPoint &p2)
{
    return TpPoint(p1.x_ - p2.x_, p1.y_ - p2.y_);
}

const TpPoint operator*(const TpPoint &p, float factor)
{
    return TpPoint(static_cast<int32_t>(p.x_ * factor), static_cast<int32_t>(p.y_ * factor));
}

const TpPoint operator*(float factor, const TpPoint &p)
{
    return p * factor;
}

const TpPoint operator*(const TpPoint &p, double factor)
{
    return TpPoint(static_cast<int32_t>(p.x_ * factor), static_cast<int32_t>(p.y_ * factor));
}

const TpPoint operator*(double factor, const TpPoint &p)
{
    return p * factor;
}

const TpPoint operator*(const TpPoint &p, int32_t factor)
{
    return TpPoint(p.x_ * factor, p.y_ * factor);
}

const TpPoint operator*(int32_t factor, const TpPoint &p)
{
    return p * factor;
}

const TpPoint operator+(const TpPoint &p)
{
    return p;
}

const TpPoint operator-(const TpPoint &p)
{
    return TpPoint(-p.x_, -p.y_);
}

const TpPoint operator/(const TpPoint &p, float divisor)
{
    if (divisor == 0.0f)
        return p;
    return TpPoint(static_cast<int32_t>(p.x_ / divisor), static_cast<int32_t>(p.y_ / divisor));
}

const TpPoint operator/(const TpPoint &p, double divisor)
{
    if (divisor == 0.0)
        return p;
    return TpPoint(static_cast<int32_t>(p.x_ / divisor), static_cast<int32_t>(p.y_ / divisor));
}

const TpPoint operator/(const TpPoint &p, int32_t divisor)
{
    if (divisor == 0)
        return p;
    return TpPoint(p.x_ / divisor, p.y_ / divisor);
}