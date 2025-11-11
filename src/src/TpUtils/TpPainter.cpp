#include "TpPainter.h"
#include "TpFont.h"
#include "TpDef.h"
#include "TpImage_p.h"
#include "TpGradient_p.h"
#include "TpPainter_p.h"
#include "TpPainterPath_p.h"
#include "TpObject_p.h"
#include "TpWidget_p.h"

#include <thread>
#include <cmath>

TpPainter::TpPainter(tpShared<TpSurface> surface, int32_t offsetX, int32_t offsetY, TpWidget* object)
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

    painterData->paintWidget = object;

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
#if 1

    auto fontScene = tvg::Scene::gen();
    // 加载字体文件
    // tvg::Text::load("/home/hawk/Public/TinyPiXOS/src/data/fonts/SourceHanSansCN/SourceHanSansCN-Normal.otf");
    tvg::Text::load("/root/examplesApp/data/SourceHanSerifCN-Regular.ttf");
    tvg::Text::load("/root/examplesApp/data/SourceHanSerifCN-Bold.ttf");

    // ===== 普通文本 =====  
    auto normalText = tvg::Text::gen();  
    normalText->font("SourceHanSerifCN-Bold");  
    normalText->size(12);  
    normalText->text("普通文本");  
    normalText->fill(255, 200, 0);  
    normalText->translate(0, 0);  
    fontScene->push(normalText);  
  
    // ===== 斜体文本 =====  
    auto italicText = tvg::Text::gen();  
    italicText->font("SourceHanSerifCN-Regular");  
    italicText->size(12);  
    italicText->text("斜体文本");  
    italicText->fill(255, 200, 0);  
    italicText->italic(0.18f);  // 应用斜体效果  
    italicText->translate(0, 30);  
    fontScene->push(italicText);  
  
    // ===== 带轮廓的文本 =====  
    auto outlineText = tvg::Text::gen();  
    outlineText->font("SourceHanSerifCN-Regular");  
    outlineText->size(18);  
    outlineText->text("轮廓文本");  
    outlineText->fill(255, 255, 0);      // 黄色填充  
    outlineText->outline(2, 255, 100, 0);  // 蓝色轮廓,宽度1  
    outlineText->translate(0, 60);  
    fontScene->push(outlineText);  
  
    // ===== 斜体 + 轮廓组合 =====  
    auto combinedText = tvg::Text::gen();  
    combinedText->font("SourceHanSerifCN-Regular");  
    combinedText->size(12);  
    combinedText->text("斜体+轮廓");  
    combinedText->fill(255, 100, 100);  
    combinedText->italic(0.2f);           // 斜体  
    combinedText->outline(2, 255, 255, 255);  // 白色轮廓  
    combinedText->translate(0, 90);  
    fontScene->push(combinedText);  

    // 添加到 Canvas 并绘制
    // painterData->tvgScene->push(std::move(text));

        // ===== 横向布局示例 =====  
    auto horizontalText = tvg::Text::gen();  
    horizontalText->font("SourceHanSerifCN-Regular");  
    horizontalText->size(12);  
    horizontalText->text("这是一段横向排列的中文文本示例");  
    horizontalText->fill(255, 255, 255);  // 白色文字  
      
    // 设置横向布局约束和对齐方式  
    horizontalText->layout(400, 0);  // 宽度限制为400,高度不限制  
    horizontalText->align(0.0f, 0.0f);  // 左对齐,顶部对齐  
    horizontalText->translate(0, 120);  // 位置  

    fontScene->push(horizontalText);

    // ===== 横向居中布局示例 =====  
    auto horizontalCenterText = tvg::Text::gen();  
    horizontalCenterText->font("SourceHanSerifCN-Regular");  
    horizontalCenterText->size(12);  
    horizontalCenterText->text("这是居中对齐的横向文本");  
    horizontalCenterText->fill(255, 200, 0);  // 橙色文字  
      
    horizontalCenterText->layout(400, 0);  
    horizontalCenterText->align(0.5f, 0.0f);  // 水平居中,顶部对齐  
    horizontalCenterText->translate(0, 150);  

    fontScene->push(horizontalCenterText);

    // ===== 横向右对齐布局示例 =====  
    auto horizontalRightText = tvg::Text::gen();  
    horizontalRightText->font("SourceHanSerifCN-Regular");  
    horizontalRightText->size(12);  
    horizontalRightText->text("这是右对齐的横向文本");  
    horizontalRightText->fill(0, 255, 255);  // 青色文字  
      
    horizontalRightText->layout(400, 0);  
    horizontalRightText->align(1.0f, 0.0f);  // 右对齐,顶部对齐  
    horizontalRightText->translate(0, 180);  

    fontScene->push(horizontalRightText);

    // ===== 竖向布局示例(通过旋转实现) =====  
    auto verticalText = tvg::Text::gen();  
    verticalText->font("SourceHanSerifCN-Regular");  
    verticalText->size(12);  
    verticalText->text("竖向文本");  
    verticalText->fill(255, 100, 0);  // 红色文字  
      
    // 竖向布局:先设置横向布局,然后旋转90度  
    verticalText->layout(100, 0);  // 高度限制为200  
    verticalText->align(0.0f, 0.0f);  
    
    verticalText->translate(20, 210);  
      // 旋转90度 (π/2 弧度)  

    verticalText->rotate(90.0f);

    fontScene->push(verticalText);

    // ===== 带换行的横向布局示例 =====
    auto wrappedText = tvg::Text::gen();
    wrappedText->font("SourceHanSerifCN-Regular");
    wrappedText->size(12);
    wrappedText->text("这是一段很长的文本,需要自动换行显示。ThorVG支持多种换行模式,包括字符换行和单词换行。");
    wrappedText->fill(150, 255, 150);  // 绿色文字

    wrappedText->layout(100, 0);  // 宽度限制为100
    wrappedText->align(0.0f, 0.0f);
    wrappedText->wrap(tvg::TextWrap::Smart);  // 按单词换行
    wrappedText->translate(0, 300);

    fontScene->push(wrappedText);

    // 8. 获取文本边界框信息  
    float x, y, w, h;  
    if (verticalText->bounds(&x, &y, &w, &h) == tvg::Result::Success) {  
        std::cout << "verticalText Text bounds:" << std::endl;  
        std::cout << "  Position: (" << x << ", " << y << ")" << std::endl;  
        std::cout << "  Size: " << w << " x " << h << std::endl;  
    }  

    // painterData->tvgScene->push(fontScene);
    
#endif

    // auto tmpScene = tvg::Scene::gen();

    auto tmpScene = static_cast<tvg::Scene *>(fontScene->duplicate());

    auto clipShape = tvg::Shape::gen();
    clipShape->appendRect(0, 300, 300, 300);  // 玻璃面板位置
    clipShape->fill(255, 255, 255, 128);  // 填充透明色
    fontScene->clip(clipShape);
    // fontScene->push(tvg::SceneEffect::GaussianBlur, 30.0, 0, 0, 80);

    auto scene = tvg::Scene::gen();

    // 创建背景矩形
    auto background = tvg::Shape::gen();
    background->appendRect(0, 300, 300, 300);
    background->fill(255, 0, 0, 200);  // 红色,半透明 (alpha=128)
    
    // 先添加背景,再添加其他内容  
    scene->push(background);  
    scene->push(fontScene);
    scene->push(tvg::SceneEffect::GaussianBlur, 30.0, 0, 0, 80);

    painterData->tvgScene->push(tmpScene);

    painterData->tvgScene->push(scene);
    
    
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

    // 第一次渲染周期 - 触发Picture加载
    // painterData->swCanvas->push(std::move(pictureCopy));
    // painterData->swCanvas->update();
    // painterData->swCanvas->draw();
    // painterData->swCanvas->sync();
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
        painterData->clipRect = rect;
        painterData->TpSurfacePtr->setClipRect(rect);
    }
}

TpRect TpPainter::clipRect()
{
    TpPainterData *painterData = static_cast<TpPainterData *>(data_);
    return painterData->clipRect;
}

void TpPainter::erase()
{
    TpPainterData *painterData = static_cast<TpPainterData *>(data_);

    if (painterData && painterData->beUsed)
    {
        refreshCanvasTarget(painterData);

        // 清除并绘制
        painterData->swCanvas->draw(true); // true参数会清除目标缓冲区
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

void TpPainter::sync(void *object)
{
    TpPainterData *painterData = static_cast<TpPainterData *>(data_);

    // 绘制并同步
    // painterData->swCanvas->update();
    painterData->swCanvas->draw();
    painterData->swCanvas->sync();

    if (object)
    {
        TpWidget *paintWidget = static_cast<TpWidget *>(object);
        TpWidgetData *paintWidgetData = static_cast<TpWidgetData *>(paintWidget->objectSets());

        paintWidgetData->grapImage.load(painterData->TpSurfacePtr->matrix(), TpSize(painterData->clipRect.width(), painterData->clipRect.height()));

        // static int32_t saveIndexS = 0;
        // TpString savePngPath = "/home/hawk/Public/TinyPiXOS/examples/TpGUI/test/grapWindow_" + std::to_string(saveIndexS++) + ".png";

        // // 加载原始像素数据
        // TpImage grapImage;
        // grapImage.load(painterData->TpSurfacePtr->matrix(), TpRect(painterData->clipRect.x(), painterData->clipRect.y(), painterData->clipRect.width(), painterData->clipRect.height()));
        // grapImage.save(savePngPath, TpImage::PNG_FMT);
    }
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
