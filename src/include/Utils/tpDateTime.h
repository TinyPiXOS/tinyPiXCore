#ifndef __TP_DATE_TIME_H
#define __TP_DATE_TIME_H

#include <tpUtils.h>
#include <tpString.h>
#include "tpDate.h"
#include "tpTime.h"

TP_DEF_VOID_TYPE_VAR(ItpDateTimeData);
/// @brief 日期时间复合类，提供精确到毫秒的日期时间操作
class tpDateTime
{
public:
    tpDateTime();
    ~tpDateTime();

    /// @brief 获取当前系统日期时间（本地时区）
    /// @return 当前日期时间对象
    static tpDateTime currentDateTime();
    /// @brief 从格式字符串解析日期时间
    /// @param s 日期时间字符串，如 "2023-08-25 14:30:45.500"
    /// @param format 格式模板，支持：
    ///   - 日期部分：yyyy(年), MM(月), dd(日)
    ///   - 时间部分：HH(时), mm(分), ss(秒), zzz(毫秒)
    /// @return 解析后的日期时间对象
    /// @throws std::invalid_argument 格式不匹配时抛出
    static tpDateTime fromString(const tpString &s, const tpString &format);
    /// @brief 从毫秒时间戳创建对象（UTC时区）
    /// @param msecs 自1970-01-01T00:00:00 UTC的毫秒数
    /// @return 对应UTC日期时间对象
    static tpDateTime fromMSecsSinceEpoch(int64_t msecs);
    /// @brief 从秒时间戳创建对象（UTC时区）
    /// @param secs 自1970-01-01T00:00:00 UTC的秒数
    /// @return 对应UTC日期时间对象
    static tpDateTime fromSecsSinceEpoch(int64_t secs);

    /// @brief 获取当前UTC毫秒时间戳
    /// @return 当前时刻的毫秒时间戳
    static int64_t currentMSecsSinceEpoch() noexcept;
    /// @brief 获取当前UTC秒时间戳
    /// @return 当前时刻的秒时间戳
    static int64_t currentSecsSinceEpoch() noexcept;

    /// @brief 获取日期部分
    /// @return 日期对象副本
    tpDate date() const;
    /// @brief 获取时间部分
    /// @return 时间对象副本
    tpTime time() const;

    /// @brief 转换为UTC毫秒时间戳
    /// @return 自epoch的毫秒数
    int64_t toMSecsSinceEpoch() const;
    /// @brief 转换为UTC秒时间戳
    /// @return 自epoch的秒数
    int64_t toSecsSinceEpoch() const;

    /// @brief 通过UTC毫秒时间戳设置时间
    /// @param msecs 自epoch的毫秒数
    void setMSecsSinceEpoch(int64_t msecs);
    /// @brief 通过UTC秒时间戳设置时间
    /// @param secs 自epoch的秒数
    void setSecsSinceEpoch(int64_t secs);

    /// @brief 设置日期部分（保持时间不变）
    /// @param date 新日期对象
    void setDate(const tpDate &date);
    /// @brief 设置时间部分（保持日期不变）
    /// @param time 新时间对象
    void setTime(const tpTime &time);

    /// @brief 格式化输出日期时间
    /// @param format 格式字符串，如"yyyy-MM-dd HH:mm:ss.zzz"
    /// @return 格式化后的字符串
    tpString toString(const tpString &format) const;

    /// @brief 增加指定天数
    /// @param days 要增加的天数（可为负数）
    /// @return 新日期时间对象
    tpDateTime addDays(int64_t days) const;
    /// @brief 增加指定月数
    /// @param months 要增加的月数（可为负数）
    /// @return 新日期时间对象（自动调整月末日期）
    tpDateTime addMonths(int32_t months) const;
    /// @brief 增加指定年数
    /// @param years 要增加的年数（可为负数）
    /// @return 新日期时间对象（自动处理闰年）
    tpDateTime addYears(int32_t years) const;
    /// @brief 增加指定秒数
    /// @param secs 要增加的秒数（可为负数）
    /// @return 新日期时间对象
    tpDateTime addSecs(int64_t secs) const;
    /// @brief 增加指定毫秒数
    /// @param msecs 要增加的毫秒数（可为负数）
    /// @return 新日期时间对象
    tpDateTime addMSecs(int64_t msecs) const;

    /// @brief 计算到目标日期的天数差
    /// @param other 目标日期时间
    /// @return 间隔天数（this早于other时为负）
    int64_t daysTo(const tpDateTime &other) const;
    /// @brief 计算到目标时间的秒数差
    /// @param other 目标日期时间
    /// @return 间隔秒数（this早于other时为负）
    int64_t secsTo(const tpDateTime &other) const;
    /// @brief 计算到目标时间的毫秒数差
    /// @param other 目标日期时间
    /// @return 间隔毫秒数（this早于other时为负）
    int64_t msecsTo(const tpDateTime &other) const;

public:
    tpDateTime &operator=(const tpDateTime &other) noexcept;
    bool operator==(const tpDateTime &other) const;
    bool operator<(const tpDateTime &other) const;
    inline bool operator!=(const tpDateTime &other) const { return !(*this == other); }
    inline bool operator<=(const tpDateTime &other) const { return !(other < *this); }
    inline bool operator>(const tpDateTime &other) const { return other < *this; }
    inline bool operator>=(const tpDateTime &other) const { return !(*this < other); }

private:
    ItpDateTimeData *data_;
};

#endif
