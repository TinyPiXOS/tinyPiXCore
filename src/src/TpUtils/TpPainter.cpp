#include "TpPainter.h"
#include "TpFont.h"
#include "TpDef.h"
#include "TpImage_p.h"
#include "TpGradient_p.h"
#include "TpPainter_p.h"
#include "TpPainterPath_p.h"

#include <thread>
#include <cmath>

TpPainter::TpPainter(tpShared<TpSurface> surface, int32_t offsetX, int32_t offsetY, int32_t width, int32_t height)
{
    // 根据CPU核心数；分配绘图引擎线程数
    uint32_t cores = std::thread::hardware_concurrency();
    tvg::Initializer::init(cores / 2);

    if (!surface)
        return;

    TpPainterData *painterData = new TpPainterData();
    if (!painterData)
        return;

    painterData->beUsed = false;
    painterData->offsetX = offsetX;
    painterData->offsetY = offsetY;

    painterData->width = width;
    painterData->height = height;

    // painterData->offsetX = 0;
    // painterData->offsetY = 0;

    painterData->TpSurfacePtr = surface;
    painterData->beUsed = (surface != nullptr);

    // TODO判断是GPU环境还是CPU环境
    // painterData->swCanvas = tvg::SwCanvas::gen();

    // refreshCanvasTarget(painterData);

    this->data_ = painterData;
}

TpPainter::~TpPainter()
{
    tvg::Initializer::term();

    TpPainterData *painterData = static_cast<TpPainterData *>(data_);
    if (!painterData)
        return;

    if (!painterData->beUsed)
        return;

    painterData->TpSurfacePtr = nullptr;
    painterData->beUsed = false;

    // delete painterData->swCanvas;
    // delete painterData->glCanvas;

    delete painterData;
    painterData = nullptr;
    data_ = nullptr;
}

void TpPainter::paintTest()
{
    TpPainterData *painterData = static_cast<TpPainterData *>(data_);
    if (!painterData)
        return;

    if (!painterData->swCanvas)
        return;

    refreshCanvasTarget(painterData);

    // 加载字体文件
    // tvg::Text::load("/home/hawk/Public/tinyPiXCore/src/data/fonts/SourceHanSansCN/SourceHanSansCN-Normal.otf");
    tvg::Text::load("/home/hawk/Public/tinyPiXCore/src/data/fonts/Taipei Sans TC Beta.ttf");

    // 创建文本对象
    auto text = tvg::Text::gen();

    // text->font("Source Han Sans CN", 32);     // 设置字体名称和大小
    text->font("Taipei Sans TC Beta", 32); // 设置字体名称和大小
    text->text("Hello ThorVG 哈哈哈!");    // 设置文本内容
    text->fill(255, 0, 0);                 // 设置文本颜色 (红色)
    text->translate(painterData->offsetX, painterData->offsetY);

    float textX, textY, textWidth, textHeight;
    text->bounds(&textX, &textY, &textWidth, &textHeight);

    std::cout << "文本坐标信息： " << textX << "  " << textY << "  " << textWidth << "  " << textHeight << std::endl;

    // 添加到 Canvas 并绘制
    painterData->tvgScene->push(std::move(text));
    // painterData->swCanvas->draw();
    // painterData->swCanvas->sync();
}

void TpPainter::setPen(const TpColors &color)
{
    TpPainterData *painterData = static_cast<TpPainterData *>(data_);
    painterData->drawPen.setColor(color);
}

void TpPainter::setPen(const TpPen &pen)
{
    TpPainterData *painterData = static_cast<TpPainterData *>(data_);
    painterData->drawPen = pen;
}

TpPen &TpPainter::pen() const
{
    TpPainterData *painterData = static_cast<TpPainterData *>(data_);
    return painterData->drawPen;
}

void TpPainter::setBrush(const TpBrush &brush)
{
    TpPainterData *painterData = static_cast<TpPainterData *>(data_);
    painterData->drawBrush = brush;
}

TpBrush &TpPainter::brush() const
{
    TpPainterData *painterData = static_cast<TpPainterData *>(data_);
    return painterData->drawBrush;
}

void TpPainter::drawPoint(int32_t x, int32_t y)
{
    TpPainterData *painterData = static_cast<TpPainterData *>(data_);

    if (painterData && painterData->beUsed)
    {
        x = OFFSET_X(painterData, x);
        y = OFFSET_Y(painterData, y);

        renderPoint(painterData, x, y);
    }
}

void TpPainter::drawPoint(const TpPoint &point)
{
    drawPoint(point.x(), point.y());
}

void TpPainter::drawHLine(int32_t x1, int32_t x2, int32_t y)
{
    drawLine(TpPoint(x1, y), TpPoint(x2, y));
}

void TpPainter::drawVLine(int32_t x, int32_t y1, int32_t y2)
{
    drawLine(TpPoint(x, y1), TpPoint(x, y2));
}

void TpPainter::drawLine(int32_t x1, int32_t y1, int32_t x2, int32_t y2)
{
    drawLine(TpPoint(x1, y1), TpPoint(x2, y2));
}

void TpPainter::drawLine(const TpPoint &point1, const TpPoint &point2)
{
    TpPainterData *painterData = static_cast<TpPainterData *>(data_);

    if (painterData && painterData->beUsed)
    {
        TpPoint actualP1(OFFSET_X(painterData, point1.x()), OFFSET_Y(painterData, point1.y()));
        TpPoint actualP2(OFFSET_X(painterData, point2.x()), OFFSET_Y(painterData, point2.y()));
        renderLine(painterData, actualP1, actualP2);
    }
}

void TpPainter::drawRect(int32_t x1, int32_t y1, int32_t w, int32_t h, int32_t rad, const TpHollowMask &hollowMaskData)
{
    drawRect(TpRect(x1, y1, w, h), rad, hollowMaskData);
}

void TpPainter::drawRect(const TpRect &rect, int32_t rad, const TpHollowMask &hollowMaskData)
{
    TpPainterData *painterData = static_cast<TpPainterData *>(data_);

    if (painterData && painterData->beUsed)
    {
        TpRect actualRect(OFFSET_X(painterData, rect.x()), OFFSET_Y(painterData, rect.y()), rect.width(), rect.height());
        renderRect(painterData, actualRect, rad, hollowMaskData);
    }
}

void TpPainter::drawEllipse(int32_t x, int32_t y, int32_t rx, int32_t ry, const TpHollowMask &hollowMaskData)
{
    drawEllipse(TpPoint(x, y), rx, ry, hollowMaskData);
}

void TpPainter::drawEllipse(const TpPoint &center, int32_t rx, int32_t ry, const TpHollowMask &hollowMaskData)
{
    TpPainterData *painterData = static_cast<TpPainterData *>(data_);

    if (painterData && painterData->beUsed)
    {
        TpPoint actualPoint(OFFSET_X(painterData, center.x()), OFFSET_Y(painterData, center.y()));
        renderEllipse(painterData, actualPoint, rx, ry, hollowMaskData);
    }
}

void TpPainter::drawArc(int32_t x, int32_t y, int32_t rad, int32_t start, int32_t end)
{
    drawArc(TpPoint(x, y), rad, start, end);
}

void TpPainter::drawArc(const TpPoint &center, int32_t rad, int32_t start, int32_t end)
{
    TpPainterData *painterData = static_cast<TpPainterData *>(data_);

    if (painterData && painterData->beUsed)
    {
        TpPoint actualPoint(OFFSET_X(painterData, center.x()), OFFSET_Y(painterData, center.y()));
        renderArc(painterData, actualPoint, rad, start, end, false);
    }
}

void TpPainter::drawPie(int32_t x, int32_t y, int32_t rad, int32_t start, int32_t end, const TpHollowMask &hollowMaskData)
{
    drawPie(TpPoint(x, y), rad, start, end, hollowMaskData);
}

void TpPainter::drawPie(const TpPoint &center, int32_t rad, int32_t start, int32_t end, const TpHollowMask &hollowMaskData)
{
    TpPainterData *painterData = static_cast<TpPainterData *>(data_);

    if (painterData && painterData->beUsed)
    {
        TpPoint actualPoint(OFFSET_X(painterData, center.x()), OFFSET_Y(painterData, center.y()));
        renderArc(painterData, actualPoint, rad, start, end, true, hollowMaskData);
    }
}

void TpPainter::drawPolygon(const TpVector<TpPoint> &pointList, const TpHollowMask &hollowMaskData)
{
    TpPainterData *painterData = static_cast<TpPainterData *>(data_);

    if (painterData && painterData->beUsed)
    {
        renderPolygon(painterData, pointList, hollowMaskData);
    }
}

void TpPainter::drawCubic(int32_t startX, int32_t startY, int32_t cx1, int32_t cy1, int32_t cx2, int32_t cy2, int32_t endX, int32_t endY)
{
    drawCubic(TpPoint(startX, startY), TpPoint(cx1, cy1), TpPoint(cx2, cy2), TpPoint(endX, endY));
}

void TpPainter::drawCubic(const TpPoint &startPoint, const TpPoint &cPoint, const TpPoint &c2Point, const TpPoint &endPoint)
{
    TpPainterData *painterData = static_cast<TpPainterData *>(data_);

    if (!painterData)
        return;
    if (!painterData->beUsed)
        return;
    if (!painterData->swCanvas)
        return;

    refreshCanvasTarget(painterData);

    tvg::Shape *shape = tvg::Shape::gen();

    // 移动到起始点
    shape->moveTo(painterData->offsetX + startPoint.x(), painterData->offsetY + startPoint.y());

    // 绘制三次贝塞尔曲线
    shape->cubicTo(painterData->offsetX + cPoint.x(), painterData->offsetY + cPoint.y(),      // 第一个控制点
                   painterData->offsetX + c2Point.x(), painterData->offsetY + c2Point.y(),    // 第二个控制点
                   painterData->offsetX + endPoint.x(), painterData->offsetY + endPoint.y()); // 终点

    // 设置描边属性
    int32_t fillColor = painterData->drawPen.color().rgba();
    shape->strokeWidth(painterData->drawPen.width());
    shape->strokeFill(_R(fillColor), _G(fillColor), _B(fillColor), _A(fillColor));

    painterData->tvgScene->push(std::move(shape));
}

void TpPainter::drawImage(const int32_t &x, const int32_t &y, const TpImage &image, int32_t roundRad)
{
    drawImage(TpPoint(x, y), image, roundRad);
}

void TpPainter::drawImage(const TpPoint &point, const TpImage &image, int32_t roundRad)
{
    TpPainterData *painterData = static_cast<TpPainterData *>(data_);
    if (!painterData->swCanvas)
        return;

    refreshCanvasTarget(painterData);

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
            painterData->offsetX + point.x() + drawX,
            painterData->offsetY + point.y() + drawY);
    }
    else
    {
        pictureCopy->translate(painterData->offsetX + point.x(), painterData->offsetY + point.y());
    }

    if (roundRad != 0)
    {
        int32_t imageMaxRound = (image.width() < image.height() ? image.width() : image.height()) / 2.0;
        if (roundRad > imageMaxRound)
            roundRad = imageMaxRound;

        // 添加圆角遮罩
        auto clipper = tvg::Shape::gen();
        clipper->appendRect(painterData->offsetX + point.x(), painterData->offsetY + point.y(), image.width(), image.height(), roundRad, roundRad);

        // 应用Alpha实现遮罩圆角
        clipper->fill(255, 255, 255, 255);
        pictureCopy->mask(clipper, tvg::MaskMethod::Alpha);
    }

    painterData->tvgScene->push(std::move(pictureCopy));
}

void TpPainter::drawText(TpFont &font, int32_t x, int32_t y, const TpString &text)
{
    TpPainterData *painterData = static_cast<TpPainterData *>(data_);
    if (!painterData)
        return;

    if (!painterData->beUsed)
        return;

    x = OFFSET_X(painterData, x);
    y = OFFSET_Y(painterData, y);

    uint32_t *textBuffer = font.drawText(text.c_str());

    TpSize pixelSize = font.pixelSize();
    tvg::Picture *picture = tvg::Picture::gen();
    picture->load(textBuffer, pixelSize.width(), pixelSize.height(), tvg::ColorSpace::ARGB8888, true);
    picture->translate(x, y);

    refreshCanvasTarget(painterData);
    painterData->tvgScene->push(std::move(picture));

    delete[] textBuffer;
    textBuffer = nullptr;
}

void TpPainter::drawText(TpFont &font, int32_t x, int32_t y)
{
    drawText(font, x, y, font.text());
}

void TpPainter::drawPath(const TpPainterPath &path)
{
    TpPainterData *painterData = static_cast<TpPainterData *>(data_);
    if (!painterData || !painterData->beUsed)
        return;

    // 获取路径数据
    TpPainterPathData *pathData = static_cast<TpPainterPathData *>(path.data_);
    if (!pathData || pathData->elements.empty())
        return;

    refreshCanvasTarget(painterData);

    // 创建形状对象
    auto shape = tvg::Shape::gen();
    if (!shape)
        return;

    // 遍历路径元素并构建 ThorVG 路径
    TpPoint currentPoint;
    TpPoint startPoint;
    bool hasMoveTo = false;

    for (const auto &element : pathData->elements)
    {
        switch (element.type)
        {
        case TpPathElementType::MoveTo:
            if (!element.points.empty())
            {
                currentPoint = element.points[0];
                startPoint = currentPoint;
                shape->moveTo(painterData->offsetX + currentPoint.x(),
                              painterData->offsetY + currentPoint.y());
                hasMoveTo = true;
            }
            break;

        case TpPathElementType::LineTo:
            if (!element.points.empty() && hasMoveTo)
            {
                currentPoint = element.points[0];
                shape->lineTo(painterData->offsetX + currentPoint.x(),
                              painterData->offsetY + currentPoint.y());
            }
            break;

        case TpPathElementType::CubicTo:
            if (element.points.size() >= 3 && hasMoveTo)
            {
                TpPoint cp1 = element.points[0];
                TpPoint cp2 = element.points[1];
                TpPoint endPoint = element.points[2];

                shape->cubicTo(painterData->offsetX + cp1.x(), painterData->offsetY + cp1.y(),
                               painterData->offsetX + cp2.x(), painterData->offsetY + cp2.y(),
                               painterData->offsetX + endPoint.x(), painterData->offsetY + endPoint.y());

                currentPoint = endPoint;
            }
            break;

        case TpPathElementType::CloseSubpath:
            if (hasMoveTo && currentPoint != startPoint)
            {
                shape->lineTo(painterData->offsetX + startPoint.x(),
                              painterData->offsetY + startPoint.y());
                currentPoint = startPoint;
            }
            break;

        default:
            break;
        }
    }

    tvg::Fill *gradientPtr = parseGradientPtr(painterData);

    int32_t penColor = painterData->drawPen.color().rgba();
    int32_t brushColor = painterData->drawBrush.color().rgba();

    if (painterData->drawBrush.style() == Tp::NoBrush)
    {
        if (gradientPtr)
            shape->strokeFill(gradientPtr);
        else
            shape->strokeFill(_R(penColor), _G(penColor), _B(penColor), _A(penColor));

        shape->strokeWidth(painterData->drawPen.width());

        // 设置线帽样式
        switch (painterData->drawPen.capStyle())
        {
        case Tp::ButtCap:
            shape->strokeCap(tvg::StrokeCap::Butt);
            break;
        case Tp::RoundCap:
            shape->strokeCap(tvg::StrokeCap::Round);
            break;
        case Tp::SquareCap:
            shape->strokeCap(tvg::StrokeCap::Square);
            break;
        default:
            shape->strokeCap(tvg::StrokeCap::Round);
            break;
        }

        // 设置连接样式
        switch (painterData->drawPen.joinStyle())
        {
        case Tp::MiterJoin:
            shape->strokeJoin(tvg::StrokeJoin::Miter);
            break;
        case Tp::BevelJoin:
            shape->strokeJoin(tvg::StrokeJoin::Bevel);
            break;
        case Tp::RoundJoin:
            shape->strokeJoin(tvg::StrokeJoin::Round);
            break;
        default:
            shape->strokeJoin(tvg::StrokeJoin::Miter);
            break;
        }
    }
    else if (painterData->drawBrush.style() == Tp::SolidPattern)
    {
        if (gradientPtr)
            shape->fill(gradientPtr);
        else
            shape->fill(_R(brushColor), _G(brushColor), _B(brushColor), _A(brushColor));
    }
    else
    {
        if (gradientPtr)
            shape->fill(gradientPtr);
        else
            shape->fill(_R(brushColor), _G(brushColor), _B(brushColor), _A(brushColor));
    }

    // 添加到场景并绘制
    painterData->tvgScene->push(std::move(shape));
}

void TpPainter::setClipRect(const TpRect &rect)
{
    TpPainterData *painterData = static_cast<TpPainterData *>(data_);

    if (painterData && painterData->beUsed)
    {
        painterData->TpSurfacePtr->setClipRect(rect);
    }
}

TpRect TpPainter::clipRect()
{
    TpPainterData *painterData = static_cast<TpPainterData *>(data_);
    return painterData->TpSurfacePtr->clipRect();
}

void TpPainter::erase()
{
    TpPainterData *painterData = static_cast<TpPainterData *>(data_);

    if (painterData && painterData->beUsed)
    {
        // // 方法1：清除所有 Paint 对象
        // painterData->swCanvas->remove();

        // // 方法2：清除缓冲区并重新绘制
        // painterData->swCanvas->draw(true);
        // painterData->swCanvas->sync();

        TpPainterData *painterData = static_cast<TpPainterData *>(data_);
        if (!painterData || !painterData->swCanvas || !painterData->beUsed)
            return;

        // 获取裁剪矩形（类似 SDL_GetClipRect）
        TpRect clipRect = painterData->TpSurfacePtr->clipRect();

        // 设置视口到裁剪区域
        // <cite> inc / thorvg.h : 846 - 871 < / cite >
        painterData->swCanvas->viewport(clipRect.x(), clipRect.y(), clipRect.width(), clipRect.height());

        // 清除该区域（相当于 CAIRO_OPERATOR_CLEAR）
        // <cite> inc / thorvg.h : 825 - 843 < / cite >
        painterData->swCanvas->draw(true); // true 参数会清除缓冲区

        // 恢复完整视口
        int32_t surfaceWidth = painterData->TpSurfacePtr->width();
        int32_t surfaceHeight = painterData->TpSurfacePtr->height();
        painterData->swCanvas->viewport(0, 0, surfaceWidth, surfaceHeight);

        // 同步操作
        painterData->swCanvas->sync();
    }
}

void TpPainter::addScene(void *canvas, void *scene)
{
    TpPainterData *painterData = static_cast<TpPainterData *>(data_);

    if (painterData->swCanvas)
    {
        delete painterData->swCanvas;
        painterData->swCanvas = nullptr;
    }

    tvg::SwCanvas *addCanvas = (tvg::SwCanvas *)canvas;
    painterData->swCanvas = addCanvas;

    tvg::Scene *addScene = (tvg::Scene *)scene;
    addScene->remove();
    painterData->tvgScene = addScene;
}

void TpPainter::sync()
{
    TpPainterData *painterData = static_cast<TpPainterData *>(data_);

    // 绘制并同步
    painterData->swCanvas->draw();
    painterData->swCanvas->sync();
}

TpHollowMask::TpHollowMask()
{
}

TpHollowMask::~TpHollowMask()
{
}

void TpHollowMask::addRectHollow(const TpRect &region, const uint32_t &round)
{
    addRectHollow(TpHollowMask::RectHollow(region, round));
}

void TpHollowMask::addRectHollow(const RectHollow &data)
{
    rectList_.emplace_back(data);
}

TpVector<TpHollowMask::RectHollow> TpHollowMask::rectHollowList() const
{
    return rectList_;
}

void TpHollowMask::addCircleHollow(const int32_t &x, const int32_t &y, const uint32_t &radius)
{
    addCircleHollow(TpHollowMask::CircleHollow(x, y, radius));
}

void TpHollowMask::addCircleHollow(const CircleHollow &data)
{
    circleList_.emplace_back(data);
}

TpVector<TpHollowMask::CircleHollow> TpHollowMask::circleHollowList() const
{
    return circleList_;
}

void TpHollowMask::addPieHollow(const int32_t &x, const int32_t &y, const uint32_t &radius, const int32_t &start, const int32_t &end)
{
    addPieHollow(PieHollow(x, y, radius, start, end));
}

void TpHollowMask::addPieHollow(const PieHollow &data)
{
    pieList_.emplace_back(data);
}

TpVector<TpHollowMask::PieHollow> TpHollowMask::pieHollowList() const
{
    return pieList_;
}

void TpHollowMask::addPolygonHollow(const PolygonHollow &polygon)
{
    polygonList_.emplace_back(polygon);
}

TpVector<TpHollowMask::PolygonHollow> TpHollowMask::polygonHollowList() const
{
    return polygonList_;
}
