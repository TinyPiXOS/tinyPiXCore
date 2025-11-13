#include "TpPainterPath.h"
#include "TpPainterPath_p.h"

TpPainterPath::TpPainterPath()
{
    data_ = new TpPainterPathData();
}

TpPainterPath::TpPainterPath(const TpPainterPath &other)
{
    TpPainterPathData *otherData = static_cast<TpPainterPathData *>(other.data_);
    TpPainterPathData *newData = new TpPainterPathData();
    newData->elements = otherData->elements;
    newData->currentPoint = otherData->currentPoint;
    newData->startPoint = otherData->startPoint;
    newData->isClosed = otherData->isClosed;
    data_ = newData;
}

TpPainterPath::TpPainterPath(const TpPoint &startPoint)
{
    TpPainterPathData *pathData = new TpPainterPathData();
    pathData->currentPoint = startPoint;
    pathData->startPoint = startPoint;
    pathData->addElement(TpPathElementType::MoveTo, TpVector<TpPoint>{startPoint});
    data_ = pathData;
}

void TpPainterPath::moveTo(const TpPoint &point)
{
    TpPainterPathData *pathData = static_cast<TpPainterPathData *>(data_);
    pathData->addElement(TpPathElementType::MoveTo, {point});
}

void TpPainterPath::lineTo(const TpPoint &endPoint)
{
    TpPainterPathData *pathData = static_cast<TpPainterPathData *>(data_);
    pathData->addElement(TpPathElementType::LineTo, {endPoint});
}

void TpPainterPath::cubicTo(const TpPoint &controlPoint1,
                            const TpPoint &controlPoint2,
                            const TpPoint &endPoint)
{
    TpPainterPathData *pathData = static_cast<TpPainterPathData *>(data_);
    pathData->addElement(TpPathElementType::CubicTo, {controlPoint1, controlPoint2, endPoint});
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
    const float k = 0.5522847498f; // 贝塞尔曲线近似圆弧的常数
    float w = rect.width() / 2.0f;
    float h = rect.height() / 2.0f;
    TpPoint center = rect.center();

    // 四个端点
    TpPoint p0 = center + TpPoint(w, 0);
    TpPoint p1 = center + TpPoint(0, h);
    TpPoint p2 = center + TpPoint(-w, 0);
    TpPoint p3 = center + TpPoint(0, -h);

    // 控制点
    TpPoint p0_c1 = p0 + TpPoint(0, k * h);
    TpPoint p0_c2 = p1 + TpPoint(k * w, 0);

    TpPoint p1_c1 = p1 + TpPoint(-k * w, 0);
    TpPoint p1_c2 = p2 + TpPoint(0, k * h);

    TpPoint p2_c1 = p2 + TpPoint(0, -k * h);
    TpPoint p2_c2 = p3 + TpPoint(-k * w, 0);

    TpPoint p3_c1 = p3 + TpPoint(k * w, 0);
    TpPoint p3_c2 = p0 + TpPoint(0, -k * h);

    moveTo(p0);
    cubicTo(p0_c1, p0_c2, p1);
    cubicTo(p1_c1, p1_c2, p2);
    cubicTo(p2_c1, p2_c2, p3);
    cubicTo(p3_c1, p3_c2, p0);
    closeSubpath();
}

void TpPainterPath::addRoundedRect(const TpRect &rect, float radius)
{
    float rad = std::min(radius, std::min(rect.width(), rect.height()) / 2.0f);
    TpPoint topLeft = rect.topLeft();
    TpPoint topRight = rect.topRight();
    TpPoint bottomRight = rect.bottomRight();
    TpPoint bottomLeft = rect.bottomLeft();

    // 圆角矩形的四个角
    moveTo(topLeft + TpPoint(rad, 0));
    lineTo(topRight - TpPoint(rad, 0));

    // 右上角圆弧
    cubicTo(topRight - TpPoint(rad, 0),
            topRight + TpPoint(0, rad),
            topRight + TpPoint(0, rad));

    lineTo(bottomRight - TpPoint(0, rad));

    // 右下角圆弧
    cubicTo(bottomRight - TpPoint(0, rad),
            bottomRight - TpPoint(rad, 0),
            bottomRight - TpPoint(rad, 0));

    lineTo(bottomLeft + TpPoint(rad, 0));

    // 左下角圆弧
    cubicTo(bottomLeft + TpPoint(rad, 0),
            bottomLeft + TpPoint(0, rad),
            bottomLeft + TpPoint(0, rad));

    lineTo(topLeft + TpPoint(0, rad));

    // 左上角圆弧
    cubicTo(topLeft + TpPoint(0, rad),
            topLeft + TpPoint(rad, 0),
            topLeft + TpPoint(rad, 0));

    closeSubpath();
}

void TpPainterPath::addArc(const TpPoint &center, float radius, float startAngle, float endAngle)
{
    TpPainterPathData *pathData = static_cast<TpPainterPathData *>(data_);

    if (radius <= 0 || tpFuzzyCompare(startAngle, endAngle) || startAngle > endAngle)
        return;

    // 将角度转换为弧度
    float startRad = startAngle * M_PI / 180.0f;
    float sweepRad = (endAngle - startRad) * M_PI / 180.0f;

    // 确定圆弧分段数（每90度至少4段）
    int segments = static_cast<int>(std::ceil(std::abs(sweepRad) / (M_PI / 2.0f))) * 4;
    float segmentAngle = sweepRad / segments;

    // 计算起始点
    float angle = startRad;
    TpPoint startPoint(center.x() + radius * std::cos(angle),
                       center.y() + radius * std::sin(angle));
    moveTo(startPoint);

    // 添加圆弧段
    for (int i = 1; i <= segments; ++i)
    {
        angle = startRad + i * segmentAngle;
        TpPoint endPoint(center.x() + radius * std::cos(angle),
                         center.y() + radius * std::sin(angle));

        // 使用二次贝塞尔曲线近似圆弧段
        float midAngle = startRad + (i - 0.5f) * segmentAngle;
        TpPoint controlPoint(center.x() + radius * std::cos(midAngle) / std::cos(segmentAngle / 2),
                             center.y() + radius * std::sin(midAngle) / std::cos(segmentAngle / 2));

        // 使用二次贝塞尔曲线（用三次贝塞尔曲线模拟）
        TpPoint p1 = pathData->currentPoint + (controlPoint - pathData->currentPoint) * (2.0f / 3.0f);
        TpPoint p2 = endPoint + (controlPoint - endPoint) * (2.0f / 3.0f);
        cubicTo(p1, p2, endPoint);
    }
}

void TpPainterPath::addPie(const TpPoint &center, float radius, float startAngle, float endAngle)
{
    if (radius <= 0 || tpFuzzyCompare(startAngle, endAngle) || startAngle > endAngle)
        return;

    // 将角度转换为弧度
    float startRad = startAngle * M_PI / 180.0f;
    float sweepRad = (endAngle - startRad) * M_PI / 180.0f;

    // 计算起始点和结束点
    TpPoint startPoint(center.x() + radius * std::cos(startRad),
                       center.y() + radius * std::sin(startRad));
    TpPoint endPoint(center.x() + radius * std::cos(startRad + sweepRad),
                     center.y() + radius * std::sin(startRad + sweepRad));

    // 移动到圆心，然后添加线段到起始点
    moveTo(center);
    lineTo(startPoint);

    // 添加圆弧
    addArc(center, radius, startAngle, endAngle);

    // 闭合路径（回到圆心）
    lineTo(center);
    closeSubpath();
}

void TpPainterPath::closeSubpath()
{
    TpPainterPathData *pathData = static_cast<TpPainterPathData *>(data_);
    if (!pathData->isClosed && pathData->currentPoint != pathData->startPoint)
    {
        pathData->addElement(TpPathElementType::CloseSubpath, {});
    }
}

void TpPainterPath::clear()
{
    TpPainterPathData *pathData = static_cast<TpPainterPathData *>(data_);
    pathData->elements.clear();
    pathData->currentPoint = TpPoint();
    pathData->startPoint = TpPoint();
    pathData->isClosed = false;
}

bool TpPainterPath::isEmpty() const
{
    TpPainterPathData *pathData = static_cast<TpPainterPathData *>(data_);
    return pathData->elements.empty();
}

TpRect TpPainterPath::boundingRect() const
{
    TpPainterPathData *pathData = static_cast<TpPainterPathData *>(data_);
    if (pathData->elements.empty())
    {
        return TpRect();
    }

    float minX = std::numeric_limits<float>::max();
    float minY = std::numeric_limits<float>::max();
    float maxX = std::numeric_limits<float>::lowest();
    float maxY = std::numeric_limits<float>::lowest();

    for (const auto &element : pathData->elements)
    {
        for (const auto &point : element.points)
        {
            minX = TP_MIN(minX, point.x());
            minY = TP_MIN(minY, point.y());
            maxX = TP_MAX(maxX, point.x());
            maxY = TP_MAX(maxY, point.y());
        }
    }

    return TpRect(minX, minY, maxX - minX, maxY - minY);
}

TpPainterPath TpPainterPath::operator+(const TpPainterPath &other) const
{
    TpPainterPath result(*this);
    result += other;
    return result;
}

TpPainterPath &TpPainterPath::operator+=(const TpPainterPath &other)
{
    TpPainterPathData *pathData = static_cast<TpPainterPathData *>(data_);
    TpPainterPathData *otherData = static_cast<TpPainterPathData *>(other.data_);

    // 添加所有元素
    for (const auto &element : otherData->elements)
    {
        pathData->elements.push_back(element);
    }

    // 更新当前点和起点
    pathData->currentPoint = otherData->currentPoint;
    pathData->startPoint = otherData->startPoint;
    pathData->isClosed = otherData->isClosed;

    return *this;
}
