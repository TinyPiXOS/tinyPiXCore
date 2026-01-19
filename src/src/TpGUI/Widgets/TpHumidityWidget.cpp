#include <TpHumidityWidget.h>
#include <TpPainter.h>
#include <TpLinearGradient.h>
#include <TpFont.h>
#include <TpEvent.h>

struct TpHumidityWidgetData
{
    TpString titleText = "湿度";

    // 最大、最小温度；当前温度
    int32_t maxValue = 100;
    int32_t minValue = 0;
    int32_t curValue = 60;

    int32_t lineWidth = 3;

    TpVector<int32_t> colorList;

    TpFont titleTextFont;
    TpFont percentFont;
};

// 求解三次方程实数根
std::vector<double> solveCubicEquation(double a, double b, double c, double d)
{
    std::vector<double> roots;

    if (fabs(a) < 1e-12)
    {
        // 退化为二次方程
        if (fabs(b) < 1e-12)
        {
            // 退化为一次方程
            if (fabs(c) > 1e-12)
            {
                roots.push_back(-d / c);
            }
            return roots;
        }

        double discriminant = c * c - 4 * b * d;
        if (discriminant >= 0)
        {
            double sqrtDisc = sqrt(discriminant);
            roots.push_back((-c + sqrtDisc) / (2 * b));
            roots.push_back((-c - sqrtDisc) / (2 * b));
        }
        return roots;
    }

    // 归一化三次方程: t³ + p*t² + q*t + r = 0
    double p = b / a;
    double q = c / a;
    double r = d / a;

    double p2 = p * p;
    double Q = (3 * q - p2) / 9.0;
    double R = (9 * p * q - 27 * r - 2 * p * p2) / 54.0;
    double D = Q * Q * Q + R * R;

    if (D > 0)
    {
        // 一个实根，两个复根
        double sqrtD = sqrt(D);
        double S = cbrt(R + sqrtD);
        double T = cbrt(R - sqrtD);
        roots.push_back(S + T - p / 3.0);
    }
    else
    {
        // 三个实根
        double theta = acos(R / sqrt(-Q * Q * Q));
        double sqrtNegQ = 2 * sqrt(-Q);
        roots.push_back(sqrtNegQ * cos(theta / 3) - p / 3);
        roots.push_back(sqrtNegQ * cos((theta + 2 * M_PI) / 3) - p / 3);
        roots.push_back(sqrtNegQ * cos((theta + 4 * M_PI) / 3) - p / 3);
    }

    // 过滤在[0,1]范围内的根
    std::vector<double> validRoots;
    for (double t : roots)
    {
        if (t >= 0 && t <= 1)
        {
            validRoots.push_back(t);
        }
    }

    return validRoots;
}

// 计算贝塞尔曲线上某点的坐标
TpPoint bezierTpPoint(const TpPoint &p0, const TpPoint &p1, const TpPoint &p2, const TpPoint &p3, double t)
{
    double u = 1 - t;
    double uu = u * u;
    double uuu = uu * u;
    double tt = t * t;
    double ttt = tt * t;

    double x = uuu * p0.x() + 3 * uu * t * p1.x() + 3 * u * tt * p2.x() + ttt * p3.x();
    double y = uuu * p0.y() + 3 * uu * t * p1.y() + 3 * u * tt * p2.y() + ttt * p3.y();

    return TpPoint(x, y);
}

// 使用De Casteljau 德卡斯特里奥 算法计算部分曲线的控制点
void getPartialBezierControls(const TpPoint &p0, const TpPoint &p1, const TpPoint &p2, const TpPoint &p3,
                              double t, TpPoint &newP1, TpPoint &newP2, TpPoint &endTpPoint)
{
    // 第一级中间点
    TpPoint q0 = TpPoint((1 - t) * p0.x() + t * p1.x(), (1 - t) * p0.y() + t * p1.y());
    TpPoint q1 = TpPoint((1 - t) * p1.x() + t * p2.x(), (1 - t) * p1.y() + t * p2.y());
    TpPoint q2 = TpPoint((1 - t) * p2.x() + t * p3.x(), (1 - t) * p2.y() + t * p3.y());

    // 第二级中间点
    TpPoint r0 = TpPoint((1 - t) * q0.x() + t * q1.x(), (1 - t) * q0.y() + t * q1.y());
    TpPoint r1 = TpPoint((1 - t) * q1.x() + t * q2.x(), (1 - t) * q1.y() + t * q2.y());

    // 第三级中间点（曲线上的点）
    TpPoint s0 = TpPoint((1 - t) * r0.x() + t * r1.x(), (1 - t) * r0.y() + t * r1.y());

    // 部分曲线的控制点
    newP1 = q0;      // 第一个控制点
    newP2 = r0;      // 第二个控制点
    endTpPoint = s0; // 终点（交点）
}

// 获取从起点到水平线交点的部分曲线控制点
bool getBezierIntersectionSegment(const TpPoint &p0, const TpPoint &p1, const TpPoint &p2, const TpPoint &p3,
                                  double lineY, TpPoint &segmentP0, TpPoint &segmentP1,
                                  TpPoint &segmentP2, TpPoint &segmentP3)
{
    // 计算贝塞尔曲线与水平线交点的参数
    double a = -p0.y() + 3 * p1.y() - 3 * p2.y() + p3.y();
    double b = 3 * p0.y() - 6 * p1.y() + 3 * p2.y();
    double c = -3 * p0.y() + 3 * p1.y();
    double d = p0.y() - lineY;

    std::vector<double> tValues = solveCubicEquation(a, b, c, d);

    // 无交点
    if (tValues.empty())
    {
        return false;
    }

    // 取最小的t值（最靠近起点的交点）
    double t = *std::min_element(tValues.begin(), tValues.end());

    // 使用De Casteljau算法计算部分曲线的控制点
    TpPoint newP1, newP2, endTpPoint;
    getPartialBezierControls(p0, p1, p2, p3, t, newP1, newP2, endTpPoint);

    // 设置输出参数
    segmentP0 = p0;         // 起点不变
    segmentP1 = newP1;      // 新的第一个控制点
    segmentP2 = newP2;      // 新的第二个控制点
    segmentP3 = endTpPoint; // 终点（交点）

    return true;
}

TpHumidityWidget::TpHumidityWidget(TpWidget *parent) : TpWidget(parent)
{
    TpHumidityWidgetData *tempData = new TpHumidityWidgetData();
    data_ = tempData;

    tempData->colorList.emplace_back(_RGB(204, 242, 252));
    tempData->colorList.emplace_back(_RGB(34, 132, 255));

    tempData->titleTextFont.setFontColor(_RGB(54, 59, 100));
    tempData->titleTextFont.setFontSize(13);

    tempData->percentFont.setFontColor(_RGB(54, 59, 100));
    tempData->percentFont.setFontSize(13);
}

TpHumidityWidget::~TpHumidityWidget()
{
    TpHumidityWidgetData *tempData = static_cast<TpHumidityWidgetData *>(data_);
    if (tempData)
    {
        delete tempData;
        tempData = nullptr;
        data_ = nullptr;
    }
}

void TpHumidityWidget::setTitle(const TpString &title)
{
    TpHumidityWidgetData *tempData = static_cast<TpHumidityWidgetData *>(data_);
    tempData->titleText = title;
}

TpString TpHumidityWidget::title() const
{
    TpHumidityWidgetData *tempData = static_cast<TpHumidityWidgetData *>(data_);
    return tempData->titleText;
}

void TpHumidityWidget::setRange(int32_t minTemp, int32_t maxTemp)
{
    TpHumidityWidgetData *tempData = static_cast<TpHumidityWidgetData *>(data_);
    tempData->minValue = minTemp;
    tempData->maxValue = maxTemp;

    if (tempData->minValue >= tempData->maxValue)
        tempData->minValue = tempData->maxValue - 1;

    update();
}

void TpHumidityWidget::setValue(int32_t currentTemp)
{
    TpHumidityWidgetData *tempData = static_cast<TpHumidityWidgetData *>(data_);
    tempData->curValue = currentTemp;

    if (tempData->curValue > tempData->maxValue)
        tempData->curValue = tempData->maxValue;
    if (tempData->curValue < tempData->minValue)
        tempData->curValue = tempData->minValue;

    update();
}

int32_t TpHumidityWidget::value() const
{
    TpHumidityWidgetData *tempData = static_cast<TpHumidityWidgetData *>(data_);
    return tempData->curValue;
}

void TpHumidityWidget::setColorList(const TpVector<int32_t> &colorList)
{
    if (colorList.size() == 0)
        return;
    TpHumidityWidgetData *tempData = static_cast<TpHumidityWidgetData *>(data_);
    tempData->colorList = colorList;
}

void TpHumidityWidget::setLineWidth(int32_t width)
{
    TpHumidityWidgetData *tempData = static_cast<TpHumidityWidgetData *>(data_);
    tempData->lineWidth = width;
    update();
}

int32_t TpHumidityWidget::lineWidth() const
{
    TpHumidityWidgetData *tempData = static_cast<TpHumidityWidgetData *>(data_);
    return tempData->lineWidth;
}

void TpHumidityWidget::setWidth(const int32_t &width)
{
    TpWidget::setWidth(width);

    // 保证宽高比例为2:3
    TpWidget::setHeight(1.0 * width * 3.0 / 2.0);
}

void TpHumidityWidget::setHeight(const int32_t &height)
{
    // 设置高度无效
}

bool TpHumidityWidget::onPaintEvent(TpPaintEvent *event)
{
    TpHumidityWidgetData *tempData = static_cast<TpHumidityWidgetData *>(data_);

    int32_t startY = 10;

    int32_t humidityWidth = width() * 0.8;
    int32_t humidityHeight = height() * 0.75;

    TpPainter *painter = event->painter();

    int32_t arcRadius = humidityWidth / 2.0;

    // 先绘制半圆弧，X位置窗口中心位置
    int32_t arcX = (width() - humidityWidth) / 2.0 + arcRadius;
    int32_t arcY = startY + humidityHeight - arcRadius;

    // 计算左侧曲线（从圆弧左端点到水滴底部尖点）
    // 顶点坐标（arcX, startY）
    // 控制点1坐标
    TpPoint leftCTpPoint1(arcX - arcRadius, arcY - arcRadius * 0.6);
    // 控制点2坐标
    TpPoint leftCTpPoint2(arcX - arcRadius * 0.6, startY + arcRadius * 0.5);

    // 计算右侧曲线（从水滴底部尖点到圆弧右端点）
    // 控制点1坐标
    TpPoint rightCTpPoint1(arcX + arcRadius * 0.6, startY + arcRadius * 0.5);
    // 控制点2坐标
    TpPoint rightCTpPoint2(arcX + arcRadius, arcY - arcRadius * 0.6);

    // 计算填充值百分比
    float percentValue = 1.0 * (tempData->curValue - tempData->minValue) / (tempData->maxValue - tempData->minValue);

    // 绘制填充渐变
    // 构建渐变效果
    int32_t colorListSize = tempData->colorList.size();

    TpLinearGradient lineGradient;
    for (int i = 0; i < colorListSize; ++i)
    {
        lineGradient.setColorAt(1.0 * i / (colorListSize - 1), tempData->colorList.at(i));
    }

    if (colorListSize > 0)
    {
        lineGradient.setStart(arcX, arcY + arcRadius);
        lineGradient.setFinalStop(arcX, startY);
        painter->setBrush(TpBrush(&lineGradient));
    }

    // 水滴总高度，用于计算填充绘制高度
    float valueHeight = percentValue * humidityHeight;
    int32_t fillCircleRadius = arcRadius - tempData->lineWidth;

    if (tempData->curValue <= tempData->minValue)
    {
        // 小于等于最小值；什么也不绘制
    }
    else if (tempData->curValue >= tempData->maxValue)
    {
        // 大于等于最大值，绘制完全填充
        painter->drawPie(arcX, arcY, arcRadius, 0, 180);

        // 绘制上半区域
        TpPainterPath upPath;
        upPath.moveTo(TpPoint(arcX - arcRadius, arcY));
        upPath.cubicTo(TpPoint(leftCTpPoint1.x(), leftCTpPoint1.y()),
                       TpPoint(leftCTpPoint2.x(), leftCTpPoint2.y()),
                       TpPoint(arcX, startY));

        upPath.cubicTo(TpPoint(rightCTpPoint1.x(), rightCTpPoint1.y()),
                       TpPoint(rightCTpPoint2.x(), rightCTpPoint2.y()),
                       TpPoint(arcX + arcRadius, arcY));

        upPath.closeSubpath();

        painter->drawPath(upPath);
    }
    else
    {
        if (valueHeight < arcRadius)
        {
            // 先计算弦与圆相交的两个点坐标
            TpPoint leftTpPoint, rightTpPoint;

            // 弦的一半长度
            float lineW = 0;

            // 镂空一个三角形
            TpHollowMask hollowMask;
            TpHollowMask::PolygonHollow polygonHollow;

            float startAngle = 0;
            float endAngle = 0;
            double cosValue = 0;

            lineW = std::sqrt(std::pow(arcRadius, 2) - std::pow(arcRadius - valueHeight, 2));

            leftTpPoint.setX(arcX - lineW);
            leftTpPoint.setY(arcY + arcRadius - valueHeight);

            rightTpPoint.setX(arcX + lineW);
            rightTpPoint.setY(arcY + arcRadius - valueHeight);

            // 在扇形基础上镂空一个三角形
            polygonHollow.posintList.emplace_back(TpPoint(arcX, arcY));
            polygonHollow.posintList.emplace_back(TpPoint(leftTpPoint.x(), leftTpPoint.y()));
            polygonHollow.posintList.emplace_back(TpPoint(rightTpPoint.x(), rightTpPoint.y()));
            hollowMask.addPolygonHollow(polygonHollow);

            // 余弦定理 计算弦两个顶点的角度
            cosValue = (leftTpPoint.x() - arcX) * (rightTpPoint.x() - arcX) +
                       (leftTpPoint.y() - arcY) * (rightTpPoint.y() - arcY);
            cosValue /= (std::sqrt(std::pow(leftTpPoint.x() - arcX, 2) + std::pow(leftTpPoint.y() - arcY, 2)) *
                         std::sqrt(std::pow(rightTpPoint.x() - arcX, 2) + std::pow(rightTpPoint.y() - arcY, 2)));
            float angle = std::acos(cosValue) * 180.0 / M_PI;
            startAngle = (180 - angle) / 2.0;
            endAngle = startAngle + angle;

            // 防止浮点误差超出范围
            if (cosValue < -1.0)
                cosValue = -1.0;
            else if (cosValue > 1.0)
                cosValue = 1.0;

            painter->drawPie(arcX, arcY, arcRadius, startAngle, endAngle, hollowMask);
        }
        else
        {
            // 高度末尾处于上半区域
            // 先绘制半圆
            painter->drawPie(arcX, arcY, arcRadius, 0, 180);

            // 绘制半填充；需要计算与贝塞尔曲线的交点
            TpPoint leftSegP0, leftSegP1, leftSegP2, leftSegP3;
            TpPoint rightSegP0, rightSegP1, rightSegP2, rightSegP3;
            getBezierIntersectionSegment(TpPoint(arcX - arcRadius, arcY),
                                         leftCTpPoint1,
                                         leftCTpPoint2,
                                         TpPoint(arcX, startY),
                                         arcY + arcRadius - valueHeight + tempData->lineWidth * 2,
                                         leftSegP0, leftSegP1, leftSegP2, leftSegP3);

            // 右侧贝塞尔曲线，需要从底部到顶部端点
            getBezierIntersectionSegment(TpPoint(arcX + arcRadius, arcY),
                                         rightCTpPoint2,
                                         rightCTpPoint1,
                                         TpPoint(arcX, startY),
                                         arcY + arcRadius - valueHeight + tempData->lineWidth * 2,
                                         rightSegP0, rightSegP1, rightSegP2, rightSegP3);

            // 绘制上半区域
            TpPainterPath upPath;

            // 从左侧贝塞尔曲线起点开始（底部左端点）
            upPath.moveTo(leftSegP0);

            // 绘制左曲线：从起点到交点（部分曲线）
            upPath.cubicTo(leftSegP1, leftSegP2, leftSegP3);

            upPath.lineTo(rightSegP3);

            // 绘制右曲线：控制点顺序反向
            upPath.cubicTo(rightSegP2, rightSegP1, rightSegP0);

            upPath.closeSubpath();

            painter->drawPath(upPath);
        }
    }

    // 绘制边框
    painter->setPen(_RGB(116, 121, 150));
    painter->pen().setWidth(tempData->lineWidth);
    // 重置渐变
    painter->setBrush(TpBrush(Tp::NoBrush));

    painter->drawArc(arcX, arcY, arcRadius, 1, 179);

    // 绘制左侧曲线：从圆弧左端点到顶点
    painter->drawCubic(
        arcX - arcRadius, arcY,               // 起点：圆弧左端点
        leftCTpPoint1.x(), leftCTpPoint1.y(), // 控制点1
        leftCTpPoint2.x(), leftCTpPoint2.y(), // 控制点2
        arcX, startY);                        // 终点：顶点

    // 绘制右侧曲线：从顶点到圆弧右端点
    painter->drawCubic(
        arcX, startY,                           // 起点：顶点
        rightCTpPoint1.x(), rightCTpPoint1.y(), // 控制点1
        rightCTpPoint2.x(), rightCTpPoint2.y(), // 控制点2
        arcX + arcRadius, arcY);                // 终点：圆弧右端点

    // 绘制百分比文本
    tempData->percentFont.setText(TpString::number(int32_t(percentValue * 100)) + "%");
    int32_t percentTextWidth = tempData->percentFont.pixelWidth();
    int32_t percentTextHeight = tempData->percentFont.pixelHeight();
    int32_t percentTextX = (width() - percentTextWidth) / 2.0;
    // int32_t percentTextY = height() - percentTextHeight;
    painter->drawText(tempData->percentFont, percentTextX, arcY);

    // 绘制标题文本
    tempData->titleTextFont.setText(tempData->titleText);
    int32_t titleTextWidth = tempData->titleTextFont.pixelWidth();
    int32_t titleTextHeight = tempData->titleTextFont.pixelHeight();
    int32_t titleTextX = (width() - titleTextWidth) / 2.0;
    int32_t titleTextY = height() - titleTextHeight;
    painter->drawText(tempData->titleTextFont, titleTextX, titleTextY);

    return true;
}
