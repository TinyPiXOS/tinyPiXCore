/*
 * 版权声明 (Copyright Declaration)
 * 作者 (Author)：张金圣
 * 邮箱 (Email)：3270141465@qq.com
 * 版权所有 (Copyright)：© 2026 张金圣. All rights reserved.
 * 描述 (Description)：该类实现逻辑数据与物理像素的映射及坐标轴布局计算
 */

#ifndef TP_AXIS_H
#define TP_AXIS_H

#include <TpVector.h>
#include <TpRect.h>

TP_DEF_VOID_TYPE_VAR(ITpAxisData);

class TpAxis
{
public:
    /// @brief 轴类型
    enum class AxisMode
    {
        Value,
        Category
    };

    /// @brief 采样点统计信息，用于下采样和绘制优化
    struct SamplePoint
    {
        double firstVal;
        double lastVal;
        double minVal;
        double maxVal;
        double minRelX;
        double maxRelX;
        double meanVal;
        double stdDev;

        SamplePoint()
            : firstVal(0)
            , lastVal(0)
            , minVal(0)
            , maxVal(0)
            , minRelX(0)
            , maxRelX(0)
            , meanVal(0)
            , stdDev(0)
        {
        }
    };

    /// @brief 柱状图布局信息
    struct BarGeometry
    {
        int32_t position;
        int32_t size;

        BarGeometry(int32_t p = 0, int32_t s = 0)
            : position(p)
            , size(s)
        {
        }
    };

public:
    /// @brief 创建坐标轴对象
    TpAxis();
    /// @brief 销毁坐标轴对象
    virtual ~TpAxis();

public:
    /// @brief 设置轴范围
    /// @param min 最小值
    /// @param max 最大值
    /// @param includeZero 是否强制包含 0
    void setRange(double min, double max, bool includeZero = false);

    /// @brief 设置轴模式，数值轴或分类轴
    void setMode(AxisMode mode);

    /// @brief 设置分类轴的类别数量
    void setCategoryCount(int32_t count);

    /// @brief 根据目标刻度数重新计算刻度
    void updateNiceTicks(int32_t targetCount);

    /// @brief 开启或关闭自动范围计算
    void setAutoRange(bool autoRange);

    /// @brief 当前是否开启自动范围
    bool isAutoRange() const;

    /// @brief 将数值映射到像素位置
    int32_t mapToPixel(double value, int32_t length, int32_t offset, bool inverse = false) const;

    /// @brief 当前范围是否包含 0
    bool containsZero() const;

    /// @brief 获取零点在像素上的位置
    int32_t ZeroPixel(int32_t length, int32_t offset, bool inverse = false) const;

    /// @brief 获取柱状图在坐标轴上的布局
    BarGeometry mapBarToGeometry(int32_t catIndex, int32_t seriesCount, int32_t seriesIndex,
                                 int32_t length, int32_t offset, bool inverse = false) const;

    /// @brief 将像素位置映射回数值
    double mapToValue(int32_t pixel, int32_t length, int32_t offset, bool inverse = false) const;

    /// @brief 获取当前刻度值列表
    const TpVector<double>& getTickValues() const;

    /// @brief 获取最小值
    double min() const;

    /// @brief 获取最大值
    double max() const;

    /// @brief 对原始数据做下采样
    static TpVector<SamplePoint> downsample(const TpVector<double>& rawData, int32_t startIndex, int32_t count, int32_t pixelWidth);

    /// @brief 将全部数据点映射到像素坐标
    static TpVector<int32_t> mapAllToPixels(const TpAxis& axis, const TpVector<double>& rawData, int32_t startIndex,
                                            int32_t count, int32_t length, int32_t offset, bool inverse = false);

    /// @brief 设置刻度数量
    void setTickCount(int32_t count);

    /// @brief 设置刻度步长
    void setTickStep(double step);

    /// @brief 设置是否对称显示
    void setSymmetric(bool enabled);

    /// @brief 是否对称显示
    bool isSymmetric() const;

    /// @brief 设置是否启用智能自动范围
    void setAutoRangeNice(bool enabled);

    /// @brief 是否启用智能自动范围
    bool isAutoRangeNice() const;

    /// @brief 设置是否启用滚动模式
    void setRollingMode(bool enabled);

    /// @brief 是否启用滚动模式
    bool isRollingMode() const;

    /// @brief 设置 X 轴方向的留白比例
    void setXPadding(double leftRatio, double rightRatio);

    /// @brief 设置 Y 轴方向的留白比例
    void setYPadding(double topRatio, double bottomRatio);

    /// @brief 获取 X 轴左侧留白比例
    double xLeftPaddingRatio() const;

    /// @brief 获取 X 轴右侧留白比例
    double xRightPaddingRatio() const;

    /// @brief 获取 Y 轴顶部留白比例
    double yTopPaddingRatio() const;

    /// @brief 获取 Y 轴底部留白比例
    double yBottomPaddingRatio() const;

private:
    ITpAxisData* data_;
};

#endif // TP_AXIS_H