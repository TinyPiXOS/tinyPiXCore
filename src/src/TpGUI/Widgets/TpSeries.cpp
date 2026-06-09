/*
 * 鐗堟潈澹版槑 (Copyright Declaration)
 * 浣滆€?(Author)锛氬紶瀹跺簡
 * 閭 (Email)锛?494197384@qq.com
 * 鐗堟潈鎵€鏈?(Copyright)锛毬?2026 寮犲搴嗐€侫ll rights reserved.
 * 鎻忚堪 (Description)锛氭暟鎹郴鍒楃被 API 瀹氫箟锛岀敤浜庣鐞嗗浘琛ㄤ腑鐨勬暟鎹偣闆嗗悎
 */

#include "TpSeries.h"
#include <algorithm>
#include <cmath>
#include "TpRenderUtils.h"
#include "TpBarGeometryHelper_p.h"
#include "TpPainter.h"
#include "TpRect.h"
#include "TpApp.h"
#include "TpCssData.h"

/// @brief 鏁版嵁绯诲垪绉佹湁瀹炵幇绫伙紝鍖呭惈鎵€鏈夌郴鍒楃被鍨嬬殑鍏叡灞炴€у拰鏁版嵁
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

/// @brief 鎶樼嚎绯诲垪绉佹湁瀹炵幇绫?
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

/// @brief 鏌辩姸绯诲垪绉佹湁瀹炵幇绫?
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

/// @brief 鏁ｇ偣绯诲垪绉佹湁瀹炵幇绫?
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

/// @brief 楗煎浘绯诲垪绉佹湁瀹炵幇绫?
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

/// @brief 灏嗘暣鏁板€奸檺鍒跺湪 [minValue, maxValue] 鑼冨洿鍐?
static int32_t clampInt32(int32_t value, int32_t minValue, int32_t maxValue)
{
    if (value < minValue) return minValue;
    if (value > maxValue) return maxValue;
    return value;
}

/// @brief 鏍规嵁绱㈠紩杩斿洖楗煎浘鎵囧尯鐨勯粯璁ら鑹诧紙8 鑹插惊鐜級
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

// ============ TpSeries 鍩虹被鏂规硶 ============

TpSeries::TpSeries(SeriesType type)
    : data_(new TpSeriesPrivate(type))
{
}

TpSeries::~TpSeries()
{
    if (data_)
    {
        delete data_;
        data_ = nullptr;
    }
}

void TpSeries::setName(const TpString& name)
{
    if (data_)
    {
        data_->m_name = name;
    }
}

void TpSeries::setName(const char* name)
{
    if (data_)
    {
        data_->m_name = name;
    }
}

const TpString& TpSeries::name() const
{
    return data_->m_name;
}

void TpSeries::setVisible(bool visible)
{
    if (data_)
    {
        data_->m_visible = visible;
    }
}

bool TpSeries::isVisible() const
{
    return data_ ? data_->m_visible : false;
}

void TpSeries::setColor(int32_t color)
{
    if (data_)
    {
        data_->m_color = color;
    }
}

int32_t TpSeries::color() const
{
    return data_ ? data_->m_color : 0;
}

/// @brief 娣诲姞鏁版嵁鐐癸紱鑻ヨ秴鍑烘渶澶х偣鏁伴檺鍒跺垯绉婚櫎鏈€鏃╃殑鏁版嵁鐐?
void TpSeries::addPoint(double x, double y)
{
    if (!data_)
        return;

    data_->m_data.push_back(TpDataPoint(x, y));

    if (data_->m_maxCount > 0)
    {
        while (data_->m_data.size() > data_->m_maxCount)
        {
            data_->m_data.remove(0);
        }
    }
}

void TpSeries::clear()
{
    if (data_)
    {
        data_->m_data.clear();
        if (data_->m_type == TypePie)
        {
            static_cast<TpPieSeriesPrivate*>(data_)->m_slices.clear();
        }
    }
}

const TpVector<TpDataPoint>& TpSeries::data() const
{
    return data_->m_data;
}

TpSeries::SeriesType TpSeries::type() const
{
    return data_->m_type;
}

void TpSeries::setMaxPointCount(int32_t count)
{
    if (data_)
    {
        data_->m_maxCount = count < 0 ? 0 : count;
        if (data_->m_maxCount > 0)
        {
            while (data_->m_data.size() > data_->m_maxCount)
            {
                data_->m_data.remove(0);
            }
        }
    }
}

/// @brief 鎸?CSS 鐘舵€佸簲鐢ㄧ郴鍒楁牱寮?
/// @param className CSS 绫诲悕锛屼緥濡?"TpLineSeries"
/// @param status CSS 鐘舵€侊紝渚嬪 Hover銆丳ressed 绛?
void TpSeries::applyCssData(const TpString& className, TpCssParser::MouseStatus status)
{
    tpShared<TpCssData> cssData = TpApp::Inst()->cssParser()->readCss(className, "", status);

    TpString typeName = name();
    if (!typeName.empty())
    {
        tpShared<TpCssData> namedCssData = TpApp::Inst()->cssParser()->readCss(className, typeName, status);
        if (namedCssData && (namedCssData->colorIsGradient() || namedCssData->color() != 0))
        {
            cssData = namedCssData;
        }
    }

    if (cssData)
    {
        int32_t resolvedColor = cssData->color();
        bool hasColor = cssData->colorIsGradient() || resolvedColor != 0;
        if (cssData->colorIsGradient())
        {
            TpGradient* gradient = cssData->colorGradiant();
            if (gradient)
            {
                TpList<std::pair<float, int32_t>> colorAtList = gradient->getColors();
                if (!colorAtList.empty())
                {
                    resolvedColor = colorAtList.front().second;
                }
            }
        }

        if (hasColor && (color() == 0xFF000000 || resolvedColor != 0))
        {
            data_->m_color = resolvedColor;
        }

        if (type() == TypeLine)
        {
            auto* lineData = static_cast<TpLineSeriesPrivate*>(data_);
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
            auto* scatterData = static_cast<TpScatterSeriesPrivate*>(data_);
            if (scatterData)
            {
                int32_t cssPointSize = cssData->borderWidth();
                if (cssPointSize > 0)
                {
                    scatterData->m_pointSize = cssPointSize;
                }
            }
        }
        else if (type() == TypeBar)
        {
            auto* barData = static_cast<TpBarSeriesPrivate*>(data_);
            if (barData && hasColor)
            {
                barData->m_color = resolvedColor;
                barData->m_colorEnd = resolvedColor;

                if (cssData->colorIsGradient())
                {
                    TpGradient* gradient = cssData->colorGradiant();
                    if (gradient)
                    {
                        TpList<std::pair<float, int32_t>> colorAtList = gradient->getColors();
                        if (!colorAtList.empty())
                        {
                            barData->m_color = colorAtList.front().second;
                            barData->m_colorEnd = colorAtList.back().second;
                        }
                    }
                }
            }
        }
    }
}

// ============ TpLineSeries 鎶樼嚎绯诲垪 ============

/// @brief 鏋勯€犳姌绾跨郴鍒楋紝鏇挎崲鍩虹被榛樿绉佹湁鏁版嵁涓?TpLineSeriesPrivate
TpLineSeries::TpLineSeries()
    : TpSeries(TypeLine)
{
    delete data_;
    data_ = new TpLineSeriesPrivate();
}

TpLineSeries::~TpLineSeries()
{
}

void TpLineSeries::setLineWidth(int32_t width)
{
    auto* d = static_cast<TpLineSeriesPrivate*>(data_);
    if (d)
    {
        d->m_lineWidth = width;
    }
}

void TpLineSeries::setSmooth(bool smooth)
{
    auto* d = static_cast<TpLineSeriesPrivate*>(data_);
    if (d)
    {
        d->m_smooth = smooth;
    }
}

void TpLineSeries::setTension(float tension)
{
    auto* d = static_cast<TpLineSeriesPrivate*>(data_);
    if (d)
    {
        d->m_tension = tension;
    }
}

void TpLineSeries::setUseDownsample(bool enabled)
{
    auto* d = static_cast<TpLineSeriesPrivate*>(data_);
    if (d)
    {
        d->m_useDownsample = enabled;
    }
}

bool TpLineSeries::useDownsample() const
{
    auto* d = static_cast<TpLineSeriesPrivate*>(data_);
    return d ? d->m_useDownsample : false;
}

/// @brief 缁樺埗鎶樼嚎鍥撅細灏嗘暟鎹偣鏄犲皠涓哄儚绱犲潗鏍囧悗缁樺埗鎶樼嚎/骞虫粦鏇茬嚎鍙婇敋鐐?
/// @param painter 鐢荤瑪瀵硅薄
/// @param axisX   X 杞达紙鐢ㄤ簬灏嗘暟鎹?X 鍊兼槧灏勪负鍍忕礌 X 鍧愭爣锛?
/// @param axisY   Y 杞达紙鐢ㄤ簬灏嗘暟鎹?Y 鍊兼槧灏勪负鍍忕礌 Y 鍧愭爣锛?
/// @param rect    缁樺埗鍖哄煙鐭╁舰
void TpLineSeries::draw(TpPainter* painter, const TpAxis& axisX, const TpAxis& axisY, const TpRect& rect)
{
    auto* d = static_cast<TpLineSeriesPrivate*>(data_);
    if (!d || !d->m_visible || d->m_data.size() < 2 || !painter)
        return;

    int32_t rectX = rect.x();
    int32_t rectY = rect.y();
    int32_t rectW = rect.width();
    int32_t rectH = rect.height();

    // 鏍规嵁缁樺埗鍖哄煙璁＄畻鍔ㄦ€佺缉鏀惧洜瀛愶紝纭繚绾垮鍜岄敋鐐瑰湪涓嶅悓鍒嗚鲸鐜囦笅淇濇寔鍚堢悊灏哄
    double scaleX = rectW / 800.0;
    double scaleY = rectH / 600.0;
    double scale = scaleX < scaleY ? scaleX : scaleY;
    if (scale < 0.5)
        scale = 0.5;
    if (scale > 2.5)
        scale = 2.5;

    int32_t dynamicLineWidth = static_cast<int32_t>(d->m_lineWidth * scale);
    if (dynamicLineWidth < 1)
        dynamicLineWidth = 1;

    int32_t dynamicRadius = static_cast<int32_t>(4 * scale);
    if (dynamicRadius < 2)
        dynamicRadius = 2;

    TpVector<TpPoint> pixelPoints;

    // Downsample only when the point count is far beyond the drawable width
    if (d->m_useDownsample && rectW > 0 && d->m_data.size() > rectW * 2)
    {
        struct SampledPoint
        {
            TpPoint pixelPoint;
        };

        TpVector<SampledPoint> sampledPoints;
        sampledPoints.reserve(rectW * 2);

        const int32_t totalPoints = d->m_data.size();
        for (int32_t bucket = 0; bucket < rectW; ++bucket)
        {
            int32_t startIndex = (bucket * totalPoints) / rectW;
            int32_t endIndex = ((bucket + 1) * totalPoints) / rectW;
            if (startIndex >= totalPoints)
            {
                break;
            }
            if (endIndex <= startIndex)
            {
                endIndex = startIndex + 1;
            }
            if (endIndex > totalPoints)
            {
                endIndex = totalPoints;
            }

            int32_t minIndex = startIndex;
            int32_t maxIndex = startIndex;
            double minValue = d->m_data[startIndex].y;
            double maxValue = minValue;

            for (int32_t i = startIndex + 1; i < endIndex; ++i)
            {
                double value = d->m_data[i].y;
                if (value < minValue)
                {
                    minValue = value;
                    minIndex = i;
                }
                if (value > maxValue)
                {
                    maxValue = value;
                    maxIndex = i;
                }
            }

            if (minIndex == maxIndex)
            {
                const TpDataPoint& point = d->m_data[minIndex];
                SampledPoint sampled;
                sampled.pixelPoint = TpPoint(axisX.mapToPixel(point.x, rectW, rectX, false),
                                             axisY.mapToPixel(point.y, rectH, rectY, true));
                sampledPoints.push_back(sampled);
            }
            else
            {
                if (minIndex < maxIndex)
                {
                    const TpDataPoint& minPoint = d->m_data[minIndex];
                    SampledPoint sampledMin;
                    sampledMin.pixelPoint = TpPoint(axisX.mapToPixel(minPoint.x, rectW, rectX, false),
                                                    axisY.mapToPixel(minPoint.y, rectH, rectY, true));
                    sampledPoints.push_back(sampledMin);

                    const TpDataPoint& maxPoint = d->m_data[maxIndex];
                    SampledPoint sampledMax;
                    sampledMax.pixelPoint = TpPoint(axisX.mapToPixel(maxPoint.x, rectW, rectX, false),
                                                    axisY.mapToPixel(maxPoint.y, rectH, rectY, true));
                    sampledPoints.push_back(sampledMax);
                }
                else
                {
                    const TpDataPoint& maxPoint = d->m_data[maxIndex];
                    SampledPoint sampledMax;
                    sampledMax.pixelPoint = TpPoint(axisX.mapToPixel(maxPoint.x, rectW, rectX, false),
                                                    axisY.mapToPixel(maxPoint.y, rectH, rectY, true));
                    sampledPoints.push_back(sampledMax);

                    const TpDataPoint& minPoint = d->m_data[minIndex];
                    SampledPoint sampledMin;
                    sampledMin.pixelPoint = TpPoint(axisX.mapToPixel(minPoint.x, rectW, rectX, false),
                                                    axisY.mapToPixel(minPoint.y, rectH, rectY, true));
                    sampledPoints.push_back(sampledMin);
                }
            }
        }

        pixelPoints.reserve(sampledPoints.size());
        for (int32_t i = 0; i < sampledPoints.size(); ++i)
        {
            pixelPoints.push_back(sampledPoints[i].pixelPoint);
        }
    }
    // 鍏ㄩ噺缁樺埗锛氬皢姣忎釜鏁版嵁鐐规槧灏勪负鍍忕礌鍧愭爣
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

    // 鏍规嵁鏄惁骞虫粦缁樺埗鎶樼嚎鎴栧钩婊戞洸绾?
    if (d->m_smooth)
    {
        TpRenderUtils::drawSmoothCurve(painter, pixelPoints, rect, d->m_color, dynamicLineWidth, d->m_tension);
    }
    else
    {
        TpRenderUtils::drawPolyline(painter, pixelPoints, rect, d->m_color, dynamicLineWidth);
    }

    // 鐐规暟杈冨皯鏃剁粯鍒舵暟鎹敋鐐?
    if (pixelPoints.size() < 50)
    {
        for (int32_t i = 0; i < pixelPoints.size(); ++i)
        {
            TpRenderUtils::drawAnchorPoint(painter, pixelPoints[i], dynamicRadius, d->m_color, 0xFFFFFFFF);
        }
    }
}

// ============ TpBarSeries 鏌辩姸绯诲垪 ============

/// @brief 鏋勯€犳煴鐘剁郴鍒楋紝鏇挎崲鍩虹被榛樿绉佹湁鏁版嵁涓?TpBarSeriesPrivate
TpBarSeries::TpBarSeries()
    : TpSeries(TypeBar)
{
    delete data_;
    data_ = new TpBarSeriesPrivate();
}

TpBarSeries::~TpBarSeries()
{
}

void TpBarSeries::setGradientColor(int32_t endColor)
{
    auto* d = static_cast<TpBarSeriesPrivate*>(data_);
    if (d)
    {
        d->m_colorEnd = endColor;
    }
}

int32_t TpBarSeries::colorEnd() const
{
    auto* d = static_cast<TpBarSeriesPrivate*>(data_);
    return d ? d->m_colorEnd : 0;
}

void TpBarSeries::setLayoutInfo(int32_t seriesIndex, int32_t seriesCount)
{
    auto* d = static_cast<TpBarSeriesPrivate*>(data_);
    if (d)
    {
        d->m_seriesIndex = seriesIndex;
        d->m_seriesCount = seriesCount;
    }
}

void TpBarSeries::setLabelsVisible(bool visible)
{
    auto* d = static_cast<TpBarSeriesPrivate*>(data_);
    if (d)
    {
        d->m_showLabels = visible;
    }
}

void TpBarSeries::setLabelColor(int32_t color)
{
    auto* d = static_cast<TpBarSeriesPrivate*>(data_);
    if (d)
    {
        d->m_labelColor = color;
    }
}

void TpBarSeries::setLabelSize(int32_t size)
{
    auto* d = static_cast<TpBarSeriesPrivate*>(data_);
    if (d)
    {
        d->m_labelSize = size;
    }
}

/// @brief 缁樺埗鏌辩姸鍥撅細鏍规嵁 X/Y 杞存槧灏勮绠楁煴瀛愪綅缃拰楂樺害锛屾敮鎸佹笎鍙樿壊鍜屾暟鍊兼爣绛?
void TpBarSeries::draw(TpPainter* painter, const TpAxis& axisX, const TpAxis& axisY, const TpRect& rect)
{
    auto* d = static_cast<TpBarSeriesPrivate*>(data_);
    if (!d || !d->m_visible || d->m_data.size() == 0 || !painter)
        return;

    int32_t rectX = rect.x();

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
        TpRect barRect = tpBuildBarRect(axisX, axisY, rect, pt.x, pt.y, d->m_seriesIndex, d->m_seriesCount);

        if (barRect.right() < rectX || barRect.x() > rect.right())
        {
            continue;
        }

        TpRenderUtils::drawGradientBar(painter, barRect, cStart, cEnd);

        if (d->m_showLabels)
        {
            TpString sVal = TpString::number(static_cast<int32_t>(pt.y));
            labelFont.setText(sVal);

            int32_t textX = barRect.x() + (barRect.width() - labelFont.pixelWidth()) / 2;
            int32_t textY = (pt.y >= 0) ? (barRect.y() - labelFont.pixelHeight() - 2) : (barRect.y() + barRect.height() + 2);

            painter->drawText(labelFont, textX, textY);
        }
    }
}

// ============ TpScatterSeries 鏁ｇ偣绯诲垪 ============

/// @brief 鏋勯€犳暎鐐圭郴鍒楋紝鏇挎崲鍩虹被榛樿绉佹湁鏁版嵁涓?TpScatterSeriesPrivate
TpScatterSeries::TpScatterSeries()
    : TpSeries(TypeScatter)
{
    delete data_;
    data_ = new TpScatterSeriesPrivate();
}

TpScatterSeries::~TpScatterSeries()
{
}

void TpScatterSeries::setPointSize(int32_t size)
{
    auto* d = static_cast<TpScatterSeriesPrivate*>(data_);
    if (d)
    {
        d->m_pointSize = size > 0 ? size : 1;
    }
}

int32_t TpScatterSeries::pointSize() const
{
    auto* d = static_cast<TpScatterSeriesPrivate*>(data_);
    return d ? d->m_pointSize : 0;
}

void TpScatterSeries::setBorderColor(int32_t color)
{
    auto* d = static_cast<TpScatterSeriesPrivate*>(data_);
    if (d)
    {
        d->m_borderColor = color;
    }
}

int32_t TpScatterSeries::borderColor() const
{
    auto* d = static_cast<TpScatterSeriesPrivate*>(data_);
    return d ? d->m_borderColor : 0;
}

void TpScatterSeries::setLabelsVisible(bool visible)
{
    auto* d = static_cast<TpScatterSeriesPrivate*>(data_);
    if (d)
    {
        d->m_showLabels = visible;
    }
}

void TpScatterSeries::setLabelColor(int32_t color)
{
    auto* d = static_cast<TpScatterSeriesPrivate*>(data_);
    if (d)
    {
        d->m_labelColor = color;
    }
}

void TpScatterSeries::setLabelSize(int32_t size)
{
    auto* d = static_cast<TpScatterSeriesPrivate*>(data_);
    if (d)
    {
        d->m_labelSize = size > 0 ? size : 1;
    }
}

/// @brief 缁樺埗鏁ｇ偣鍥撅細灏嗘瘡涓暟鎹偣鏄犲皠涓烘き鍦嗭紝鏀寔杈规鑹层€佸～鍏呰壊鍜屽潗鏍囨爣绛?
void TpScatterSeries::draw(TpPainter* painter, const TpAxis& axisX, const TpAxis& axisY, const TpRect& rect)
{
    auto* d = static_cast<TpScatterSeriesPrivate*>(data_);
    if (!d || !d->m_visible || d->m_data.size() == 0 || !painter)
        return;

    int32_t rectX = rect.x();
    int32_t rectY = rect.y();
    int32_t rectW = rect.width();
    int32_t rectH = rect.height();

    // 鍔ㄦ€佺缉鏀剧偣鍗婂緞锛屼娇鍏跺湪涓嶅悓缁樺埗鍖哄煙涓嬩繚鎸佸悎鐞嗗ぇ灏?
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

    // 杈规鑹插拰濉厖鑹插洖閫€閫昏緫锛歜orderColor 鈫?m_color 鈫?榛樿榛戣壊
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

        // 瑁佸壀锛氳烦杩囧畬鍏ㄥ湪缁樺埗鍖哄煙澶栫殑鐐?
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

            // 鏍囩榛樿鏀惧湪鐐圭殑鍙充笂鏂癸紝绌洪棿涓嶈冻鏃惰嚜鍔ㄨ皟鏁村埌宸︿晶鎴栦笅鏂?
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

// ============ TpPieSeries 楗煎浘绯诲垪 ============

/// @brief 鏋勯€犻ゼ鍥剧郴鍒楋紝鏇挎崲鍩虹被榛樿绉佹湁鏁版嵁涓?TpPieSeriesPrivate
TpPieSeries::TpPieSeries()
    : TpSeries(TypePie)
{
    delete data_;
    data_ = new TpPieSeriesPrivate();
}

TpPieSeries::~TpPieSeries()
{
}

void TpPieSeries::addSlice(const TpString& name, double value, int32_t color)
{
    auto* d = static_cast<TpPieSeriesPrivate*>(data_);
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
    auto* d = static_cast<TpPieSeriesPrivate*>(data_);
    if (d)
    {
        d->m_slices.clear();
    }
}

int32_t TpPieSeries::sliceCount() const
{
    auto* d = static_cast<TpPieSeriesPrivate*>(data_);
    return d ? d->m_slices.size() : 0;
}

const TpString& TpPieSeries::sliceName(int32_t index) const
{
    static TpString emptyText;
    auto* d = static_cast<TpPieSeriesPrivate*>(data_);
    if (!d || index < 0 || index >= d->m_slices.size())
        return emptyText;
    return d->m_slices[index].name;
}

double TpPieSeries::sliceValue(int32_t index) const
{
    auto* d = static_cast<TpPieSeriesPrivate*>(data_);
    if (!d || index < 0 || index >= d->m_slices.size())
        return 0.0;
    return d->m_slices[index].value;
}

int32_t TpPieSeries::sliceColor(int32_t index) const
{
    auto* d = static_cast<TpPieSeriesPrivate*>(data_);
    if (!d || index < 0 || index >= d->m_slices.size())
        return 0;
    return d->m_slices[index].color;
}

void TpPieSeries::setSliceVisible(int32_t index, bool visible)
{
    auto* d = static_cast<TpPieSeriesPrivate*>(data_);
    if (!d || index < 0 || index >= d->m_slices.size())
        return;

    d->m_slices[index].visible = visible;
}

bool TpPieSeries::isSliceVisible(int32_t index) const
{
    auto* d = static_cast<TpPieSeriesPrivate*>(data_);
    if (!d || index < 0 || index >= d->m_slices.size())
        return false;

    return d->m_slices[index].visible;
}

void TpPieSeries::setLabelsVisible(bool visible)
{
    auto* d = static_cast<TpPieSeriesPrivate*>(data_);
    if (d)
    {
        d->m_showLabels = visible;
    }
}

void TpPieSeries::setPercentVisible(bool visible)
{
    auto* d = static_cast<TpPieSeriesPrivate*>(data_);
    if (d)
    {
        d->m_showPercent = visible;
    }
}

void TpPieSeries::setDonutVisible(bool visible)
{
    auto* d = static_cast<TpPieSeriesPrivate*>(data_);
    if (d)
    {
        d->m_donutVisible = visible;
    }
}

bool TpPieSeries::donutVisible() const
{
    auto* d = static_cast<TpPieSeriesPrivate*>(data_);
    return d ? d->m_donutVisible : false;
}

void TpPieSeries::setDonutRatio(double ratio)
{
    auto* d = static_cast<TpPieSeriesPrivate*>(data_);
    if (d)
    {
        if (ratio < 0.05)
            ratio = 0.05;
        if (ratio > 0.95)
            ratio = 0.95;
        d->m_donutRatio = ratio;
    }
}

double TpPieSeries::donutRatio() const
{
    auto* d = static_cast<TpPieSeriesPrivate*>(data_);
    return d ? d->m_donutRatio : 0.0;
}

void TpPieSeries::setStartAngle(int32_t angle)
{
    auto* d = static_cast<TpPieSeriesPrivate*>(data_);
    if (d)
    {
        d->m_startAngle = angle;
    }
}

int32_t TpPieSeries::startAngle() const
{
    auto* d = static_cast<TpPieSeriesPrivate*>(data_);
    return d ? d->m_startAngle : 0;
}

void TpPieSeries::setExplodedIndex(int32_t index)
{
    auto* d = static_cast<TpPieSeriesPrivate*>(data_);
    if (d)
    {
        d->m_explodedIndex = index;
    }
}

int32_t TpPieSeries::explodedIndex() const
{
    auto* d = static_cast<TpPieSeriesPrivate*>(data_);
    return d ? d->m_explodedIndex : -1;
}

void TpPieSeries::setExplodeDistance(int32_t distance)
{
    auto* d = static_cast<TpPieSeriesPrivate*>(data_);
    if (d)
    {
        d->m_explodeDistance = distance > 0 ? distance : 0;
    }
}

int32_t TpPieSeries::explodeDistance() const
{
    auto* d = static_cast<TpPieSeriesPrivate*>(data_);
    return d ? d->m_explodeDistance : 0;
}

void TpPieSeries::setLabelColor(int32_t color)
{
    auto* d = static_cast<TpPieSeriesPrivate*>(data_);
    if (d)
    {
        d->m_labelColor = color;
    }
}

int32_t TpPieSeries::labelColor() const
{
    auto* d = static_cast<TpPieSeriesPrivate*>(data_);
    return d ? d->m_labelColor : 0;
}

void TpPieSeries::setLabelSize(int32_t size)
{
    auto* d = static_cast<TpPieSeriesPrivate*>(data_);
    if (d)
    {
        d->m_labelSize = size > 0 ? size : 1;
    }
}

int32_t TpPieSeries::labelSize() const
{
    auto* d = static_cast<TpPieSeriesPrivate*>(data_);
    return d ? d->m_labelSize : 0;
}

/// @brief 缁樺埗楗煎浘锛氭寜鍚勬墖鍖哄崰姣斾緷娆＄粯鍒跺渾寮э紝鏀寔鐜舰銆佺垎鐐稿紡鍜屾爣绛?鐧惧垎姣旀樉绀?
void TpPieSeries::draw(TpPainter* painter, const TpAxis&, const TpAxis&, const TpRect& rect)
{
    auto* d = static_cast<TpPieSeriesPrivate*>(data_);
    if (!d || !d->m_visible || !painter || d->m_slices.size() == 0)
        return;

    // 璁＄畻鎵€鏈夊彲瑙佹墖鍖虹殑鎬诲€硷紝鐢ㄤ簬鍚庣画鐧惧垎姣旇绠?
    double totalValue = 0.0;
    for (int32_t i = 0; i < d->m_slices.size(); ++i)
    {
        const TpPieSlice& slice = d->m_slices[i];
        if (!slice.visible)
            continue;

        double value = slice.value;
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

    // 鏍规嵁缁樺埗鍖哄煙璁＄畻楗煎浘鍗婂緞鍜屽渾蹇冿紝灏忓昂瀵告椂缂╁皬杈硅窛
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
        if (!slice.visible || slice.value <= 0.0)
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

        // 鑻ュ綋鍓嶆墖鍖轰负鐖嗙偢寮忔墖鍖猴紝鍒欏皢缁樺埗涓績娌夸腑瑙掓柟鍚戝亸绉?
        int32_t drawCenterX = centerX;
        int32_t drawCenterY = centerY;
        if (d->m_explodedIndex == i && d->m_explodeDistance > 0)
        {
            drawCenterX += static_cast<int32_t>(std::cos(midRad) * d->m_explodeDistance + 0.5);
            drawCenterY += static_cast<int32_t>(std::sin(midRad) * d->m_explodeDistance + 0.5);
        }

        // 鐜舰鍥撅細鍦ㄦ墖鍖轰腑蹇冩坊鍔犲渾褰㈤晜绌洪伄缃?
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

                // 鏍囩浣嶇疆锛氱幆褰㈡椂鏀惧湪鍐呭鍗婂緞涓棿锛屽惁鍒欐斁鍦ㄦ墖鍖轰腑閮紱灏忔墖鍖烘爣绛炬斁鍒板渚?
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
