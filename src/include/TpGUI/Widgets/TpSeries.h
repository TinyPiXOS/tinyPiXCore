/*
 * 版权声明 (Copyright Declaration)
 * 作者 (Author)：张家庆
 * 邮箱 (Email)：1494197384@qq.com
 * 版权所有 (Copyright)：© 2026 张家庆。All rights reserved.
 * 描述 (Description)：数据系列类 API 定义，用于管理图表中的数据点集合
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

/// @brief 折线/柱状/散点系列使用的基础数据点
struct TpDataPoint
{
    /// @brief X 坐标值
    double x;
    /// @brief Y 坐标值
    double y;

    /// @brief 默认构造函数
    TpDataPoint()
        : x(0)
        , y(0)
    {
    }

    TpDataPoint(double _x, double _y)
        : x(_x)
        , y(_y)
    {
    }
};

/// @brief 饼图扇区数据
struct TpPieSlice
{
    /// @brief 扇区名称
    TpString name;
    /// @brief 扇区值
    double value;
    /// @brief 扇区颜色
    int32_t color;
    /// @brief 是否显示
    bool visible;

    /// @brief 默认构造函数
    TpPieSlice()
        : value(0)
        , color(0)
        , visible(true)
    {
    }

    TpPieSlice(const TpString& _name, double _value, int32_t _color)
        : name(_name)
        , value(_value)
        , color(_color)
        , visible(true)
    {
    }
};

class TpSeriesPrivate;

/// @brief 数据系列基类，负责统一管理点、样式和可见性
class TpSeries
{
public:
    /// @brief 系列类型
    enum SeriesType
    {
        TypeLine,
        TypeBar,
        TypeScatter,
        TypePie
    };

public:
    /// @brief 创建指定类型的数据系列
    TpSeries(SeriesType type);
    /// @brief 销毁数据系列
    virtual ~TpSeries();

public:
    /// @brief 设置系列名称
    void setName(const TpString& name);
    /// @brief 设置系列名称
    void setName(const char* name);
    /// @brief 获取系列名称
    const TpString& name() const;

    /// @brief 设置系列是否可见
    void setVisible(bool visible);
    /// @brief 当前系列是否可见
    bool isVisible() const;

    /// @brief 设置系列主颜色
    void setColor(int32_t color);
    /// @brief 获取系列主颜色
    int32_t color() const;

    /// @brief 添加一个数据点
    void addPoint(double x, double y);
    /// @brief 设置保留的最大点数，超过后会自动裁剪旧数据
    void setMaxPointCount(int32_t count);
    /// @brief 清空当前系列数据
    void clear();

    /// @brief 获取原始数据
    const TpVector<TpDataPoint>& data() const;
    /// @brief 获取系列类型
    SeriesType type() const;
    /// @brief 按 CSS 状态应用系列样式
    void applyCssData(const TpString& className, TpCssParser::MouseStatus status);

    /// @brief 绘制系列
    virtual void draw(TpPainter* painter, const TpAxis& axisX, const TpAxis& axisY, const TpRect& rect) = 0;

protected:
    TpSeriesPrivate* data_;
};

/// @brief 折线系列
class TpLineSeries : public TpSeries
{
public:
    /// @brief 创建折线系列
    TpLineSeries();
    /// @brief 销毁折线系列
    virtual ~TpLineSeries();

    /// @brief 设置线宽
    void setLineWidth(int32_t width);
    /// @brief 设置是否启用平滑曲线
    void setSmooth(bool smooth);
    /// @brief 设置平滑曲线张力
    void setTension(float tension);
    /// @brief 设置是否启用下采样
    void setUseDownsample(bool enabled);
    /// @brief 是否启用了下采样
    bool useDownsample() const;

    virtual void draw(TpPainter* painter, const TpAxis& axisX, const TpAxis& axisY, const TpRect& rect) override;
};

/// @brief 柱状系列
class TpBarSeries : public TpSeries
{
public:
    /// @brief 创建柱状系列
    TpBarSeries();
    /// @brief 销毁柱状系列
    virtual ~TpBarSeries();

    /// @brief 设置渐变终点颜色
    void setGradientColor(int32_t endColor);
    /// @brief 获取渐变终点颜色
    int32_t colorEnd() const;

    /// @brief 设置柱状图布局信息
    void setLayoutInfo(int32_t seriesIndex, int32_t seriesCount);
    /// @brief 设置是否显示标签
    void setLabelsVisible(bool visible);
    /// @brief 设置标签颜色
    void setLabelColor(int32_t color);
    /// @brief 设置标签字号
    void setLabelSize(int32_t size);

    virtual void draw(TpPainter* painter, const TpAxis& axisX, const TpAxis& axisY, const TpRect& rect) override;
};

/// @brief 散点系列
class TpScatterSeries : public TpSeries
{
public:
    /// @brief 创建散点系列
    TpScatterSeries();
    /// @brief 销毁散点系列
    virtual ~TpScatterSeries();

    /// @brief 设置点大小
    void setPointSize(int32_t size);
    /// @brief 获取点大小
    int32_t pointSize() const;

    /// @brief 设置边框颜色
    void setBorderColor(int32_t color);
    /// @brief 获取边框颜色
    int32_t borderColor() const;

    /// @brief 设置是否显示标签
    void setLabelsVisible(bool visible);
    /// @brief 设置标签颜色
    void setLabelColor(int32_t color);
    /// @brief 设置标签字号
    void setLabelSize(int32_t size);

    virtual void draw(TpPainter* painter, const TpAxis& axisX, const TpAxis& axisY, const TpRect& rect) override;
};

/// @brief 饼图系列
class TpPieSeries : public TpSeries
{
public:
    /// @brief 创建饼图系列
    TpPieSeries();
    /// @brief 销毁饼图系列
    virtual ~TpPieSeries();

    /// @brief 添加一个扇区
    void addSlice(const TpString& name, double value, int32_t color);
    /// @brief 添加一个扇区
    void addSlice(const char* name, double value, int32_t color);
    /// @brief 清空全部扇区
    void clearSlices();

    /// @brief 扇区数量
    int32_t sliceCount() const;
    /// @brief 获取指定扇区名称
    const TpString& sliceName(int32_t index) const;
    /// @brief 获取指定扇区值
    double sliceValue(int32_t index) const;
    /// @brief 获取指定扇区颜色
    int32_t sliceColor(int32_t index) const;

    /// @brief 设置指定扇区是否可见
    void setSliceVisible(int32_t index, bool visible);
    /// @brief 指定扇区是否可见
    bool isSliceVisible(int32_t index) const;

    /// @brief 设置是否显示标签
    void setLabelsVisible(bool visible);
    /// @brief 设置是否显示百分比
    void setPercentVisible(bool visible);
    /// @brief 设置是否显示环形图效果
    void setDonutVisible(bool visible);
    /// @brief 当前是否为环形图
    bool donutVisible() const;

    /// @brief 设置内圈半径比例
    void setDonutRatio(double ratio);
    /// @brief 获取内圈半径比例
    double donutRatio() const;

    /// @brief 设置起始角度
    void setStartAngle(int32_t angle);
    /// @brief 获取起始角度
    int32_t startAngle() const;

    /// @brief 设置当前高亮或弹出的扇区索引
    void setExplodedIndex(int32_t index);
    /// @brief 获取当前高亮或弹出的扇区索引
    int32_t explodedIndex() const;

    /// @brief 设置扇区弹出距离
    void setExplodeDistance(int32_t distance);
    /// @brief 获取扇区弹出距离
    int32_t explodeDistance() const;

    /// @brief 设置标签颜色
    void setLabelColor(int32_t color);
    /// @brief 获取标签颜色
    int32_t labelColor() const;

    /// @brief 设置标签字号
    void setLabelSize(int32_t size);
    /// @brief 获取标签字号
    int32_t labelSize() const;

    virtual void draw(TpPainter* painter, const TpAxis& axisX, const TpAxis& axisY, const TpRect& rect) override;
};

#endif // TP_SERIES_H