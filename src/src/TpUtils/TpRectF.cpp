#include "TpRectF.h"
#include <algorithm>

// 定义矩形数据结构体
struct TpRectData
{
    double x = 0; // 矩形左上角x坐标
    double y = 0; // 矩形左上角y坐标
    double w = 0; // 矩形宽度
    double h = 0; // 矩形高度
};

TpRectF::TpRectF()
{
    data_ = new TpRectData();
}

TpRectF::TpRectF(const TpRectF &other)
{
    TpRectData *rectData = new TpRectData();
    TpRectData *othersData = static_cast<TpRectData *>(other.data_);
    *rectData = *othersData;
    data_ = rectData;
}

TpRectF::TpRectF(const TpPointF &leftTop, const TpPointF &rightBottom)
{
    TpRectData *rectData = new TpRectData();
    rectData->x = leftTop.x();
    rectData->y = leftTop.y();
    rectData->w = rightBottom.x() - leftTop.x();
    rectData->h = rightBottom.y() - leftTop.y();

    data_ = rectData;
}

TpRectF::TpRectF(const TpPointF &leftTop, const TpSizeF &size)
{
    TpRectData *rectData = new TpRectData();
    rectData->x = leftTop.x();
    rectData->y = leftTop.y();
    rectData->w = size.width();
    rectData->h = size.height();

    data_ = rectData;
}

TpRectF::TpRectF(double x, double y, double w, double h)
{
    TpRectData *rectData = new TpRectData();
    rectData->x = x;
    rectData->y = y;
    rectData->w = w;
    rectData->h = h;

    data_ = rectData;
}

TpRectF::~TpRectF()
{
    TpRectData *rectData = static_cast<TpRectData *>(data_);
    if (rectData)
    {
        delete rectData;
        rectData = nullptr;
        data_ = nullptr;
    }
}

bool TpRectF::isNull() const noexcept
{
    TpRectData *rectData = static_cast<TpRectData *>(data_);
    return tpFuzzyIsNull(rectData->w) && tpFuzzyIsNull(rectData->h);
}

bool TpRectF::isEmpty() const noexcept
{
    TpRectData *rectData = static_cast<TpRectData *>(data_);
    return (rectData->w <= 0 || rectData->h <= 0);
}

bool TpRectF::isValid() const noexcept
{
    TpRectData *rectData = static_cast<TpRectData *>(data_);
    return (rectData->w > 0 && rectData->h > 0);
}

double TpRectF::left() const noexcept
{
    TpRectData *rectData = static_cast<TpRectData *>(data_);
    return rectData->x;
}

double TpRectF::top() const noexcept
{
    TpRectData *rectData = static_cast<TpRectData *>(data_);
    return rectData->y;
}

double TpRectF::right() const noexcept
{
    TpRectData *rectData = static_cast<TpRectData *>(data_);
    return rectData->x + rectData->w;
}

double TpRectF::bottom() const noexcept
{
    TpRectData *rectData = static_cast<TpRectData *>(data_);
    return rectData->y + rectData->h;
}

double TpRectF::x() const noexcept
{
    TpRectData *rectData = static_cast<TpRectData *>(data_);
    return rectData->x;
}

double TpRectF::y() const noexcept
{
    TpRectData *rectData = static_cast<TpRectData *>(data_);
    return rectData->y;
}

void TpRectF::setLeft(double pos) noexcept
{
    TpRectData *rectData = static_cast<TpRectData *>(data_);
    rectData->w += rectData->x - pos;
    rectData->x = pos;
}

void TpRectF::setTop(double pos) noexcept
{
    TpRectData *rectData = static_cast<TpRectData *>(data_);
    rectData->h += rectData->y - pos;
    rectData->y = pos;
}

void TpRectF::setRight(double pos) noexcept
{
    TpRectData *rectData = static_cast<TpRectData *>(data_);
    rectData->w = pos - rectData->x;
}

void TpRectF::setBottom(double pos) noexcept
{
    TpRectData *rectData = static_cast<TpRectData *>(data_);
    rectData->h = pos - rectData->y;
}

void TpRectF::setX(double x) noexcept
{
    TpRectData *rectData = static_cast<TpRectData *>(data_);
    rectData->x = x;
}

void TpRectF::setY(double y) noexcept
{
    TpRectData *rectData = static_cast<TpRectData *>(data_);
    rectData->y = y;
}

void TpRectF::setTopLeft(const TpPointF &p) noexcept
{
    setX(p.x());
    setY(p.y());
}

void TpRectF::setBottomRight(const TpPointF &p) noexcept
{
    setRight(p.x());
    setBottom(p.y());
}

void TpRectF::setTopRight(const TpPointF &p) noexcept
{
    setRight(p.x());
    setY(p.y());
}

void TpRectF::setBottomLeft(const TpPointF &p) noexcept
{
    setX(p.x());
    setBottom(p.y());
}

TpPointF TpRectF::topLeft() const noexcept
{
    TpRectData *rectData = static_cast<TpRectData *>(data_);
    return TpPointF(rectData->x, rectData->y);
}

TpPointF TpRectF::bottomRight() const noexcept
{
    TpRectData *rectData = static_cast<TpRectData *>(data_);
    return TpPointF(rectData->x + rectData->w, rectData->y + rectData->h);
}

TpPointF TpRectF::topRight() const noexcept
{
    TpRectData *rectData = static_cast<TpRectData *>(data_);
    return TpPointF(rectData->x + rectData->w, rectData->y);
}

TpPointF TpRectF::bottomLeft() const noexcept
{
    TpRectData *rectData = static_cast<TpRectData *>(data_);
    return TpPointF(rectData->x, rectData->y + rectData->h);
}

TpPointF TpRectF::center() const noexcept
{
    TpRectData *rectData = static_cast<TpRectData *>(data_);
    return TpPointF(rectData->x + rectData->w / 2, rectData->y + rectData->h / 2);
}

void TpRectF::setRect(double x, double y, double w, double h) noexcept
{
    TpRectData *rectData = static_cast<TpRectData *>(data_);
    rectData->x = x;
    rectData->y = y;
    rectData->w = w;
    rectData->h = h;
}

void TpRectF::getRect(double *x, double *y, double *w, double *h) const
{
    TpRectData *rectData = static_cast<TpRectData *>(data_);
    if (x)
        *x = rectData->x;
    if (y)
        *y = rectData->y;
    if (w)
        *w = rectData->w;
    if (h)
        *h = rectData->h;
}

void TpRectF::setCoords(double x1, double y1, double x2, double y2) noexcept
{
    TpRectData *rectData = static_cast<TpRectData *>(data_);
    rectData->x = x1;
    rectData->y = y1;
    rectData->w = x2 - x1;
    rectData->h = y2 - y1;
}

void TpRectF::getCoords(double *x1, double *y1, double *x2, double *y2) const
{
    TpRectData *rectData = static_cast<TpRectData *>(data_);
    if (x1)
        *x1 = rectData->x;
    if (y1)
        *y1 = rectData->y;
    if (x2)
        *x2 = rectData->x + rectData->w;
    if (y2)
        *y2 = rectData->y + rectData->h;
}

TpSizeF TpRectF::size() const noexcept
{
    TpRectData *rectData = static_cast<TpRectData *>(data_);
    return TpSizeF(rectData->w, rectData->h);
}

double TpRectF::width() const noexcept
{
    TpRectData *rectData = static_cast<TpRectData *>(data_);
    return rectData->w;
}

double TpRectF::height() const noexcept
{
    TpRectData *rectData = static_cast<TpRectData *>(data_);
    return rectData->h;
}

void TpRectF::setWidth(double w) noexcept
{
    TpRectData *rectData = static_cast<TpRectData *>(data_);
    rectData->w = w;
}

void TpRectF::setHeight(double h) noexcept
{
    TpRectData *rectData = static_cast<TpRectData *>(data_);
    rectData->h = h;
}

void TpRectF::setSize(const TpSizeF &s) noexcept
{
    TpRectData *rectData = static_cast<TpRectData *>(data_);
    rectData->w = s.width();
    rectData->h = s.height();
}

bool TpRectF::contains(double x, double y)
{
    TpRectData *rectData = static_cast<TpRectData *>(data_);
    return (x >= rectData->x) && (x <= rectData->x + rectData->w) &&
           (y >= rectData->y) && (y <= rectData->y + rectData->h);
}

bool TpRectF::contains(const TpPointF &point)
{
    return contains(point.x(), point.y());
}

bool TpRectF::intersect(const TpRectF &rect)
{
    return intersect(rect.x(), rect.y(), rect.width(), rect.height());
}

bool TpRectF::intersect(double x, double y, double w, double h)
{
    TpRectData *rectData = static_cast<TpRectData *>(data_);

    double left = std::max(rectData->x, x);
    double right = std::min(rectData->x + rectData->w, x + static_cast<double>(w));
    double top = std::max(rectData->y, y);
    double bottom = std::min(rectData->y + rectData->h, y + static_cast<double>(h));

    return (left < right) && (top < bottom);
}

bool TpRectF::unions(const TpRectF &rect)
{
    return unions(rect.x(), rect.y(), rect.width(), rect.height());
}

bool TpRectF::unions(double x, double y, double w, double h)
{
    TpRectData *rectData = static_cast<TpRectData *>(data_);

    if (tpFuzzyIsNull(w) || tpFuzzyIsNull(h))
        return true;

    double left = std::min(rectData->x, x);
    double top = std::min(rectData->y, y);
    double right = std::max(rectData->x + rectData->w, x + static_cast<double>(w));
    double bottom = std::max(rectData->y + rectData->h, y + static_cast<double>(h));

    setCoords(left, top, right, bottom);
    return true;
}

TpRectF TpRectF::operator=(const TpRectF &other)
{
    if (this != &other)
    {
        TpRectData *rectData = static_cast<TpRectData *>(data_);
        TpRectData *otherData = static_cast<TpRectData *>(other.data_);

        rectData->x = otherData->x;
        rectData->y = otherData->y;
        rectData->w = otherData->w;
        rectData->h = otherData->h;
    }
    return *this;
}

bool TpRectF::operator==(const TpRectF &other)
{
    TpRectData *rectData = static_cast<TpRectData *>(data_);
    TpRectData *otherData = static_cast<TpRectData *>(other.data_);

    return tpFuzzyCompare(rectData->x, otherData->x) &&
           tpFuzzyCompare(rectData->y, otherData->y) &&
           tpFuzzyCompare(rectData->w, otherData->w) &&
           tpFuzzyCompare(rectData->h, otherData->h);
}

bool TpRectF::operator!=(const TpRectF &other)
{
    return !(*this == other);
}