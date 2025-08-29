#include "TpCanvas.h"
#include "TpSurface.h"
#include "TpFont.h"
#include "TpRect.h"
#include "TpDef.h"
#include "thorVG/thorvg.h"
#include "TpImage_p.h"
#include "TpGradient_p.h"
#include "TpLinearGradient.h"
#include "TpRadialGradient.h"

#include <thread>
#include <cmath>

#define OFFSET_X(set, x) (set->offsetX + x)
#define OFFSET_Y(set, y) (set->offsetY + y)

struct TpCanvasData
{
    tpShared<TpSurface> TpSurfacePtr;
    int32_t offsetX, offsetY;
    bool beUsed;

    // 渐变属性；如果有渐变属性则填充图形接口给入的颜色不再生效
    TpGradient *gradientProperty = nullptr;

    // CPU绘制引擎
    tvg::SwCanvas *swCanvas = nullptr;
    // 使用OpenGL加速的绘制引擎；需有GPU和OpenGL才能使用；暂时无用
    tvg::GlCanvas *glCanvas = nullptr;

    tvg::Scene *tvgScene = nullptr;
};

// 绘制圆角图片资源数据
struct RoundSurfaceData
{
    tpShared<TpSurface> surface = nullptr;
    int32_t roundRad = 0;

    // 绘制坐标起始点
    int32_t x = 0;
    int32_t y = 0;

    // 偏移量
    int32_t offsetX = 0;
    int32_t offsetY = 0;
};

// 重设canvas的target
static inline void refreshCanvasTarget(TpCanvasData *set)
{
    // static bool test = true;
    // if (test)
    {
        int32_t surfaceWidth = set->TpSurfacePtr->width();
        int32_t surfaceHeight = set->TpSurfacePtr->height();
        set->swCanvas->target((uint32_t *)set->TpSurfacePtr->matrix(), surfaceWidth, surfaceWidth, surfaceHeight, tvg::ColorSpace::ARGB8888);
    }
    // test = false;
}

// 解析渐变信息；无渐变则返回空指针
static inline tvg::Fill *parseGradientPtr(TpCanvasData *set)
{
    if (!set->gradientProperty)
        return nullptr;

    TpList<std::pair<float, int32_t>> colorList = set->gradientProperty->getColors();
    if (colorList.size() == 0)
        return nullptr;

    tvg::Fill *resGradientPtr = nullptr;

    TpGradient::GradientType gradientType = set->gradientProperty->gradientType();
    if (gradientType == TpGradient::LinearGradient)
    {
        // 创建线性渐变
        TpLinearGradient *linearGrad = dynamic_cast<TpLinearGradient *>(set->gradientProperty);
        if (!linearGrad)
            return nullptr;

        ItpPointF startPoint = linearGrad->start();
        ItpPointF stopPoint = linearGrad->finalStop();

        tvg::LinearGradient *linearGradient = tvg::LinearGradient::gen();
        linearGradient->linear(set->offsetX + startPoint.x, set->offsetY + startPoint.y, set->offsetX + stopPoint.x, set->offsetY + stopPoint.y);

        resGradientPtr = linearGradient;
    }
    else if (gradientType == TpGradient::RadialGradient)
    {
        // 创建径向渐变
        TpRadialGradient *radialGrad = dynamic_cast<TpRadialGradient *>(set->gradientProperty);
        if (!radialGrad)
            return nullptr;

        ItpPointF centerPoint = radialGrad->center();
        float centerRadius = radialGrad->centerRadius();

        ItpPointF focalPoint = radialGrad->focalPoint();
        float focalRadius = radialGrad->focalRadius();

        tvg::RadialGradient *radialGradient = tvg::RadialGradient::gen();
        // 设置中心点和半径
        radialGradient->radial(set->offsetX + centerPoint.x, set->offsetY + centerPoint.y, centerRadius,
                               set->offsetX + focalPoint.x, set->offsetY + focalPoint.y, focalRadius);

        resGradientPtr = radialGradient;
    }
    else
    {
    }

    if (!resGradientPtr)
        return resGradientPtr;

    // 设置扩散模式
    tvg::FillSpread spreadMode = (tvg::FillSpread)set->gradientProperty->spread();
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

TpCanvas::TpCanvas(tpShared<TpSurface> surface, int32_t offsetX, int32_t offsetY)
{
    // 根据CPU核心数；分配绘图引擎线程数
    uint32_t cores = std::thread::hardware_concurrency();
    tvg::Initializer::init(cores / 2);

    if (!surface)
        return;

    TpCanvasData *set = new TpCanvasData();

    if (!set)
        return;

    set->beUsed = false;
    set->offsetX = offsetX;
    set->offsetY = offsetY;

    set->TpSurfacePtr = surface;
    set->beUsed = (surface != nullptr);

    // TODO判断是GPU环境还是CPU环境
    // set->swCanvas = tvg::SwCanvas::gen();

    // refreshCanvasTarget(set);

    this->data_ = set;
}

TpCanvas::~TpCanvas()
{
    tvg::Initializer::term();

    TpCanvasData *set = static_cast<TpCanvasData *>(data_);
    if (!set)
        return;

    if (!set->beUsed)
        return;

    set->TpSurfacePtr = nullptr;
    set->beUsed = false;

    // delete set->swCanvas;
    // delete set->glCanvas;

    delete set;
}

void TpCanvas::paintTest()
{
    TpCanvasData *set = static_cast<TpCanvasData *>(data_);
    if (!set)
        return;

    if (!set->swCanvas)
        return;

    refreshCanvasTarget(set);

    // 加载字体文件
    // tvg::Text::load("/home/hawk/Public/tinyPiXCore/src/data/fonts/SourceHanSansCN/SourceHanSansCN-Normal.otf");
    tvg::Text::load("/home/hawk/Public/tinyPiXCore/src/data/fonts/Taipei Sans TC Beta.ttf");

    // 创建文本对象
    auto text = tvg::Text::gen();

    // text->font("Source Han Sans CN", 32);     // 设置字体名称和大小
    text->font("Taipei Sans TC Beta", 32); // 设置字体名称和大小
    text->text("Hello ThorVG 哈哈哈!");    // 设置文本内容
    text->fill(255, 0, 0);                 // 设置文本颜色 (红色)
    text->translate(set->offsetX, set->offsetY);

    float textX, textY, textWidth, textHeight;
    text->bounds(&textX, &textY, &textWidth, &textHeight);

    std::cout << "文本坐标信息： " << textX << "  " << textY << "  " << textWidth << "  " << textHeight << std::endl;

    // 添加到 Canvas 并绘制
    set->tvgScene->push(std::move(text));
    // set->swCanvas->draw();
    // set->swCanvas->sync();
}

bool TpCanvas::setTarget(tpShared<TpSurface> surface, int32_t offsetX, int32_t offsetY)
{
    TpCanvasData *set = static_cast<TpCanvasData *>(data_);

    if (!set)
        return false;

    if (surface == nullptr)
        return false;

    if (set->beUsed)
    {
        set->TpSurfacePtr = nullptr;
    }

    set->TpSurfacePtr = surface;
    set->offsetX = offsetX;
    set->offsetY = offsetY;
    set->beUsed = true;

    return true;
}

tpShared<TpSurface> TpCanvas::surface()
{
    TpCanvasData *set = static_cast<TpCanvasData *>(data_);

    if (set && set->beUsed)
    {
        return set->TpSurfacePtr;
    }

    return nullptr;
}

void TpCanvas::setClipRect(const ItpRect &rect)
{
    TpCanvasData *set = static_cast<TpCanvasData *>(data_);

    if (set && set->beUsed)
    {
        set->TpSurfacePtr->setClipRect(rect);
    }
}

ItpRect TpCanvas::clipRect()
{
    TpCanvasData *set = static_cast<TpCanvasData *>(data_);
    return set->TpSurfacePtr->clipRect();
}

void TpCanvas::erase()
{
    TpCanvasData *set = static_cast<TpCanvasData *>(data_);

    if (set && set->beUsed)
    {
        // // 方法1：清除所有 Paint 对象
        // set->swCanvas->remove();

        // // 方法2：清除缓冲区并重新绘制
        // set->swCanvas->draw(true);
        // set->swCanvas->sync();

        TpCanvasData *set = static_cast<TpCanvasData *>(data_);
        if (!set || !set->swCanvas || !set->beUsed)
            return;

        // 获取裁剪矩形（类似 SDL_GetClipRect）
        ItpRect clipRect = set->TpSurfacePtr->clipRect();

        // 设置视口到裁剪区域
        // <cite> inc / thorvg.h : 846 - 871 < / cite >
        set->swCanvas->viewport(clipRect.x, clipRect.y, clipRect.w, clipRect.h);

        // 清除该区域（相当于 CAIRO_OPERATOR_CLEAR）
        // <cite> inc / thorvg.h : 825 - 843 < / cite >
        set->swCanvas->draw(true); // true 参数会清除缓冲区

        // 恢复完整视口
        int32_t surfaceWidth = set->TpSurfacePtr->width();
        int32_t surfaceHeight = set->TpSurfacePtr->height();
        set->swCanvas->viewport(0, 0, surfaceWidth, surfaceHeight);

        // 同步操作
        set->swCanvas->sync();
    }
}

void TpCanvas::setGradient(TpGradient *gradient)
{
    TpCanvasData *set = static_cast<TpCanvasData *>(data_);
    set->gradientProperty = gradient;
}

static inline void drawPixel(TpCanvasData *set, int32_t x, int32_t y, int32_t color)
{
    if (!set->swCanvas)
        return;

    refreshCanvasTarget(set);

    auto pixel = tvg::Shape::gen();
    pixel->appendCircle(x, y, 0.5, 0.5); // 半径 0.5 的圆形

    tvg::Fill *gradientPtr = parseGradientPtr(set);

    if (gradientPtr)
        pixel->fill(gradientPtr);
    else
        pixel->fill(_R(color), _G(color), _B(color), _A(color));

    set->tvgScene->push(std::move(pixel));
    // set->swCanvas->push(std::move(pixel));

    // set->swCanvas->draw();
    // set->swCanvas->sync();
}

void TpCanvas::pixel(int32_t x, int32_t y, int32_t color)
{
    TpCanvasData *set = static_cast<TpCanvasData *>(data_);

    if (set && set->beUsed)
    {
        x = OFFSET_X(set, x);
        y = OFFSET_Y(set, y);

        drawPixel(set, x, y, color);
    }
}

static inline void drawLine(TpCanvasData *set, int32_t x1, int32_t y1, int32_t x2, int32_t y2, int32_t color, double width)
{
    if (!set->swCanvas)
        return;

    refreshCanvasTarget(set);

    // 创建直线
    auto line = tvg::Shape::gen();
    line->moveTo(x1, y1);
    line->lineTo(x2, y2);

    // 设置描边属性
    line->strokeWidth(width); // 线宽

    tvg::Fill *gradientPtr = parseGradientPtr(set);

    if (gradientPtr)
        line->strokeFill(gradientPtr);
    else
        line->strokeFill(_R(color), _G(color), _B(color), _A(color));

    line->strokeCap(tvg::StrokeCap::Round);   // 圆角线头
    line->strokeJoin(tvg::StrokeJoin::Round); // 圆角连接

    set->tvgScene->push(std::move(line));
    // set->swCanvas->push(std::move(line));

    // 绘制并同步
    // set->swCanvas->draw();
    // set->swCanvas->sync();

    return;
}

void TpCanvas::hline(int32_t x1, int32_t x2, int32_t y, int32_t color, double width)
{
    TpCanvasData *set = static_cast<TpCanvasData *>(data_);

    if (set && set->beUsed)
    {

        x1 = OFFSET_X(set, x1);
        x2 = OFFSET_X(set, x2);
        y = OFFSET_Y(set, y);

        drawLine(set, x1, y, x2, y, color, width);
    }
}

void TpCanvas::vline(int32_t x, int32_t y1, int32_t y2, int32_t color, double width)
{
    TpCanvasData *set = static_cast<TpCanvasData *>(data_);

    if (set && set->beUsed)
    {
        x = OFFSET_X(set, x);
        y1 = OFFSET_Y(set, y1);
        y2 = OFFSET_Y(set, y2);

        drawLine(set, x, y1, x, y2, color, width);
    }
}

void TpCanvas::line(int32_t x1, int32_t y1, int32_t x2, int32_t y2, int32_t color, double width)
{
    TpCanvasData *set = static_cast<TpCanvasData *>(data_);

    if (set && set->beUsed)
    {
        x1 = OFFSET_X(set, x1);
        y1 = OFFSET_Y(set, y1);
        x2 = OFFSET_X(set, x2);
        y2 = OFFSET_Y(set, y2);

        drawLine(set, x1, y1, x2, y2, color, width);
    }
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
static void applyHollowMask(tvg::Shape *fillShapePtr, int32_t x, int32_t y, const HollowMask &hollowMaskData)
{
    if (!fillShapePtr)
        return;

    // 矩形镂空
    auto clipper = tvg::Shape::gen();

    // 矩形镂空
    TpVector<HollowMask::RectHollow> rectHollow = hollowMaskData.rectHollowList();
    for (const auto &hollowData : rectHollow)
    {
        // 创建裁剪形状
        clipper->appendRect(hollowData.region.x + x, hollowData.region.y + y, hollowData.region.w, hollowData.region.h, hollowData.round, hollowData.round);
    }

    // 圆形镂空
    TpVector<HollowMask::CircleHollow> circleHollow = hollowMaskData.circleHollowList();
    for (const auto &hollowData : circleHollow)
    {
        // 创建裁剪形状
        clipper->appendCircle(hollowData.x + x, hollowData.y + y, hollowData.radius, hollowData.radius);
    }

    // 扇形镂空
    TpVector<HollowMask::PieHollow> pieHollowList = hollowMaskData.pieHollowList();
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
    TpVector<HollowMask::PolygonHollow> polygonHollowList = hollowMaskData.polygonHollowList();
    for (const auto &hollowPolygon : polygonHollowList)
    {
        // 少于两个点的多边形不处理
        if (polygonHollowList.size() <= 2)
            continue;

        for (int i = 0; i < hollowPolygon.posintList.size(); ++i)
        {
            ItpPoint polygonPoint = hollowPolygon.posintList.at(i);

            if (i == 0)
            {
                // 移动到起始点
                clipper->moveTo(polygonPoint.x, polygonPoint.y);
            }
            else if (i == (hollowPolygon.posintList.size() - 1))
            {
                // 闭合多边形
                clipper->lineTo(polygonPoint.x, polygonPoint.y);
                clipper->close();
            }
            else
            {
                clipper->lineTo(polygonPoint.x, polygonPoint.y);
            }
        }
    }

    // 应用反向Alpha遮罩实现镂空
    clipper->fill(255, 255, 255, 255);
    fillShapePtr->mask(clipper, tvg::MaskMethod::InvAlpha);
}

static inline void drawRectangle(TpCanvasData *set, int32_t x1, int32_t y1, int32_t x2, int32_t y2, int32_t color, double width, int32_t rad, bool isFill, const HollowMask &hollowMaskData = HollowMask())
{
    if (!set->swCanvas)
        return;

    refreshCanvasTarget(set);

    // 绘制矩形填充
    auto rect = tvg::Shape::gen();
    rect->appendRect(x1, y1, x2 - x1, y2 - y1, rad, rad);

    tvg::Fill *gradientPtr = parseGradientPtr(set);

    if (isFill)
    {
        if (gradientPtr)
            rect->fill(gradientPtr);
        else
            rect->fill(_R(color), _G(color), _B(color), _A(color)); // set its color (r, g, b)

        applyHollowMask(rect, x1, y1, hollowMaskData);
    }
    else
    {
        if (gradientPtr)
            rect->strokeFill(gradientPtr);
        else
            rect->strokeFill(_R(color), _G(color), _B(color), _A(color));

        rect->strokeWidth(width);
    }

    set->tvgScene->push(std::move(rect));
    // set->swCanvas->push(std::move(rect));

    // 绘制并同步
    // set->swCanvas->draw();
    // set->swCanvas->sync();
}

void TpCanvas::rectangle(int32_t x1, int32_t y1, int32_t x2, int32_t y2, int32_t color, double width)
{
    TpCanvasData *set = static_cast<TpCanvasData *>(data_);

    if (set && set->beUsed)
    {
        x1 = OFFSET_X(set, x1);
        y1 = OFFSET_Y(set, y1);
        x2 = OFFSET_X(set, x2);
        y2 = OFFSET_Y(set, y2);

        drawRectangle(set, x1, y1, x2, y2, color, width, 0, false);
    }
}

void TpCanvas::roundedRectangle(int32_t x1, int32_t y1, int32_t x2, int32_t y2, int32_t rad, int32_t color, double width)
{
    TpCanvasData *set = static_cast<TpCanvasData *>(data_);

    if (set && set->beUsed)
    {
        x1 = OFFSET_X(set, x1);
        y1 = OFFSET_Y(set, y1);
        x2 = OFFSET_X(set, x2);
        y2 = OFFSET_Y(set, y2);

        drawRectangle(set, x1, y1, x2, y2, color, width, rad, false);
    }
}

void TpCanvas::box(int32_t x1, int32_t y1, int32_t x2, int32_t y2, int32_t color, const HollowMask &hollowMaskData)
{
    TpCanvasData *set = static_cast<TpCanvasData *>(data_);

    if (set && set->beUsed)
    {
        x1 = OFFSET_X(set, x1);
        y1 = OFFSET_Y(set, y1);
        x2 = OFFSET_X(set, x2);
        y2 = OFFSET_Y(set, y2);

        drawRectangle(set, x1, y1, x2, y2, color, 1, 0, true, hollowMaskData);
    }
}

void TpCanvas::roundedBox(int32_t x1, int32_t y1, int32_t x2, int32_t y2, int32_t rad, int32_t color, const HollowMask &hollowMaskData)
{
    TpCanvasData *set = static_cast<TpCanvasData *>(data_);

    if (set &&
        set->beUsed)
    {
        x1 = OFFSET_X(set, x1);
        y1 = OFFSET_Y(set, y1);
        x2 = OFFSET_X(set, x2);
        y2 = OFFSET_Y(set, y2);

        drawRectangle(set, x1, y1, x2, y2, color, 1, rad, true, hollowMaskData);
    }
}

/// @brief 绘制圆形、椭圆；填充或线条
/// @param set
/// @param x 坐标
/// @param y
/// @param rx 长轴半径
/// @param ry 短轴半径
/// @param color 颜色
/// @param width 线条宽度，当填充绘制时线宽无效
/// @param isFill 是否是填充颜色图形
static inline void drawEllipse(TpCanvasData *set, const int32_t &x, const int32_t &y, const int32_t &rx, const int32_t &ry, const int32_t &color, double width, const bool &isFill, const HollowMask &hollowMaskData = HollowMask())
{
    if (!set->swCanvas)
        return;

    refreshCanvasTarget(set);

    auto circle = tvg::Shape::gen();
    circle->appendCircle(x, y, rx, ry);

    tvg::Fill *gradientPtr = parseGradientPtr(set);

    if (isFill)
    {
        if (gradientPtr)
            circle->fill(gradientPtr);
        else
            circle->fill(_R(color), _G(color), _B(color), _A(color)); // set its color (r, g, b)

        applyHollowMask(circle, x - rx, y - ry, hollowMaskData);
    }
    else
    {
        if (gradientPtr)
            circle->strokeFill(gradientPtr);
        else
            circle->strokeFill(_R(color), _G(color), _B(color), _A(color));

        circle->strokeWidth(width);
    }

    set->tvgScene->push(std::move(circle));
    // set->swCanvas->push(std::move(circle));

    // 绘制并同步
    // set->swCanvas->draw();
    // set->swCanvas->sync();
}

void TpCanvas::circle(int32_t x, int32_t y, int32_t rad, int32_t color, double width)
{
    TpCanvasData *set = static_cast<TpCanvasData *>(data_);

    if (set && set->beUsed)
    {
        x = OFFSET_X(set, x);
        y = OFFSET_Y(set, y);

        drawEllipse(set, x, y, rad, rad, color, width, false);
    }
}

void TpCanvas::filledCircle(int32_t x, int32_t y, int32_t rad, int32_t color, const HollowMask &hollowMaskData)
{
    TpCanvasData *set = static_cast<TpCanvasData *>(data_);

    if (set && set->beUsed)
    {
        x = OFFSET_X(set, x);
        y = OFFSET_Y(set, y);

        drawEllipse(set, x, y, rad, rad, color, 0, true);
    }
}

void TpCanvas::ellipse(int32_t x, int32_t y, int32_t rx, int32_t ry, int32_t color, double width)
{
    TpCanvasData *set = static_cast<TpCanvasData *>(data_);

    if (set && set->beUsed)
    {
        x = OFFSET_X(set, x);
        y = OFFSET_Y(set, y);

        drawEllipse(set, x, y, rx, ry, color, width, false);
    }
}

void TpCanvas::filledEllipse(int32_t x, int32_t y, int32_t rx, int32_t ry, int32_t color, const HollowMask &hollowMaskData)
{
    TpCanvasData *set = static_cast<TpCanvasData *>(data_);

    if (set && set->beUsed)
    {
        x = OFFSET_X(set, x);
        y = OFFSET_Y(set, y);

        drawEllipse(set, x, y, rx, ry, color, 0, true);
    }
}

static inline void drawArc(TpCanvasData *set, const int32_t &x, const int32_t &y, const int32_t &rad,
                           const double &start, const double &end, const int32_t &color, double width,
                           const bool &isRound, bool isPie, bool isFill, const HollowMask &hollowMaskData = HollowMask())
{
    if (!set->swCanvas)
        return;

    refreshCanvasTarget(set);

    // 绘制矩形填充
    tvg::Shape *arc = tvg::Shape::gen();

    // 将角度转换为弧度
    float startRad = start * M_PI / 180.0f;
    float endRad = end * M_PI / 180.0f;

    // 计算起点和终点
    float startX = x + rad * cosf(startRad);
    float startY = y + rad * sinf(startRad);
    float endX = x + rad * cosf(endRad);
    float endY = y + rad * sinf(endRad);

    if (isPie)
    {
        // 绘制扇形路径
        arc->moveTo(x, y);           // 移动到圆心
        arc->lineTo(startX, startY); // 画线到起点
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

    tvg::Fill *gradientPtr = parseGradientPtr(set);

    if (isPie)
    {
        // 封口
        arc->lineTo(x, y); // 画线到起点
        arc->close();      // 闭合路径回到圆心

        if (isFill)
        {
            if (gradientPtr)
                arc->fill(gradientPtr);
            else
                arc->fill(_R(color), _G(color), _B(color), _A(color));

            applyHollowMask(arc, x - rad, y - rad, hollowMaskData);
        }
        else
        {
            if (gradientPtr)
                arc->strokeFill(gradientPtr);
            else
                arc->strokeFill(_R(color), _G(color), _B(color), _A(color));

            arc->strokeWidth(width);
        }
    }
    else
    {
        if (gradientPtr)
            arc->strokeFill(gradientPtr);
        else
            arc->strokeFill(_R(color), _G(color), _B(color), _A(color));

        arc->strokeWidth(width);

        if (isRound)
        {
            arc->strokeCap(tvg::StrokeCap::Round);   // 圆角线头
            arc->strokeJoin(tvg::StrokeJoin::Round); // 圆角连接
        }
    }

    set->tvgScene->push(std::move(arc));
    // set->swCanvas->push(std::move(arc));

    // 绘制并同步
    // set->swCanvas->draw();
    // set->swCanvas->sync();
}

void TpCanvas::arc(int32_t x, int32_t y, int32_t rad, int32_t start, int32_t end, int32_t color, double width, const bool &isRound)
{
    TpCanvasData *set = static_cast<TpCanvasData *>(data_);

    if (set && set->beUsed)
    {
        x = OFFSET_X(set, x);
        y = OFFSET_Y(set, y);

        drawArc(set, x, y, rad, start, end, color, width, isRound, false, false);
    }
}

void TpCanvas::pie(int32_t x, int32_t y, int32_t rad, int32_t start, int32_t end, int32_t color, double width)
{
    TpCanvasData *set = static_cast<TpCanvasData *>(data_);

    if (set && set->beUsed)
    {
        x = OFFSET_X(set, x);
        y = OFFSET_Y(set, y);

        drawArc(set, x, y, rad, start, end, color, width, false, true, false);
    }
}

void TpCanvas::filledPie(int32_t x, int32_t y, int32_t rad, int32_t start, int32_t end, int32_t color, const HollowMask &hollowMaskData)
{
    TpCanvasData *set = static_cast<TpCanvasData *>(data_);

    if (set && set->beUsed)
    {
        x = OFFSET_X(set, x);
        y = OFFSET_Y(set, y);

        drawArc(set, x, y, rad, start, end, color, 1, false, true, true);
    }
}

static inline void drawPolygon(TpCanvasData *set, const TpVector<ItpPoint> &pointList, int32_t color, double width, bool isFill, const HollowMask &hollowMaskData = HollowMask())
{
    if (pointList.size() == 0)
        return;

    if (!set->swCanvas)
        return;

    // 只有一个点，绘制一个像素点
    if (pointList.size() == 1)
    {
        drawPixel(set, pointList.front().x + set->offsetX, pointList.front().y + set->offsetY, color);
    }
    else if (pointList.size() == 2)
    {
        // 两个点，绘制线
        const auto &firstPoint = pointList[0];
        const auto &secondPoint = pointList[1];
        drawLine(set, firstPoint.x + set->offsetX, firstPoint.y + set->offsetY, secondPoint.x + set->offsetX, secondPoint.y + set->offsetY, color, width);
    }
    else
    {
        // 绘制多边形
        refreshCanvasTarget(set);

        auto polygon = tvg::Shape::gen();

        // 移动到第一个顶点
        polygon->moveTo(pointList.front().x + set->offsetX, pointList.front().y + set->offsetY);

        for (int i = 1; i < pointList.size(); ++i)
        {
            const auto &curPoint = pointList[i];
            polygon->lineTo(curPoint.x + set->offsetX, curPoint.y + set->offsetY);
        }
        // 闭合路径回到起点
        polygon->close();

        tvg::Fill *gradientPtr = parseGradientPtr(set);

        if (isFill)
        {
            if (gradientPtr)
                polygon->fill(gradientPtr);
            else
                polygon->fill(_R(color), _G(color), _B(color), _A(color));

            applyHollowMask(polygon, pointList.front().x + set->offsetX, pointList.front().y + set->offsetY, hollowMaskData);
        }
        else
        {
            if (gradientPtr)
                polygon->strokeFill(gradientPtr);
            else
                polygon->strokeFill(_R(color), _G(color), _B(color), _A(color));

            polygon->strokeWidth(width);
        }

        set->tvgScene->push(std::move(polygon));
        // set->swCanvas->push(std::move(polygon));
        // set->swCanvas->draw();
        // set->swCanvas->sync();
    }
}

void TpCanvas::polygon(const TpVector<ItpPoint> &pointList, int32_t color, double width)
{
    TpCanvasData *set = static_cast<TpCanvasData *>(data_);

    if (set && set->beUsed)
    {
        drawPolygon(set, pointList, color, width, false);
    }
}

void TpCanvas::filledPolygon(const TpVector<ItpPoint> &pointList, int32_t color, const HollowMask &hollowMaskData)
{
    TpCanvasData *set = static_cast<TpCanvasData *>(data_);

    if (set && set->beUsed)
    {
        drawPolygon(set, pointList, color, 1, true);
    }
}

void TpCanvas::paintImage(const int32_t &x, const int32_t &y, const TpImage &image, int32_t roundRad)
{
    TpCanvasData *set = static_cast<TpCanvasData *>(data_);
    if (!set->swCanvas)
        return;

    refreshCanvasTarget(set);

    TpImageData *imageData = static_cast<TpImageData *>(image.data_);

    // 创建深拷贝（不修改原对象）
    tvg::Picture *pictureCopy = static_cast<tvg::Picture *>(imageData->tvgPicture->duplicate());
    if (image.isRotated())
    {
        float width = image.width();
        float height = image.height();
        float angle = image.rotateAngle(); // 获取旋转角度
        float rad = angle * M_PI / 180.0f;
        float cosθ = cos(-rad);
        float sinθ = sin(-rad);

        // 计算原始中心点
        float originalCenterX = width / 2.0f;
        float originalCenterY = height / 2.0f;

        float rotatedCenterX = originalCenterX * cosθ + originalCenterY * sinθ;
        float rotatedCenterY = -originalCenterX * sinθ + originalCenterY * cosθ;

        // 计算绘制坐标，使得旋转后图片的中心位于指定位置
        float drawX = -(rotatedCenterX - originalCenterX);
        float drawY = -(rotatedCenterY - originalCenterY);

        // std::cout << "原始中心点坐标： " << originalCenterX << " , " << originalCenterY << std::endl;
        // std::cout << "旋转后中心点坐标： " << rotatedCenterX << " , " << rotatedCenterY << std::endl;
        // std::cout << "新的绘制顶点坐标 " << drawX << " , " << drawY << std::endl;

        // 调整绘制位置：减去偏移量，使中心点回到原位
        pictureCopy->translate(
            set->offsetX + x + drawX,
            set->offsetY + y + drawY);
    }
    else
    {
        pictureCopy->translate(set->offsetX + x, set->offsetY + y);
    }

    if (roundRad != 0)
    {
        int32_t imageMaxRound = (image.width() < image.height() ? image.width() : image.height()) / 2.0;
        if (roundRad > imageMaxRound)
            roundRad = imageMaxRound;

        // 添加圆角遮罩
        auto clipper = tvg::Shape::gen();
        clipper->appendRect(set->offsetX + x, set->offsetY + y, image.width(), image.height(), roundRad, roundRad);

        // 应用Alpha实现遮罩圆角
        clipper->fill(255, 255, 255, 255);
        pictureCopy->mask(clipper, tvg::MaskMethod::Alpha);
    }

    set->tvgScene->push(std::move(pictureCopy));
    // set->swCanvas->push(std::move(pictureCopy));
    // set->swCanvas->draw();
    // set->swCanvas->sync();
}

void TpCanvas::renderText(TpFont &font, int32_t x, int32_t y, const TpString &text)
{
    TpCanvasData *set = static_cast<TpCanvasData *>(data_);
    if (!set)
        return;

    if (!set->beUsed)
        return;

    x = OFFSET_X(set, x);
    y = OFFSET_Y(set, y);

    uint32_t *textBuffer = font.renderText(text.c_str());

    ItpSize pixelSize = font.pixelSize();
    tvg::Picture *picture = tvg::Picture::gen();
    picture->load(textBuffer, pixelSize.w, pixelSize.h, tvg::ColorSpace::ARGB8888, true);
    picture->translate(x, y);

    refreshCanvasTarget(set);
    set->tvgScene->push(std::move(picture));

    delete[] textBuffer;
    textBuffer = nullptr;
}

void TpCanvas::renderText(TpFont &font, int32_t x, int32_t y)
{
    renderText(font, x, y, font.text());
}

void TpCanvas::addScene(void *canvas, void *scene)
{
    TpCanvasData *set = static_cast<TpCanvasData *>(data_);

    if (set->swCanvas)
    {
        delete set->swCanvas;
        set->swCanvas = nullptr;
    }

    tvg::SwCanvas *addCanvas = (tvg::SwCanvas *)canvas;
    set->swCanvas = addCanvas;

    // 如果已有 Scene，先移除
    // if (set->tvgScene)
    // {
    //     set->swCanvas->remove(set->tvgScene);
    //     set->tvgScene = nullptr;
    // }

    tvg::Scene *addScene = (tvg::Scene *)scene;
    set->tvgScene = addScene;
    // set->tvgScene = static_cast<tvg::Scene *>(addScene->duplicate());
    // set->swCanvas->push(set->tvgScene);

    // if (set->tvgScene == nullptr)
    // {
    //     tvg::Scene* addScene = (tvg::Scene*)scene;
    //     set->tvgScene = static_cast<tvg::Scene*>(addScene->duplicate());
    //     set->swCanvas->push(set->tvgScene);
    // }
}

void TpCanvas::sync()
{
    TpCanvasData *set = static_cast<TpCanvasData *>(data_);

    // 确保设置了渲染目标
    // refreshCanvasTarget(set);

    // 强制场景更新
    // if (set->tvgScene)
    // {
    //     std::list<tvg::Paint *> paintsList = set->tvgScene->paints(); // 关键！更新场景状态
    //     std::cout << "绘制对象数量： " << paintsList.size() << std::endl;
    // }

    // 使用viewport限制清除区域
    // ItpRect clipRect = set->TpSurfacePtr->clipRect();

    // std::cout << "clipRect 区域： " << clipRect.x << " , " << clipRect.y << " , "
    //           << clipRect.w << " , " << clipRect.h << std::endl;

    // set->swCanvas->sync();
    // tvg::Result viewportRes = set->swCanvas->viewport(clipRect.x, clipRect.y, clipRect.w, clipRect.h); // 只影响小区域
    // std::cout << "viewportRes 设置结果： " << (int32_t)viewportRes <<std::endl;

    set->swCanvas->draw();
    set->swCanvas->sync();

    // 绘制并同步
    // set->swCanvas->push(std::move(set->tvgScene));

    // set->swCanvas->draw();
    // set->swCanvas->sync();
}

HollowMask::HollowMask()
{
}

HollowMask::~HollowMask()
{
}

void HollowMask::addRectHollow(const ItpRect &region, const uint32_t &round)
{
    addRectHollow(HollowMask::RectHollow(region, round));
}

void HollowMask::addRectHollow(const RectHollow &data)
{
    rectList_.emplace_back(data);
}

TpVector<HollowMask::RectHollow> HollowMask::rectHollowList() const
{
    return rectList_;
}

void HollowMask::addCircleHollow(const int32_t &x, const int32_t &y, const uint32_t &radius)
{
    addCircleHollow(HollowMask::CircleHollow(x, y, radius));
}

void HollowMask::addCircleHollow(const CircleHollow &data)
{
    circleList_.emplace_back(data);
}

TpVector<HollowMask::CircleHollow> HollowMask::circleHollowList() const
{
    return circleList_;
}

void HollowMask::addPieHollow(const int32_t &x, const int32_t &y, const uint32_t &radius, const int32_t &start, const int32_t &end)
{
    addPieHollow(PieHollow(x, y, radius, start, end));
}

void HollowMask::addPieHollow(const PieHollow &data)
{
    pieList_.emplace_back(data);
}

TpVector<HollowMask::PieHollow> HollowMask::pieHollowList() const
{
    return pieList_;
}

void HollowMask::addPolygonHollow(const PolygonHollow &polygon)
{
    polygonList_.emplace_back(polygon);
}

TpVector<HollowMask::PolygonHollow> HollowMask::polygonHollowList() const
{
    return polygonList_;
}
