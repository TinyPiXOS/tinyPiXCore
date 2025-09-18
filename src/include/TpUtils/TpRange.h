#include "TpUtils.h"

TP_DEF_VOID_TYPE_VAR(ITpRangeData);
/// @brief 取值范围工具类；用于维护一个取值范围和当前值
class TpRange
{
public:
    TpRange(int64_t min = 0, int64_t max = 100);
    ~TpRange();

public:
    /// @brief 设置最大最小值;当给入最大值小于等于最小值时，会重设最大值为最小值加一
    /// @param min 最小值
    /// @param max 最大值
    void setRange(int64_t min, int64_t max);
    /// @brief 设置当前值；大于最大值则为最大值，小于最小值则为最小值
    /// @param value 当前值
    void setValue(int64_t value);
    /// @brief 获取当前值
    /// @return
    int64_t value() const noexcept;

    /// @brief 根据百分比设置当前值；小于0则value为最小值，大于0则value为最大值
    /// @param percent 取值百分比，取值范围[0,1]
    void setPercent(double percent);
    /// @brief 获取当前百分比
    /// @return 百分比
    double percent() const noexcept;

    /// @brief 获取当前最小值
    /// @return 最小值
    int64_t min() const noexcept;
    /// @brief 获取当前最大值
    /// @return 最大值
    int64_t max() const noexcept;

private:
    ITpRangeData *data_;
};
