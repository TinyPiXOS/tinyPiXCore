/*
 * 版权声明 (Copyright Declaration)
 * 作者 (Author)：刘可扬
 * 邮箱 (Email)：2226544626@qq.com
 * 版权所有 (Copyright)：© 2026 刘可扬。All rights reserved.
 * 描述 (Description)：图表渲染工具类 API 定义，提供核心绘图功能的静态工具类
 */

#ifndef TPRENDERUTILS_H
#define TPRENDERUTILS_H

#include <TpPainter.h>
#include <TpVector.h>
#include <TpRect.h>
#include <TpPoint.h>
#include "TpAxis.h"
#include <TpString.h>
#include <TpFont.h>

/// @brief 图表渲染工具类，提供静态绘图辅助方法
class TpRenderUtils
{
private:
    /// @brief 禁止实例化
    TpRenderUtils();

    /// @brief 禁止析构
    ~TpRenderUtils();

public:
    /// @brief 图例类型枚举
    enum LegendType
    {
        TypeLine = 0,   ///< @brief 折线图例
        TypeBar = 1     ///< @brief 柱状图例
    };

    /// @brief 填充渐变矩形
    /// @param painter 绘图对象
    /// @param rect 绘制区域
    /// @param colorStart 渐变起始颜色
    /// @param colorEnd 渐变结束颜色
    static void fillGradientRect(TpPainter* painter, const TpRect& rect, uint32_t colorStart, uint32_t colorEnd);

    /// @brief 绘制网格
    /// @param painter 绘图对象
    /// @param rect 绘制区域
    /// @param rows 网格行数
    /// @param cols 网格列数
    /// @param gridColor 网格颜色
    static void drawGrid(TpPainter* painter, const TpRect& rect, int32_t rows, int32_t cols, uint32_t gridColor);

    /// @brief 裁剪线段绘制
    /// @param painter 绘图对象
    /// @param x0 起始点 x 坐标
    /// @param y0 起始点 y 坐标
    /// @param x1 结束点 x 坐标
    /// @param y1 结束点 y 坐标
    /// @param clipRect 裁剪区域
    static void drawClippedLine(TpPainter* painter, int32_t x0, int32_t y0, int32_t x1, int32_t y1, const TpRect& clipRect);

    /// @brief 绘制渐变柱状图
    /// @param painter 绘图对象
    /// @param barRect 柱状图区域
    /// @param colorStart 渐变起始颜色
    /// @param colorEnd 渐变结束颜色
    static void drawGradientBar(TpPainter* painter, const TpRect& barRect, uint32_t colorStart, uint32_t colorEnd);

    /// @brief 绘制标准折线
    /// @param painter 绘图对象
    /// @param points 点集
    /// @param clipRect 裁剪区域
    /// @param color 线条颜色
    /// @param lineWidth 线条宽度
    static void drawPolyline(TpPainter* painter, const TpVector<TpPoint>& points, const TpRect& clipRect, int32_t color, int32_t lineWidth);

    /// @brief 绘制平滑曲线
    /// @param painter 绘图对象
    /// @param points 点集
    /// @param clipRect 裁剪区域
    /// @param color 线条颜色
    /// @param lineWidth 线条宽度
    /// @param tension 张力参数，默认 0.4f
    static void drawSmoothCurve(TpPainter* painter, const TpVector<TpPoint>& points, const TpRect& clipRect, int32_t color, int32_t lineWidth, float tension = 0.4f);

    /// @brief 绘制锚点
    /// @param painter 绘图对象
    /// @param center 中心点坐标
    /// @param radius 半径
    /// @param borderColor 边框颜色
    /// @param fillColor 填充颜色
    static void drawAnchorPoint(TpPainter* painter, const TpPoint& center, int32_t radius, int32_t borderColor, int32_t fillColor);

    /// @brief 绘制 X 轴
    /// @param painter 绘图对象
    /// @param rect 绘制区域
    /// @param axisX X 轴对象
    /// @param axisY Y 轴对象（用于确定零点位置）
    /// @param color 轴颜色
    /// @param drawGrid 是否绘制网格线
    /// @param gridColor 网格线颜色
    static void drawAxisX(TpPainter* painter, const TpRect& rect, const TpAxis& axisX, const TpAxis& axisY, uint32_t color, bool drawGrid, uint32_t gridColor);

    /// @brief 绘制 Y 轴
    /// @param painter 绘图对象
    /// @param rect 绘制区域
    /// @param axisY Y 轴对象
    /// @param axisX X 轴对象（用于确定零点位置）
    /// @param color 轴颜色
    /// @param drawGrid 是否绘制网格线
    /// @param gridColor 网格线颜色
    static void drawAxisY(TpPainter* painter, const TpRect& rect, const TpAxis& axisY, const TpAxis& axisX, uint32_t color, bool drawGrid, uint32_t gridColor);

    /// @brief 绘制图例
    /// @param painter 绘图对象
    /// @param totalRect 总区域
    /// @param chartRect 图表区域
    /// @param names 图例名称列表
    /// @param colors 图例颜色列表
    /// @param endColors 图例结束颜色列表（用于渐变）
    /// @param types 图例类型列表
    static void drawLegendOutside(TpPainter* painter, const TpRect& totalRect, const TpRect& chartRect,
                                   const TpVector<const char*>& names, const TpVector<int32_t>& colors,
                                   const TpVector<int32_t>& endColors, const TpVector<int32_t>& types);
};

#endif // TPRENDERUTILS_H
