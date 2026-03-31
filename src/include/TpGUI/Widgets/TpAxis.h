/*
 * 版权声明 (Copyright Declaration)
 * 作者 (Author)：张金圣
 * 邮箱 (Email)：3270141465@qq.com
 * 版权所有 (Copyright)：© 2026 张金圣。All rights reserved.
 * 描述 (Description)：坐标轴类 API 定义，实现逻辑数据与物理像素的映射及坐标轴布局计算
 */

#ifndef TP_AXIS_H
#define TP_AXIS_H

#include <TpVector.h>
#include <TpRect.h>

TP_DEF_VOID_TYPE_VAR(ITpAxisData);

/// @brief 坐标轴逻辑处理类，用于值域映射、美化刻度及图表几何布局计算
class TpAxis
{
public:
    /// @brief 坐标轴模式
    enum class AxisMode
    {
        Value,      ///< @brief 数值轴：基于连续数值映射
        Category    ///< @brief 类目轴：基于离散索引映射
    };

    /// @brief 采样点结构，用于大数据量降采样显示
    struct SamplePoint
    {
        double firstVal;    ///< @brief 第一个值
        double lastVal;     ///< @brief 最后一个值
        double minVal;      ///< @brief 最小值
        double maxVal;      ///< @brief 最大值
        double minRelX;     ///< @brief 最小值相对位置
        double maxRelX;     ///< @brief 最大值相对位置
        double meanVal;     ///< @brief 平均值
        double stdDev;      ///< @brief 标准差

        /// @brief 默认构造函数
        SamplePoint()
            : firstVal(0)
            , lastVal(0)
            , minVal(0)
            , maxVal(0)
            , minRelX(0)
            , maxRelX(0)
            , meanVal(0)
            , stdDev(0) {}
    };

    /// @brief 柱状图几何信息结构
    struct BarGeometry
    {
        int32_t position;   ///< @brief 柱体起始像素位置
        int32_t size;       ///< @brief 柱体宽度/高度像素值

        /// @brief 构造函数
        /// @param p 位置
        /// @param s 大小
        BarGeometry(int32_t p = 0, int32_t s = 0)
            : position(p)
            , size(s) {}
    };

public:
    /// @brief 构造函数
    TpAxis();

    /// @brief 析构函数
    virtual ~TpAxis();

public:
    /// @brief 设置值域范围
    /// @param min 最小值
    /// @param max 最大值
    /// @param includeZero 是否强制包含零点
    void setRange(double min, double max, bool includeZero = false);

    /// @brief 设置坐标轴模式
    /// @param mode 模式（数值或类目）
    void setMode(AxisMode mode);

    /// @brief 设置类目数量（仅在 Category 模式下有效）
    /// @param count 分类总数
    void setCategoryCount(int32_t count);

    /// @brief 更新美化刻度，计算人类易读的步长
    /// @param targetCount 期望的刻度数量
    void updateNiceTicks(int32_t targetCount);

    /// @brief 设置是否自动计算范围
    /// @param autoRange 是否自动计算
    void setAutoRange(bool autoRange);

    /// @brief 获取当前是否是自动范围
    /// @return 自动范围返回 true，否则返回 false
    bool isAutoRange() const;

    /// @brief 逻辑值转换为像素坐标
    /// @param value 输入的逻辑数值或索引
    /// @param length 绘制区域的总长度
    /// @param offset 绘制区域的起始偏移
    /// @param inverse 是否反向绘制（如用于 Y 轴从下往上）
    /// @return 对应的像素位置
    int32_t mapToPixel(double value, int32_t length, int32_t offset, bool inverse = false) const;

    /// @brief 判断当前范围是否包含零点
    /// @return 包含返回 true，否则返回 false
    bool containsZero() const;

    /// @brief 获取零点的像素位置
    /// @param length 区域长度
    /// @param offset 区域偏移
    /// @param inverse 是否反向
    /// @return 零点对应的像素位置
    int32_t ZeroPixel(int32_t length, int32_t offset, bool inverse = false) const;

    /// @brief 计算柱状图在特定类目下的几何布局
    /// @param catIndex 类目索引
    /// @param seriesCount 系列总数（用于多柱并列）
    /// @param seriesIndex 当前系列索引
    /// @param length 区域长度
    /// @param offset 区域偏移
    /// @param inverse 是否反向
    /// @return 包含位置和宽度的几何结构
    BarGeometry mapBarToGeometry(int32_t catIndex, int32_t seriesCount, int32_t seriesIndex,
                                  int32_t length, int32_t offset, bool inverse = false) const;

    /// @brief 像素坐标转逻辑值
    /// @param pixel 像素位置
    /// @param length 区域长度
    /// @param offset 区域偏移
    /// @param inverse 是否反向
    /// @return 对应的逻辑数值或索引
    double mapToValue(int32_t pixel, int32_t length, int32_t offset, bool inverse = false) const;

    /// @brief 获取计算后的刻度值列表
    /// @return 刻度值数组引用
    const TpVector<double>& getTickValues() const;

    /// @brief 获取当前最小值
    /// @return 最小值
    double min() const;

    /// @brief 获取当前最大值
    /// @return 最大值
    double max() const;

    /// @brief 静态降采样算法，支持缩放范围选取
    /// @param rawData 原始全量数据
    /// @param startIndex 缩放窗口的起始索引
    /// @param count 缩放窗口涵盖的数据点数量
    /// @param pixelWidth 目标显示区域的像素宽度
    /// @return 降采样后的采样点数组
    static TpVector<SamplePoint> downsample(const TpVector<double>& rawData, int32_t startIndex, int32_t count, int32_t pixelWidth);

    /// @brief 将原始数据全量转换为像素坐标（不进行任何降采样）
    /// @param axis 坐标轴对象
    /// @param rawData 原始数据
    /// @param startIndex 起始索引
    /// @param count 转换数量
    /// @param length 绘制区域长度
    /// @param offset 绘制区域偏移
    /// @param inverse 是否反向
    /// @return 转换后的像素坐标数组
    static TpVector<int32_t> mapAllToPixels(const TpAxis& axis, const TpVector<double>& rawData, int32_t startIndex,
                                             int32_t count, int32_t length, int32_t offset, bool inverse = false);

private:
    /// @brief 计算人类易读步长内部函数
    /// @param range 范围
    /// @param targetCount 目标刻度数量
    /// @return 计算后的步长值
    double calculateNiceStep(double range, int32_t targetCount);

private:
    ITpAxisData* data_;
};

#endif // TP_AXIS_H
