#include "tpCanvas.h"
#include "tpSurface.h"
#include "tpFont.h"
#include "tpRect.h"
#include "tpDef.h"
#include "thorVG/thorvg.h"

#include <thread>
#include <cmath>

#define OFFSET_X(set, x) (set->offsetX + x)
#define OFFSET_Y(set, y) (set->offsetY + y)

struct ItpCanvasSet
{
    tpShared<tpSurface> tpSurfacePtr;
    int32_t offsetX, offsetY;
    bool beUsed;

    // CPU绘制引擎
    tvg::SwCanvas *swCanvas = nullptr;
    // 使用OpenGL加速的绘制引擎；需有GPU和OpenGL才能使用；暂时无用
    tvg::GlCanvas *glCanvas = nullptr;
};

// 绘制圆角图片资源数据
struct RoundSurfaceData
{
    tpShared<tpSurface> surface = nullptr;
    int32_t roundRad = 0;

    // 绘制坐标起始点
    int32_t x = 0;
    int32_t y = 0;

    // 偏移量
    int32_t offsetX = 0;
    int32_t offsetY = 0;
};

// 重设canvas的target
static inline void refreshCanvasTarget(ItpCanvasSet *set)
{
    int32_t surfaceWidth = set->tpSurfacePtr->width();
    int32_t surfaceHeight = set->tpSurfacePtr->height();
    set->swCanvas->target((uint32_t *)set->tpSurfacePtr->matrix(), surfaceWidth, surfaceWidth, surfaceHeight, tvg::ColorSpace::ARGB8888);
}

tpCanvas::tpCanvas(tpShared<tpSurface> surface, int32_t offsetX, int32_t offsetY)
{
    if (!surface)
        return;

    ItpCanvasSet *set = new ItpCanvasSet();

    if (!set)
        return;

    set->beUsed = false;
    set->offsetX = offsetX;
    set->offsetY = offsetY;

    set->tpSurfacePtr = surface;
    set->beUsed = (surface != nullptr);

    // 根据CPU核心数；分配绘图引擎线程数
    uint32_t cores = std::thread::hardware_concurrency();
    tvg::Initializer::init(cores / 2);

    // TODO判断是GPU环境还是CPU环境
    set->swCanvas = tvg::SwCanvas::gen();

    refreshCanvasTarget(set);

    this->data_ = set;
}

tpCanvas::~tpCanvas()
{
    ItpCanvasSet *set = static_cast<ItpCanvasSet *>(data_);
    if (!set)
        return;

    if (!set->beUsed)
        return;

    set->tpSurfacePtr = nullptr;
    set->beUsed = false;

    delete set->swCanvas;
    delete set->glCanvas;

    tvg::Initializer::term();

    delete set;
}

tpShared<tpSurface> tpCanvas::convertFromCairoToSurface(cairo_surface_t *cairo_surface)
{
    if (cairo_surface == nullptr)
    {
        return nullptr;
    }

    void *addr = (void *)cairo_image_surface_get_data(cairo_surface);
    cairo_format_t fmt = cairo_image_surface_get_format(cairo_surface);
    int32_t width = cairo_image_surface_get_width(cairo_surface);
    int32_t height = cairo_image_surface_get_height(cairo_surface);
    int32_t stride = cairo_image_surface_get_stride(cairo_surface);

    if (addr == nullptr)
    {
        return nullptr;
    }

    ItpFormat format = TP_RGB_UNKOWN;
    int32_t Rmask = 0, Gmask = 0, Bmask = 0, Amask = 0;

    switch (fmt)
    {
    case CAIRO_FORMAT_ARGB32:
    case CAIRO_FORMAT_RGB24:
    {
        format = TP_RGB_32;

        Rmask = 0x00ff0000;
        Gmask = 0x0000ff00;
        Bmask = 0x000000ff;
        Amask = (fmt == CAIRO_FORMAT_RGB24) ? 0x00000000 : 0xff000000;
    }
    break;
    case CAIRO_FORMAT_RGB16_565:
    {
        format = TP_RGB_16;

        Rmask = 0x0000f800;
        Gmask = 0x000007e0;
        Bmask = 0x0000001f;
    }
    break;
    case CAIRO_FORMAT_A8:
    {
        format = TP_RGB_8;
    }
    break;
    default:
        return nullptr;
    }

    tpShared<tpSurface> surface = tpMakeShared<tpSurface>();
    bool ret = surface->create(addr, width, height, format, stride, Rmask, Gmask, Bmask, Amask);

    if (ret == false)
    {
        return nullptr;
    }

    return surface;
}

void tpCanvas::paintTest()
{

    // 绘制并同步
    // set->swCanvas->draw();
    // set->swCanvas->sync();
}

bool tpCanvas::setTarget(tpShared<tpSurface> surface, int32_t offsetX, int32_t offsetY)
{
    ItpCanvasSet *set = static_cast<ItpCanvasSet *>(data_);

    if (!set)
        return false;

    if (surface == nullptr)
        return false;

    if (set->beUsed)
    {
        set->tpSurfacePtr = nullptr;
    }

    set->tpSurfacePtr = surface;
    set->offsetX = offsetX;
    set->offsetY = offsetY;
    set->beUsed = true;

    return true;
}

tpShared<tpSurface> tpCanvas::surface()
{
    ItpCanvasSet *set = static_cast<ItpCanvasSet *>(data_);

    if (set && set->beUsed)
    {
        return set->tpSurfacePtr;
    }

    return nullptr;
}

void tpCanvas::setClipRect(tpRect &rect)
{
    ItpCanvasSet *set = static_cast<ItpCanvasSet *>(data_);

    if (set && set->beUsed)
    {
        set->tpSurfacePtr->setClipRect(&rect);
    }
}

void tpCanvas::setClipRect(ItpRect *rect)
{
    if (rect)
    {
        tpRect r(rect);
        this->setClipRect(r);
    }
}

void tpCanvas::erase()
{
    ItpCanvasSet *set = static_cast<ItpCanvasSet *>(data_);

    if (set && set->beUsed)
    {
        // 方法1：清除所有 Paint 对象
        set->swCanvas->remove();

        // 方法2：清除缓冲区并重新绘制
        set->swCanvas->draw(true);
        set->swCanvas->sync();
    }
}

static inline void drawPixel(ItpCanvasSet *set, int32_t x, int32_t y, int32_t color)
{
    if (!set->swCanvas)
        return;

    refreshCanvasTarget(set);

    auto pixel = tvg::Shape::gen();
    pixel->appendCircle(x, y, 0.5, 0.5); // 半径 0.5 的圆形
    pixel->fill(_R(color), _G(color), _B(color), _A(color));
    set->swCanvas->push(std::move(pixel));

    set->swCanvas->draw();
    set->swCanvas->sync();
}

void tpCanvas::pixel(int32_t x, int32_t y, int32_t color)
{
    ItpCanvasSet *set = static_cast<ItpCanvasSet *>(data_);

    if (set && set->beUsed)
    {
        x = OFFSET_X(set, x);
        y = OFFSET_Y(set, y);

        drawPixel(set, x, y, color);
    }
}

static inline void drawLine(ItpCanvasSet *set, int32_t x1, int32_t y1, int32_t x2, int32_t y2, int32_t color, double width)
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
    line->strokeFill(_R(color), _G(color), _B(color), _A(color));
    line->strokeCap(tvg::StrokeCap::Round);   // 圆角线头
    line->strokeJoin(tvg::StrokeJoin::Round); // 圆角连接

    set->swCanvas->push(std::move(line));

    // 绘制并同步
    set->swCanvas->draw();
    set->swCanvas->sync();

    return;
}

void tpCanvas::hline(int32_t x1, int32_t x2, int32_t y, int32_t color, double width)
{
    ItpCanvasSet *set = static_cast<ItpCanvasSet *>(data_);

    if (set && set->beUsed)
    {

        x1 = OFFSET_X(set, x1);
        x2 = OFFSET_X(set, x2);
        y = OFFSET_Y(set, y);

        drawLine(set, x1, y, x2, y, color, width);
    }
}

void tpCanvas::vline(int32_t x, int32_t y1, int32_t y2, int32_t color, double width)
{
    ItpCanvasSet *set = static_cast<ItpCanvasSet *>(data_);

    if (set && set->beUsed)
    {
        x = OFFSET_X(set, x);
        y1 = OFFSET_Y(set, y1);
        y2 = OFFSET_Y(set, y2);

        drawLine(set, x, y1, x, y2, color, width);
    }
}

void tpCanvas::line(int32_t x1, int32_t y1, int32_t x2, int32_t y2, int32_t color, double width)
{
    ItpCanvasSet *set = static_cast<ItpCanvasSet *>(data_);

    if (set && set->beUsed)
    {
        x1 = OFFSET_X(set, x1);
        y1 = OFFSET_Y(set, y1);
        x2 = OFFSET_X(set, x2);
        y2 = OFFSET_Y(set, y2);

        drawLine(set, x1, y1, x2, y2, color, width);
    }
}

static inline void drawRectangle(ItpCanvasSet *set, int32_t x1, int32_t y1, int32_t x2, int32_t y2, int32_t color, double width, int32_t rad, bool isFill)
{
    if (!set->swCanvas)
        return;

    refreshCanvasTarget(set);

    // 绘制矩形填充
    auto rect = tvg::Shape::gen();
    rect->appendRect(x1, y1, x2 - x1, y2 - y1, rad, rad);

    if (isFill)
    {
        rect->fill(_R(color), _G(color), _B(color), _A(color)); // set its color (r, g, b)
    }
    else
    {
        rect->strokeFill(_R(color), _G(color), _B(color), _A(color));
        rect->strokeWidth(width);
    }

    set->swCanvas->push(std::move(rect));

    // 绘制并同步
    set->swCanvas->draw();
    set->swCanvas->sync();
}

void tpCanvas::rectangle(int32_t x1, int32_t y1, int32_t x2, int32_t y2, int32_t color, double width)
{
    ItpCanvasSet *set = static_cast<ItpCanvasSet *>(data_);

    if (set && set->beUsed)
    {
        x1 = OFFSET_X(set, x1);
        y1 = OFFSET_Y(set, y1);
        x2 = OFFSET_X(set, x2);
        y2 = OFFSET_Y(set, y2);

        drawRectangle(set, x1, y1, x2, y2, color, width, 0, false);
    }
}

void tpCanvas::roundedRectangle(int32_t x1, int32_t y1, int32_t x2, int32_t y2, int32_t rad, int32_t color, double width)
{
    ItpCanvasSet *set = static_cast<ItpCanvasSet *>(data_);

    if (set && set->beUsed)
    {
        x1 = OFFSET_X(set, x1);
        y1 = OFFSET_Y(set, y1);
        x2 = OFFSET_X(set, x2);
        y2 = OFFSET_Y(set, y2);

        drawRectangle(set, x1, y1, x2, y2, color, width, rad, false);
    }
}

void tpCanvas::box(int32_t x1, int32_t y1, int32_t x2, int32_t y2, int32_t color)
{
    ItpCanvasSet *set = static_cast<ItpCanvasSet *>(data_);

    if (set && set->beUsed)
    {
        x1 = OFFSET_X(set, x1);
        y1 = OFFSET_Y(set, y1);
        x2 = OFFSET_X(set, x2);
        y2 = OFFSET_Y(set, y2);

        drawRectangle(set, x1, y1, x2, y2, color, 1, 0, true);
    }
}

void tpCanvas::roundedBox(int32_t x1, int32_t y1, int32_t x2, int32_t y2, int32_t rad, int32_t color)
{
    ItpCanvasSet *set = static_cast<ItpCanvasSet *>(data_);

    if (set &&
        set->beUsed)
    {
        x1 = OFFSET_X(set, x1);
        y1 = OFFSET_Y(set, y1);
        x2 = OFFSET_X(set, x2);
        y2 = OFFSET_Y(set, y2);

        drawRectangle(set, x1, y1, x2, y2, color, 1, rad, true);
    }
}

/// @brief 绘制圆形、椭圆；填充或线条
/// @param set
/// @param x 坐标
/// @param y
/// @param rx 长轴
/// @param ry 短轴
/// @param color 颜色
/// @param width 线条宽度，当填充绘制时线宽无效
/// @param isFill 是否是填充颜色图形
static inline void drawEllipse(ItpCanvasSet *set, const int32_t &x, const int32_t &y, const int32_t &rx, const int32_t &ry, const int32_t &color, double width, const bool &isFill)
{
    if (!set->swCanvas)
        return;

    refreshCanvasTarget(set);

    auto circle = tvg::Shape::gen();
    circle->appendCircle(x, y, rx, ry);

    if (isFill)
    {
        circle->fill(_R(color), _G(color), _B(color), _A(color)); // set its color (r, g, b)
    }
    else
    {
        circle->strokeFill(_R(color), _G(color), _B(color), _A(color));
        circle->strokeWidth(width);
    }

    set->swCanvas->push(std::move(circle)); // push the rectangle into the canvas

    // 绘制并同步
    set->swCanvas->draw();
    set->swCanvas->sync();
}

void tpCanvas::circle(int32_t x, int32_t y, int32_t rad, int32_t color, double width)
{
    ItpCanvasSet *set = static_cast<ItpCanvasSet *>(data_);

    if (set && set->beUsed)
    {
        x = OFFSET_X(set, x);
        y = OFFSET_Y(set, y);

        drawEllipse(set, x, y, rad, rad, color, width, false);
    }
}

void tpCanvas::filledCircle(int32_t x, int32_t y, int32_t rad, int32_t color)
{
    ItpCanvasSet *set = static_cast<ItpCanvasSet *>(data_);

    if (set && set->beUsed)
    {
        x = OFFSET_X(set, x);
        y = OFFSET_Y(set, y);

        drawEllipse(set, x, y, rad, rad, color, 0, true);
    }
}

void tpCanvas::ellipse(int32_t x, int32_t y, int32_t rx, int32_t ry, int32_t color, double width)
{
    ItpCanvasSet *set = static_cast<ItpCanvasSet *>(data_);

    if (set && set->beUsed)
    {
        x = OFFSET_X(set, x);
        y = OFFSET_Y(set, y);

        drawEllipse(set, x, y, rx, ry, color, width, false);
    }
}

void tpCanvas::filledEllipse(int32_t x, int32_t y, int32_t rx, int32_t ry, int32_t color)
{
    ItpCanvasSet *set = static_cast<ItpCanvasSet *>(data_);

    if (set && set->beUsed)
    {
        x = OFFSET_X(set, x);
        y = OFFSET_Y(set, y);

        drawEllipse(set, x, y, rx, ry, color, 0, true);
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

static inline void drawArc(ItpCanvasSet *set, const int32_t &x, const int32_t &y, const int32_t &rad,
                           const double &start, const double &end, const int32_t &color, double width,
                           const bool &isRound, bool isPie, bool isFill)
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

    if (isPie)
    {
        // 封口
        arc->lineTo(x, y); // 画线到起点
        arc->close();      // 闭合路径回到圆心

        if (isFill)
        {
            arc->fill(_R(color), _G(color), _B(color), _A(color));
        }
        else
        {
            arc->strokeFill(_R(color), _G(color), _B(color), _A(color));
            arc->strokeWidth(width);
        }
    }
    else
    {
        arc->strokeFill(_R(color), _G(color), _B(color), _A(color));
        arc->strokeWidth(width);

        if (isRound)
        {
            arc->strokeCap(tvg::StrokeCap::Round);   // 圆角线头
            arc->strokeJoin(tvg::StrokeJoin::Round); // 圆角连接
        }
    }

    set->swCanvas->push(std::move(arc));

    // 绘制并同步
    set->swCanvas->draw();
    set->swCanvas->sync();
}

void tpCanvas::arc(int32_t x, int32_t y, int32_t rad, int32_t start, int32_t end, int32_t color, double width, const bool &isRound)
{
    ItpCanvasSet *set = static_cast<ItpCanvasSet *>(data_);

    if (set && set->beUsed)
    {
        x = OFFSET_X(set, x);
        y = OFFSET_Y(set, y);

        drawArc(set, x, y, rad, start, end, color, width, isRound, false, false);
    }
}

void tpCanvas::pie(int32_t x, int32_t y, int32_t rad, int32_t start, int32_t end, int32_t color, double width)
{
    ItpCanvasSet *set = static_cast<ItpCanvasSet *>(data_);

    if (set && set->beUsed)
    {
        x = OFFSET_X(set, x);
        y = OFFSET_Y(set, y);

        drawArc(set, x, y, rad, start, end, color, width, false, true, false);
    }
}

void tpCanvas::filledPie(int32_t x, int32_t y, int32_t rad, int32_t start, int32_t end, int32_t color)
{
    ItpCanvasSet *set = static_cast<ItpCanvasSet *>(data_);

    if (set && set->beUsed)
    {
        x = OFFSET_X(set, x);
        y = OFFSET_Y(set, y);

        drawArc(set, x, y, rad, start, end, color, 1, false, true, true);
    }
}

static inline void drawPolygon(ItpCanvasSet *set, const tpVector<ItpPoint> &pointList, int32_t color, double width, bool isFill)
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

        if (isFill)
        {
            polygon->fill(_R(color), _G(color), _B(color), _A(color));
        }
        else
        {
            polygon->strokeFill(_R(color), _G(color), _B(color), _A(color));
            polygon->strokeWidth(width);
        }

        set->swCanvas->push(std::move(polygon));

        // 绘制并同步
        set->swCanvas->draw();
        set->swCanvas->sync();
    }
}

void tpCanvas::polygon(const tpVector<ItpPoint> &pointList, int32_t color, double width)
{
    ItpCanvasSet *set = static_cast<ItpCanvasSet *>(data_);

    if (set && set->beUsed)
    {
        drawPolygon(set, pointList, color, width, false);
    }
}

void tpCanvas::filledPolygon(const tpVector<ItpPoint> &pointList, int32_t color)
{
    ItpCanvasSet *set = static_cast<ItpCanvasSet *>(data_);

    if (set && set->beUsed)
    {
        drawPolygon(set, pointList, color, 1, true);
    }
}

// 公共掏空操作函数
static void applyHollowMask(ItpCanvasSet *set, int32_t x1, int32_t y1, int32_t x2, int32_t y2, int32_t color, int32_t rad, const HollowMask &hollowMaskData)
{
    if (!set->swCanvas)
        return;

    refreshCanvasTarget(set);

    // 创建底纹矩形
    tvg::Shape *rect = tvg::Shape::gen();
    rect->appendRect(x1, y1, x2 - x1, y2 - y1, rad, rad);
    rect->fill(_R(color), _G(color), _B(color), _A(color));

    // 矩形镂空
    auto clipper = tvg::Shape::gen();
    tpVector<ItpRect> rectHollow = hollowMaskData.rectHollowList();
    for (const auto &hollowData : rectHollow)
    {
        // 创建裁剪形状
        clipper->appendRect(hollowData.x + x1, hollowData.y + y1, hollowData.w, hollowData.h);
    }

    // 圆角矩形镂空
    tpVector<HollowMask::RoundRectHollow> roundHollow = hollowMaskData.roundRectHollowList();
    for (const auto &hollowData : roundHollow)
    {
        // 创建裁剪形状
        clipper->appendRect(hollowData.region.x + x1, hollowData.region.y + y1, hollowData.region.w, hollowData.region.h, hollowData.round, hollowData.round);
    }

    // 圆形镂空
    tpVector<HollowMask::CircleHollow> circleHollow = hollowMaskData.circleHollowList();
    for (const auto &hollowData : circleHollow)
    {
        // 创建裁剪形状
        clipper->appendCircle(hollowData.x + x1, hollowData.y + y1, hollowData.radius, hollowData.radius);
    }

    // 扇形镂空
    tpVector<HollowMask::PieHollow> pieHollowList = hollowMaskData.pieHollowList();
    for (const auto &hollowData : pieHollowList)
    {
        // 将角度转换为弧度
        float startRad = hollowData.start * M_PI / 180.0f;
        float endRad = hollowData.end * M_PI / 180.0f;

        // 计算起点和终点
        float startX = hollowData.x + x1 + hollowData.radius * cosf(startRad);
        float startY = hollowData.y + y1 + hollowData.radius * sinf(startRad);
        float endX = hollowData.x + x1 + hollowData.radius * cosf(endRad);
        float endY = hollowData.y + y1 + hollowData.radius * sinf(endRad);

        // 绘制扇形路径
        clipper->moveTo(hollowData.x + x1, hollowData.y + y1); // 移动到圆心
        clipper->lineTo(startX, startY);                       // 画线到起点

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
        clipper->lineTo(hollowData.x + x1, hollowData.y + y1); // 画线到起点
        clipper->close();                                      // 闭合路径回到圆心
    }

    // 应用反向Alpha遮罩实现镂空
    clipper->fill(255, 255, 255, 255);
    rect->mask(clipper, tvg::MaskMethod::InvAlpha);

    set->swCanvas->push(std::move(rect));
    set->swCanvas->draw();
    set->swCanvas->sync();
}

void tpCanvas::hollowBox(int32_t x1, int32_t y1, int32_t x2, int32_t y2, int32_t color, const HollowMask &hollowMaskData)
{
    ItpCanvasSet *set = static_cast<ItpCanvasSet *>(data_);
    if (!set)
        return;

    if (!set->beUsed)
        return;

    x1 = OFFSET_X(set, x1);
    y1 = OFFSET_Y(set, y1);
    x2 = OFFSET_X(set, x2);
    y2 = OFFSET_Y(set, y2);

    applyHollowMask(set, x1, y1, x2, y2, color, 0, hollowMaskData);
}

void tpCanvas::hollowRoundedBox(int32_t x1, int32_t y1, int32_t x2, int32_t y2, int32_t rad, int32_t color, const HollowMask &hollowMaskData)
{
    ItpCanvasSet *set = static_cast<ItpCanvasSet *>(data_);
    if (!set)
        return;

    if (!set->beUsed)
        return;

    x1 = OFFSET_X(set, x1);
    y1 = OFFSET_Y(set, y1);
    x2 = OFFSET_X(set, x2);
    y2 = OFFSET_Y(set, y2);

    applyHollowMask(set, x1, y1, x2, y2, color, rad, hollowMaskData);
}

void tpCanvas::paintSurface(const int32_t &x, const int32_t &y, const tpShared<tpSurface> &surface)
{
    ItpCanvasSet *set = static_cast<ItpCanvasSet *>(data_);
    if (!set->swCanvas)
        return;

    refreshCanvasTarget(set);

    auto picture = tvg::Picture::gen();
    picture->load((uint32_t *)surface->matrix(), surface->width(), surface->height(), tvg::ColorSpace::ARGB8888, false);
    // 设置图片位置
    picture->translate(set->offsetX + x, set->offsetY + y);

    set->swCanvas->push(std::move(picture));
    set->swCanvas->draw();
    set->swCanvas->sync();
}

void tpCanvas::paintRoundSurface(const int32_t &x, const int32_t &y, int32_t rad, const tpShared<tpSurface> &surface)
{
    ItpCanvasSet *set = static_cast<ItpCanvasSet *>(data_);
    if (!set->swCanvas)
        return;

    refreshCanvasTarget(set);

    auto picture = tvg::Picture::gen();
    picture->load((uint32_t *)surface->matrix(), surface->width(), surface->height(), tvg::ColorSpace::ARGB8888, false);
    // 设置图片位置
    picture->translate(set->offsetX + x, set->offsetY + y);

    // 添加圆角遮罩
    auto clipper = tvg::Shape::gen();
    clipper->appendRect(set->offsetX + x, set->offsetY + y, surface->width(), surface->height(), rad, rad);

    // 应用Alpha实现遮罩圆角
    clipper->fill(255, 255, 255, 255);
    picture->mask(clipper, tvg::MaskMethod::Alpha);

    set->swCanvas->push(std::move(picture));
    set->swCanvas->draw();
    set->swCanvas->sync();
}

void tpCanvas::renderText(tpFont &font, int32_t x, int32_t y, const tpString &text)
{
    ItpCanvasSet *set = static_cast<ItpCanvasSet *>(data_);

    if (set && set->beUsed)
    {
        x = OFFSET_X(set, x);
        y = OFFSET_Y(set, y);

        font.renderText(set->tpSurfacePtr, text.c_str(), x, y);
    }
}

void tpCanvas::renderText(tpFont &font, int32_t x, int32_t y)
{
    ItpCanvasSet *set = static_cast<ItpCanvasSet *>(data_);

    if (set && set->beUsed)
    {
        x = OFFSET_X(set, x);
        y = OFFSET_Y(set, y);

        font.render(set->tpSurfacePtr, x, y);
    }
}

void tpCanvas::renderMarkUp(tpFont &font, int32_t x, int32_t y, const tpString &text)
{
    this->renderMarkUp(font, x, y, text.c_str());
}

void tpCanvas::renderMarkUp(tpFont &font, int32_t x, int32_t y, const char *text)
{
    ItpCanvasSet *set = static_cast<ItpCanvasSet *>(data_);

    if (set && set->beUsed)
    {
        x = OFFSET_X(set, x);
        y = OFFSET_Y(set, y);

        font.renderMarkUp(set->tpSurfacePtr, text, x, y);
    }
}

HollowMask::HollowMask()
{
}

HollowMask::~HollowMask()
{
}

void HollowMask::addRectHollow(const ItpRect &region)
{
    rectList_.emplace_back(region);
}

tpVector<ItpRect> HollowMask::rectHollowList() const
{
    return rectList_;
}

void HollowMask::addRoundRectHollow(const ItpRect &region, const uint32_t &round)
{
    addRoundRectHollow(HollowMask::RoundRectHollow(region, round));
}

void HollowMask::addRoundRectHollow(const RoundRectHollow &data)
{
    roundRectList_.emplace_back(data);
}

tpVector<HollowMask::RoundRectHollow> HollowMask::roundRectHollowList() const
{
    return roundRectList_;
}

void HollowMask::addCircleHollow(const int32_t &x, const int32_t &y, const uint32_t &radius)
{
    addCircleHollow(HollowMask::CircleHollow(x, y, radius));
}

void HollowMask::addCircleHollow(const CircleHollow &data)
{
    circleList_.emplace_back(data);
}

tpVector<HollowMask::CircleHollow> HollowMask::circleHollowList() const
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

tpVector<HollowMask::PieHollow> HollowMask::pieHollowList() const
{
    return pieList_;
}
