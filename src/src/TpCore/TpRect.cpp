#include "TpRect.h"
#include <algorithm>

TpRect::TpRect() : x_(0), y_(0), w_(0), h_(0)
{
}

TpRect::TpRect(const TpRect &other)
{
    this->x_ = other.x_;
    this->y_ = other.y_;
    this->w_ = other.w_;
    this->h_ = other.h_;
}

TpRect::TpRect(const TpPoint &leftTop, const TpPoint &rightBottom)
{
    this->x_ = leftTop.x();
    this->y_ = leftTop.y();
    this->w_ = rightBottom.x() - leftTop.x();
    this->h_ = rightBottom.y() - leftTop.y();
}

TpRect::TpRect(const TpPoint &leftTop, const TpSize &size)
{
    this->x_ = leftTop.x();
    this->y_ = leftTop.y();
    this->w_ = size.width();
    this->h_ = size.height();
}

TpRect::TpRect(int32_t x, int32_t y, int32_t w, int32_t h)
{
    this->x_ = x;
    this->y_ = y;
    this->w_ = w;
    this->h_ = h;
}

TpRect::~TpRect()
{
}

bool TpRect::isNull() const noexcept
{
    return (this->w_ == 0 && this->h_ == 0);
}

bool TpRect::isEmpty() const noexcept
{
    return (this->w_ <= 0 || this->h_ <= 0);
}

bool TpRect::isValid() const noexcept
{
    return (this->w_ > 0 && this->h_ > 0);
}

int32_t TpRect::left() const noexcept
{
    return this->x_;
}

int32_t TpRect::top() const noexcept
{
    return this->y_;
}

int32_t TpRect::right() const noexcept
{
    return this->x_ + this->w_;
}

int32_t TpRect::bottom() const noexcept
{
    return this->y_ + this->h_;
}

int32_t TpRect::x() const noexcept
{
    return this->x_;
}

int32_t TpRect::y() const noexcept
{
    return this->y_;
}

void TpRect::setLeft(int32_t pos) noexcept
{
    this->w_ += this->x_ - pos;
    this->x_ = pos;
}

void TpRect::setTop(int32_t pos) noexcept
{
    this->h_ += this->y_ - pos;
    this->y_ = pos;
}

void TpRect::setRight(int32_t pos) noexcept
{
    this->w_ = pos - this->x_;
}

void TpRect::setBottom(int32_t pos) noexcept
{
    this->h_ = pos - this->y_;
}

void TpRect::setX(int32_t x) noexcept
{
    this->x_ = x;
}

void TpRect::setY(int32_t y) noexcept
{
    this->y_ = y;
}

void TpRect::setTopLeft(const TpPoint &p) noexcept
{
    setX(p.x());
    setY(p.y());
}

void TpRect::setBottomRight(const TpPoint &p) noexcept
{
    setRight(p.x());
    setBottom(p.y());
}

void TpRect::setTopRight(const TpPoint &p) noexcept
{
    setRight(p.x());
    setY(p.y());
}

void TpRect::setBottomLeft(const TpPoint &p) noexcept
{
    setX(p.x());
    setBottom(p.y());
}

TpPoint TpRect::topLeft() const noexcept
{
    return TpPoint(this->x_, this->y_);
}

TpPoint TpRect::bottomRight() const noexcept
{
    return TpPoint(this->x_ + this->w_, this->y_ + this->h_);
}

TpPoint TpRect::topRight() const noexcept
{
    return TpPoint(this->x_ + this->w_, this->y_);
}

TpPoint TpRect::bottomLeft() const noexcept
{
    return TpPoint(this->x_, this->y_ + this->h_);
}

TpPoint TpRect::center() const noexcept
{
    return TpPoint(this->x_ + this->w_ / 2, this->y_ + this->h_ / 2);
}

void TpRect::setRect(int32_t x, int32_t y, int32_t w, int32_t h) noexcept
{
    this->x_ = x;
    this->y_ = y;
    this->w_ = w;
    this->h_ = h;
}

void TpRect::getRect(int32_t *x, int32_t *y, int32_t *w, int32_t *h) const
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

void TpRect::setCoords(int32_t x1, int32_t y1, int32_t x2, int32_t y2) noexcept
{
    this->x_ = x1;
    this->y_ = y1;
    this->w_ = x2 - x1;
    this->h_ = y2 - y1;
}

void TpRect::getCoords(int32_t *x1, int32_t *y1, int32_t *x2, int32_t *y2) const
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

TpSize TpRect::size() const noexcept
{
    return TpSize(this->w_, this->h_);
}

int32_t TpRect::width() const noexcept
{
    return this->w_;
}

int32_t TpRect::height() const noexcept
{
    return this->h_;
}

void TpRect::setWidth(int32_t w) noexcept
{
    this->w_ = w;
}

void TpRect::setHeight(int32_t h) noexcept
{
    this->h_ = h;
}

void TpRect::setSize(const TpSize &s) noexcept
{
    this->w_ = s.width();
    this->h_ = s.height();
}

bool TpRect::contains(int32_t x, int32_t y)
{
    return (x >= this->x_) && (x <= this->x_ + this->w_) &&
           (y >= this->y_) && (y <= this->y_ + this->h_);
}

bool TpRect::contains(const TpPoint &point)
{
    return contains(point.x(), point.y());
}

bool TpRect::intersect(const TpRect &rect)
{
    return intersect(rect.x(), rect.y(), rect.width(), rect.height());
}

bool TpRect::intersect(int32_t x, int32_t y, uint32_t w, uint32_t h)
{
    int32_t left = std::max(this->x_, x);
    int32_t right = std::min(this->x_ + this->w_, x + static_cast<int32_t>(w));
    int32_t top = std::max(this->y_, y);
    int32_t bottom = std::min(this->y_ + this->h_, y + static_cast<int32_t>(h));

    return (left < right) && (top < bottom);
}

bool TpRect::unions(const TpRect &rect)
{
    return unions(rect.x(), rect.y(), rect.width(), rect.height());
}

bool TpRect::unions(int32_t x, int32_t y, uint32_t w, uint32_t h)
{
    if (w == 0 || h == 0)
        return true;

    int32_t left = std::min(this->x_, x);
    int32_t top = std::min(this->y_, y);
    int32_t right = std::max(this->x_ + this->w_, x + static_cast<int32_t>(w));
    int32_t bottom = std::max(this->y_ + this->h_, y + static_cast<int32_t>(h));

    setCoords(left, top, right, bottom);
    return true;
}

TpRect TpRect::operator=(const TpRect &other)
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

bool TpRect::operator==(const TpRect &other)
{
    return this->x_ == other.x_ &&
           this->y_ == other.y_ &&
           this->w_ == other.w_ &&
           this->h_ == other.h_;
}

bool TpRect::operator!=(const TpRect &other)
{
    return !(*this == other);
}