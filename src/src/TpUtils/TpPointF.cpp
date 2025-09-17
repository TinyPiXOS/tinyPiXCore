#include "TpPointF.h"
#include <cmath> // for fabs in division operations

// 定义点数据结构体
struct TpPointFData
{
    double x = 0;
    double y = 0;
};

TpPointF::TpPointF()
{
    data_ = new TpPointFData();
}

TpPointF::TpPointF(const TpPointF &other)
{
    TpPointFData *pointData = new TpPointFData();
    pointData->x = other.x();
    pointData->y = other.y();
    data_ = pointData;
}

TpPointF::TpPointF(double xpos, double ypos)
{
    TpPointFData *pointData = new TpPointFData();
    pointData->x = xpos;
    pointData->y = ypos;
    data_ = pointData;
}

TpPointF::~TpPointF()
{
    TpPointFData *pointData = static_cast<TpPointFData *>(data_);
    if (pointData)
    {
        delete pointData;
        data_ = nullptr;
    }
}

bool TpPointF::isNull() const
{
    TpPointFData *pointData = static_cast<TpPointFData *>(data_);
    return (tpFuzzyIsNull(pointData->x) && tpFuzzyIsNull(pointData->y));
}

double TpPointF::x() const
{
    TpPointFData *pointData = static_cast<TpPointFData *>(data_);
    return pointData->x;
}

double TpPointF::y() const
{
    TpPointFData *pointData = static_cast<TpPointFData *>(data_);
    return pointData->y;
}

void TpPointF::setX(double x)
{
    TpPointFData *pointData = static_cast<TpPointFData *>(data_);
    pointData->x = x;
}

void TpPointF::setY(double y)
{
    TpPointFData *pointData = static_cast<TpPointFData *>(data_);
    pointData->y = y;
}

double TpPointF::manhattanLength() const
{
    TpPointFData *pointData = static_cast<TpPointFData *>(data_);
    return std::fabs(pointData->x) + std::fabs(pointData->y);
}

TpPointF TpPointF::transposed() const noexcept
{
    TpPointFData *pointData = static_cast<TpPointFData *>(data_);
    return TpPointF(pointData->y, pointData->x);
}

double &TpPointF::rx()
{
    TpPointFData *pointData = static_cast<TpPointFData *>(data_);
    return pointData->x;
}

double &TpPointF::ry()
{
    TpPointFData *pointData = static_cast<TpPointFData *>(data_);
    return pointData->y;
}

const TpPointF &TpPointF::operator=(const TpPointF &p)
{
    TpPointFData *pointData = static_cast<TpPointFData *>(data_);
    TpPointFData *otherData = static_cast<TpPointFData *>(p.data_);
    pointData->x = otherData->x;
    pointData->y = otherData->y;
    return *this;
}

TpPointF &TpPointF::operator+=(const TpPointF &p)
{
    TpPointFData *pointData = static_cast<TpPointFData *>(data_);
    TpPointFData *otherData = static_cast<TpPointFData *>(p.data_);
    pointData->x += otherData->x;
    pointData->y += otherData->y;
    return *this;
}

TpPointF &TpPointF::operator-=(const TpPointF &p)
{
    TpPointFData *pointData = static_cast<TpPointFData *>(data_);
    TpPointFData *otherData = static_cast<TpPointFData *>(p.data_);
    pointData->x -= otherData->x;
    pointData->y -= otherData->y;
    return *this;
}

TpPointF &TpPointF::operator*=(float factor)
{
    TpPointFData *pointData = static_cast<TpPointFData *>(data_);
    pointData->x = static_cast<int32_t>(pointData->x * factor);
    pointData->y = static_cast<int32_t>(pointData->y * factor);
    return *this;
}

TpPointF &TpPointF::operator*=(double factor)
{
    TpPointFData *pointData = static_cast<TpPointFData *>(data_);
    pointData->x = static_cast<int32_t>(pointData->x * factor);
    pointData->y = static_cast<int32_t>(pointData->y * factor);
    return *this;
}

TpPointF &TpPointF::operator*=(int32_t factor)
{
    TpPointFData *pointData = static_cast<TpPointFData *>(data_);
    pointData->x *= factor;
    pointData->y *= factor;
    return *this;
}

TpPointF &TpPointF::operator/=(float divisor)
{
    if (divisor == 0.0f)
        return *this; // Avoid division by zero
    TpPointFData *pointData = static_cast<TpPointFData *>(data_);
    pointData->x = static_cast<int32_t>(pointData->x / divisor);
    pointData->y = static_cast<int32_t>(pointData->y / divisor);
    return *this;
}

TpPointF &TpPointF::operator/=(double divisor)
{
    if (divisor == 0.0)
        return *this; // Avoid division by zero
    TpPointFData *pointData = static_cast<TpPointFData *>(data_);
    pointData->x = static_cast<int32_t>(pointData->x / divisor);
    pointData->y = static_cast<int32_t>(pointData->y / divisor);
    return *this;
}

TpPointF &TpPointF::operator/=(int32_t divisor)
{
    if (divisor == 0)
        return *this; // Avoid division by zero
    TpPointFData *pointData = static_cast<TpPointFData *>(data_);
    pointData->x /= divisor;
    pointData->y /= divisor;
    return *this;
}

double TpPointF::dotProduct(const TpPointF &p1, const TpPointF &p2)
{
    TpPointFData *p1Data = static_cast<TpPointFData *>(p1.data_);
    TpPointFData *p2Data = static_cast<TpPointFData *>(p2.data_);
    return p1Data->x * p2Data->x + p1Data->y * p2Data->y;
}

inline bool operator==(const TpPointF &p1, const TpPointF &p2)
{
    TpPointFData *p1Data = static_cast<TpPointFData *>(p1.data_);
    TpPointFData *p2Data = static_cast<TpPointFData *>(p2.data_);
    return tpFuzzyCompare(p1Data->x, p2Data->x) && tpFuzzyCompare(p1Data->y, p2Data->y);
}

inline bool operator!=(const TpPointF &p1, const TpPointF &p2)
{
    return !(p1 == p2);
}

inline const TpPointF operator+(const TpPointF &p1, const TpPointF &p2)
{
    TpPointFData *p1Data = static_cast<TpPointFData *>(p1.data_);
    TpPointFData *p2Data = static_cast<TpPointFData *>(p2.data_);
    return TpPointF(p1Data->x + p2Data->x, p1Data->y + p2Data->y);
}

inline const TpPointF operator-(const TpPointF &p1, const TpPointF &p2)
{
    TpPointFData *p1Data = static_cast<TpPointFData *>(p1.data_);
    TpPointFData *p2Data = static_cast<TpPointFData *>(p2.data_);
    return TpPointF(p1Data->x - p2Data->x, p1Data->y - p2Data->y);
}

inline const TpPointF operator*(const TpPointF &p, float factor)
{
    TpPointFData *pData = static_cast<TpPointFData *>(p.data_);
    return TpPointF(static_cast<int32_t>(pData->x * factor), static_cast<int32_t>(pData->y * factor));
}

inline const TpPointF operator*(float factor, const TpPointF &p)
{
    return p * factor;
}

inline const TpPointF operator*(const TpPointF &p, double factor)
{
    TpPointFData *pData = static_cast<TpPointFData *>(p.data_);
    return TpPointF(static_cast<int32_t>(pData->x * factor), static_cast<int32_t>(pData->y * factor));
}

inline const TpPointF operator*(double factor, const TpPointF &p)
{
    return p * factor;
}

inline const TpPointF operator*(const TpPointF &p, int32_t factor)
{
    TpPointFData *pData = static_cast<TpPointFData *>(p.data_);
    return TpPointF(pData->x * factor, pData->y * factor);
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
    TpPointFData *pData = static_cast<TpPointFData *>(p.data_);
    return TpPointF(-pData->x, -pData->y);
}

inline const TpPointF operator/(const TpPointF &p, float divisor)
{
    if (divisor == 0.0f)
        return p;
    TpPointFData *pData = static_cast<TpPointFData *>(p.data_);
    return TpPointF(static_cast<int32_t>(pData->x / divisor), static_cast<int32_t>(pData->y / divisor));
}

inline const TpPointF operator/(const TpPointF &p, double divisor)
{
    if (divisor == 0.0)
        return p;
    TpPointFData *pData = static_cast<TpPointFData *>(p.data_);
    return TpPointF(static_cast<int32_t>(pData->x / divisor), static_cast<int32_t>(pData->y / divisor));
}

inline const TpPointF operator/(const TpPointF &p, int32_t divisor)
{
    if (divisor == 0)
        return p;
    TpPointFData *pData = static_cast<TpPointFData *>(p.data_);
    return TpPointF(pData->x / divisor, pData->y / divisor);
}