/*
 * 闁绘鐗婂鍫熺珶閻楀牊顫?(Copyright Declaration)
 * 濞达絾绮忛埀?(Author)闁挎稒鑹鹃崹顒勫矗椤栨稑顥?
 * 闂侇収鍠氶?(Email)闁?226544626@qq.com
 * 闁绘鐗婂鍫ュ箥閳ь剟寮?(Copyright)闁挎稒顕?2026 闁告帗锚瑜版煡骞? All rights reserved.
 * 闁硅绻楅崼?(Description)闁挎稒鑹惧ù妯兼偘閵婏箒顩柡灞炬尭娴兼劙宕楁搴ゎ潶 (TpRenderUtils) - 闁圭粯鍔掔欢鐢稿冀缁嬭法濡囩紓浣姑ù姗€宕濋悢璇插幋闁汇劌瀚板銈夊箑娴ｉ晲绱ｉ柛蹇涱棑鐞?
 */

#include "TpRenderUtils.h"
#include "TpLinearGradient.h"
#include "TpPainterPath.h"
#include <TpString.h>
#include <algorithm>
#include <cmath>
#include <cstdio>
#include <stdlib.h>

// Macro guard for RGB helper
#ifndef _RGB
#define _RGB(r, g, b) ((0xFF << 24) | ((r) << 16) | ((g) << 8) | (b))
#endif

namespace {

    // --- 缂佸鐒﹀﹢浣虹磼閹惧鈧垱鎷?---
    struct PointF {
        float x, y;
        PointF(float _x, float _y) : x(_x), y(_y) {}
        PointF() : x(0), y(0) {}
    };

    // --- 缂佸鐒﹀﹢浣烘暜閹间礁娅?---
    const int32_t INSIDE = 0; // 0000
    const int32_t LEFT   = 1; // 0001
    const int32_t RIGHT  = 2; // 0010
    const int32_t BOTTOM = 4; // 0100
    const int32_t TOP    = 8; // 1000

    // --- 缂佸鐒﹀﹢浣规綇閸涱厼袠缂佺姵顨嗙涵?---

    // 閻犱緤绱曢悾鑽ゆ啑娴ｇ顥呯紓鍌涚墱閻?
    int32_t computeOutCode(int32_t x, int32_t y, const TpRect &r) {
        int32_t code = INSIDE;
        if (x < r.x())          code |= LEFT;
        else if (x > r.right()) code |= RIGHT;
        if (y < r.y())          code |= TOP;
        else if (y > r.bottom())code |= BOTTOM;
        return code;
    }

    // 闁煎浜埀顒€鍊哥花鑼偓娑欍仦缂嶅寰勮閻剛鎷嬮敍鍕毈
    int32_t getAdaptiveFontSize(const TpRect& rect, float ratio, int32_t minSize, int32_t maxSize) {
        int32_t base = (rect.width() < rect.height()) ? rect.width() : rect.height();
        int32_t size = (int)(base * ratio);
        if (size < minSize) return minSize;
        if (size > maxSize) return maxSize;
        return size;
    }

    // 濡増绮忔竟濠偳庡畡鐗堝€?
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

    // Fill helper for solid circles
    void fillCircleHelper(TpPainter* painter, int32_t cx, int32_t cy, int32_t radius) {
        if (radius <= 0) return;

        for(int32_t y = -radius; y <= radius; y++) {
            int32_t halfWidth = (int32_t)(std::sqrt(radius * radius - y * y) + 0.5);
            if(halfWidth > 0) {
                painter->drawLine(cx - halfWidth, cy + y, cx + halfWidth, cy + y);
            }
        }
    }

    static PointF evaluateCubicBezier(const PointF& p0, const PointF& p1, const PointF& p2, const PointF& p3, float t)
    {
        float u = 1.0f - t;
        float uu = u * u;
        float tt = t * t;
        float uuu = uu * u;
        float ttt = tt * t;

        PointF result;
        result.x = uuu * p0.x + 3.0f * uu * t * p1.x + 3.0f * u * tt * p2.x + ttt * p3.x;
        result.y = uuu * p0.y + 3.0f * uu * t * p1.y + 3.0f * u * tt * p2.y + ttt * p3.y;
        return result;
    }

}


// ------ 闂侇偅姘ㄩ弫銈夋嚄鐏炵偓鐝☉鎾虫捣缂嶅寮?------

// 濠靛鍋勯崢鏍с€掗幇顒€缍侀柣顓涙櫅閼?
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

// 缂備焦锚閸╂绱旈幋鐐靛
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

//  ------ 闁硅埖顭囬崵搴ㄥ炊閸欍儳鐟╅柣?------

// 閻熶椒绀佹竟鈧紒鐐椤斿瞼绱掑Ο鍝勭厬
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

// 闁哄秴娲ら崳顖炲箮濡厧娈?
void TpRenderUtils::drawPolyline(TpPainter* painter, const TpVector<TpPoint>& points, const TpRect& clipRect, int32_t color, int32_t lineWidth) {
    if (points.size() < 2) return;

    TpPen pen(color, lineWidth);
    painter->setPen(pen);

    // Cache clip bounds
    int32_t left = clipRect.x(), right = clipRect.right();
    int32_t top = clipRect.y(), bottom = clipRect.bottom();

    // 婵☆偀鍋撻柡灞诲劜濡叉悂宕ラ敂钘夘暡闁哄牆顦遍崑锝夋焾閽樺韬悷浣风婢光偓闁告牕鎼崬?
    bool allInside = true;
    for (size_t i = 0; i < points.size(); ++i) {
        const TpPoint& p = points[i];
        if (p.x() < left || p.x() > right || p.y() < top || p.y() > bottom) {
            allInside = false;
            break;
        }
    }

    if (allInside) {
        // Batch path drawing for performance
        TpPainterPath path;
        path.moveTo(points[0]);
        for (size_t i = 1; i < points.size(); ++i) {
            path.lineTo(points[i]);
        }
        painter->drawPath(path);
    } else {
        // 闂侇偅鍔栭宀€鎲楁担绋款梾缂備焦锚閸╂鏁嶉崼婊呯閻犲洣鐒﹂婊呮兜椤旇В鍋撹缁辨繃绂掗崨顓熻含闁哄牆顦遍崑锝囨惥閸涱厼姣夐柡鍐╂构婵炲洭鎮介…鎺旂
        for (size_t i = 0; i < points.size() - 1; ++i) {
            const TpPoint& p1 = points[i];
            const TpPoint& p2 = points[i+1];
            drawClippedLine(painter, p1.x(), p1.y(), p2.x(), p2.y(), clipRect);
        }
    }
}

// Smooth curve (Catmull-Rom approximation)
void TpRenderUtils::drawSmoothCurve(TpPainter* painter, const TpVector<TpPoint>& points, const TpRect& clipRect, int32_t color, int32_t lineWidth, float tension)
{
    if (points.size() < 2) return;

    TpPen pen(color, lineWidth);
    painter->setPen(pen);

    int32_t count = points.size();

    if (count == 2) {
        TpRenderUtils::drawClippedLine(painter, points[0].x(), points[0].y(), points[1].x(), points[1].y(), clipRect);
        return;
    }

    float scale = tension * 0.5f;
    const int32_t segmentsPerCurve = 12;

    for (int32_t i = 0; i < count - 1; ++i)
    {
        PointF p0 = (i == 0) ? PointF(points[i].x(), points[i].y()) : PointF(points[i - 1].x(), points[i - 1].y());
        PointF p1 = PointF(points[i].x(), points[i].y());
        PointF p2 = PointF(points[i + 1].x(), points[i + 1].y());
        PointF p3 = (i + 2 < count) ? PointF(points[i + 2].x(), points[i + 2].y()) : p2;

        PointF cp1;
        cp1.x = p1.x + (p2.x - p0.x) * scale;
        cp1.y = p1.y + (p2.y - p0.y) * scale;

        PointF cp2;
        cp2.x = p2.x - (p3.x - p1.x) * scale;
        cp2.y = p2.y - (p3.y - p1.y) * scale;

        PointF prev = p1;
        for (int32_t step = 1; step <= segmentsPerCurve; ++step)
        {
            float t = static_cast<float>(step) / static_cast<float>(segmentsPerCurve);
            PointF cur = evaluateCubicBezier(p1, cp1, cp2, p2, t);
            TpRenderUtils::drawClippedLine(painter,
                                           static_cast<int32_t>(prev.x + 0.5f),
                                           static_cast<int32_t>(prev.y + 0.5f),
                                           static_cast<int32_t>(cur.x + 0.5f),
                                           static_cast<int32_t>(cur.y + 0.5f),
                                           clipRect);
            prev = cur;
        }
    }
}

// ------ 闁哄矁浜慨鎼佸炊閸欍儳鐟╅柣?------

// Gradient bar fill
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

// ------ UI 缂備礁瀚▎銏ゅ储閻旀椿鍤?------

// 缂備焦锚閸╂宕堕崣銉т紣
void TpRenderUtils::drawLegendOutside(TpPainter* painter, const TpRect& totalRect, const TpRect& chartRect,
                                      const TpVector<const char*>& names,
                                      const TpVector<int32_t>& colors,
                                      const TpVector<int32_t>& endColors,
                                      const TpVector<int32_t>& types)
{
    int32_t count = names.size();
    if (count == 0) return;

    // 閻犱緤绱曢悾缁樼▔鎼存繂鐦滈柛銉﹀礃閵嗗啰鈧懓鑻崣蹇旂▔閳ь剟鎳涚€靛憡鐣辩紓鍌楁櫆閺備線宕堕悩鑼憤
    double scaleX = totalRect.width() / 800.0;
    double scaleY = totalRect.height() / 600.0;
    double scale = scaleX < scaleY ? scaleX : scaleY;
    if (scale < 0.5) scale = 0.5;
    if (scale > 2.5) scale = 2.5;

    // 閻忓繐妫涢垾鏍磽閺嶎偆鍨抽弶鐑嗗墯瀹曞弶绋夐搹鍦礆闁衡偓閹冃楅柟顑跨閳?    // 閻庢稒銇炵紞瀣緞瑜嶉惃?
    int32_t legendSize = (int32_t)(12 * scale);
    if (legendSize < 9) legendSize = 9; // 鐎规挸鑻崣鍡楊嚕韫囧海绠介幖瀛樻礀閻⊙囧矗閸戙倗绀夐梻鍐ㄥ级椤掓稓鍖栨繝鍐ㄧ亣濞戞挴鍋撻柛?
    // Layout spacing and sizes
    int32_t startY = totalRect.y() + (int32_t)(45 * scale);
    int32_t itemSpacing = (int32_t)(20 * scale);            // 闁搞儱褰炵欢銉︺亜闁稓顓洪梻鍌氼嚟濞堟垿姊荤壕瀣崺
    int32_t iconW = (int32_t)(16 * scale);                  // 闁搞儱褰炵欢銉╁炊閻愵剛鍨奸悗纭呮鐎?
    int32_t iconH = (int32_t)(12 * scale);                  // 闁搞儱褰炵欢銉╁炊閻愵剛鍨煎Δ鍌浢€?
    int32_t textOffset = iconW + (int32_t)(4 * scale);      // 闁哄倸娲ら悺褏绮嬬拠鍙夌闁哄秴娲ㄥ▓鎴犳崉濠靛牜鐎?
    int32_t totalItemBaseW = textOffset + (int32_t)(4 * scale); // 婵絽绻嬮柌?Item 闁搞儱鎼悾楣冩儍閸曨偆鍞ㄧ痪顓涘亾闁告濮撮?

    TpFont font;
    font.setFontSize(legendSize);
    font.setFontColor(_RGB(0,0,0));

    // 閻犱緤绱曢悾濠氬箥閳ь剟寮垫径濠冪濞撴艾顑呮慨鐐垫導闁垮闄嶉柣銊ュ閳ь剝顕ч鏃€鎯?(闁活潿鍔嬬花顒備沪閸涱剝鍘悗闈涚秺缂?
    int32_t totalContentWidth = 0;
    for (int32_t i = 0; i < count; ++i) {
        TpString tmpName(names[i]);
        font.setText(tmpName);
        totalContentWidth += (totalItemBaseW + font.pixelWidth());
        if (i < count - 1) totalContentWidth += itemSpacing;
    }

    // 閻犙冨槻椤劗绱掑Ο鍝勭厬闁?X 闁秆勫姈閻?(婵ɑ娼欓柦鈺冧沪閸涱剝鍘?
    int32_t currentX = totalRect.x() + (totalRect.width() - totalContentWidth) / 2;

    // 闂侇剙绉村鑽ょ磼濡搫鐓?
    for (int32_t i = 0; i < count; ++i) {
        int32_t y = startY;

        TpString tpName(names[i]);
        font.setText(tpName);
        int32_t textW = font.pixelWidth();
        int32_t textH = font.pixelHeight();
        if (textH <= 0) textH = legendSize; // 闂傚啫寮堕娑欐償閺囩偟婀撮悗鍦仧楠炲洤鈻介埄鍐ㄧ憦闁告帊鍗抽悵顔芥償閿旇姤顦ч柣銊ュ椤旀劙鏌ㄥ▎蹇ｆП闁?
        int32_t textCenterY = y + textH / 2;
        int32_t iconTopY = y + (textH - iconH) / 2 + 2;

        int32_t type = (i < types.size()) ? types[i] : 0;
        int32_t colorStart = (i < colors.size()) ? colors[i] : 0;
        int32_t colorEnd = (i < endColors.size()) ? endColors[i] : colorStart;

        // 缂備焦锚閸╂宕滃澶嬫〃闁汇劌瀚惃顒勫炊閻愵剛鍨?
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

            // Dynamic point size for legend markers
            int32_t pointR = (int32_t)(3 * scale);
            if (pointR < 1) pointR = 1;
            TpRect pointRect(currentX + (iconW / 2) - pointR, lineY - pointR, pointR * 2, pointR * 2);
            TpRenderUtils::fillGradientRect(painter, pointRect, colorStart, colorStart);
        }

        // 缂備焦锚閸╂寮崶褏鎽?
        painter->drawText(font, currentX + textOffset, y);

        // 婵炴挸鎲￠悥锝夊触閹存繂绀侀柟鎭掑妿浜?
        currentX += (totalItemBaseW + textW + itemSpacing);
    }
}

// 闁轰胶澧楀畵渚€鏌ㄥ鍗炰化
void TpRenderUtils::drawAnchorPoint(TpPainter* painter, const TpPoint& center, int32_t radius, int32_t borderColor, int32_t fillColor) {
    if (!painter) return;

    TpPen fillPen(fillColor, 1);
    painter->setPen(fillPen);

    fillCircleHelper(painter, center.x(), center.y(), radius - 1); // 閻犲鍟伴弫銈夊礌閸喗鍊崇紒灞炬そ濡潡宕欓懞銉︽

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

// X 閺夌偛顕划顖炲礆?
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

// Y axis drawing
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
