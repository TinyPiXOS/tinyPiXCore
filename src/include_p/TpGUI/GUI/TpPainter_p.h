#ifndef __TP_PAINTER_PRIVATE_H
#define __TP_PAINTER_PRIVATE_H

#include "TpSurface.h"
#include "thorVG/thorvg.h"
#include "TpLinearGradient.h"
#include "TpRadialGradient.h"
#include "TpPen.h"
#include "TpBrush.h"
#include "TpColors.h"
#include "TpVector.h"
#include "TpPainter.h"
#include "TpRect.h"
#include "TpPoint.h"

#define OFFSET_X(painterData, x) (painterData->offsetX + x)
#define OFFSET_Y(painterData, y) (painterData->offsetY + y)

struct TpPainterData
{
    tpShared<TpSurface> TpSurfacePtr = nullptr;
    TpWidget *paintWidget = nullptr;

    // 当前窗体屏幕坐标和宽高
    int32_t offsetX = 0;
    int32_t offsetY = 0;

    TpRect clipRect;

    bool beUsed;

    // 绘制画笔和画刷
    TpPen drawPen;
    TpBrush drawBrush;

    // CPU绘制引擎
    tvg::SwCanvas *swCanvas = nullptr;
    // 使用OpenGL加速的绘制引擎；需有GPU和OpenGL才能使用；暂时无用
    // tvg::GlCanvas *glCanvas = nullptr;

    tvg::Scene *tvgScene = nullptr;

    TpPainterData()
    {
        drawBrush.setStyle(Tp::NoBrush);
    }
};

// 重设canvas的target
static inline void refreshCanvasTarget(TpPainterData *painterData)
{
    // int32_t surfaceWidth = painterData->TpSurfacePtr->width();
    // int32_t surfaceHeight = painterData->TpSurfacePtr->height();

    // painterData->swCanvas->target((uint32_t *)painterData->TpSurfacePtr->matrix(), surfaceWidth, surfaceWidth, surfaceHeight, tvg::ColorSpace::ARGB8888);

    // // 限制绘制区域
    // painterData->swCanvas->viewport(painterData->clipRect.x(), painterData->clipRect.y(), painterData->clipRect.width(), painterData->clipRect.height());

    // std::cout << "裁剪区域： " << painterData->clipRect.x() << " , " << painterData->clipRect.y()
    //           << " , " << painterData->clipRect.width() << " , " << painterData->clipRect.height() << std::endl;
}

// 根据线性渐变角度 计算线性渐变射线与矩形边界的交点
std::pair<TpPoint, TpPoint> calculateRayIntersections(float angleDeg, float width, float height)
{
    const float cx = width / 2.0f;
    const float cy = height / 2.0f;

    // 角度转弧度
    float angle_rad = angleDeg * M_PI / 180.0f;

    // 计算方向向量
    float dx = std::sin(angle_rad);
    float dy = -std::cos(angle_rad);

    // 处理零向量情况
    if (std::abs(dx) < 1e-6f && std::abs(dy) < 1e-6f)
    {
        return {TpPoint(cx, cy), TpPoint(cx, cy)};
    }

    // 计算正方向交点
    float t_positive = std::numeric_limits<float>::max();
    if (std::abs(dx) > 1e-6f)
    {
        float t_left = (0.0f - cx) / dx;
        float t_right = (width - cx) / dx;
        if (t_left > 0)
            t_positive = std::min(t_positive, t_left);
        if (t_right > 0)
            t_positive = std::min(t_positive, t_right);
    }
    if (std::abs(dy) > 1e-6f)
    {
        float t_top = (0.0f - cy) / dy;
        float t_bottom = (height - cy) / dy;
        if (t_top > 0)
            t_positive = std::min(t_positive, t_top);
        if (t_bottom > 0)
            t_positive = std::min(t_positive, t_bottom);
    }

    // 计算负方向交点
    float t_negative = std::numeric_limits<float>::max();
    if (std::abs(dx) > 1e-6f)
    {
        float t_left = (0.0f - cx) / (-dx);
        float t_right = (width - cx) / (-dx);
        if (t_left > 0)
            t_negative = std::min(t_negative, t_left);
        if (t_right > 0)
            t_negative = std::min(t_negative, t_right);
    }
    if (std::abs(dy) > 1e-6f)
    {
        float t_top = (0.0f - cy) / (-dy);
        float t_bottom = (height - cy) / (-dy);
        if (t_top > 0)
            t_negative = std::min(t_negative, t_top);
        if (t_bottom > 0)
            t_negative = std::min(t_negative, t_bottom);
    }

    // 计算交点坐标
    TpPoint endPoint(cx + t_positive * dx, cy + t_positive * dy);
    TpPoint startPoint(cx - t_negative * dx, cy - t_negative * dy);

    return {startPoint, endPoint};
}

// 解析渐变信息；无渐变则返回空指针
static inline tvg::Fill *parseGradientPtr(TpPainterData *painterData)
{
    TpGradient *brushGradientPtr = nullptr;

    TpBrush gradientBrush = painterData->drawBrush;
    TpBrush penBrush = painterData->drawPen.brush();

    if (gradientBrush.style() == Tp::LinearGradientPattern ||
        gradientBrush.style() == Tp::RadialGradientPattern)
    {
        brushGradientPtr = gradientBrush.gradient();
    }
    else
    {
        if (penBrush.style() == Tp::LinearGradientPattern ||
            penBrush.style() == Tp::RadialGradientPattern)
        {
            brushGradientPtr = penBrush.gradient();
        }
    }

    if (!brushGradientPtr)
        return nullptr;

    TpList<std::pair<float, int32_t>> colorList = brushGradientPtr->getColors();
    if (colorList.size() == 0)
        return nullptr;

    tvg::Fill *resGradientPtr = nullptr;

    TpGradient::GradientType gradientType = brushGradientPtr->gradientType();
    if (gradientType == TpGradient::LinearGradient)
    {
        // 创建线性渐变
        TpLinearGradient *linearGrad = dynamic_cast<TpLinearGradient *>(brushGradientPtr);

        tvg::LinearGradient *linearGradient = tvg::LinearGradient::gen();

        if (linearGrad->hasAngle())
        {
            float lineearAngle = linearGrad->angle();
            // 根据角度计算起始点和终止点
            std::pair<TpPoint, TpPoint> pointList = calculateRayIntersections(lineearAngle, painterData->paintWidget->width(), painterData->paintWidget->height());

            linearGradient->linear(painterData->offsetX + pointList.first.x(), painterData->offsetY + pointList.first.y(),
                                   painterData->offsetX + pointList.second.x(), painterData->offsetY + pointList.second.y());
        }
        else
        {
            TpPointF startPoint = linearGrad->start();
            TpPointF stopPoint = linearGrad->finalStop();

            linearGradient->linear(painterData->offsetX + startPoint.x(), painterData->offsetY + startPoint.y(),
                                   painterData->offsetX + stopPoint.x(), painterData->offsetY + stopPoint.y());
        }

        resGradientPtr = linearGradient;
    }
    else if (gradientType == TpGradient::RadialGradient)
    {
        // 创建径向渐变
        TpRadialGradient *radialGrad = dynamic_cast<TpRadialGradient *>(brushGradientPtr);

        TpPointF centerPoint = radialGrad->center();
        float centerRadius = radialGrad->centerRadius();

        TpPointF focalPoint = radialGrad->focalPoint();
        float focalRadius = radialGrad->focalRadius();

        tvg::RadialGradient *radialGradient = tvg::RadialGradient::gen();
        // 设置中心点和半径
        radialGradient->radial(painterData->offsetX + centerPoint.x(), painterData->offsetY + centerPoint.y(), centerRadius,
                               painterData->offsetX + focalPoint.x(), painterData->offsetY + focalPoint.y(), focalRadius);

        resGradientPtr = radialGradient;
    }
    else
    {
    }

    if (!resGradientPtr)
        return resGradientPtr;

    // 设置扩散模式
    tvg::FillSpread spreadMode = (tvg::FillSpread)brushGradientPtr->spread();
    resGradientPtr->spread(spreadMode);

    tvg::Fill::ColorStop colorStops[colorList.size()];
    for (int i = 0; i < colorList.size(); ++i)
    {
        const auto &colorIter = colorList[i];
        // 颜色 (offset, r, g, b, a)
        colorStops[i].offset = colorIter.first;
        colorStops[i].r = _R(colorIter.second);
        colorStops[i].g = _G(colorIter.second);
        colorStops[i].b = _B(colorIter.second);
        colorStops[i].a = _A(colorIter.second);
    };
    resGradientPtr->colorStops(colorStops, colorList.size());

    return resGradientPtr;
}

// 核心圆弧转贝塞尔曲线算法（基于 _pathAppendArcTo）
static void appendArcToPath(tvg::Shape *shape, float startX, float startY,
                            float endX, float endY, float rx, float ry,
                            float angle, bool largeArc, bool sweep)
{
    tvg::Point start = tvg::Point{startX, startY};
    tvg::Point next = tvg::Point{endX, endY};
    tvg::Point radius = tvg::Point{rx, ry};

    float cosPhi = cosf(angle);
    float sinPhi = sinf(angle);

    tvg::Point d2;
    d2.x = (start.x - next.x) * 0.5f;
    d2.y = (start.y - next.y) * 0.5f;
    // auto d2 = (start - next) * 0.5f;

    float x1p = cosPhi * d2.x + sinPhi * d2.y;
    float y1p = cosPhi * d2.y - sinPhi * d2.x;
    float x1p2 = x1p * x1p;
    float y1p2 = y1p * y1p;
    tvg::Point radius2 = tvg::Point{radius.x * radius.x, radius.y * radius.y};
    float lambda = (x1p2 / radius2.x) + (y1p2 / radius2.y);

    // 校正超出范围的半径
    if (lambda > 1.0f)
    {
        radius.x = radius.x * sqrtf(lambda);
        radius.y = radius.y * sqrtf(lambda);
        // radius *= sqrtf(lambda);

        radius2 = {radius.x * radius.x, radius.y * radius.y};
    }

    tvg::Point cp, center;
    float c = (radius2.x * radius2.y) - (radius2.x * y1p2) - (radius2.y * x1p2);

    if (c < 0.0f)
    {
        radius.x = radius.x * sqrtf(1.0f - c / (radius2.x * radius2.y));
        radius.y = radius.y * sqrtf(1.0f - c / (radius2.x * radius2.y));
        // radius *= sqrtf(1.0f - c / (radius2.x * radius2.y));

        radius2 = {radius.x * radius.x, radius.y * radius.y};
        cp = {0.0f, 0.0f};
        center = {0.0f, 0.0f};
    }
    else
    {
        c = sqrtf(c / ((radius2.x * y1p2) + (radius2.y * x1p2)));
        if (largeArc == sweep)
            c = -c;

        cp.x = c * (radius.x * y1p / radius.y);
        cp.y = c * (-radius.y * x1p / radius.x);

        // cp = c * tvg::Point{(radius.x * y1p / radius.y), (-radius.y * x1p / radius.x)};

        center = {cosPhi * cp.x - sinPhi * cp.y, sinPhi * cp.x + cosPhi * cp.y};
    }

    center.x = center.x + (start.x + next.x) * 0.5f;
    center.y = center.y + (start.y + next.y) * 0.5f;
    // center += (start + next) * 0.5f;

    // 计算角度
    auto at = atan2f(((y1p - cp.y) / radius.y), ((x1p - cp.x) / radius.x));
    auto theta1 = (at < 0.0f) ? 2.0f * M_PI + at : at;
    auto nat = atan2f(((-y1p - cp.y) / radius.y), ((-x1p - cp.x) / radius.x));
    auto deltaTheta = (nat < at) ? 2.0f * M_PI - at + nat : nat - at;

    if (sweep)
    {
        if (deltaTheta < 0.0f)
            deltaTheta += 2.0f * M_PI;
    }
    else
    {
        if (deltaTheta > 0.0f)
            deltaTheta -= 2.0f * M_PI;
    }

    // 分段处理，每段小于90度
    auto segments = int(fabsf(deltaTheta / (M_PI / 2)) + 1.0f);
    auto delta = deltaTheta / segments;
    auto bcp = 4.0f / 3.0f * (1.0f - cosf(delta / 2.0f)) / sinf(delta / 2.0f);

    auto cosPhiR = tvg::Point{cosPhi * radius.x, cosPhi * radius.y};
    auto sinPhiR = tvg::Point{sinPhi * radius.x, sinPhi * radius.y};
    auto cosTheta1 = cosf(theta1);
    auto sinTheta1 = sinf(theta1);

    for (int i = 0; i < segments; ++i)
    {
        auto theta2 = theta1 + delta;
        auto cosTheta2 = cosf(theta2);
        auto sinTheta2 = sinf(theta2);

        // 第一个控制点
        tvg::Point c1;
        c1.x = start.x + -bcp * (cosPhiR.x * sinTheta1 + sinPhiR.y * cosTheta1);
        c1.y = start.y + bcp * (cosPhiR.y * cosTheta1 - sinPhiR.x * sinTheta1);

        // 终点
        tvg::Point e;
        e.x = center.x + cosPhiR.x * cosTheta2 - sinPhiR.y * sinTheta2;
        e.y = center.y + sinPhiR.x * cosTheta2 + cosPhiR.y * sinTheta2;

        // 第二个控制点
        tvg::Point c2;
        c2.x = e.x + bcp * (cosPhiR.x * sinTheta2 + sinPhiR.y * cosTheta2);
        c2.y = e.y + bcp * (sinPhiR.x * sinTheta2 - cosPhiR.y * cosTheta2);

        shape->cubicTo(c1.x, c1.y, c2.x, c2.y, e.x, e.y);

        start = e;
        theta1 = theta2;
        cosTheta1 = cosTheta2;
        sinTheta1 = sinTheta2;
    }
}

// 公共掏空操作函数
static void applyHollowMask(tvg::Shape *fillShapePtr, int32_t x, int32_t y, const TpHollowMask &hollowMaskData)
{
    if (!fillShapePtr)
        return;

    // 矩形镂空
    auto clipper = tvg::Shape::gen();

    // 矩形镂空
    TpVector<TpHollowMask::RectHollow> rectHollow = hollowMaskData.rectHollowList();
    for (const auto &hollowData : rectHollow)
    {
        // 创建裁剪形状
        clipper->appendRect(hollowData.region.x() + x, hollowData.region.y() + y,
                            hollowData.region.width(), hollowData.region.height(), hollowData.round, hollowData.round);
    }

    // 圆形镂空
    TpVector<TpHollowMask::CircleHollow> circleHollow = hollowMaskData.circleHollowList();
    for (const auto &hollowData : circleHollow)
    {
        // 创建裁剪形状
        clipper->appendCircle(hollowData.x + x, hollowData.y + y, hollowData.radius, hollowData.radius);
    }

    // 扇形镂空
    TpVector<TpHollowMask::PieHollow> pieHollowList = hollowMaskData.pieHollowList();
    for (const auto &hollowData : pieHollowList)
    {
        // 将角度转换为弧度
        float startRad = hollowData.start * M_PI / 180.0f;
        float endRad = hollowData.end * M_PI / 180.0f;

        // 计算起点和终点
        float startX = hollowData.x + x + hollowData.radius * cosf(startRad);
        float startY = hollowData.y + y + hollowData.radius * sinf(startRad);
        float endX = hollowData.x + x + hollowData.radius * cosf(endRad);
        float endY = hollowData.y + y + hollowData.radius * sinf(endRad);

        // 绘制扇形路径
        clipper->moveTo(hollowData.x + x, hollowData.y + y); // 移动到圆心
        clipper->lineTo(startX, startY);                     // 画线到起点

        // 计算角度差
        float angleDiff = endRad - startRad;

        // 确保角度在正确范围内
        while (angleDiff > 2 * M_PI)
            angleDiff -= 2 * M_PI;
        while (angleDiff < -2 * M_PI)
            angleDiff += 2 * M_PI;

        // 使用类似 _pathAppendArcTo 的算法
        appendArcToPath(clipper, startX, startY, endX, endY, hollowData.radius, hollowData.radius, 0.0f,
                        std::fabs(angleDiff) > M_PI, true);

        // 封口
        clipper->lineTo(hollowData.x + x, hollowData.y + y); // 画线到起点
        clipper->close();                                    // 闭合路径回到圆心
    }

    // 多边形镂空
    TpVector<TpHollowMask::PolygonHollow> polygonHollowList = hollowMaskData.polygonHollowList();
    for (const auto &hollowPolygon : polygonHollowList)
    {
        // 少于两个点的多边形不处理
        if (hollowPolygon.posintList.size() <= 2)
            continue;

        for (int i = 0; i < hollowPolygon.posintList.size(); ++i)
        {
            TpPoint polygonPoint = hollowPolygon.posintList.at(i);

            if (i == 0)
            {
                // 移动到起始点
                clipper->moveTo(polygonPoint.x() + x, polygonPoint.y() + y);
            }
            else if (i == (hollowPolygon.posintList.size() - 1))
            {
                // 闭合多边形
                clipper->lineTo(polygonPoint.x() + x, polygonPoint.y() + y);
                clipper->close();
            }
            else
            {
                clipper->lineTo(polygonPoint.x() + x, polygonPoint.y() + y);
            }
        }
    }

    // 应用反向Alpha遮罩实现镂空
    clipper->fill(255, 255, 255, 255);
    fillShapePtr->mask(clipper, tvg::MaskMethod::InvAlpha);
}

/// @brief 绘制像素点
static inline void renderPoint(TpPainterData *painterData, int32_t x, int32_t y)
{
    if (!painterData->swCanvas)
        return;

    refreshCanvasTarget(painterData);

    auto pixel = tvg::Shape::gen();
    pixel->appendCircle(x, y, 0.5 * painterData->drawPen.width(), 0.5 * painterData->drawPen.width()); // 半径 0.5 的圆形

    int32_t colorRGBA = painterData->drawPen.color().rgba();

    tvg::Fill *gradientPtr = parseGradientPtr(painterData);

    if (gradientPtr)
        pixel->fill(gradientPtr);
    else
        pixel->fill(_R(colorRGBA), _G(colorRGBA), _B(colorRGBA), _A(colorRGBA));

    painterData->tvgScene->push(std::move(pixel));
}

/// @brief 绘制线段
static inline void renderLine(TpPainterData *painterData, const TpPoint &point1, const TpPoint &point2)
{
    if (!painterData->swCanvas)
        return;

    refreshCanvasTarget(painterData);

    // 创建直线
    auto line = tvg::Shape::gen();
    line->moveTo(point1.x(), point1.y());
    line->lineTo(point2.x(), point2.y());

    // 设置描边属性
    line->strokeWidth(painterData->drawPen.width()); // 线宽

    tvg::Fill *gradientPtr = parseGradientPtr(painterData);

    if (gradientPtr)
    {
        line->strokeFill(gradientPtr);
    }
    else
    {
        int32_t colorRGBA = painterData->drawPen.color().rgba();
        line->strokeFill(_R(colorRGBA), _G(colorRGBA), _B(colorRGBA), _A(colorRGBA));
    }

    // 线头
    line->strokeCap((tvg::StrokeCap)painterData->drawPen.capStyle());
    // 连接
    line->strokeJoin((tvg::StrokeJoin)painterData->drawPen.joinStyle());

    painterData->tvgScene->push(std::move(line));
}

/// @brief 绘制矩形/圆角矩形
static inline void renderRect(TpPainterData *painterData, const TpRect &rect, int32_t rad, const TpHollowMask &hollowMaskData = TpHollowMask())
{
    if (!painterData->swCanvas)
        return;

    refreshCanvasTarget(painterData);

    int32_t minWH = TP_MIN(rect.width(), rect.height());
    if (rad > (0.5 * minWH))
        rad = 0.5 * minWH;

    // 绘制矩形填充
    auto rectShape = tvg::Shape::gen();
    rectShape->appendRect(rect.x(), rect.y(), rect.width(), rect.height(), rad, rad);

    tvg::Fill *gradientPtr = parseGradientPtr(painterData);

    int32_t penColor = painterData->drawPen.color().rgba();
    int32_t brushColor = painterData->drawBrush.color().rgba();

    if (painterData->drawBrush.style() == Tp::NoBrush)
    {
        if (gradientPtr)
            rectShape->strokeFill(gradientPtr);
        else
            rectShape->strokeFill(_R(penColor), _G(penColor), _B(penColor), _A(penColor));

        rectShape->strokeWidth(painterData->drawPen.width());
    }
    else if (painterData->drawBrush.style() == Tp::SolidPattern)
    {
        if (gradientPtr)
            rectShape->fill(gradientPtr);
        else
            rectShape->fill(_R(brushColor), _G(brushColor), _B(brushColor), _A(brushColor));

        applyHollowMask(rectShape, rect.x(), rect.y(), hollowMaskData);
    }
    else
    {
        if (gradientPtr)
            rectShape->fill(gradientPtr);
        else
            rectShape->fill(_R(brushColor), _G(brushColor), _B(brushColor), _A(brushColor));

        applyHollowMask(rectShape, rect.x(), rect.y(), hollowMaskData);
    }

    painterData->tvgScene->push(std::move(rectShape));
}

/// @brief 绘制圆形、椭圆；填充或线条
/// @param x 圆心坐标
/// @param rx 长轴半径
/// @param ry 短轴半径
static inline void renderEllipse(TpPainterData *painterData, const TpPoint &center, const int32_t &rx, const int32_t &ry, const TpHollowMask &hollowMaskData = TpHollowMask())
{
    if (!painterData->swCanvas)
        return;

    refreshCanvasTarget(painterData);

    auto circle = tvg::Shape::gen();
    circle->appendCircle(center.x(), center.y(), rx, ry);

    tvg::Fill *gradientPtr = parseGradientPtr(painterData);

    int32_t penColor = painterData->drawPen.color().rgba();
    int32_t brushColor = painterData->drawBrush.color().rgba();

    if (painterData->drawBrush.style() == Tp::NoBrush)
    {
        if (gradientPtr)
            circle->strokeFill(gradientPtr);
        else

            circle->strokeWidth(painterData->drawPen.width());
    }
    else if (painterData->drawBrush.style() == Tp::SolidPattern)
    {
        if (gradientPtr)
            circle->fill(gradientPtr);
        else
            circle->fill(_R(brushColor), _G(brushColor), _B(brushColor), _A(brushColor));

        applyHollowMask(circle, center.x() - rx, center.y() - ry, hollowMaskData);
    }
    else
    {
        if (gradientPtr)
            circle->fill(gradientPtr);
        else
            circle->fill(_R(brushColor), _G(brushColor), _B(brushColor), _A(brushColor));

        applyHollowMask(circle, center.x() - rx, center.y() - ry, hollowMaskData);
    }

    painterData->tvgScene->push(std::move(circle));
}

// 绘制圆环
static inline void renderArc(TpPainterData *painterData, const TpPoint &center, int32_t rad,
                             const double &start, const double &end, bool isPie, const TpHollowMask &hollowMaskData = TpHollowMask())
{
    if (!painterData->swCanvas)
        return;

    refreshCanvasTarget(painterData);

    // 绘制矩形填充
    tvg::Shape *arc = tvg::Shape::gen();

    // 将角度转换为弧度
    float startRad = start * M_PI / 180.0f;
    float endRad = end * M_PI / 180.0f;

    // 计算起点和终点
    float startX = center.x() + rad * cosf(startRad);
    float startY = center.y() + rad * sinf(startRad);
    float endX = center.x() + rad * cosf(endRad);
    float endY = center.y() + rad * sinf(endRad);

    if (isPie)
    {
        // 绘制扇形路径
        arc->moveTo(center.x(), center.y()); // 移动到圆心
        arc->lineTo(startX, startY);         // 画线到起点
    }
    else
    {
        // 移动到起点
        arc->moveTo(startX, startY);
    }

    // 计算角度差
    float angleDiff = endRad - startRad;
    // if (!clockwise)
    //     angleDiff = -angleDiff;

    // 确保角度在正确范围内
    while (angleDiff > 2 * M_PI)
        angleDiff -= 2 * M_PI;
    while (angleDiff < -2 * M_PI)
        angleDiff += 2 * M_PI;

    // 使用类似 _pathAppendArcTo 的算法
    appendArcToPath(arc, startX, startY, endX, endY, rad, rad, 0.0f,
                    fabsf(angleDiff) > M_PI, true);

    tvg::Fill *gradientPtr = parseGradientPtr(painterData);

    int32_t penColor = painterData->drawPen.color().rgba();
    int32_t brushColor = painterData->drawBrush.color().rgba();

    if (isPie)
    {
        // 封口
        arc->lineTo(center.x(), center.y()); // 画线到起点
        arc->close();                        // 闭合路径回到圆心

        if (painterData->drawBrush.style() == Tp::NoBrush)
        {
            if (gradientPtr)
                arc->strokeFill(gradientPtr);
            else
                arc->strokeFill(_R(penColor), _G(penColor), _B(penColor), _A(penColor));

            arc->strokeWidth(painterData->drawPen.width());
        }
        else if (painterData->drawBrush.style() == Tp::SolidPattern)
        {
            if (gradientPtr)
                arc->fill(gradientPtr);
            else
                arc->fill(_R(brushColor), _G(brushColor), _B(brushColor), _A(brushColor));

            applyHollowMask(arc, center.x() - rad, center.y() - rad, hollowMaskData);
        }
        else
        {
            if (gradientPtr)
                arc->fill(gradientPtr);
            else
                arc->fill(_R(brushColor), _G(brushColor), _B(brushColor), _A(brushColor));

            applyHollowMask(arc, center.x() - rad, center.y() - rad, hollowMaskData);
        }
    }
    else
    {
        if (gradientPtr)
            arc->strokeFill(gradientPtr);
        else
            arc->strokeFill(_R(penColor), _G(penColor), _B(penColor), _A(penColor));

        arc->strokeWidth(painterData->drawPen.width());

        // 线头
        arc->strokeCap((tvg::StrokeCap)painterData->drawPen.capStyle());
        // 连接
        arc->strokeJoin((tvg::StrokeJoin)painterData->drawPen.joinStyle());
    }

    painterData->tvgScene->push(std::move(arc));
}

// 绘制多边形
static inline void renderPolygon(TpPainterData *painterData, const TpVector<TpPoint> &pointList, const TpHollowMask &hollowMaskData = TpHollowMask())
{
    if (pointList.size() == 0)
        return;

    if (!painterData->swCanvas)
        return;

    // 只有一个点，绘制一个像素点
    if (pointList.size() == 1)
    {
        renderPoint(painterData, pointList.front().x() + painterData->offsetX, pointList.front().y() + painterData->offsetY);
    }
    else if (pointList.size() == 2)
    {
        TpPoint offsetPoint(painterData->offsetX, painterData->offsetY);

        // 两个点，绘制线
        const auto &firstPoint = pointList[0];
        const auto &secondPoint = pointList[1];
        renderLine(painterData, firstPoint + offsetPoint, secondPoint + offsetPoint);
    }
    else
    {
        // 绘制多边形
        refreshCanvasTarget(painterData);

        auto polygon = tvg::Shape::gen();

        // 移动到第一个顶点
        polygon->moveTo(pointList.front().x() + painterData->offsetX, pointList.front().y() + painterData->offsetY);

        for (int i = 1; i < pointList.size(); ++i)
        {
            const auto &curPoint = pointList[i];
            polygon->lineTo(curPoint.x() + painterData->offsetX, curPoint.y() + painterData->offsetY);
        }
        // 闭合路径回到起点
        polygon->close();

        tvg::Fill *gradientPtr = parseGradientPtr(painterData);

        int32_t penColor = painterData->drawPen.color().rgba();
        int32_t brushColor = painterData->drawBrush.color().rgba();

        if (painterData->drawBrush.style() == Tp::NoBrush)
        {
            if (gradientPtr)
                polygon->strokeFill(gradientPtr);
            else
                polygon->strokeFill(_R(penColor), _G(penColor), _B(penColor), _A(penColor));

            polygon->strokeWidth(painterData->drawPen.width());
        }
        else if (painterData->drawBrush.style() == Tp::SolidPattern)
        {
            if (gradientPtr)
                polygon->fill(gradientPtr);
            else
                polygon->fill(_R(brushColor), _G(brushColor), _B(brushColor), _A(brushColor));

            applyHollowMask(polygon, pointList.front().x() + painterData->offsetX, pointList.front().y() + painterData->offsetY, hollowMaskData);
        }
        else
        {
            if (gradientPtr)
                polygon->fill(gradientPtr);
            else
                polygon->fill(_R(brushColor), _G(brushColor), _B(brushColor), _A(brushColor));

            applyHollowMask(polygon, pointList.front().x() + painterData->offsetX, pointList.front().y() + painterData->offsetY, hollowMaskData);
        }

        painterData->tvgScene->push(std::move(polygon));
    }
}

// 由于thorvg基于基线计算，所以如果顶部对齐，需要补偿 XY坐标
TpPoint caculateTextOffset(Tp::Alignment alignFlag, const TpSize &textLayout, tvg::Text *textPtr)
{
    TpPoint offsetPoint;
    float verticalOffset = 0;
    float horizonOffset = 0;

    // 水平对齐处理
    bool needsHorizonOffset = (alignFlag & Tp::AlignLeft) ||
                              !(alignFlag & (Tp::AlignRight | Tp::AlignHCenter));

    if (needsHorizonOffset)
    {
        // 左对齐或默认（左对齐）：获取文本左边界的X坐标
        float y, textWidth, textHeight;
        textPtr->bounds(&horizonOffset, &y, &textWidth, &textHeight);
    }
    else if (alignFlag & Tp::AlignRight)
    {
        // 右对齐：计算使文本紧贴右边界的偏移量
        float bx, by, bw, bh;
        textPtr->bounds(&bx, &by, &bw, &bh);

        float actualRight = bx + bw;
        float layoutRight = textLayout.width();
        horizonOffset = -(layoutRight - actualRight); // 向右平移使文本贴右
    }
    else
    {
    }

    // 垂直对齐处理
    bool needsVerticalOffset = (alignFlag & Tp::AlignTop) ||
                               !(alignFlag & (Tp::AlignVCenter | Tp::AlignBottom));

    if (needsVerticalOffset)
    {
        // 顶部对齐或默认（顶部对齐）：获取文本上边界的Y坐标
        float x, textWidth, textHeight;
        textPtr->bounds(&x, &verticalOffset, &textWidth, &textHeight);
    }
    else if (alignFlag & Tp::AlignBottom)
    {
        // 底部对齐：计算使文本紧贴底部边界的偏移量
        float bx, by, bw, bh;
        textPtr->bounds(&bx, &by, &bw, &bh);

        float actualBottom = by + bh;
        float layoutBottom = textLayout.height();
        verticalOffset = -(layoutBottom - actualBottom); // 向下平移使文本贴底
    }
    else
    {
    }

    offsetPoint.setX(horizonOffset);
    offsetPoint.setY(verticalOffset);

    return offsetPoint;
}
#endif
