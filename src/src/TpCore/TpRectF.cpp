#include "TpRectF.h"
#include <algorithm>

TpRectF::TpRectF() : x_(0), y_(0), w_(0), h_(0)
{
}

TpRectF::TpRectF(const TpRectF &other)
{
    this->x_ = other.x_;
    this->y_ = other.y_;
    this->w_ = other.w_;
    this->h_ = other.h_;
}

TpRectF::TpRectF(const TpPointF &leftTop, const TpPointF &rightBottom)
{
    this->x_ = leftTop.x();
    this->y_ = leftTop.y();
    this->w_ = rightBottom.x() - leftTop.x();
    this->h_ = rightBottom.y() - leftTop.y();
}

TpRectF::TpRectF(const TpPointF &leftTop, const TpSizeF &size)
{
    this->x_ = leftTop.x();
    this->y_ = leftTop.y();
    this->w_ = size.width();
    this->h_ = size.height();
}

TpRectF::TpRectF(double x, double y, double w, double h)
{
    this->x_ = x;
    this->y_ = y;
    this->w_ = w;
    this->h_ = h;
}

TpRectF::~TpRectF()
{
}

bool TpRectF::isNull() const noexcept
{
    return tpFuzzyIsNull(this->w_) && tpFuzzyIsNull(this->h_);
}

bool TpRectF::isEmpty() const noexcept
{
    return (this->w_ <= 0 || this->h_ <= 0);
}

bool TpRectF::isValid() const noexcept
{
    return (this->w_ > 0 && this->h_ > 0);
}

double TpRectF::left() const noexcept
{
    return this->x_;
}

double TpRectF::top() const noexcept
{
    return this->y_;
}

double TpRectF::right() const noexcept
{
    return this->x_ + this->w_;
}

double TpRectF::bottom() const noexcept
{
    return this->y_ + this->h_;
}

double TpRectF::x() const noexcept
{
    return this->x_;
}

double TpRectF::y() const noexcept
{
    return this->y_;
}

void TpRectF::setLeft(double pos) noexcept
{
    this->w_ += this->x_ - pos;
    this->x_ = pos;
}

void TpRectF::setTop(double pos) noexcept
{
    this->h_ += this->y_ - pos;
    this->y_ = pos;
}

void TpRectF::setRight(double pos) noexcept
{
    this->w_ = pos - this->x_;
}

void TpRectF::setBottom(double pos) noexcept
{
    this->h_ = pos - this->y_;
}

void TpRectF::setX(double x) noexcept
{
    this->x_ = x;
}

void TpRectF::setY(double y) noexcept
{
    this->y_ = y;
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
    return TpPointF(this->x_, this->y_);
}

TpPointF TpRectF::bottomRight() const noexcept
{
    return TpPointF(this->x_ + this->w_, this->y_ + this->h_);
}

TpPointF TpRectF::topRight() const noexcept
{
    return TpPointF(this->x_ + this->w_, this->y_);
}

TpPointF TpRectF::bottomLeft() const noexcept
{
    return TpPointF(this->x_, this->y_ + this->h_);
}

TpPointF TpRectF::center() const noexcept
{
    return TpPointF(this->x_ + this->w_ / 2, this->y_ + this->h_ / 2);
}

void TpRectF::setRect(double x, double y, double w, double h) noexcept
{
    this->x_ = x;
    this->y_ = y;
    this->w_ = w;
    this->h_ = h;
}

void TpRectF::getRect(double *x, double *y, double *w, double *h) const
{
    if (x)
        *x = this->x_;
    if (y)
        *y = this->y_;
    if (w)
        *w = this->w_;
    if (h)
        *h = this->h_;
}

void TpRectF::setCoords(double x1, double y1, double x2, double y2) noexcept
{
    this->x_ = x1;
    this->y_ = y1;
    this->w_ = x2 - x1;
    this->h_ = y2 - y1;
}

void TpRectF::getCoords(double *x1, double *y1, double *x2, double *y2) const
{
    if (x1)
        *x1 = this->x_;
    if (y1)
        *y1 = this->y_;
    if (x2)
        *x2 = this->x_ + this->w_;
    if (y2)
        *y2 = this->y_ + this->h_;
}

TpSizeF TpRectF::size() const noexcept
{
    return TpSizeF(this->w_, this->h_);
}

double TpRectF::width() const noexcept
{
    return this->w_;
}

double TpRectF::height() const noexcept
{
    return this->h_;
}

void TpRectF::setWidth(double w) noexcept
{
    this->w_ = w;
}

void TpRectF::setHeight(double h) noexcept
{
    this->h_ = h;
}

void TpRectF::setSize(const TpSizeF &s) noexcept
{
    this->w_ = s.width();
    this->h_ = s.height();
}

bool TpRectF::contains(double x, double y)
{
    return (x >= this->x_) && (x <= this->x_ + this->w_) &&
           (y >= this->y_) && (y <= this->y_ + this->h_);
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
    double left = std::max(this->x_, x);
    double right = std::min(this->x_ + this->w_, x + static_cast<double>(w));
    double top = std::max(this->y_, y);
    double bottom = std::min(this->y_ + this->h_, y + static_cast<double>(h));

    return (left < right) && (top < bottom);
}

bool TpRectF::unions(const TpRectF &rect)
{
    return unions(rect.x(), rect.y(), rect.width(), rect.height());
}

bool TpRectF::unions(double x, double y, double w, double h)
{
    if (tpFuzzyIsNull(w) || tpFuzzyIsNull(h))
        return true;

    double left = std::min(this->x_, x);
    double top = std::min(this->y_, y);
    double right = std::max(this->x_ + this->w_, x + static_cast<double>(w));
    double bottom = std::max(this->y_ + this->h_, y + static_cast<double>(h));

    setCoords(left, top, right, bottom);
    return true;
}

TpRectF TpRectF::operator=(const TpRectF &other)
{
    if (this != &other)
    {
        this->x_ = other.x_;
        this->y_ = other.y_;
        this->w_ = other.w_;
        this->h_ = other.h_;
    }
    return *this;
}

bool TpRectF::operator==(const TpRectF &other)
{
    return tpFuzzyCompare(this->x_, other.x_) &&
           tpFuzzyCompare(this->y_, other.y_) &&
           tpFuzzyCompare(this->w_, other.w_) &&
           tpFuzzyCompare(this->h_, other.h_);
}

bool TpRectF::operator!=(const TpRectF &other)
{
    return !(*this == other);
}