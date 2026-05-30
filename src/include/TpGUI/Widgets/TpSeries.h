/*
 * 鐗堟潈澹版槑 (Copyright Declaration)
 * 浣滆€?(Author)锛氬紶瀹跺簡
 * 閭 (Email)锛?494197384@qq.com
 * 鐗堟潈鎵€鏈?(Copyright)锛毬?2026 寮犲搴嗐€侫ll rights reserved.
 * 鎻忚堪 (Description)锛氬浘琛ㄧ郴鍒楀熀绫诲強鍏舵淳鐢熺被鐨?API 瀹氫箟
 */

#ifndef TP_SERIES_H
#define TP_SERIES_H

#include <stdint.h>
#include <TpVector.h>
#include <TpString.h>
#include <TpPoint.h>
#include <TpFont.h>
#include <TpCssParser.h>
#include <TpCssData.h>
#include "TpAxis.h"
#include <TpPainter.h>

/// @brief 鏁版嵁鐐圭粨鏋勪綋
struct TpDataPoint
{
    double x;     ///< @brief X 鍧愭爣鍊?
    double y;     ///< @brief Y 鍧愭爣鍊?
    /// @brief 榛樿鏋勯€犲嚱鏁?
    TpDataPoint()
        : x(0)
        , y(0) {}

    /// @brief 甯﹀弬鏁扮殑鏋勯€犲嚱鏁?
    /// @param _x X 鍧愭爣鍊?
    /// @param _y Y 鍧愭爣鍊?
    TpDataPoint(double _x, double _y)
        : x(_x)
        , y(_y) {}
};

/// @brief 楗煎浘鍒囩墖鏁版嵁缁撴瀯
struct TpPieSlice
{
    TpString name;   ///< @brief 鍒囩墖鍚嶇О
    double value;    ///< @brief 鍒囩墖鏁板€?
    int32_t color;   ///< @brief 鍒囩墖棰滆壊
    bool visible;    ///< @brief 鍒囩墖鍙鐘舵€?
    /// @brief 榛樿鏋勯€犲嚱鏁?
    TpPieSlice()
        : value(0)
        , color(0)
        , visible(true) {}

    /// @brief 甯﹀弬鏁扮殑鏋勯€犲嚱鏁?
    /// @param _name 鍒囩墖鍚嶇О
    /// @param _value 鍒囩墖鏁板€?
    /// @param _color 鍒囩墖棰滆壊
    TpPieSlice(const TpString& _name, double _value, int32_t _color)
        : name(_name)
        , value(_value)
        , color(_color)
        , visible(true) {}
};

/// @brief 鍓嶅悜澹版槑
class TpSeriesPrivate;

/// @brief 鍥捐〃绯诲垪鍩虹被锛屾墍鏈夊浘琛ㄧ被鍨嬬殑鐖剁被
class TpSeries
{
public:
    /// @brief 绯诲垪绫诲瀷鏋氫妇
    enum SeriesType
    {
        TypeLine,    ///< @brief 鎶樼嚎鍥?
        TypeBar,     ///< @brief 鏌辩姸鍥?
        TypeScatter, ///< @brief 鏁ｇ偣鍥?
        TypePie      ///< @brief 楗煎浘
    };

public:
    /// @brief 鏋勯€犲嚱鏁?
    /// @param type 绯诲垪绫诲瀷
    TpSeries(SeriesType type);

    /// @brief 鏋愭瀯鍑芥暟
    virtual ~TpSeries();

public:
    /// @brief 璁剧疆绯诲垪鍚嶇О
    /// @param name 鍚嶇О瀛楃涓?
    void setName(const TpString& name);

    /// @brief 璁剧疆绯诲垪鍚嶇О锛堝吋瀹?C 瀛楃涓诧級
    /// @param name 鍚嶇О瀛楃涓?
    void setName(const char* name);

    /// @brief 鑾峰彇绯诲垪鍚嶇О
    /// @return 鍚嶇О瀛楃涓插紩鐢?
    const TpString& name() const;

    /// @brief 璁剧疆鏄惁鍙
    /// @param visible 鍙鐘舵€侊紝true=鍙锛宖alse=闅愯棌
    void setVisible(bool visible);

    /// @brief 鑾峰彇鍙鐘舵€?
    /// @return 鍙鐘舵€侊紝true=鍙锛宖alse=闅愯棌
    bool isVisible() const;

    /// @brief 璁剧疆绯诲垪棰滆壊
    /// @param color 棰滆壊鍊硷紙ARGB 鏍煎紡锛?
    void setColor(int32_t color);

    /// @brief 鑾峰彇绯诲垪棰滆壊
    /// @return 棰滆壊鍊?
    int32_t color() const;

    /// @brief 娣诲姞鏁版嵁鐐?
    /// @param x X 鍧愭爣鍊?
    /// @param y Y 鍧愭爣鍊?
    void addPoint(double x, double y);

    /// @brief 璁剧疆鏈€澶ф暟鎹偣涓暟锛堢敤浜庢粦鍔ㄧ獥鍙ｏ紝濡傚績鐢靛浘锛?
    /// @param count 鏈€澶х偣鏁帮紝璁句负 0 琛ㄧず涓嶉檺鍒?
    void setMaxPointCount(int32_t count);

    /// @brief 娓呴櫎鎵€鏈夋暟鎹偣
    void clear();

    /// @brief 鑾峰彇鏁版嵁鐐瑰垪琛?
    /// @return 鏁版嵁鐐瑰悜閲忓紩鐢?
    const TpVector<TpDataPoint>& data() const;

    /// @brief 鑾峰彇绯诲垪绫诲瀷
    /// @return 绯诲垪绫诲瀷鏋氫妇鍊?
    SeriesType type() const;

    /// @brief 浠嶤SS鑾峰彇棰滆壊鍊?
    /// @param className 绫诲悕
    /// @param status 鐘舵€?
    void applyCssData(const TpString& className, TpCssParser::MouseStatus status);

    /// @brief 缁樺埗鍥捐〃绯诲垪锛堢函铏氬嚱鏁帮級
    /// @param painter 缁樺埗鍣ㄥ璞?
    /// @param axisX X 杞村璞?
    /// @param axisY Y 杞村璞?
    /// @param rect 缁樺埗鍖哄煙鐭╁舰
    virtual void draw(TpPainter* painter, const TpAxis& axisX, const TpAxis& axisY, const TpRect& rect) = 0;

protected:
    TpSeriesPrivate* d_ptr;   ///< @brief Pimpl 妯″紡绉佹湁鏁版嵁鎸囬拡
};

/// @brief 鎶樼嚎鍥剧郴鍒楃被
class TpLineSeries : public TpSeries
{
public:
    /// @brief 鏋勯€犲嚱鏁?
    TpLineSeries();

    /// @brief 鏋愭瀯鍑芥暟
    virtual ~TpLineSeries();

    /// @brief 璁剧疆绾垮
    /// @param width 绾垮鍍忕礌鍊?
    void setLineWidth(int32_t width);

    /// @brief 璁剧疆鏄惁鍚敤骞虫粦鏇茬嚎
    /// @param smooth 骞虫粦鐘舵€侊紝true=骞虫粦鏇茬嚎锛宖alse=鎶樼嚎
    void setSmooth(bool smooth);

    /// @brief 璁剧疆鏇茬嚎寮犲姏锛堝钩婊戝害锛?
    /// @param tension 寮犲姏鍊硷紝鑼冨洿 0-1锛岄粯璁?0.5
    void setTension(float tension);

    /// @brief 璁剧疆鏄惁鍚敤闄嶉噰鏍?
    /// @param enabled 闄嶉噰鏍风姸鎬侊紝true=鍚敤闄嶉噰鏍凤紝false=缁樺埗鎵€鏈夌偣
    void setUseDownsample(bool enabled);

    /// @brief 鑾峰彇闄嶉噰鏍峰惎鐢ㄧ姸鎬?
    /// @return 闄嶉噰鏍风姸鎬侊紝true=宸插惎鐢紝false=鏈惎鐢?
    bool useDownsample() const;

    /// @brief 缁樺埗鎶樼嚎鍥?
    /// @param painter 缁樺埗鍣ㄥ璞?
    /// @param axisX X 杞村璞?
    /// @param axisY Y 杞村璞?
    /// @param rect 缁樺埗鍖哄煙鐭╁舰
    virtual void draw(TpPainter* painter, const TpAxis& axisX, const TpAxis& axisY, const TpRect& rect) override;
};

/// @brief 鏌辩姸鍥剧郴鍒楃被
class TpBarSeries : public TpSeries
{
public:
    /// @brief 鏋勯€犲嚱鏁?
    TpBarSeries();

    /// @brief 鏋愭瀯鍑芥暟
    virtual ~TpBarSeries();

    /// @brief 璁剧疆娓愬彉缁撴潫棰滆壊
    /// @param endColor 缁撴潫棰滆壊鍊?
    void setGradientColor(int32_t endColor);

    /// @brief 鑾峰彇娓愬彉缁撴潫棰滆壊
    /// @return 缁撴潫棰滆壊鍊?
    int32_t colorEnd() const;

    /// @brief 璁剧疆甯冨眬淇℃伅锛堢敤浜庡绯诲垪鍒嗙粍锛?
    /// @param seriesIndex 褰撳墠绯诲垪绱㈠紩
    /// @param seriesCount 绯诲垪鎬绘暟
    void setLayoutInfo(int32_t seriesIndex, int32_t seriesCount);

    /// @brief 璁剧疆鏄惁鏄剧ず鏁版嵁鏍囩
    /// @param visible 鏍囩鍙鐘舵€侊紝true=鏄剧ず锛宖alse=闅愯棌
    void setLabelsVisible(bool visible);

    /// @brief 璁剧疆鏁版嵁鏍囩棰滆壊
    /// @param color 鏍囩棰滆壊鍊?
    void setLabelColor(int32_t color);

    /// @brief 璁剧疆鏁版嵁鏍囩瀛椾綋澶у皬
    /// @param size 瀛椾綋澶у皬
    void setLabelSize(int32_t size);

    /// @brief 缁樺埗鏌辩姸鍥?
    /// @param painter 缁樺埗鍣ㄥ璞?
    /// @param axisX X 杞村璞?
    /// @param axisY Y 杞村璞?
    /// @param rect 缁樺埗鍖哄煙鐭╁舰
    virtual void draw(TpPainter* painter, const TpAxis& axisX, const TpAxis& axisY, const TpRect& rect) override;
};

/// @brief 鏁ｇ偣鍥剧郴鍒楃被
class TpScatterSeries : public TpSeries
{
public:
    /// @brief 鏋勯€犲嚱鏁?
    TpScatterSeries();

    /// @brief 鏋愭瀯鍑芥暟
    virtual ~TpScatterSeries();

    /// @brief 璁剧疆鐐瑰ぇ灏?
    /// @param size 鐐瑰崐寰勫儚绱犲€?
    void setPointSize(int32_t size);

    /// @brief 鑾峰彇鐐瑰ぇ灏?
    /// @return 鐐瑰崐寰勫儚绱犲€?
    int32_t pointSize() const;

    /// @brief 璁剧疆杈规棰滆壊
    /// @param color 杈规棰滆壊鍊?
    void setBorderColor(int32_t color);

    /// @brief 鑾峰彇杈规棰滆壊
    /// @return 杈规棰滆壊鍊?
    int32_t borderColor() const;

    /// @brief 璁剧疆鏄惁鏄剧ず鏍囩
    /// @param visible 鏍囩鍙鐘舵€?
    void setLabelsVisible(bool visible);

    /// @brief 璁剧疆鏍囩棰滆壊
    /// @param color 鏍囩棰滆壊鍊?
    void setLabelColor(int32_t color);

    /// @brief 璁剧疆鏍囩澶у皬
    /// @param size 瀛椾綋澶у皬
    void setLabelSize(int32_t size);

    /// @brief 缁樺埗鏁ｇ偣鍥?
    /// @param painter 缁樺埗鍣ㄥ璞?
    /// @param axisX X 杞村璞?
    /// @param axisY Y 杞村璞?
    /// @param rect 缁樺埗鍖哄煙鐭╁舰
    virtual void draw(TpPainter* painter, const TpAxis& axisX, const TpAxis& axisY, const TpRect& rect) override;
};

/// @brief 楗煎浘绯诲垪绫?
class TpPieSeries : public TpSeries
{
public:
    /// @brief 鏋勯€犲嚱鏁?
    TpPieSeries();

    /// @brief 鏋愭瀯鍑芥暟
    virtual ~TpPieSeries();

    /// @brief 娣诲姞鍒囩墖
    /// @param name 鍒囩墖鍚嶇О
    /// @param value 鍒囩墖鏁板€?
    /// @param color 鍒囩墖棰滆壊
    void addSlice(const TpString& name, double value, int32_t color);

    /// @brief 娣诲姞鍒囩墖
    /// @param name 鍒囩墖鍚嶇О
    /// @param value 鍒囩墖鏁板€?
    /// @param color 鍒囩墖棰滆壊
    void addSlice(const char* name, double value, int32_t color);

    /// @brief 娓呴櫎鎵€鏈夊垏鐗?
    void clearSlices();

    /// @brief 鑾峰彇鍒囩墖鏁伴噺
    /// @return 鍒囩墖鏁伴噺
    int32_t sliceCount() const;

    /// @brief 鑾峰彇鍒囩墖鍚嶇О
    /// @param index 鍒囩墖绱㈠紩
    /// @return 鍒囩墖鍚嶇О
    const TpString& sliceName(int32_t index) const;

    /// @brief 鑾峰彇鍒囩墖鏁板€?
    /// @param index 鍒囩墖绱㈠紩
    /// @return 鍒囩墖鏁板€?
    double sliceValue(int32_t index) const;

    /// @brief 鑾峰彇鍒囩墖棰滆壊
    /// @param index 鍒囩墖绱㈠紩
    /// @return 鍒囩墖棰滆壊鍊?
    int32_t sliceColor(int32_t index) const;

    /// @brief 璁剧疆鍒囩墖鏄惁鍙
    /// @param index 鍒囩墖绱㈠紩
    /// @param visible 鍙鐘舵€?
    void setSliceVisible(int32_t index, bool visible);

    /// @brief 鑾峰彇鍒囩墖鏄惁鍙
    /// @param index 鍒囩墖绱㈠紩
    /// @return 鍒囩墖鍙鐘舵€?
    bool isSliceVisible(int32_t index) const;

    /// @brief 璁剧疆鏄惁鏄剧ず鏍囩
    /// @param visible 鏍囩鍙鐘舵€?
    void setLabelsVisible(bool visible);

    /// @brief 璁剧疆鏄惁鏄剧ず鐧惧垎姣?
    /// @param visible 鐧惧垎姣斿彲瑙佺姸鎬?
    void setPercentVisible(bool visible);

    /// @brief 璁剧疆鏄惁鏄剧ず鐜舰鍥?
    /// @param visible 鐜舰鍥惧彲瑙佺姸鎬?
    void setDonutVisible(bool visible);

    /// @brief 鑾峰彇鏄惁鏄剧ず鐜舰鍥?
    /// @return 鐜舰鍥惧彲瑙佺姸鎬?
    bool donutVisible() const;

    /// @brief 璁剧疆鐜舰鍥惧唴鍗婂緞姣斾緥
    /// @param ratio 鍐呭鍗婂緞姣斾緥
    void setDonutRatio(double ratio);

    /// @brief 鑾峰彇鐜舰鍥惧唴鍗婂緞姣斾緥
    /// @return 鍐呭鍗婂緞姣斾緥
    double donutRatio() const;

    /// @brief 璁剧疆璧峰瑙掑害
    /// @param angle 璧峰瑙掑害
    void setStartAngle(int32_t angle);

    /// @brief 鑾峰彇璧峰瑙掑害
    /// @return 璧峰瑙掑害
    int32_t startAngle() const;

    /// @brief 璁剧疆绐佸嚭鍒囩墖绱㈠紩
    /// @param index 鍒囩墖绱㈠紩
    void setExplodedIndex(int32_t index);

    /// @brief 鑾峰彇绐佸嚭鍒囩墖绱㈠紩
    /// @return 鍒囩墖绱㈠紩
    int32_t explodedIndex() const;

    /// @brief 璁剧疆绐佸嚭璺濈
    /// @param distance 绐佸嚭璺濈鍍忕礌鍊?
    void setExplodeDistance(int32_t distance);

    /// @brief 鑾峰彇绐佸嚭璺濈
    /// @return 绐佸嚭璺濈鍍忕礌鍊?
    int32_t explodeDistance() const;

    /// @brief 璁剧疆鏍囩棰滆壊
    /// @param color 鏍囩棰滆壊鍊?
    void setLabelColor(int32_t color);

    /// @brief 鑾峰彇鏍囩棰滆壊
    /// @return 鏍囩棰滆壊鍊?
    int32_t labelColor() const;

    /// @brief 璁剧疆鏍囩澶у皬
    /// @param size 瀛椾綋澶у皬
    void setLabelSize(int32_t size);

    /// @brief 鑾峰彇鏍囩澶у皬
    /// @return 瀛椾綋澶у皬
    int32_t labelSize() const;

    /// @brief 缁樺埗楗煎浘
    /// @param painter 缁樺埗鍣ㄥ璞?
    /// @param axisX X 杞村璞?
    /// @param axisY Y 杞村璞?
    /// @param rect 缁樺埗鍖哄煙鐭╁舰
    virtual void draw(TpPainter* painter, const TpAxis& axisX, const TpAxis& axisY, const TpRect& rect) override;
};

#endif // TP_SERIES_H
