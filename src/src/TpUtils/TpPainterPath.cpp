#include "TpPainterPath.h"
#include <algorithm>
#include <cmath>
#include "TpVector.h"

// 路径元素类型
enum class TpPathElementType
{
    MoveTo,
    LineTo,
    CubicTo,
    CloseSubpath
};

// 路径元素结构
struct TpPathElement
{
    TpPathElementType type;
    TpVector<TpPoint> points;

    TpPathElement()
    {
    }
    TpPathElement(TpPathElementType type, const TpVector<TpPoint> &points)
        : type(type), points(points)
    {
    }
};

// 路径数据实现
struct TpPainterPathDataImpl
{
    TpVector<TpPathElement> elements;
    TpPoint currentPoint;
    TpPoint startPoint; // 当前子路径的起点
    bool isClosed = false;

    // 添加元素并更新当前点
    void addElement(TpPathElementType type, const TpVector<TpPoint> &pts)
    {
        elements.emplace_back(TpPathElement(type, pts));

        if (!pts.empty())
        {
            switch (type)
            {
            case TpPathElementType::MoveTo:
                currentPoint = pts[0];
                startPoint = currentPoint;
                isClosed = false;
                break;
            case TpPathElementType::LineTo:
                currentPoint = pts[0];
                break;
            case TpPathElementType::CubicTo:
                if (pts.size() >= 3)
                {
                    currentPoint = pts[2];
                }
                break;
            case TpPathElementType::CloseSubpath:
                currentPoint = startPoint;
                isClosed = true;
                break;
            }
        }
    }
};

TpPainterPath::TpPainterPath()
{
    data_ = new TpPainterPathDataImpl();
}

TpPainterPath::TpPainterPath(const TpPainterPath &other)
{
    TpPainterPathDataImpl *otherData = static_cast<TpPainterPathDataImpl *>(other.data_);
    TpPainterPathDataImpl *newData = new TpPainterPathDataImpl();
    newData->elements = otherData->elements;
    newData->currentPoint = otherData->currentPoint;
    newData->startPoint = otherData->startPoint;
    newData->isClosed = otherData->isClosed;
    data_ = newData;
}

TpPainterPath::TpPainterPath(const TpPoint &startPoint)
{
    TpPainterPathDataImpl *pathData = new TpPainterPathDataImpl();
    pathData->currentPoint = startPoint;
    pathData->startPoint = startPoint;
    pathData->addElement(TpPathElementType::MoveTo, TpVector<TpPoint>{startPoint});
    data_ = pathData;
}

void TpPainterPath::moveTo(const TpPoint &point)
{
    TpPainterPathDataImpl *d = static_cast<TpPainterPathDataImpl *>(data_);
    d->addElement(TpPathElementType::MoveTo, {point});
}

void TpPainterPath::lineTo(const TpPoint &endPoint)
{
    TpPainterPathDataImpl *d = static_cast<TpPainterPathDataImpl *>(data_);
    d->addElement(TpPathElementType::LineTo, {endPoint});
}

void TpPainterPath::cubicTo(const TpPoint &controlPoint1,
                            const TpPoint &controlPoint2,
                            const TpPoint &endPoint)
{
    TpPainterPathDataImpl *d = static_cast<TpPainterPathDataImpl *>(data_);
    d->addElement(TpPathElementType::CubicTo, {controlPoint1, controlPoint2, endPoint});
}

void TpPainterPath::addRect(const TpRect &rect)
{
    moveTo(TpPoint(rect.left(), rect.top()));
    lineTo(TpPoint(rect.right(), rect.top()));
    lineTo(TpPoint(rect.right(), rect.bottom()));
    lineTo(TpPoint(rect.left(), rect.bottom()));
    closeSubpath();
}

void TpPainterPath::addEllipse(const TpRect &rect)
{
    // const float k = 0.5522847498f; // 贝塞尔曲线近似圆弧的常数
    // float w = rect.width() / 2.0f;
    // float h = rect.height() / 2.0f;
    // TpPoint center = rect.center();

    // // 四个端点
    // TpPoint p0 = center + TpPoint(w, 0);
    // TpPoint p1 = center + TpPoint(0, h);
    // TpPoint p2 = center + TpPoint(-w, 0);
    // TpPoint p3 = center + TpPoint(0, -h);

    // // 控制点
    // TpPoint p0_c1 = p0 + TpPoint(0, k * h);
    // TpPoint p0_c2 = p1 + TpPoint(k * w, 0);

    // TpPoint p1_c1 = p1 + TpPoint(-k * w, 0);
    // TpPoint p1_c2 = p2 + TpPoint(0, k * h);

    // TpPoint p2_c1 = p2 + TpPoint(0, -k * h);
    // TpPoint p2_c2 = p3 + TpPoint(-k * w, 0);

    // TpPoint p3_c1 = p3 + TpPoint(k * w, 0);
    // TpPoint p3_c2 = p0 + TpPoint(0, -k * h);

    // moveTo(p0);
    // cubicTo(p0_c1, p0_c2, p1);
    // cubicTo(p1_c1, p1_c2, p2);
    // cubicTo(p2_c1, p2_c2, p3);
    // cubicTo(p3_c1, p3_c2, p0);
    // closeSubpath();
}

void TpPainterPath::addRoundedRect(const TpRect &rect, float radius)
{
    // float rad = std::min(radius, std::min(rect.width(), rect.height()) / 2.0f);
    // TpPoint topLeft = rect.topLeft();
    // TpPoint topRight = rect.topRight();
    // TpPoint bottomRight = rect.bottomRight();
    // TpPoint bottomLeft = rect.bottomLeft();

    // // 圆角矩形的四个角
    // moveTo(topLeft + TpPoint(rad, 0));
    // lineTo(topRight - TpPoint(rad, 0));

    // // 右上角圆弧
    // cubicTo(topRight - TpPoint(rad, 0),
    //         topRight + TpPoint(0, rad),
    //         topRight + TpPoint(0, rad));

    // lineTo(bottomRight - TpPoint(0, rad));

    // // 右下角圆弧
    // cubicTo(bottomRight - TpPoint(0, rad),
    //         bottomRight - TpPoint(rad, 0),
    //         bottomRight - TpPoint(rad, 0));

    // lineTo(bottomLeft + TpPoint(rad, 0));

    // // 左下角圆弧
    // cubicTo(bottomLeft + TpPoint(rad, 0),
    //         bottomLeft + TpPoint(0, rad),
    //         bottomLeft + TpPoint(0, rad));

    // lineTo(topLeft + TpPoint(0, rad));

    // // 左上角圆弧
    // cubicTo(topLeft + TpPoint(0, rad),
    //         topLeft + TpPoint(rad, 0),
    //         topLeft + TpPoint(rad, 0));

    // closeSubpath();
}

void TpPainterPath::closeSubpath()
{
    // TpPainterPathDataImpl *d = static_cast<TpPainterPathDataImpl *>(data_);
    // if (!d->isClosed && d->currentPoint != d->startPoint)
    // {
    //     d->addElement(TpPathElementType::CloseSubpath, {});
    // }
}

void TpPainterPath::clear()
{
    TpPainterPathDataImpl *d = static_cast<TpPainterPathDataImpl *>(data_);
    d->elements.clear();
    d->currentPoint = TpPoint();
    d->startPoint = TpPoint();
    d->isClosed = false;
}

bool TpPainterPath::isEmpty() const
{
    TpPainterPathDataImpl *d = static_cast<TpPainterPathDataImpl *>(data_);
    return d->elements.empty();
}

TpRect TpPainterPath::boundingRect() const
{
    return TpRect();
    // TpPainterPathDataImpl *d = static_cast<TpPainterPathDataImpl *>(data_);
    // if (d->elements.empty())
    // {
    //     return TpRect();
    // }

    // float minX = std::numeric_limits<float>::max();
    // float minY = std::numeric_limits<float>::max();
    // float maxX = std::numeric_limits<float>::lowest();
    // float maxY = std::numeric_limits<float>::lowest();

    // for (const auto &element : d->elements)
    // {
    //     for (const auto &point : element.points)
    //     {
    //         minX = std::min(minX, point.x());
    //         minY = std::min(minY, point.y());
    //         maxX = std::max(maxX, point.x());
    //         maxY = std::max(maxY, point.y());
    //     }
    // }

    // return TpRect(minX, minY, maxX - minX, maxY - minY);
}

TpPainterPath &TpPainterPath::operator+=(const TpPainterPath &other)
{
    TpPainterPathDataImpl *d = static_cast<TpPainterPathDataImpl *>(data_);
    TpPainterPathDataImpl *otherData = static_cast<TpPainterPathDataImpl *>(other.data_);

    // 添加所有元素
    for (const auto &element : otherData->elements)
    {
        d->elements.push_back(element);
    }

    // 更新当前点和起点
    d->currentPoint = otherData->currentPoint;
    d->startPoint = otherData->startPoint;
    d->isClosed = otherData->isClosed;

    return *this;
}