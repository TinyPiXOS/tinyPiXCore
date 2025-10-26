#include "TpRect.h"
#include <algorithm>

// 定义矩形数据结构体
struct TpRectData
{
    int32_t x = 0; // 矩形左上角x坐标
    int32_t y = 0; // 矩形左上角y坐标
    int32_t w = 0; // 矩形宽度
    int32_t h = 0; // 矩形高度
};

TpRect::TpRect()
{
    data_ = new TpRectData();
}

TpRect::TpRect(const TpRect &other)
{
    TpRectData *rectData = new TpRectData();
    TpRectData *othersData = static_cast<TpRectData *>(other.data_);
    *rectData = *othersData;
    data_ = rectData;
}

TpRect::TpRect(const TpPoint &leftTop, const TpPoint &rightBottom)
{
    TpRectData *rectData = new TpRectData();
    rectData->x = leftTop.x();
    rectData->y = leftTop.y();
    rectData->w = rightBottom.x() - leftTop.x();
    rectData->h = rightBottom.y() - leftTop.y();

    data_ = rectData;
}

TpRect::TpRect(const TpPoint &leftTop, const TpSize &size)
{
    TpRectData *rectData = new TpRectData();
    rectData->x = leftTop.x();
    rectData->y = leftTop.y();
    rectData->w = size.width();
    rectData->h = size.height();

    data_ = rectData;
}

TpRect::TpRect(int32_t x, int32_t y, int32_t w, int32_t h)
{
    TpRectData *rectData = new TpRectData();
    rectData->x = x;
    rectData->y = y;
    rectData->w = w;
    rectData->h = h;

    data_ = rectData;
}

TpRect::~TpRect()
{
    TpRectData *rectData = static_cast<TpRectData *>(data_);
    if (rectData)
    {
        delete rectData;
        rectData = nullptr;
        data_ = nullptr;
    }
}

bool TpRect::isNull() const noexcept
{
    TpRectData *rectData = static_cast<TpRectData *>(data_);
    return (rectData->w == 0 && rectData->h == 0);
}

bool TpRect::isEmpty() const noexcept
{
    TpRectData *rectData = static_cast<TpRectData *>(data_);
    return (rectData->w <= 0 || rectData->h <= 0);
}

bool TpRect::isValid() const noexcept
{
    TpRectData *rectData = static_cast<TpRectData *>(data_);
    return (rectData->w > 0 && rectData->h > 0);
}

int32_t TpRect::left() const noexcept
{
    TpRectData *rectData = static_cast<TpRectData *>(data_);
    return rectData->x;
}

int32_t TpRect::top() const noexcept
{
    TpRectData *rectData = static_cast<TpRectData *>(data_);
    return rectData->y;
}

int32_t TpRect::right() const noexcept
{
    TpRectData *rectData = static_cast<TpRectData *>(data_);
    return rectData->x + rectData->w;
}

int32_t TpRect::bottom() const noexcept
{
    TpRectData *rectData = static_cast<TpRectData *>(data_);
    return rectData->y + rectData->h;
}

int32_t TpRect::x() const noexcept
{
    TpRectData *rectData = static_cast<TpRectData *>(data_);
    return rectData->x;
}

int32_t TpRect::y() const noexcept
{
    TpRectData *rectData = static_cast<TpRectData *>(data_);
    return rectData->y;
}

void TpRect::setLeft(int32_t pos) noexcept
{
    TpRectData *rectData = static_cast<TpRectData *>(data_);
    rectData->w += rectData->x - pos;
    rectData->x = pos;
}

void TpRect::setTop(int32_t pos) noexcept
{
    TpRectData *rectData = static_cast<TpRectData *>(data_);
    rectData->h += rectData->y - pos;
    rectData->y = pos;
}

void TpRect::setRight(int32_t pos) noexcept
{
    TpRectData *rectData = static_cast<TpRectData *>(data_);
    rectData->w = pos - rectData->x;
}

void TpRect::setBottom(int32_t pos) noexcept
{
    TpRectData *rectData = static_cast<TpRectData *>(data_);
    rectData->h = pos - rectData->y;
}

void TpRect::setX(int32_t x) noexcept
{
    TpRectData *rectData = static_cast<TpRectData *>(data_);
    rectData->x = x;
}

void TpRect::setY(int32_t y) noexcept
{
    TpRectData *rectData = static_cast<TpRectData *>(data_);
    rectData->y = y;
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
    TpRectData *rectData = static_cast<TpRectData *>(data_);
    return TpPoint(rectData->x, rectData->y);
}

TpPoint TpRect::bottomRight() const noexcept
{
    TpRectData *rectData = static_cast<TpRectData *>(data_);
    return TpPoint(rectData->x + rectData->w, rectData->y + rectData->h);
}

TpPoint TpRect::topRight() const noexcept
{
    TpRectData *rectData = static_cast<TpRectData *>(data_);
    return TpPoint(rectData->x + rectData->w, rectData->y);
}

TpPoint TpRect::bottomLeft() const noexcept
{
    TpRectData *rectData = static_cast<TpRectData *>(data_);
    return TpPoint(rectData->x, rectData->y + rectData->h);
}

TpPoint TpRect::center() const noexcept
{
    TpRectData *rectData = static_cast<TpRectData *>(data_);
    return TpPoint(rectData->x + rectData->w / 2, rectData->y + rectData->h / 2);
}

void TpRect::setRect(int32_t x, int32_t y, int32_t w, int32_t h) noexcept
{
    TpRectData *rectData = static_cast<TpRectData *>(data_);
    rectData->x = x;
    rectData->y = y;
    rectData->w = w;
    rectData->h = h;
}

void TpRect::getRect(int32_t *x, int32_t *y, int32_t *w, int32_t *h) const
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

void TpRect::setCoords(int32_t x1, int32_t y1, int32_t x2, int32_t y2) noexcept
{
    TpRectData *rectData = static_cast<TpRectData *>(data_);
    rectData->x = x1;
    rectData->y = y1;
    rectData->w = x2 - x1;
    rectData->h = y2 - y1;
}

void TpRect::getCoords(int32_t *x1, int32_t *y1, int32_t *x2, int32_t *y2) const
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

TpSize TpRect::size() const noexcept
{
    TpRectData *rectData = static_cast<TpRectData *>(data_);
    return TpSize(rectData->w, rectData->h);
}

int32_t TpRect::width() const noexcept
{
    TpRectData *rectData = static_cast<TpRectData *>(data_);
    return rectData->w;
}

int32_t TpRect::height() const noexcept
{
    TpRectData *rectData = static_cast<TpRectData *>(data_);
    return rectData->h;
}

void TpRect::setWidth(int32_t w) noexcept
{
    TpRectData *rectData = static_cast<TpRectData *>(data_);
    rectData->w = w;
}

void TpRect::setHeight(int32_t h) noexcept
{
    TpRectData *rectData = static_cast<TpRectData *>(data_);
    rectData->h = h;
}

void TpRect::setSize(const TpSize &s) noexcept
{
    TpRectData *rectData = static_cast<TpRectData *>(data_);
    rectData->w = s.width();
    rectData->h = s.height();
}

bool TpRect::contains(int32_t x, int32_t y)
{
    TpRectData *rectData = static_cast<TpRectData *>(data_);
    return (x >= rectData->x) && (x <= rectData->x + rectData->w) &&
           (y >= rectData->y) && (y <= rectData->y + rectData->h);
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
    TpRectData *rectData = static_cast<TpRectData *>(data_);

    int32_t left = std::max(rectData->x, x);
    int32_t right = std::min(rectData->x + rectData->w, x + static_cast<int32_t>(w));
    int32_t top = std::max(rectData->y, y);
    int32_t bottom = std::min(rectData->y + rectData->h, y + static_cast<int32_t>(h));

    return (left < right) && (top < bottom);
}

bool TpRect::unions(const TpRect &rect)
{
    return unions(rect.x(), rect.y(), rect.width(), rect.height());
}

bool TpRect::unions(int32_t x, int32_t y, uint32_t w, uint32_t h)
{
    TpRectData *rectData = static_cast<TpRectData *>(data_);

    if (w == 0 || h == 0)
        return true;

    int32_t left = std::min(rectData->x, x);
    int32_t top = std::min(rectData->y, y);
    int32_t right = std::max(rectData->x + rectData->w, x + static_cast<int32_t>(w));
    int32_t bottom = std::max(rectData->y + rectData->h, y + static_cast<int32_t>(h));

    setCoords(left, top, right, bottom);
    return true;
}

TpRect TpRect::operator=(const TpRect &other)
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

bool TpRect::operator==(const TpRect &other)
{
    TpRectData *rectData = static_cast<TpRectData *>(data_);
    TpRectData *otherData = static_cast<TpRectData *>(other.data_);

    return rectData->x == otherData->x &&
           rectData->y == otherData->y &&
           rectData->w == otherData->w &&
           rectData->h == otherData->h;
}

bool TpRect::operator!=(const TpRect &other)
{
    return !(*this == other);
}