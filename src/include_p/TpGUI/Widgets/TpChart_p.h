#ifndef __TP_CHART_PRIVATE_H
#define __TP_CHART_PRIVATE_H

#include "TpChart.h"
#include "TpWidget_p.h"

struct LegendItem
{
    TpString text;
    int32_t colorStart = 0;
    int32_t colorEnd = 0;
    int32_t type = 0;
    bool visible = true;
    int32_t seriesIndex = -1;
    int32_t sliceIndex = -1;
    TpRect rect;
};

struct TpChart::Impl
{
    TpString title;
    TpString labelX;
    TpString labelY;

    int32_t backgroundColor = 0xFFFFFFFF;
    int32_t marginTop = 60;
    int32_t marginBottom = 60;
    int32_t marginLeft = 60;
    int32_t marginRight = 20;

    bool gridXVisible = true;
    bool gridYVisible = true;
    int32_t gridColor = 0xFFE6E6E6;

    bool tooltipVisible = false;
    bool crosshairVisible = false;
    bool selectionEnabled = false;
    bool panEnabled = false;
    bool wheelZoomEnabled = false;
    bool legendClickable = false;

    TpAxis* axisX = nullptr;
    TpAxis* axisY = nullptr;
    TpVector<TpSeries*> seriesList;
    TpVector<LegendItem> legendItems;

    TpPoint pointerPos;
    TpPoint hoverPos;
    TpPoint pressPos;
    TpPoint dragStartPos;

    bool hasPointerPos = false;
    bool hasHover = false;
    bool mousePressed = false;
    bool isDragging = false;

    double dragStartXMin = 0.0;
    double dragStartXMax = 0.0;
    double dragStartYMin = 0.0;
    double dragStartYMax = 0.0;

    int32_t selectedSeriesIndex = -1;
    int32_t selectedPointIndex = -1;
    int32_t selectedSliceIndex = -1;
    int32_t hoverSeriesIndex = -1;
    int32_t hoverPointIndex = -1;
    int32_t hoverSliceIndex = -1;

    TpString tooltipText;

    tpShared<TpCssData> enabledCssData;
    tpShared<TpCssData> disabledCssData;
    tpShared<TpCssData> hoverCssData;
    tpShared<TpCssData> pressCssData;
    tpShared<TpCssData> checkedCssData;
};

#endif
