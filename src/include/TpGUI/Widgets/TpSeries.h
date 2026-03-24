/*
 * 版权声明 (Copyright Declaration)
 * 作者 (Author)：张家庆
 * 邮箱 (Email)：1494197384@qq.com
 * 版权所有 (Copyright)：© 2026 张家庆。All rights reserved.
 * 描述 (Description)：图表系列基类及其派生类的 API 定义
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

/// @brief 数据点结构体
struct TpDataPoint
{
    double x;     ///< @brief X 坐标值
    double y;     ///< @brief Y 坐标值

    /// @brief 默认构造函数
    TpDataPoint()
        : x(0)
        , y(0) {}

    /// @brief 带参数的构造函数
    /// @param _x X 坐标值
    /// @param _y Y 坐标值
    TpDataPoint(double _x, double _y)
        : x(_x)
        , y(_y) {}
};

/// @brief 前向声明
class TpSeriesPrivate;

/// @brief 图表系列基类，所有图表类型的父类
class TpSeries
{
public:
    /// @brief 系列类型枚举
    enum SeriesType
    {
        TypeLine,   ///< @brief 折线图
        TypeBar     ///< @brief 柱状图
    };

public:
    /// @brief 构造函数
    /// @param type 系列类型
    TpSeries(SeriesType type);

    /// @brief 析构函数
    virtual ~TpSeries();

public:
    /// @brief 设置系列名称
    /// @param name 名称字符串
    void setName(const TpString& name);

    /// @brief 设置系列名称（兼容 C 字符串）
    /// @param name 名称字符串
    void setName(const char* name);

    /// @brief 获取系列名称
    /// @return 名称字符串引用
    const TpString& name() const;

    /// @brief 设置是否可见
    /// @param visible 可见状态，true=可见，false=隐藏
    void setVisible(bool visible);

    /// @brief 获取可见状态
    /// @return 可见状态，true=可见，false=隐藏
    bool isVisible() const;

    /// @brief 设置系列颜色
    /// @param color 颜色值（ARGB 格式）
    void setColor(int32_t color);

    /// @brief 获取系列颜色
    /// @return 颜色值
    int32_t color() const;

    /// @brief 添加数据点
    /// @param x X 坐标值
    /// @param y Y 坐标值
    void addPoint(double x, double y);

    /// @brief 设置最大数据点个数（用于滑动窗口，如心电图）
    /// @param count 最大点数，设为 0 表示不限制
    void setMaxPointCount(int32_t count);

    /// @brief 清除所有数据点
    void clear();

    /// @brief 获取数据点列表
    /// @return 数据点向量引用
    const TpVector<TpDataPoint>& data() const;

    /// @brief 获取系列类型
    /// @return 系列类型枚举值
    SeriesType type() const;

    /// @brief 从CSS获取颜色值
    /// @param className 类名
    /// @param status 状态
    void applyCssData(const TpString& className, TpCssParser::MouseStatus status);

    /// @brief 绘制图表系列（纯虚函数）
    /// @param painter 绘制器对象
    /// @param axisX X 轴对象
    /// @param axisY Y 轴对象
    /// @param rect 绘制区域矩形
    virtual void draw(TpPainter* painter, const TpAxis& axisX, const TpAxis& axisY, const TpRect& rect) = 0;

protected:
    TpSeriesPrivate* d_ptr;   ///< @brief Pimpl 模式私有数据指针
};

/// @brief 折线图系列类
class TpLineSeries : public TpSeries
{
public:
    /// @brief 构造函数
    TpLineSeries();

    /// @brief 析构函数
    virtual ~TpLineSeries();

    /// @brief 设置线宽
    /// @param width 线宽像素值
    void setLineWidth(int32_t width);

    /// @brief 设置是否启用平滑曲线
    /// @param smooth 平滑状态，true=平滑曲线，false=折线
    void setSmooth(bool smooth);

    /// @brief 设置曲线张力（平滑度）
    /// @param tension 张力值，范围 0-1，默认 0.5
    void setTension(float tension);

    /// @brief 设置是否启用降采样
    /// @param enabled 降采样状态，true=启用降采样，false=绘制所有点
    void setUseDownsample(bool enabled);

    /// @brief 获取降采样启用状态
    /// @return 降采样状态，true=已启用，false=未启用
    bool useDownsample() const;

    /// @brief 绘制折线图
    /// @param painter 绘制器对象
    /// @param axisX X 轴对象
    /// @param axisY Y 轴对象
    /// @param rect 绘制区域矩形
    virtual void draw(TpPainter* painter, const TpAxis& axisX, const TpAxis& axisY, const TpRect& rect) override;
};

/// @brief 柱状图系列类
class TpBarSeries : public TpSeries
{
public:
    /// @brief 构造函数
    TpBarSeries();

    /// @brief 析构函数
    virtual ~TpBarSeries();

    /// @brief 设置渐变结束颜色
    /// @param endColor 结束颜色值
    void setGradientColor(int32_t endColor);

    /// @brief 获取渐变结束颜色
    /// @return 结束颜色值
    int32_t colorEnd() const;

    /// @brief 设置布局信息（用于多系列分组）
    /// @param seriesIndex 当前系列索引
    /// @param seriesCount 系列总数
    void setLayoutInfo(int32_t seriesIndex, int32_t seriesCount);

    /// @brief 设置是否显示数据标签
    /// @param visible 标签可见状态，true=显示，false=隐藏
    void setLabelsVisible(bool visible);

    /// @brief 设置数据标签颜色
    /// @param color 标签颜色值
    void setLabelColor(int32_t color);

    /// @brief 设置数据标签字体大小
    /// @param size 字体大小
    void setLabelSize(int32_t size);

    /// @brief 绘制柱状图
    /// @param painter 绘制器对象
    /// @param axisX X 轴对象
    /// @param axisY Y 轴对象
    /// @param rect 绘制区域矩形
    virtual void draw(TpPainter* painter, const TpAxis& axisX, const TpAxis& axisY, const TpRect& rect) override;
};

#endif // TP_SERIES_H
