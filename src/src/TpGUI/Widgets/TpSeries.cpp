/*
 * 版权声明 (Copyright Declaration)
 * 作者 (Author)：张家庆
 * 邮箱 (Email)：1494197384@qq.com
 * 版权所有 (Copyright)：© 2026 张家庆。All rights reserved.
 * 描述 (Description)：图表系列基类及其派生类的实现 (Pimpl 模式)
 */

#include "TpSeries.h"
#include <cmath>
#include "TpRenderUtils.h"
#include "TpPainter.h"
#include "TpRect.h"
#include "TpApp.h"
#include "TpCssData.h"
#include <thread>
#include <vector>

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
    // ========== 全量绘制分支（多线程优化，不丢失任何点） ==========
    else
    {
        const int32_t totalPoints = d->m_data.size();
        // 预分配空间，避免动态扩容
        pixelPoints.resize(totalPoints);

        // 多线程并行映射
        const int32_t numThreads = std::max(1, (int32_t)std::thread::hardware_concurrency());
        std::vector<std::thread> threads;
        int32_t chunkSize = (totalPoints + numThreads - 1) / numThreads;

        auto mapper = [&](int32_t startIdx, int32_t endIdx) {
            for (int32_t i = startIdx; i < endIdx; ++i) {
                int32_t px = axisX.mapToPixel(d->m_data[i].x, rectW, rectX, false);
                int32_t py = axisY.mapToPixel(d->m_data[i].y, rectH, rectY, true);
                pixelPoints[i] = TpPoint(px, py);
            }
        };

        for (int32_t t = 0; t < numThreads; ++t) {
            int32_t start = t * chunkSize;
            int32_t end = std::min(start + chunkSize, totalPoints);
            if (start < end)
                threads.emplace_back(mapper, start, end);
        }
        for (auto& th : threads) th.join();
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

