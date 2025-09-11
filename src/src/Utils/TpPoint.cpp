#include "TpPoint.h"
#include <cmath> // for fabs in division operations

// 定义点数据结构体
struct TpPointData
{
    int32_t x = 0;
    int32_t y = 0;
};

TpPoint::TpPoint()
{
    data_ = new TpPointData();
}

TpPoint::TpPoint(const TpPoint &other)
{
    TpPointData *pointData = new TpPointData();
    pointData->x = other.x();
    pointData->y = other.y();
    data_ = pointData;
}

TpPoint::TpPoint(int32_t xpos, int32_t ypos)
{
    TpPointData *pointData = new TpPointData();
    pointData->x = xpos;
    pointData->y = ypos;
    data_ = pointData;
}

TpPoint::~TpPoint()
{
    TpPointData *pointData = static_cast<TpPointData *>(data_);
    if (pointData)
    {
        delete pointData;
        data_ = nullptr;
    }
}

bool TpPoint::isNull() const
{
    TpPointData *pointData = static_cast<TpPointData *>(data_);
    return (pointData->x == 0 && pointData->y == 0);
}

int32_t TpPoint::x() const
{
    TpPointData *pointData = static_cast<TpPointData *>(data_);
    return pointData->x;
}

int32_t TpPoint::y() const
{
    TpPointData *pointData = static_cast<TpPointData *>(data_);
    return pointData->y;
}

void TpPoint::setX(int32_t x)
{
    TpPointData *pointData = static_cast<TpPointData *>(data_);
    pointData->x = x;
}

void TpPoint::setY(int32_t y)
{
    TpPointData *pointData = static_cast<TpPointData *>(data_);
    pointData->y = y;
}

int32_t TpPoint::manhattanLength() const
{
    TpPointData *pointData = static_cast<TpPointData *>(data_);
    return std::abs(pointData->x) + std::abs(pointData->y);
}

TpPoint TpPoint::transposed() const noexcept
{
    TpPointData *pointData = static_cast<TpPointData *>(data_);
    return TpPoint(pointData->y, pointData->x);
}

int32_t &TpPoint::rx()
{
    TpPointData *pointData = static_cast<TpPointData *>(data_);
    return pointData->x;
}

int32_t &TpPoint::ry()
{
    TpPointData *pointData = static_cast<TpPointData *>(data_);
    return pointData->y;
}

const TpPoint &TpPoint::operator=(const TpPoint &p)
{
    TpPointData *pointData = static_cast<TpPointData *>(data_);
    TpPointData *otherData = static_cast<TpPointData *>(p.data_);
    pointData->x = otherData->x;
    pointData->y = otherData->y;
    return *this;
}

TpPoint &TpPoint::operator+=(const TpPoint &p)
{
    TpPointData *pointData = static_cast<TpPointData *>(data_);
    TpPointData *otherData = static_cast<TpPointData *>(p.data_);
    pointData->x += otherData->x;
    pointData->y += otherData->y;
    return *this;
}

TpPoint &TpPoint::operator-=(const TpPoint &p)
{
    TpPointData *pointData = static_cast<TpPointData *>(data_);
    TpPointData *otherData = static_cast<TpPointData *>(p.data_);
    pointData->x -= otherData->x;
    pointData->y -= otherData->y;
    return *this;
}

TpPoint &TpPoint::operator*=(float factor)
{
    TpPointData *pointData = static_cast<TpPointData *>(data_);
    pointData->x = static_cast<int32_t>(pointData->x * factor);
    pointData->y = static_cast<int32_t>(pointData->y * factor);
    return *this;
}

TpPoint &TpPoint::operator*=(double factor)
{
    TpPointData *pointData = static_cast<TpPointData *>(data_);
    pointData->x = static_cast<int32_t>(pointData->x * factor);
    pointData->y = static_cast<int32_t>(pointData->y * factor);
    return *this;
}

TpPoint &TpPoint::operator*=(int32_t factor)
{
    TpPointData *pointData = static_cast<TpPointData *>(data_);
    pointData->x *= factor;
    pointData->y *= factor;
    return *this;
}

TpPoint &TpPoint::operator/=(float divisor)
{
    if (divisor == 0.0f)
        return *this; // Avoid division by zero
    TpPointData *pointData = static_cast<TpPointData *>(data_);
    pointData->x = static_cast<int32_t>(pointData->x / divisor);
    pointData->y = static_cast<int32_t>(pointData->y / divisor);
    return *this;
}

TpPoint &TpPoint::operator/=(double divisor)
{
    if (divisor == 0.0)
        return *this; // Avoid division by zero
    TpPointData *pointData = static_cast<TpPointData *>(data_);
    pointData->x = static_cast<int32_t>(pointData->x / divisor);
    pointData->y = static_cast<int32_t>(pointData->y / divisor);
    return *this;
}

TpPoint &TpPoint::operator/=(int32_t divisor)
{
    if (divisor == 0)
        return *this; // Avoid division by zero
    TpPointData *pointData = static_cast<TpPointData *>(data_);
    pointData->x /= divisor;
    pointData->y /= divisor;
    return *this;
}

int32_t TpPoint::dotProduct(const TpPoint &p1, const TpPoint &p2)
{
    TpPointData *p1Data = static_cast<TpPointData *>(p1.data_);
    TpPointData *p2Data = static_cast<TpPointData *>(p2.data_);
    return p1Data->x * p2Data->x + p1Data->y * p2Data->y;
}

bool operator==(const TpPoint &p1, const TpPoint &p2)
{
    TpPointData *p1Data = static_cast<TpPointData *>(p1.data_);
    TpPointData *p2Data = static_cast<TpPointData *>(p2.data_);
    return (p1Data->x == p2Data->x) && (p1Data->y == p2Data->y);
}

bool operator!=(const TpPoint &p1, const TpPoint &p2)
{
    return !(p1 == p2);
}

const TpPoint operator+(const TpPoint &p1, const TpPoint &p2)
{
    TpPointData *p1Data = static_cast<TpPointData *>(p1.data_);
    TpPointData *p2Data = static_cast<TpPointData *>(p2.data_);
    return TpPoint(p1Data->x + p2Data->x, p1Data->y + p2Data->y);
}

const TpPoint operator-(const TpPoint &p1, const TpPoint &p2)
{
    TpPointData *p1Data = static_cast<TpPointData *>(p1.data_);
    TpPointData *p2Data = static_cast<TpPointData *>(p2.data_);
    return TpPoint(p1Data->x - p2Data->x, p1Data->y - p2Data->y);
}

const TpPoint operator*(const TpPoint &p, float factor)
{
    TpPointData *pData = static_cast<TpPointData *>(p.data_);
    return TpPoint(static_cast<int32_t>(pData->x * factor), static_cast<int32_t>(pData->y * factor));
}

const TpPoint operator*(float factor, const TpPoint &p)
{
    return p * factor;
}

const TpPoint operator*(const TpPoint &p, double factor)
{
    TpPointData *pData = static_cast<TpPointData *>(p.data_);
    return TpPoint(static_cast<int32_t>(pData->x * factor), static_cast<int32_t>(pData->y * factor));
}

const TpPoint operator*(double factor, const TpPoint &p)
{
    return p * factor;
}

const TpPoint operator*(const TpPoint &p, int32_t factor)
{
    TpPointData *pData = static_cast<TpPointData *>(p.data_);
    return TpPoint(pData->x * factor, pData->y * factor);
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
    TpPointData *pData = static_cast<TpPointData *>(p.data_);
    return TpPoint(-pData->x, -pData->y);
}

const TpPoint operator/(const TpPoint &p, float divisor)
{
    if (divisor == 0.0f)
        return p;
    TpPointData *pData = static_cast<TpPointData *>(p.data_);
    return TpPoint(static_cast<int32_t>(pData->x / divisor), static_cast<int32_t>(pData->y / divisor));
}

const TpPoint operator/(const TpPoint &p, double divisor)
{
    if (divisor == 0.0)
        return p;
    TpPointData *pData = static_cast<TpPointData *>(p.data_);
    return TpPoint(static_cast<int32_t>(pData->x / divisor), static_cast<int32_t>(pData->y / divisor));
}

const TpPoint operator/(const TpPoint &p, int32_t divisor)
{
    if (divisor == 0)
        return p;
    TpPointData *pData = static_cast<TpPointData *>(p.data_);
    return TpPoint(pData->x / divisor, pData->y / divisor);
}