/*
 * 版权声明 (Copyright Declaration)
 * 作者 (Author)：张家庆
 * 邮箱 (Email)：1494197384@qq.com
 * 版权所有 (Copyright)：© 2026 张家庆。All rights reserved.
 * 描述 (Description)：图表系列基类及其派生类的实现 (Pimpl 模式)
 */

#include "TpSeries.h"
#include <algorithm>
#include <cmath>
#include "TpRenderUtils.h"
#include "TpPainter.h"
#include "TpRect.h"
#include "TpApp.h"
#include "TpCssData.h"

/// @brief 系列私有数据类
class TpSeriesPrivate
{
public:
    TpSeries::SeriesType m_type;
    TpString m_name;
    bool m_visible;
    int32_t m_color;
    TpVector<TpDataPoint> m_data;
    int32_t m_maxCount;

    TpSeriesPrivate(TpSeries::SeriesType type)
        : m_type(type), m_visible(true), m_color(0xFF000000), m_maxCount(0) {}

    virtual ~TpSeriesPrivate() {}
};

/// @brief 折线图私有数据类
class TpLineSeriesPrivate : public TpSeriesPrivate
{
public:
    int32_t m_lineWidth;
    bool m_smooth;
    float m_tension;
    bool m_useDownsample;

    TpLineSeriesPrivate()
        : TpSeriesPrivate(TpSeries::TypeLine), m_lineWidth(2), m_smooth(false), m_tension(0.5f), m_useDownsample(false) {}
};

/// @brief 柱状图私有数据类
class TpBarSeriesPrivate : public TpSeriesPrivate
{
public:
    int32_t m_colorEnd;
    int32_t m_seriesIndex;
    int32_t m_seriesCount;
    bool m_showLabels;
    int32_t m_labelColor;
    int32_t m_labelSize;

    TpBarSeriesPrivate()
        : TpSeriesPrivate(TpSeries::TypeBar), m_colorEnd(0), m_seriesIndex(0), m_seriesCount(1),
          m_showLabels(false), m_labelColor(0xFF000000), m_labelSize(10) {}
};

/// @brief 散点图私有数据类
class TpScatterSeriesPrivate : public TpSeriesPrivate
{
public:
    int32_t m_pointSize;
    int32_t m_borderColor;
    bool m_showLabels;
    int32_t m_labelColor;
    int32_t m_labelSize;

    TpScatterSeriesPrivate()
        : TpSeriesPrivate(TpSeries::TypeScatter), m_pointSize(4), m_borderColor(0), m_showLabels(false),
          m_labelColor(0xFF000000), m_labelSize(10) {}
};

/// @brief 饼图私有数据类
class TpPieSeriesPrivate : public TpSeriesPrivate
{
public:
    TpVector<TpPieSlice> m_slices;
    bool m_showLabels;
    bool m_showPercent;
    bool m_donutVisible;
    double m_donutRatio;
    int32_t m_explodedIndex;
    int32_t m_explodeDistance;
    int32_t m_startAngle;
    int32_t m_labelColor;
    int32_t m_labelSize;

    TpPieSeriesPrivate()
        : TpSeriesPrivate(TpSeries::TypePie), m_showLabels(true), m_showPercent(true), m_donutVisible(false),
          m_donutRatio(0.55), m_explodedIndex(-1), m_explodeDistance(12), m_startAngle(270),
          m_labelColor(0xFF000000), m_labelSize(10) {}
};

namespace {

static int32_t clampInt32(int32_t value, int32_t minValue, int32_t maxValue)
{
    if (value < minValue) return minValue;
    if (value > maxValue) return maxValue;
    return value;
}

static int32_t defaultPieColor(int32_t index)
{
    static const uint32_t colorList[] = {
        0xFF4F81BD,
        0xFFC0504D,
        0xFF9BBB59,
        0xFF8064A2,
        0xFF4BACC6,
        0xFFF79646,
        0xFF1F497D,
        0xFF632523
    };

    int32_t count = static_cast<int32_t>(sizeof(colorList) / sizeof(colorList[0]));
    if (count <= 0) return 0xFF4F81BD;
    if (index < 0) index = 0;
    return static_cast<int32_t>(colorList[index % count]);
}

}

// TpSeries 实现

TpSeries::TpSeries(SeriesType type)
    : d_ptr(new TpSeriesPrivate(type))
{
}

TpSeries::~TpSeries()
{
    if (d_ptr)
    {
        delete d_ptr;
        d_ptr = nullptr;
    }
}

void TpSeries::setName(const TpString& name)
{
    if (d_ptr)
    {
        d_ptr->m_name = name;
    }
}

void TpSeries::setName(const char* name)
{
    if (d_ptr)
    {
        d_ptr->m_name = name;
    }
}

const TpString& TpSeries::name() const
{
    return d_ptr->m_name;
}

void TpSeries::setVisible(bool visible)
{
    if (d_ptr)
    {
        d_ptr->m_visible = visible;
    }
}

bool TpSeries::isVisible() const
{
    return d_ptr ? d_ptr->m_visible : false;
}

void TpSeries::setColor(int32_t color)
{
    if (d_ptr)
    {
        d_ptr->m_color = color;
    }
}

int32_t TpSeries::color() const
{
    return d_ptr ? d_ptr->m_color : 0;
}

void TpSeries::addPoint(double x, double y)
{
    if (!d_ptr)
        return;

    // 添加新数据
    d_ptr->m_data.push_back(TpDataPoint(x, y));

    // 超出最大容量则移除最旧的数据
    if (d_ptr->m_maxCount > 0 && d_ptr->m_data.size() > d_ptr->m_maxCount + 2)
    {
        d_ptr->m_data.remove(0);
    }
}

void TpSeries::clear()
{
    if (d_ptr)
    {
        d_ptr->m_data.clear();
        if (d_ptr->m_type == TypePie)
        {
            static_cast<TpPieSeriesPrivate*>(d_ptr)->m_slices.clear();
        }
    }
}

const TpVector<TpDataPoint>& TpSeries::data() const
{
    return d_ptr->m_data;
}

TpSeries::SeriesType TpSeries::type() const
{
    return d_ptr->m_type;
}

void TpSeries::setMaxPointCount(int32_t count)
{
    if (d_ptr)
    {
        d_ptr->m_maxCount = count < 0 ? 0 : count;
    }
}

/// @brief 从CSS获取颜色值
/// @param className 类名（如 "TpLineSeries" 或 "TpBarSeries"）
/// @param status 状态（ Enabled, Hover, Pressed等）
void TpSeries::applyCssData(const TpString& className, TpCssParser::MouseStatus status)
{
    // 1. 首先读取通用类样式（如 TpLineSeries）
    tpShared<TpCssData> cssData = TpApp::Inst()->cssParser()->readCss(className, "", status);

    // 2. 尝试读取带名称的样式（如 TpLineSeries[name="CPU Usage"]）
    TpString typeName = name();
    if (!typeName.empty())
    {
        tpShared<TpCssData> namedCssData = TpApp::Inst()->cssParser()->readCss(className, typeName, status);
        // 如果有名称匹配的样式，优先使用
        if (namedCssData && namedCssData->color() != 0)
        {
            cssData = namedCssData;
        }
    }

    if (cssData)
    {
        // 设置颜色（如果CSS中定义了）
        if (color() == 0xFF000000 || cssData->color() != 0)
        {
            d_ptr->m_color = cssData->color();
        }

        // 设置线宽（针对折线图）
        if (type() == TypeLine)
        {
            auto* lineData = static_cast<TpLineSeriesPrivate*>(d_ptr);
            if (lineData)
            {
                int32_t cssLineWidth = cssData->borderWidth();
                if (cssLineWidth > 0)
                {
                    lineData->m_lineWidth = cssLineWidth;
                }
            }
        }
        else if (type() == TypeScatter)
        {
            auto* scatterData = static_cast<TpScatterSeriesPrivate*>(d_ptr);
            if (scatterData)
            {
                int32_t cssPointSize = cssData->borderWidth();
                if (cssPointSize > 0)
                {
                    scatterData->m_pointSize = cssPointSize;
                }
            }
        }
    }
}

// TpLineSeries 实现

TpLineSeries::TpLineSeries()
    : TpSeries(TypeLine)
{
    delete d_ptr;
    d_ptr = new TpLineSeriesPrivate();
}

TpLineSeries::~TpLineSeries()
{
}

void TpLineSeries::setLineWidth(int32_t width)
{
    auto* d = static_cast<TpLineSeriesPrivate*>(d_ptr);
    if (d)
    {
        d->m_lineWidth = width;
    }
}

void TpLineSeries::setSmooth(bool smooth)
{
    auto* d = static_cast<TpLineSeriesPrivate*>(d_ptr);
    if (d)
    {
        d->m_smooth = smooth;
    }
}

void TpLineSeries::setTension(float tension)
{
    auto* d = static_cast<TpLineSeriesPrivate*>(d_ptr);
    if (d)
    {
        d->m_tension = tension;
    }
}

void TpLineSeries::setUseDownsample(bool enabled)
{
    auto* d = static_cast<TpLineSeriesPrivate*>(d_ptr);
    if (d)
    {
        d->m_useDownsample = enabled;
    }
}

bool TpLineSeries::useDownsample() const
{
    auto* d = static_cast<TpLineSeriesPrivate*>(d_ptr);
    return d ? d->m_useDownsample : false;
}

void TpLineSeries::draw(TpPainter* painter, const TpAxis& axisX, const TpAxis& axisY, const TpRect& rect)
{
    auto* d = static_cast<TpLineSeriesPrivate*>(d_ptr);
    if (!d || !d->m_visible || d->m_data.size() < 2 || !painter)
        return;

    int32_t rectX = rect.x();
    int32_t rectY = rect.y();
    int32_t rectW = rect.width();
    int32_t rectH = rect.height();

    // 计算缩放因子 (基准分辨率按 800x600 算)
    double scaleX = rectW / 800.0;
    double scaleY = rectH / 600.0;
    double scale = scaleX < scaleY ? scaleX : scaleY;
    if (scale < 0.5)
        scale = 0.5;
    if (scale > 2.5)
        scale = 2.5;

    // 动态计算线宽和数据点半径
    int32_t dynamicLineWidth = static_cast<int32_t>(d->m_lineWidth * scale);
    if (dynamicLineWidth < 1)
        dynamicLineWidth = 1;

    int32_t dynamicRadius = static_cast<int32_t>(4 * scale);
    if (dynamicRadius < 2)
        dynamicRadius = 2;

    TpVector<TpPoint> pixelPoints;

    // ========== 降采样分支 ==========
    if (d->m_useDownsample && d->m_data.size() > rectW * 2)
    {
        // 降采样模式：使用 Min-Max 算法
        TpVector<double> yData;
        yData.reserve(d->m_data.size());
        for (int32_t i = 0; i < d->m_data.size(); ++i)
        {
            yData.append(d->m_data[i].y);
        }

        // 调用 downsample 进行降采样
        TpVector<TpAxis::SamplePoint> samples = TpAxis::downsample(yData, 0, yData.size(), rectW);

        // 将降采样结果转换为像素点（每个 bucket 取 min 和 max 两个点，形成垂直线效果）
        for (int32_t i = 0; i < samples.size(); ++i)
        {
            const TpAxis::SamplePoint& sp = samples[i];
            int32_t px = rectX + i;

            // 添加最小值点
            int32_t pyMin = axisY.mapToPixel(sp.minVal, rectH, rectY, true);
            pixelPoints.push_back(TpPoint(px, pyMin));

            // 添加最大值点
            int32_t pyMax = axisY.mapToPixel(sp.maxVal, rectH, rectY, true);
            pixelPoints.push_back(TpPoint(px, pyMax));
        }
    }
    // ========== 全量绘制分支（单线程高效映射，每个点都完整绘制） ==========
    else
    {
        const int32_t totalPoints = d->m_data.size();
        pixelPoints.resize(totalPoints);

        for (int32_t i = 0; i < totalPoints; ++i) {
            int32_t px = axisX.mapToPixel(d->m_data[i].x, rectW, rectX, false);
            int32_t py = axisY.mapToPixel(d->m_data[i].y, rectH, rectY, true);
            pixelPoints[i] = TpPoint(px, py);
        }
    }

    // ========== 后续绘制逻辑 ==========
    // 使用动态线宽绘制折线或平滑曲线
    if (d->m_smooth)
    {
        TpRenderUtils::drawSmoothCurve(painter, pixelPoints, rect, d->m_color, dynamicLineWidth, d->m_tension);
    }
    else
    {
        TpRenderUtils::drawPolyline(painter, pixelPoints, rect, d->m_color, dynamicLineWidth);
    }

    // 使用动态半径绘制数据锚点（点数较少时才绘制）
    if (pixelPoints.size() < 50)
    {
        for (int32_t i = 0; i < pixelPoints.size(); ++i)
        {
            TpRenderUtils::drawAnchorPoint(painter, pixelPoints[i], dynamicRadius, d->m_color, 0xFFFFFFFF);
        }
    }
}

// TpBarSeries 实现

TpBarSeries::TpBarSeries()
    : TpSeries(TypeBar)
{
    delete d_ptr;
    d_ptr = new TpBarSeriesPrivate();
}

TpBarSeries::~TpBarSeries()
{
}

void TpBarSeries::setGradientColor(int32_t endColor)
{
    auto* d = static_cast<TpBarSeriesPrivate*>(d_ptr);
    if (d)
    {
        d->m_colorEnd = endColor;
    }
}

int32_t TpBarSeries::colorEnd() const
{
    auto* d = static_cast<TpBarSeriesPrivate*>(d_ptr);
    return d ? d->m_colorEnd : 0;
}

void TpBarSeries::setLayoutInfo(int32_t seriesIndex, int32_t seriesCount)
{
    auto* d = static_cast<TpBarSeriesPrivate*>(d_ptr);
    if (d)
    {
        d->m_seriesIndex = seriesIndex;
        d->m_seriesCount = seriesCount;
    }
}

void TpBarSeries::setLabelsVisible(bool visible)
{
    auto* d = static_cast<TpBarSeriesPrivate*>(d_ptr);
    if (d)
    {
        d->m_showLabels = visible;
    }
}

void TpBarSeries::setLabelColor(int32_t color)
{
    auto* d = static_cast<TpBarSeriesPrivate*>(d_ptr);
    if (d)
    {
        d->m_labelColor = color;
    }
}

void TpBarSeries::setLabelSize(int32_t size)
{
    auto* d = static_cast<TpBarSeriesPrivate*>(d_ptr);
    if (d)
    {
        d->m_labelSize = size;
    }
}

void TpBarSeries::draw(TpPainter* painter, const TpAxis& axisX, const TpAxis& axisY, const TpRect& rect)
{
    auto* d = static_cast<TpBarSeriesPrivate*>(d_ptr);
    if (!d || !d->m_visible || d->m_data.size() == 0 || !painter)
        return;

    int32_t rectX = rect.x();
    int32_t rectY = rect.y();
    int32_t rectW = rect.width();
    int32_t rectH = rect.height();

    int32_t yZero = axisY.ZeroPixel(rectH, rectY, true);

    int32_t x0 = axisX.mapToPixel(axisX.min(), rectW, rectX, false);
    int32_t x1 = axisX.mapToPixel(axisX.min() + 1.0, rectW, rectX, false);
    int32_t unitPixelWidth = std::abs(x1 - x0);
    if (unitPixelWidth <= 0)
        unitPixelWidth = 50;

    int32_t groupWidth = static_cast<int32_t>(unitPixelWidth * 0.6);
    int32_t barWidth = groupWidth / d->m_seriesCount;
    if (barWidth < 1)
        barWidth = 1;

    uint32_t cStart = static_cast<uint32_t>(d->m_color);
    uint32_t cEnd = (d->m_colorEnd == 0) ? cStart : static_cast<uint32_t>(d->m_colorEnd);

    TpFont labelFont;
    if (d->m_showLabels)
    {
        labelFont.setFontSize(d->m_labelSize);
        labelFont.setFontColor(d->m_labelColor);
    }

    for (int32_t i = 0; i < d->m_data.size(); ++i)
    {
        TpDataPoint pt = d->m_data[i];

        int32_t xCenter = axisX.mapToPixel(pt.x, rectW, rectX, false);
        int32_t barLeft = xCenter - (groupWidth / 2) + (d->m_seriesIndex * barWidth);
        int32_t yVal = axisY.mapToPixel(pt.y, rectH, rectY, true);

        int32_t top = (pt.y >= 0) ? yVal : yZero;
        int32_t height = std::abs(yVal - yZero);
        if (height == 0)
            height = 1;

        TpRect barRect(barLeft, top, barWidth, height);

        if (barRect.right() < rectX || barRect.x() > rect.right())
        {
            continue;
        }

        TpRenderUtils::drawGradientBar(painter, barRect, cStart, cEnd);

        if (d->m_showLabels)
        {
            TpString sVal = TpString::number(static_cast<int32_t>(pt.y));
            labelFont.setText(sVal);

            int32_t textX = barLeft + (barWidth - labelFont.pixelWidth()) / 2;
            int32_t textY = (pt.y >= 0) ? (top - labelFont.pixelHeight() - 2) : (top + height + 2);

            painter->drawText(labelFont, textX, textY);
        }
    }
}

// TpScatterSeries 瀹炵幇

TpScatterSeries::TpScatterSeries()
    : TpSeries(TypeScatter)
{
    delete d_ptr;
    d_ptr = new TpScatterSeriesPrivate();
}

TpScatterSeries::~TpScatterSeries()
{
}

void TpScatterSeries::setPointSize(int32_t size)
{
    auto* d = static_cast<TpScatterSeriesPrivate*>(d_ptr);
    if (d)
    {
        d->m_pointSize = size > 0 ? size : 1;
    }
}

int32_t TpScatterSeries::pointSize() const
{
    auto* d = static_cast<TpScatterSeriesPrivate*>(d_ptr);
    return d ? d->m_pointSize : 0;
}

void TpScatterSeries::setBorderColor(int32_t color)
{
    auto* d = static_cast<TpScatterSeriesPrivate*>(d_ptr);
    if (d)
    {
        d->m_borderColor = color;
    }
}

int32_t TpScatterSeries::borderColor() const
{
    auto* d = static_cast<TpScatterSeriesPrivate*>(d_ptr);
    return d ? d->m_borderColor : 0;
}

void TpScatterSeries::setLabelsVisible(bool visible)
{
    auto* d = static_cast<TpScatterSeriesPrivate*>(d_ptr);
    if (d)
    {
        d->m_showLabels = visible;
    }
}

void TpScatterSeries::setLabelColor(int32_t color)
{
    auto* d = static_cast<TpScatterSeriesPrivate*>(d_ptr);
    if (d)
    {
        d->m_labelColor = color;
    }
}

void TpScatterSeries::setLabelSize(int32_t size)
{
    auto* d = static_cast<TpScatterSeriesPrivate*>(d_ptr);
    if (d)
    {
        d->m_labelSize = size > 0 ? size : 1;
    }
}

void TpScatterSeries::draw(TpPainter* painter, const TpAxis& axisX, const TpAxis& axisY, const TpRect& rect)
{
    auto* d = static_cast<TpScatterSeriesPrivate*>(d_ptr);
    if (!d || !d->m_visible || d->m_data.size() == 0 || !painter)
        return;

    int32_t rectX = rect.x();
    int32_t rectY = rect.y();
    int32_t rectW = rect.width();
    int32_t rectH = rect.height();

    double scaleX = rectW / 800.0;
    double scaleY = rectH / 600.0;
    double scale = scaleX < scaleY ? scaleX : scaleY;
    if (scale < 0.5)
        scale = 0.5;
    if (scale > 2.5)
        scale = 2.5;

    int32_t pointRadius = static_cast<int32_t>(d->m_pointSize * scale);
    if (pointRadius < 2)
        pointRadius = 2;

    int32_t borderColor = d->m_borderColor;
    if (borderColor == 0)
        borderColor = d->m_color;
    if (borderColor == 0)
        borderColor = 0xFF000000;

    int32_t fillColor = d->m_color;
    if (fillColor == 0)
        fillColor = borderColor;

    TpFont labelFont;
    int32_t labelSize = static_cast<int32_t>(d->m_labelSize * scale);
    if (labelSize < 8)
        labelSize = 8;
    labelFont.setFontSize(labelSize);
    labelFont.setFontColor(d->m_labelColor == 0 ? 0xFF000000 : d->m_labelColor);

    for (int32_t i = 0; i < d->m_data.size(); ++i)
    {
        const TpDataPoint& pt = d->m_data[i];

        int32_t px = axisX.mapToPixel(pt.x, rectW, rectX, false);
        int32_t py = axisY.mapToPixel(pt.y, rectH, rectY, true);

        if (px < rectX - pointRadius || px > rect.right() + pointRadius)
            continue;
        if (py < rectY - pointRadius || py > rect.bottom() + pointRadius)
            continue;

        painter->setPen(TpPen(borderColor, 1));
        painter->setBrush(TpBrush(fillColor));
        painter->drawEllipse(TpPoint(px, py), pointRadius, pointRadius);

        if (d->m_showLabels)
        {
            TpString text = "(";
            text += TpString::number(pt.x, 2);
            text += ", ";
            text += TpString::number(pt.y, 2);
            text += ")";

            labelFont.setText(text);
            int32_t textW = labelFont.pixelWidth();
            int32_t textH = labelFont.pixelHeight();
            if (textW <= 0) textW = static_cast<int32_t>(text.length()) * (labelSize / 2 + 1);
            if (textH <= 0) textH = labelSize;

            int32_t textX = px + pointRadius + 4;
            int32_t textY = py - pointRadius - textH;
            if (textX + textW > rect.right())
            {
                textX = px - pointRadius - 4 - textW;
            }
            if (textX < rectX)
            {
                textX = rectX;
            }
            if (textY < rectY)
            {
                textY = py + pointRadius + 2;
            }

            painter->drawText(labelFont, textX, textY);
        }
    }
}

// TpPieSeries 瀹炵幇

TpPieSeries::TpPieSeries()
    : TpSeries(TypePie)
{
    delete d_ptr;
    d_ptr = new TpPieSeriesPrivate();
}

TpPieSeries::~TpPieSeries()
{
}

void TpPieSeries::addSlice(const TpString& name, double value, int32_t color)
{
    auto* d = static_cast<TpPieSeriesPrivate*>(d_ptr);
    if (d)
    {
        d->m_slices.push_back(TpPieSlice(name, value, color));
    }
}

void TpPieSeries::addSlice(const char* name, double value, int32_t color)
{
    addSlice(TpString(name), value, color);
}

void TpPieSeries::clearSlices()
{
    auto* d = static_cast<TpPieSeriesPrivate*>(d_ptr);
    if (d)
    {
        d->m_slices.clear();
    }
}

int32_t TpPieSeries::sliceCount() const
{
    auto* d = static_cast<TpPieSeriesPrivate*>(d_ptr);
    return d ? d->m_slices.size() : 0;
}

const TpString& TpPieSeries::sliceName(int32_t index) const
{
    static TpString emptyText;
    auto* d = static_cast<TpPieSeriesPrivate*>(d_ptr);
    if (!d || index < 0 || index >= d->m_slices.size())
        return emptyText;
    return d->m_slices[index].name;
}

double TpPieSeries::sliceValue(int32_t index) const
{
    auto* d = static_cast<TpPieSeriesPrivate*>(d_ptr);
    if (!d || index < 0 || index >= d->m_slices.size())
        return 0.0;
    return d->m_slices[index].value;
}

int32_t TpPieSeries::sliceColor(int32_t index) const
{
    auto* d = static_cast<TpPieSeriesPrivate*>(d_ptr);
    if (!d || index < 0 || index >= d->m_slices.size())
        return 0;
    return d->m_slices[index].color;
}

void TpPieSeries::setLabelsVisible(bool visible)
{
    auto* d = static_cast<TpPieSeriesPrivate*>(d_ptr);
    if (d)
    {
        d->m_showLabels = visible;
    }
}

void TpPieSeries::setPercentVisible(bool visible)
{
    auto* d = static_cast<TpPieSeriesPrivate*>(d_ptr);
    if (d)
    {
        d->m_showPercent = visible;
    }
}

void TpPieSeries::setDonutVisible(bool visible)
{
    auto* d = static_cast<TpPieSeriesPrivate*>(d_ptr);
    if (d)
    {
        d->m_donutVisible = visible;
    }
}

void TpPieSeries::setDonutRatio(double ratio)
{
    auto* d = static_cast<TpPieSeriesPrivate*>(d_ptr);
    if (d)
    {
        if (ratio < 0.05)
            ratio = 0.05;
        if (ratio > 0.95)
            ratio = 0.95;
        d->m_donutRatio = ratio;
    }
}

void TpPieSeries::setStartAngle(int32_t angle)
{
    auto* d = static_cast<TpPieSeriesPrivate*>(d_ptr);
    if (d)
    {
        d->m_startAngle = angle;
    }
}

void TpPieSeries::setExplodedIndex(int32_t index)
{
    auto* d = static_cast<TpPieSeriesPrivate*>(d_ptr);
    if (d)
    {
        d->m_explodedIndex = index;
    }
}

void TpPieSeries::setExplodeDistance(int32_t distance)
{
    auto* d = static_cast<TpPieSeriesPrivate*>(d_ptr);
    if (d)
    {
        d->m_explodeDistance = distance > 0 ? distance : 0;
    }
}

void TpPieSeries::setLabelColor(int32_t color)
{
    auto* d = static_cast<TpPieSeriesPrivate*>(d_ptr);
    if (d)
    {
        d->m_labelColor = color;
    }
}

void TpPieSeries::setLabelSize(int32_t size)
{
    auto* d = static_cast<TpPieSeriesPrivate*>(d_ptr);
    if (d)
    {
        d->m_labelSize = size > 0 ? size : 1;
    }
}

void TpPieSeries::draw(TpPainter* painter, const TpAxis&, const TpAxis&, const TpRect& rect)
{
    auto* d = static_cast<TpPieSeriesPrivate*>(d_ptr);
    if (!d || !d->m_visible || !painter || d->m_slices.size() == 0)
        return;

    double totalValue = 0.0;
    for (int32_t i = 0; i < d->m_slices.size(); ++i)
    {
        double value = d->m_slices[i].value;
        if (value > 0.0)
        {
            totalValue += value;
        }
    }

    if (totalValue <= 0.0)
        return;

    int32_t rectX = rect.x();
    int32_t rectY = rect.y();
    int32_t rectW = rect.width();
    int32_t rectH = rect.height();

    int32_t padding = 12;
    if (rectW < 160 || rectH < 160)
        padding = 6;

    int32_t minSide = rectW < rectH ? rectW : rectH;
    int32_t diameter = minSide - padding * 2;
    if (diameter < 20)
        diameter = minSide;

    int32_t radius = diameter / 2;
    if (radius < 1)
        return;

    int32_t centerX = rectX + rectW / 2;
    int32_t centerY = rectY + rectH / 2;

    double currentAngle = d->m_startAngle;
    int32_t labelColor = d->m_labelColor == 0 ? 0xFF000000 : d->m_labelColor;
    int32_t labelSize = d->m_labelSize;
    if (labelSize < 8)
        labelSize = 8;

    TpFont labelFont;
    labelFont.setFontSize(labelSize);
    labelFont.setFontColor(labelColor);

    for (int32_t i = 0; i < d->m_slices.size(); ++i)
    {
        const TpPieSlice& slice = d->m_slices[i];
        if (slice.value <= 0.0)
            continue;

        double sweep = (slice.value * 360.0) / totalValue;
        if (sweep <= 0.0)
            continue;

        int32_t sliceColor = slice.color;
        if (sliceColor == 0)
            sliceColor = d->m_color;
        if (sliceColor == 0)
            sliceColor = defaultPieColor(i);

        double midAngle = currentAngle + (sweep * 0.5);
        double midRad = midAngle * 3.14159265358979323846 / 180.0;

        int32_t drawCenterX = centerX;
        int32_t drawCenterY = centerY;
        if (d->m_explodedIndex == i && d->m_explodeDistance > 0)
        {
            drawCenterX += static_cast<int32_t>(std::cos(midRad) * d->m_explodeDistance + 0.5);
            drawCenterY += static_cast<int32_t>(std::sin(midRad) * d->m_explodeDistance + 0.5);
        }

        TpHollowMask hollowMask;
        int32_t innerRadius = 0;
        if (d->m_donutVisible)
        {
            innerRadius = static_cast<int32_t>(radius * d->m_donutRatio);
            if (innerRadius > 0 && innerRadius < radius)
            {
                hollowMask.addCircleHollow(drawCenterX, drawCenterY, innerRadius);
            }
            else
            {
                innerRadius = 0;
            }
        }

        painter->setPen(TpPen(sliceColor, 1));
        painter->setBrush(TpBrush(sliceColor));
        painter->drawPie(TpPoint(drawCenterX, drawCenterY), radius, static_cast<int32_t>(currentAngle), static_cast<int32_t>(currentAngle + sweep), hollowMask);

        if (d->m_showLabels || d->m_showPercent)
        {
            TpString text;
            if (d->m_showLabels && !slice.name.empty())
            {
                text += slice.name;
            }

            if (d->m_showPercent)
            {
                double percent = (slice.value * 100.0) / totalValue;
                if (!text.empty())
                {
                    text += " ";
                }
                text += TpString::number(percent, 1);
                text += "%";
            }

            if (!text.empty())
            {
                labelFont.setText(text);
                int32_t textW = labelFont.pixelWidth();
                int32_t textH = labelFont.pixelHeight();
                if (textW <= 0) textW = static_cast<int32_t>(text.length()) * (labelSize / 2 + 1);
                if (textH <= 0) textH = labelSize;

                double labelRadius = d->m_donutVisible ? (innerRadius + (radius - innerRadius) * 0.5) : (radius * 0.65);
                if (sweep < 18.0)
                {
                    labelRadius = radius + 16.0;
                }

                int32_t labelX = drawCenterX + static_cast<int32_t>(std::cos(midRad) * labelRadius + 0.5) - (textW / 2);
                int32_t labelY = drawCenterY + static_cast<int32_t>(std::sin(midRad) * labelRadius + 0.5) - (textH / 2);

                if (labelX < rectX)
                    labelX = rectX;
                if (labelY < rectY)
                    labelY = rectY;
                if (labelX + textW > rect.right())
                    labelX = rect.right() - textW;
                if (labelY + textH > rect.bottom())
                    labelY = rect.bottom() - textH;

                painter->drawText(labelFont, labelX, labelY);
            }
        }

        currentAngle += sweep;
    }
}

