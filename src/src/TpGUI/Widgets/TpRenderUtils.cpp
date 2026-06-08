/*
 * 版权声明 (Copyright Declaration)
 * 作者 (Author)：刘可扬
 * 邮箱 (Email)：2226544626@qq.com
 * 版权所有 (Copyright)：© 2026 刘可扬. All rights reserved.
 * 描述 (Description)：图表渲染工具类 (TpRenderUtils) - 提供核心绘图功能的静态工具类
 */



#include "TpRenderUtils.h"
#include "TpLinearGradient.h"
#include "TpPainterPath.h"
#include <TpString.h>
#include <algorithm>
#include <cmath>
#include <cstdio>
#include <stdlib.h> 

// 宏定义防止报错
#ifndef _RGB
#define _RGB(r, g, b) ((0xFF << 24) | ((r) << 16) | ((g) << 8) | (b))
#endif

namespace {

    // --- 私有结构体 ---
    struct PointF {
        float x, y;
        PointF(float _x, float _y) : x(_x), y(_y) {}
        PointF() : x(0), y(0) {}
    };

    // --- 私有常量 ---
    const int32_t INSIDE = 0; // 0000
    const int32_t LEFT   = 1; // 0001
    const int32_t RIGHT  = 2; // 0010
    const int32_t BOTTOM = 4; // 0100
    const int32_t TOP    = 8; // 1000

    // --- 私有辅助算法 ---
    
    // 计算裁剪编码
    int32_t computeOutCode(int32_t x, int32_t y, const TpRect &r) {
        int32_t code = INSIDE;
        if (x < r.x())          code |= LEFT;
        else if (x > r.right()) code |= RIGHT;
        if (y < r.y())          code |= TOP;
        else if (y > r.bottom())code |= BOTTOM;
        return code;
    }

    // 自适应字体大小计算
    int32_t getAdaptiveFontSize(const TpRect& rect, float ratio, int32_t minSize, int32_t maxSize) {
        int32_t base = (rect.width() < rect.height()) ? rect.width() : rect.height();
        int32_t size = (int)(base * ratio);
        if (size < minSize) return minSize;
        if (size > maxSize) return maxSize;
        return size;
    }

    // 颜色混合 
    uint32_t blendColor(uint32_t c1, uint32_t c2, int32_t ratio) {
        if (ratio <= 0) return c1;
        if (ratio >= 256) return c2;

        int32_t invRatio = 256 - ratio;
        int32_t a1 = (c1 >> 24) & 0xFF; int r1 = (c1 >> 16) & 0xFF; int32_t g1 = (c1 >> 8) & 0xFF; int32_t b1 = c1 & 0xFF;
        int32_t a2 = (c2 >> 24) & 0xFF; int r2 = (c2 >> 16) & 0xFF; int32_t g2 = (c2 >> 8) & 0xFF; int32_t b2 = c2 & 0xFF;

        int32_t a = (a1 * invRatio + a2 * ratio) >> 8;
        int32_t r = (r1 * invRatio + r2 * ratio) >> 8;
        int32_t g = (g1 * invRatio + g2 * ratio) >> 8;
        int32_t b = (b1 * invRatio + b2 * ratio) >> 8;

        return (a << 24) | (r << 16) | (g << 8) | b;
    }

    // 实心圆填充辅助
    void fillCircleHelper(TpPainter* painter, int32_t cx, int32_t cy, int32_t radius) {
        if (radius <= 0) return;
        
        for(int32_t y = -radius; y <= radius; y++) {
            int32_t halfWidth = (int32_t)(std::sqrt(radius * radius - y * y) + 0.5);
            if(halfWidth > 0) {
                painter->drawLine(cx - halfWidth, cy + y, cx + halfWidth, cy + y);
            }
        }
    }

} 


// ------ 通用背景与网格 ------

// 填充渐变矩形 
void TpRenderUtils::fillGradientRect(TpPainter *painter, const TpRect &rect, uint32_t colorStart, uint32_t colorEnd) {
    if (!painter) return;
    
    int32_t h = rect.height();
    int32_t w = rect.width();
    if (h <= 0 || w <= 0) return;

    if (colorStart == colorEnd) {
        TpPen pen(colorStart);
        TpBrush brush(colorStart); 
        painter->setPen(pen);
        painter->setBrush(brush);
        painter->drawRect(rect.x(), rect.y(), w, h);
        return;
    }

    const int32_t STEP = 4;
    for (int32_t y = 0; y < h; y += STEP) {
        int32_t bandH = (y + STEP <= h) ? STEP : (h - y);
        int32_t ratio = (y * 256) / h;
        uint32_t curColor = blendColor(colorStart, colorEnd, ratio);

        TpPen pen(curColor);
        TpBrush brush(curColor);
        painter->setPen(pen);
        painter->setBrush(brush);
        painter->drawRect(rect.x(), rect.y() + y, rect.width(), bandH);
    }
}

// 绘制网格
void TpRenderUtils::drawGrid(TpPainter *painter, const TpRect &rect, int32_t rows, int32_t cols, uint32_t gridColor) {
    if (!painter) return;

    TpPen pen(gridColor); 
    painter->setPen(pen);

    int32_t x = rect.x();
    int32_t y = rect.y();
    int32_t w = rect.width();
    int32_t h = rect.height();

    if (rows > 0) {
        for (int32_t i = 0; i <= rows; ++i) {
            int32_t  yPos = y + (h * i) / rows;
            painter->drawLine(x, yPos, x + w, yPos);
        }
    }

    if (cols > 0) {
        for (int32_t j = 0; j <= cols; ++j) {
            int32_t xPos = x + (w * j) / cols;
            painter->drawLine(xPos, y, xPos, y + h);
        }
    }
}

//  ------ 折线图专用 ------

// 裁剪线段绘制 
void TpRenderUtils::drawClippedLine(TpPainter *painter, int32_t x0, int32_t y0, int32_t x1, int32_t y1, const TpRect &rect) {
    if (!painter) return;

    int32_t outcode0 = computeOutCode(x0, y0, rect); 
    int32_t outcode1 = computeOutCode(x1, y1, rect);
    bool accept = false;

    while (true) {
        if (!(outcode0 | outcode1)) {
            accept = true;
            break;
        } else if (outcode0 & outcode1) {
            break;
        } else {
            int32_t outcodeOut = outcode0 ? outcode0 : outcode1;
            int32_t x, y;

            if (outcodeOut & BOTTOM) {
                if (y1 == y0) break; 
                x = x0 + (x1 - x0) * (rect.bottom() - y0) / (y1 - y0);
                y = rect.bottom();
            } else if (outcodeOut & TOP) {
                if (y1 == y0) break;
                x = x0 + (x1 - x0) * (rect.y() - y0) / (y1 - y0);
                y = rect.y();
            } else if (outcodeOut & RIGHT) {
                if (x1 == x0) break;
                y = y0 + (y1 - y0) * (rect.right() - x0) / (x1 - x0);
                x = rect.right();
            } else if (outcodeOut & LEFT) { 
                if (x1 == x0) break;
                y = y0 + (y1 - y0) * (rect.x() - x0) / (x1 - x0);
                x = rect.x();
            }

            if (outcodeOut == outcode0) {
                x0 = x; y0 = y;
                outcode0 = computeOutCode(x0, y0, rect);
            } else {
                x1 = x; y1 = y;
                outcode1 = computeOutCode(x1, y1, rect);
            }
        }
    }

    if (accept) {
        painter->drawLine(x0, y0, x1, y1);
    }
}

// 标准折线 
void TpRenderUtils::drawPolyline(TpPainter* painter, const TpVector<TpPoint>& points, const TpRect& clipRect, int32_t color, int32_t lineWidth) {
    if (points.size() < 2) return;

    TpPen pen(color, lineWidth);
    painter->setPen(pen);

    // 缓存边界值
    int32_t left = clipRect.x(), right = clipRect.right();
    int32_t top = clipRect.y(), bottom = clipRect.bottom();

    // 检查是否所有点都在裁剪区内
    bool allInside = true;
    for (size_t i = 0; i < points.size(); ++i) {
        const TpPoint& p = points[i];
        if (p.x() < left || p.x() > right || p.y() < top || p.y() > bottom) {
            allInside = false;
            break;
        }
    }

    if (allInside) {
        // 批量路径绘制（高性能）
        TpPainterPath path;
        path.moveTo(points[0]);
        for (size_t i = 1; i < points.size(); ++i) {
            path.lineTo(points[i]);
        }
        painter->drawPath(path);
    } else {
        // 逐段裁剪绘制（保证正确性，仅在有点超出时使用）
        for (size_t i = 0; i < points.size() - 1; ++i) {
            const TpPoint& p1 = points[i];
            const TpPoint& p2 = points[i+1];
            drawClippedLine(painter, p1.x(), p1.y(), p2.x(), p2.y(), clipRect);
        }
    }
}

// 平滑曲线（使用 Catmull-Rom 样条算法）
void TpRenderUtils::drawSmoothCurve(TpPainter* painter, const TpVector<TpPoint>& points, const TpRect& clipRect, int32_t color, int32_t lineWidth, float tension)
{
    if (points.size() < 2) return;

    TpPen pen(color, lineWidth);
    painter->setPen(pen);

    int32_t count = points.size();

    if (count == 2) {
        painter->drawLine(points[0], points[1]);
        return;
    }

    float scale = tension * 0.5f;

    for (int32_t i = 0; i < count - 1; ++i)
    {
        TpPoint p0 = (i == 0) ? points[i]     : points[i - 1];
        TpPoint p1 = points[i];
        TpPoint p2 = points[i + 1];
        TpPoint p3 = (i + 2 < count) ? points[i + 2] : p2;

        // Catmull-Rom → Bezier
        float cp1x = p1.x() + (p2.x() - p0.x()) * scale;
        float cp1y = p1.y() + (p2.y() - p0.y()) * scale;

        float cp2x = p2.x() - (p3.x() - p1.x()) * scale;
        float cp2y = p2.y() - (p3.y() - p1.y()) * scale;

        // ⭐ 直接使用 ThorVG cubic
        painter->drawCubic(
            p1.x(), p1.y(),
            (int32_t)(cp1x + 0.5f),
            (int32_t)(cp1y + 0.5f),
            (int32_t)(cp2x + 0.5f),
            (int32_t)(cp2y + 0.5f),
            p2.x(), p2.y()
        );
    }
}

// ------ 柱状图专用 ------

// 绘制渐变柱状图
void TpRenderUtils::drawGradientBar(TpPainter *painter, const TpRect &barRect, uint32_t colorStart, uint32_t colorEnd) {
    if (!painter) return;

    if (colorStart == colorEnd) {
        TpPen pen(colorEnd);
        TpBrush brush(colorStart);
        painter->setPen(pen);
        painter->setBrush(brush);
        painter->drawRect(barRect.x(), barRect.y(), barRect.width(), barRect.height());
    } else {
        TpLinearGradient gradient;
        gradient.setStart(static_cast<float>(barRect.x()), static_cast<float>(barRect.y()));
        gradient.setFinalStop(static_cast<float>(barRect.x()), static_cast<float>(barRect.bottom()));
        gradient.setColorAt(0.0f, static_cast<int32_t>(colorStart));
        gradient.setColorAt(1.0f, static_cast<int32_t>(colorEnd));

        TpPen pen(colorEnd);
        TpBrush brush(&gradient);
        painter->setPen(pen);
        painter->setBrush(brush);
        painter->drawRect(barRect.x(), barRect.y(), barRect.width(), barRect.height());
    }

    TpPen borderPen(colorEnd); 
    painter->setPen(borderPen);

    int32_t left = barRect.x();
    int32_t top = barRect.y();
    int32_t right = barRect.right();
    int32_t bottom = barRect.bottom();

    painter->drawLine(left, top, right, top);
    painter->drawLine(left, bottom, right, bottom);
    painter->drawLine(left, top, left, bottom);
    painter->drawLine(right, top, right, bottom);
}

// ------ UI 组件原语 ------

// 绘制图例 
void TpRenderUtils::drawLegendOutside(TpPainter* painter, const TpRect& totalRect, const TpRect& chartRect, 
                                      const TpVector<const char*>& names, 
                                      const TpVector<int32_t>& colors,
                                      const TpVector<int32_t>& endColors, 
                                      const TpVector<int32_t>& types) 
{
    int32_t count = names.size();
    if (count == 0) return;

    // 计算与主图表完全一致的缩放因子
    double scaleX = totalRect.width() / 800.0;
    double scaleY = totalRect.height() / 600.0;
    double scale = scaleX < scaleY ? scaleX : scaleY; 
    if (scale < 0.5) scale = 0.5;
    if (scale > 2.5) scale = 2.5;

    // 将硬编码转换为缩放动态值
    // 字体大小
    int32_t legendSize = (int32_t)(12 * scale); 
    if (legendSize < 9) legendSize = 9; // 嵌入式保底字号，防止糊成一团

    // 布局间距与尺寸
    int32_t startY = totalRect.y() + (int32_t)(45 * scale); // 顶部起始Y坐标
    int32_t itemSpacing = (int32_t)(20 * scale);            // 图例项之间的间距
    int32_t iconW = (int32_t)(16 * scale);                  // 图例图标宽度
    int32_t iconH = (int32_t)(12 * scale);                  // 图例图标高度
    int32_t textOffset = iconW + (int32_t)(4 * scale);      // 文字离图标的距离
    int32_t totalItemBaseW = textOffset + (int32_t)(4 * scale); // 每个 Item 固定的基础占宽

    TpFont font;
    font.setFontSize(legendSize);
    font.setFontColor(_RGB(0,0,0)); 

    // 计算所有图例加起来的总宽度 (用于居中对齐)
    int32_t totalContentWidth = 0;
    for (int32_t i = 0; i < count; ++i) {
        TpString tmpName(names[i]);
        font.setText(tmpName);
        totalContentWidth += (totalItemBaseW + font.pixelWidth());
        if (i < count - 1) totalContentWidth += itemSpacing;
    }

    // 起始绘制点 X 坐标 (水平居中)
    int32_t currentX = totalRect.x() + (totalRect.width() - totalContentWidth) / 2;

    // 遍历绘制
    for (int32_t i = 0; i < count; ++i) {
        int32_t y = startY;
        
        TpString tpName(names[i]);
        font.setText(tpName);
        int32_t textW = font.pixelWidth();
        int32_t textH = font.pixelHeight();
        if (textH <= 0) textH = legendSize; // 防止底层实现没拿到高度时的容错处理
        
        int32_t textCenterY = y + textH / 2;
        int32_t iconTopY = y + (textH - iconH) / 2 + 2; 

        int32_t type = (i < types.size()) ? types[i] : 0;
        int32_t colorStart = (i < colors.size()) ? colors[i] : 0;
        int32_t colorEnd = (i < endColors.size()) ? endColors[i] : colorStart;

        // 绘制前面的小图标
        if (type == TypeBar) { 
            TpRect iconRect(currentX, iconTopY, iconW, iconH);
            TpRenderUtils::fillGradientRect(painter, iconRect, colorStart, colorEnd);
        } else if (type == TypeScatter) {
            int32_t pointR = (int32_t)(3 * scale);
            if (pointR < 1) pointR = 1;
            TpPoint center(currentX + (iconW / 2), textCenterY);
            TpRenderUtils::drawAnchorPoint(painter, center, pointR, colorStart, colorStart);
        } else if (type == TypePie) {
            TpPoint center(currentX + (iconW / 2), textCenterY);
            int32_t pieR = iconH / 2;
            if (pieR < 2) pieR = 2;
            painter->setPen(TpPen(colorStart, 1));
            painter->setBrush(TpBrush(colorStart));
            painter->drawPie(center, pieR, 270, 330);
        } else {
            int32_t lineY = textCenterY; 
            TpPen linePen(colorStart, 2);
            painter->setPen(linePen);
            painter->drawLine(currentX, lineY, currentX + iconW, lineY);

            // 动态计算折线图例中心小方块的大小
            int32_t pointR = (int32_t)(3 * scale);
            if (pointR < 1) pointR = 1;
            TpRect pointRect(currentX + (iconW / 2) - pointR, lineY - pointR, pointR * 2, pointR * 2);
            TpRenderUtils::fillGradientRect(painter, pointRect, colorStart, colorStart);
        }

        // 绘制文字
        painter->drawText(font, currentX + textOffset, y);

        // 游标向右推移
        currentX += (totalItemBaseW + textW + itemSpacing);
    }
}

// 数据锚点
void TpRenderUtils::drawAnchorPoint(TpPainter* painter, const TpPoint& center, int32_t radius, int32_t borderColor, int32_t fillColor) {
    if (!painter) return;

    TpPen fillPen(fillColor, 1);
    painter->setPen(fillPen);
    
    fillCircleHelper(painter, center.x(), center.y(), radius - 1); // 调用匿名空间函数

    TpPen pen(borderColor, 2); 
    painter->setPen(pen);

    float step = 15.0f; 
    float lastX = center.x() + radius;
    float lastY = center.y();

    for (float angle = step; angle <= 360; angle += step) {
        float rad = angle * 3.1415926f / 180.0f;
        float currX = center.x() + radius * std::cos(rad);
        float currY = center.y() + radius * std::sin(rad);

        painter->drawLine((int32_t)lastX, (int32_t)lastY, (int32_t)currX, (int32_t)currY);
        lastX = currX;
        lastY = currY;
    }
}

// X 轴绘制 
void TpRenderUtils::drawAxisX(TpPainter* painter, const TpRect& rect, const TpAxis& axisX, const TpAxis& axisY, unsigned int color, bool drawGrid, unsigned int gridColor) {
    if (!painter) return;

    TpPen axisPen(color, 2);
    TpPen gridPen(gridColor, 1);
    
    TpFont font;
    int32_t fontSize = getAdaptiveFontSize(rect, 0.025f, 9, 20);
    font.setFontSize(fontSize);   
    font.setFontColor(color); 
    
    int32_t axisYPos = rect.bottom(); 
    
    if (axisY.min() < 0 && axisY.max() > 0) {
        axisYPos = axisY.mapToPixel(0, rect.height(), rect.y(), true); 
    } 
    else if (axisY.max() <= 0) {
        axisYPos = rect.y();
    }

    painter->setPen(axisPen);
    painter->drawLine(rect.x(), axisYPos, rect.right(), axisYPos);

    const TpVector<double>& ticks = axisX.getTickValues();
    int32_t count = ticks.size();

    for (int32_t i = 0; i < count; ++i) {
        double val = ticks[i];
        int32_t x = axisX.mapToPixel(val, rect.width(), rect.x(), false);

        if (x < rect.x() || x > rect.right()) continue;

        painter->setPen(axisPen);
        painter->drawLine(x, axisYPos, x, axisYPos + 5);

        if (drawGrid) {
            painter->setPen(gridPen);
            painter->drawLine(x, rect.bottom(), x, rect.y());
        }

        TpString tpText;
        if (std::abs(val - (int)val) < 1e-9) {
            tpText = TpString::number((int)val);
        } else {
            tpText = TpString::number(val, 1);
        }

        font.setText(tpText);
        int32_t textW = font.pixelWidth();
        
        int32_t drawX = x - (textW / 2);
        int32_t drawY = axisYPos + 8; 

        painter->drawText(font, drawX, drawY);
    }
}

// Y 轴绘制
void TpRenderUtils::drawAxisY(TpPainter* painter, const TpRect& rect, const TpAxis& axisY, const TpAxis& axisX, uint32_t color, bool drawGrid, uint32_t gridColor) {
    if (!painter) return;

    TpPen axisPen(color, 2);
    TpPen gridPen(gridColor, 1);
    
    TpFont font;
    int32_t fontSize = getAdaptiveFontSize(rect, 0.025f, 9, 20);
    font.setFontSize(fontSize);
    font.setFontColor(color);

    int32_t axisXPos = rect.x(); 

    if (axisX.min() < 0 && axisX.max() > 0) {
        axisXPos = axisX.mapToPixel(0, rect.width(), rect.x(), false);
    } 
    else if (axisX.max() <= 0) {
        axisXPos = rect.right(); 
    }

    painter->setPen(axisPen);
    painter->drawLine(axisXPos, rect.y(), axisXPos, rect.bottom());

    const TpVector<double>& ticks = axisY.getTickValues();
    int32_t count = ticks.size();

    for (int32_t i = 0; i < count; ++i) {
        double val = ticks[i];
        int32_t y = axisY.mapToPixel(val, rect.height(), rect.y(), true); 
        if (y < rect.y() || y > rect.bottom()) continue;

        painter->setPen(axisPen);
        painter->drawLine(axisXPos, y, axisXPos - 5, y);

        if (drawGrid) {
            painter->setPen(gridPen);
            painter->drawLine(rect.x(), y, rect.right(), y);
        }

        TpString tpText;

        if (std::abs(val - (int)val) < 1e-9) {
            tpText = TpString::number((int)val);
        } else {
            tpText = TpString::number(val, 1);
        }

        font.setText(tpText); 
        int32_t textW = font.pixelWidth();
        int32_t textH = font.pixelHeight();

        int32_t drawX = axisXPos - 5 - 3 - textW;
        int32_t drawY = y - (textH / 2);

        painter->drawText(font, drawX, drawY);
    }
}
