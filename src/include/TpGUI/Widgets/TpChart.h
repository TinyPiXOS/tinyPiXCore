/*
 * 鐗堟潈澹版槑 (Copyright Declaration)
 * 浣滆€?(Author)锛氬垬鏉?
 * 閭 (Email)锛?825143438@qq.com
 * 鐗堟潈鎵€鏈?(Copyright)锛毬?2026 鍒樻潹銆侫ll rights reserved.
 * 鎻忚堪 (Description)锛歍pChart 鍥捐〃鎺т欢绫?API 瀹氫箟
 */

#ifndef TPCHART_H
#define TPCHART_H

#include <TpWidget.h>
#include <TpVector.h>
#include <TpString.h>
#include "TpAxis.h"
#include "TpSeries.h"

/// @brief 鍥捐〃鎺т欢绫?
class TpChart : public TpWidget
{
public:
    /// @brief 鏋勯€犲嚱鏁?
    TpChart();

    /// @brief 鏋愭瀯鍑芥暟
    virtual ~TpChart();

public:
    /// @brief 璁剧疆鍥捐〃鏍囬
    /// @param title 鏍囬瀛楃涓?
    void setTitle(const char* title);

    /// @brief 璁剧疆鍥捐〃鑳屾櫙棰滆壊
    /// @param color 棰滆壊鍊?(ARGB 鏍煎紡)
    void setBackgroundColor(int32_t color);

    /// @brief 鑾峰彇 X 杞存寚閽?
    /// @return X 杞存寚閽?
    TpAxis* axisX();

    /// @brief 鑾峰彇 Y 杞存寚閽?
    /// @return Y 杞存寚閽?
    TpAxis* axisY();

    /// @brief 娣诲姞鏁版嵁绯诲垪
    /// @param series 鏁版嵁绯诲垪鎸囬拡
    void addSeries(TpSeries* series);

    /// @brief 绉婚櫎鎵€鏈夋暟鎹郴鍒?
    void removeAllSeries();

    /// @brief 缁樺埗浜嬩欢
    /// @param event 缁樺埗浜嬩欢鎸囬拡
    virtual bool onPaintEvent(TpPaintEvent* event) override;

    /// @brief 榧犳爣绉诲姩浜嬩欢
    /// @param event 榧犳爣浜嬩欢鎸囬拡
    virtual bool onMouseMoveEvent(TpMouseEvent* event) override;

    /// @brief 榧犳爣鎸変笅浜嬩欢
    /// @param event 榧犳爣浜嬩欢鎸囬拡
    virtual bool onMousePressEvent(TpMouseEvent* event) override;

    /// @brief 榧犳爣閲婃斁浜嬩欢
    /// @param event 榧犳爣浜嬩欢鎸囬拡
    virtual bool onMouseRleaseEvent(TpMouseEvent* event) override;

    /// @brief 婊氳疆浜嬩欢
    /// @param event 婊氳疆浜嬩欢鎸囬拡
    virtual bool onWheelEvent(TpWheelEvent* event) override;

    /// @brief 绂诲紑浜嬩欢
    /// @param event 绂诲紑浜嬩欢鎸囬拡
    virtual bool onLeaveEvent(TpLeaveEvent* event) override;

    /// @brief 鑾峰彇鎻掍欢绫诲瀷
    /// @return 鎻掍欢绫诲瀷瀛楃涓?
    virtual TpString pluginType() override { return "TpChart"; }

    /// @brief 璁剧疆鍧愭爣杞存爣绛?
    /// @param xLabel X 杞存爣绛惧瓧绗︿覆
    /// @param yLabel Y 杞存爣绛惧瓧绗︿覆
    void setAxisLabels(const char* xLabel, const char* yLabel);

    /// @brief 璁剧疆鏍峰紡琛紙CSS锛?
    /// @param styleSheet 鏍峰紡琛ㄥ瓧绗︿覆
    void setStyleSheet(const TpString& styleSheet);

    /// @brief 鑾峰彇褰撳墠鏍峰紡琛紙CSS锛?
    /// @return 鏍峰紡琛ㄥ瓧绗︿覆
    TpString styleSheet();

    /// @brief 璁剧疆 X 杞寸綉鏍肩嚎鏄惁鍙锛堝瀭鐩寸綉鏍肩嚎锛?
    /// @param visible 鏄惁鍙
    void setGridXVisible(bool visible);

    /// @brief 璁剧疆 Y 杞寸綉鏍肩嚎鏄惁鍙锛堟按骞崇綉鏍肩嚎锛?
    /// @param visible 鏄惁鍙
    void setGridYVisible(bool visible);

    /// @brief 璁剧疆缃戞牸绾块鑹?
    /// @param color 棰滆壊鍊?(ARGB 鏍煎紡)
    void setGridColor(int32_t color);

    /// @brief 璁剧疆鏄惁鏄剧ず鎻愮ず妗?
    /// @param visible 鏄惁鍙
    void setTooltipVisible(bool visible);

    /// @brief 璁剧疆鏄惁鏄剧ず鍗佸瓧绾?
    /// @param visible 鏄惁鍙
    void setCrosshairVisible(bool visible);

    /// @brief 璁剧疆鏄惁鍚敤閫変腑
    /// @param enabled 鏄惁鍚敤
    void setSelectionEnabled(bool enabled);

    /// @brief 璁剧疆鏄惁鍏佽鎷栧姩骞崇Щ
    /// @param enabled 鏄惁鍚敤
    void setPanEnabled(bool enabled);

    /// @brief 璁剧疆鏄惁鍏佽婊氳疆缂╂斁
    /// @param enabled 鏄惁鍚敤
    void setWheelZoomEnabled(bool enabled);

    /// @brief 璁剧疆鍥句緥鏄惁鍙偣鍑?
    /// @param enabled 鏄惁鍚敤
    void setLegendClickable(bool enabled);

    /// @brief 鎭㈠榛樿瑙嗗浘
    void resetView();

    /// @brief 鑾峰彇褰撳墠閫変腑绯诲垪绱㈠紩
    /// @return 绯诲垪绱㈠紩
    int32_t selectedSeriesIndex() const;

    /// @brief 鑾峰彇褰撳墠閫変腑鏁版嵁鐐圭储寮?
    /// @return 鏁版嵁鐐圭储寮?
    int32_t selectedPointIndex() const;

    /// @brief 鑾峰彇褰撳墠閫変腑鍒囩墖绱㈠紩
    /// @return 鍒囩墖绱㈠紩
    int32_t selectedSliceIndex() const;

private:
    /// @brief 璁＄畻甯冨眬鐭╁舰
    /// @param totalRect 鎬讳綋鐭╁舰
    /// @return 缁樺浘鍖虹煩褰?
    TpRect calculateLayout(const TpRect& totalRect);

    /// @brief 鏇存柊鍧愭爣杞磋寖鍥?
    void updateAxisRange();

    /// @brief 缁樺埗鑳屾櫙
    /// @param painter 鐢荤瑪鎸囬拡
    /// @param totalRect 鎬讳綋鐭╁舰
    /// @param chartRect 缁樺浘鍖虹煩褰?
    void drawBackground(TpPainter* painter, const TpRect& totalRect, const TpRect& chartRect);

    /// @brief 缁樺埗缃戞牸
    /// @param painter 鐢荤瑪鎸囬拡
    /// @param chartRect 缁樺浘鍖虹煩褰?
    void drawGrid(TpPainter* painter, const TpRect& chartRect);

    /// @brief 缁樺埗鍧愭爣杞?
    /// @param painter 鐢荤瑪鎸囬拡
    /// @param chartRect 缁樺浘鍖虹煩褰?
    void drawAxes(TpPainter* painter, const TpRect& chartRect);

    /// @brief 缁樺埗鏍囬
    /// @param painter 鐢荤瑪鎸囬拡
    /// @param totalRect 鎬讳綋鐭╁舰
    void drawTitle(TpPainter* painter, const TpRect& totalRect);

    /// @brief 缁樺埗鍥句緥
    /// @param painter 鐢荤瑪鎸囬拡
    /// @param totalRect 鎬讳綋鐭╁舰
    void drawLegend(TpPainter* painter, const TpRect& totalRect, const TpRect& chartRect);

    /// @brief 鍒ゆ柇鏄惁杩涘叆楗煎浘妯″紡
    /// @return 鏄惁涓虹函楗煎浘妯″紡
    bool isPieChartMode() const;

    /// @brief 缁樺埗楗煎浘妯″紡
    /// @param painter 缁樺埗鍣ㄦ寚閽?
    /// @param chartRect 缁樺浘鍖哄煙鐭╁舰
    void drawPieChart(TpPainter* painter, const TpRect& chartRect);

    /// @brief 缁樺埗鎻愮ず妗?
    /// @param painter 缁樺埗鍣ㄦ寚閽?
    /// @param chartRect 缁樺浘鍖哄煙鐭╁舰
    void drawTooltip(TpPainter* painter, const TpRect& chartRect);

    /// @brief 缁樺埗鍗佸瓧绾?
    /// @param painter 缁樺埗鍣ㄦ寚閽?
    /// @param chartRect 缁樺浘鍖哄煙鐭╁舰
    void drawCrosshair(TpPainter* painter, const TpRect& chartRect);

    /// @brief 缁樺埗閫変腑鏁堟灉
    /// @param painter 缁樺埗鍣ㄦ寚閽?
    /// @param chartRect 缁樺浘鍖哄煙鐭╁舰
    void drawSelection(TpPainter* painter, const TpRect& chartRect);

    /// @brief 鏋勫缓鍥句緥鏁版嵁
    /// @param totalRect 鎬诲尯鍩熺煩褰?
    /// @param chartRect 缁樺浘鍖哄煙鐭╁舰
    /// @param names 鍥句緥鍚嶇О
    /// @param colors 鍥句緥棰滆壊
    /// @param endColors 鍥句緥缁撴潫棰滆壊
    /// @param types 鍥句緥绫诲瀷
    void buildLegendData(const TpRect& totalRect, const TpRect& chartRect, TpVector<const char*>& names,
                         TpVector<int32_t>& colors, TpVector<int32_t>& endColors, TpVector<int32_t>& types);

    /// @brief 璁＄畻褰撳墠甯冨眬鐘舵€?
    /// @param totalRect 鎬诲尯鍩熺煩褰?
    /// @param chartRect 缁樺浘鍖哄煙鐭╁舰
    /// @param showTitleAndLegend 鏄惁鏄剧ず鏍囬鍜屽浘渚?
    /// @param showAxisLabels 鏄惁鏄剧ず鍧愭爣鏍囩
    /// @param showAxisTicks 鏄惁鏄剧ず鍧愭爣鍒诲害
    /// @param pieChartMode 鏄惁涓洪ゼ鍥炬ā寮?
    /// @param scale 缂╂斁绯绘暟
    void prepareLayoutState(const TpRect& totalRect, TpRect& chartRect, bool& showTitleAndLegend, bool& showAxisLabels,
                            bool& showAxisTicks, bool& pieChartMode, double& scale);

    /// @brief 鍛戒腑娴嬭瘯
    /// @param chartRect 缁樺浘鍖哄煙鐭╁舰
    /// @param pos 榧犳爣浣嶇疆
    /// @param pieMode 鏄惁涓洪ゼ鍥炬ā寮?
    /// @param seriesIndex 鍛戒腑绯诲垪绱㈠紩
    /// @param pointIndex 鍛戒腑鏁版嵁鐐圭储寮?
    /// @param sliceIndex 鍛戒腑鍒囩墖绱㈠紩
    /// @param text 鎻愮ず鏂囨湰
    /// @param hitPos 鍛戒腑浣嶇疆
    /// @return 鏄惁鍛戒腑
    bool hitTestAt(const TpRect& chartRect, const TpPoint& pos, bool pieMode, int32_t& seriesIndex, int32_t& pointIndex,
                   int32_t& sliceIndex, TpString& text, TpPoint& hitPos);

    void clearHoverState();

    /// @brief 鎸夊綋鍓嶄綅缃缉鏀惧潗鏍囪酱
    /// @param pos 榧犳爣浣嶇疆
    /// @param chartRect 缁樺浘鍖哄煙鐭╁舰
    /// @param factor 缂╂斁鍥犲瓙
    void zoomAxisAt(const TpPoint& pos, const TpRect& chartRect, double factor);

    /// @brief 鎸夊綋鍓嶄綅缃钩绉诲潗鏍囪酱
    /// @param pos 榧犳爣浣嶇疆
    /// @param chartRect 缁樺浘鍖哄煙鐭╁舰
    void panAxisTo(const TpPoint& pos, const TpRect& chartRect);

    /// @brief 澶勭悊鍥句緥鐐瑰嚮
    /// @param pos 榧犳爣浣嶇疆
    /// @param totalRect 鎬诲尯鍩熺煩褰?
    /// @param chartRect 缁樺浘鍖哄煙鐭╁舰
    bool toggleLegendAt(const TpPoint& pos, const TpRect& totalRect, const TpRect& chartRect);

    /// @brief 鑾峰彇褰撳墠鐘舵€佺殑CSS鏁版嵁
    tpShared<TpCssData> currentStatusCss();

    /// @brief 鍒锋柊鍩虹CSS鏍峰紡
    void refreshBaseCss();

private:
    /// @brief 绉佹湁瀹炵幇缁撴瀯浣?
    struct Impl;
    Impl* m_impl;

    /// @brief CSS鏁版嵁 - 鍚敤鐘舵€?
    tpShared<TpCssData> enabledCssData;

    /// @brief CSS鏁版嵁 - 绂佺敤鐘舵€?
    tpShared<TpCssData> disabledCssData;

    /// @brief CSS鏁版嵁 - 鎮仠鐘舵€?
    tpShared<TpCssData> hoverCssData;

    /// @brief CSS鏁版嵁 - 鎸変笅鐘舵€?
    tpShared<TpCssData> pressCssData;

    /// @brief CSS鏁版嵁 - 閫変腑鐘舵€?
    tpShared<TpCssData> checkedCssData;
};

#endif // TPCHART_H
