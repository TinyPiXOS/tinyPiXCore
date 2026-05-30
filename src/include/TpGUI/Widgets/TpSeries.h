/*
 * 版权声明 (Copyright Declaration)
 * 作者: 刘杨
 * 邮箱: 825143438@qq.com
 * 版权所有: 2026 刘杨. All rights reserved.
 * 描述: TpSeries 数据系列类 API 定义
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

/// @brief 饼图切片数据结构
struct TpPieSlice
{
    TpString name;   ///< @brief 切片名称
    double value;    ///< @brief 切片数值
    int32_t color;   ///< @brief 切片颜色
    bool visible;    ///< @brief 切片可见状态
    /// @brief 默认构造函数
    TpPieSlice()
        : value(0)
        , color(0)
        , visible(true) {}

    /// @brief 带参数的构造函数
    /// @param _name 切片名称
    /// @param _value 切片数值
    /// @param _color 切片颜色
    TpPieSlice(const TpString& _name, double _value, int32_t _color)
        : name(_name)
        , value(_value)
        , color(_color)
        , visible(true) {}
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
        TypeLine,    ///< @brief 折线图
        TypeBar,     ///< @brief 柱状图
        TypeScatter, ///< @brief 散点图
        TypePie      ///< @brief 饼图
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

    /// @brief 设置最大数据点数量（用于滑动窗口，例如心电图）
    /// @param count 最大点数，设为 0 表示不限制
    void setMaxPointCount(int32_t count);

    /// @brief 清除所有数据点
    void clear();

    /// @brief 获取数据点列表
    /// @return 数据点向量引用
    const TpVector<TpDataPoint>& data() const;

    /// @brief 获取系列绫诲瀷
    /// @return 系列类型枚举值
    SeriesType type() const;

    /// @brief 从 CSS 获取颜色值
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
    TpSeriesPrivate* d_ptr;   ///< @brief Pimpl 模式绉佹湁数据指针
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

/// @brief 散点图系列类
class TpScatterSeries : public TpSeries
{
public:
    /// @brief 构造函数
    TpScatterSeries();

    /// @brief 析构函数
    virtual ~TpScatterSeries();

    /// @brief 设置点大小
    /// @param size 点半径像素值
    void setPointSize(int32_t size);

    /// @brief 获取点大小
    /// @return 点半径像素值
    int32_t pointSize() const;

    /// @brief 设置边框颜色
    /// @param color 边框颜色值
    void setBorderColor(int32_t color);

    /// @brief 获取边框颜色
    /// @return 边框颜色值
    int32_t borderColor() const;

    /// @brief 设置是否显示标签
    /// @param visible 标签可见状态
    void setLabelsVisible(bool visible);

    /// @brief 设置标签颜色
    /// @param color 标签颜色值
    void setLabelColor(int32_t color);

    /// @brief 设置标签字体大小
    /// @param size 字体大小
    void setLabelSize(int32_t size);

    /// @brief 绘制散点图
    /// @param painter 绘制器对象
    /// @param axisX X 轴对象
    /// @param axisY Y 轴对象
    /// @param rect 绘制区域矩形
    virtual void draw(TpPainter* painter, const TpAxis& axisX, const TpAxis& axisY, const TpRect& rect) override;
};

/// @brief 饼图系列类
class TpPieSeries : public TpSeries
{
public:
    /// @brief 构造函数
    TpPieSeries();

    /// @brief 析构函数
    virtual ~TpPieSeries();

    /// @brief 添加切片
    /// @param name 切片名称
    /// @param value 切片数值
    /// @param color 切片颜色
    void addSlice(const TpString& name, double value, int32_t color);

    /// @brief 添加切片
    /// @param name 切片名称
    /// @param value 切片数值
    /// @param color 切片颜色
    void addSlice(const char* name, double value, int32_t color);

    /// @brief 清除所有切片
    void clearSlices();

    /// @brief 获取切片数量
    /// @return 切片数量
    int32_t sliceCount() const;

    /// @brief 获取切片名称
    /// @param index 切片索引
    /// @return 切片名称
    const TpString& sliceName(int32_t index) const;

    /// @brief 获取切片数值
    /// @param index 切片索引
    /// @return 切片数值
    double sliceValue(int32_t index) const;

    /// @brief 获取切片颜色
    /// @param index 切片索引
    /// @return 切片颜色值
    int32_t sliceColor(int32_t index) const;

    /// @brief 设置切片是否可见
    /// @param index 切片索引
    /// @param visible 可见状态
    void setSliceVisible(int32_t index, bool visible);

    /// @brief 获取切片是否可见
    /// @param index 切片索引
    /// @return 切片可见状态
    bool isSliceVisible(int32_t index) const;

    /// @brief 设置是否显示标签
    /// @param visible 标签可见状态
    void setLabelsVisible(bool visible);

    /// @brief 设置是否显示百分比
    /// @param visible 百分比可见状态
    void setPercentVisible(bool visible);

    /// @brief 设置是否显示环形图
    /// @param visible 环形图可见状态
    void setDonutVisible(bool visible);

    /// @brief 获取是否显示环形图
    /// @return 环形图可见状态
    bool donutVisible() const;

    /// @brief 设置环形图内半径比例
    /// @param ratio 内外半径比例
    void setDonutRatio(double ratio);

    /// @brief 获取环形图内半径比例
    /// @return 内外半径比例
    double donutRatio() const;

    /// @brief 设置起始角度
    /// @param angle 起始角度
    void setStartAngle(int32_t angle);

    /// @brief 获取起始角度
    /// @return 起始角度
    int32_t startAngle() const;

    /// @brief 设置突出切片索引
    /// @param index 切片索引
    void setExplodedIndex(int32_t index);

    /// @brief 获取突出切片索引
    /// @return 切片索引
    int32_t explodedIndex() const;

    /// @brief 设置突出距离
    /// @param distance 突出距离像素值
    void setExplodeDistance(int32_t distance);

    /// @brief 获取突出距离
    /// @return 突出距离像素值
    int32_t explodeDistance() const;

    /// @brief 设置标签颜色
    /// @param color 标签颜色值
    void setLabelColor(int32_t color);

    /// @brief 获取标签颜色
    /// @return 标签颜色值
    int32_t labelColor() const;

    /// @brief 设置标签字体大小
    /// @param size 字体大小
    void setLabelSize(int32_t size);

    /// @brief 获取标签字体大小
    /// @return 字体大小
    int32_t labelSize() const;

    /// @brief 绘制饼图
    /// @param painter 绘制器对象
    /// @param axisX X 轴对象
    /// @param axisY Y 轴对象
    /// @param rect 绘制区域矩形
    virtual void draw(TpPainter* painter, const TpAxis& axisX, const TpAxis& axisY, const TpRect& rect) override;
};

#endif // TP_SERIES_H
