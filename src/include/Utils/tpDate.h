#ifndef __TP_DATE_H
#define __TP_DATE_H

#include <tpUtils.h>
#include <tpString.h>

TP_DEF_VOID_TYPE_VAR(ItpDateData);
/// @brief 日期类，提供日期相关操作
class tpDate
{
public:
    tpDate();
    explicit tpDate(const int32_t &y, const int32_t &m, const int32_t &d);
    ~tpDate();

    /// @brief 将日期字符串转换日期对象
    /// @param s 日期字符串
    /// @param format 格式化字符串 例如yyyy-MM-dd
    /// @return tpDate对象
    static tpDate fromString(const tpString &s, const tpString &format);
    /// @brief 获取当前日期
    /// @return 当前日期对象
    static tpDate currentDate();

public:
    /// @brief 获取日期的年份
    /// @return 年份
    int32_t year() const;
    /// @brief 获取日期的月份
    /// @return 月份
    int32_t month() const;
    /// @brief 获取日期的天
    /// @return 天数
    int32_t day() const;

    /// @brief 设置年份
    /// @param year 年份
    void setYear(const int32_t &year);
    /// @brief 设置月份
    /// @param month 月份
    void setMonth(const int32_t &month);
    /// @brief 设置日期
    /// @param day 日期
    void setDay(const int32_t &day);

    /// @brief 获取日期是当周的第几天
    /// @return 当周的第几天，1为星期一，7为星期日
    int32_t dayOfWeek() const;
    /// @brief 获取当前日期在该年中的天数
    /// @return 当年的第几天
    int32_t dayOfYear() const;
    /// @brief 获取当前日期所在月份的天数
    /// @return 当月有多少天
    int32_t daysInMonth() const;
    /// @brief 返回当前日期所在年份共计多少天
    /// @return 天数365；366
    int32_t daysInYear() const;

    /// @brief 基于当前日期添加指定天数
    /// @param days 添加天数
    /// @return 返回叠加后的新日期对象
    tpDate addDays(const uint64_t &days) const;
    /// @brief 基于当前日期添加指定月数
    /// @param months 添加月数
    /// @return 返回叠加后的新日期对象
    tpDate addMonths(const uint32_t &months) const;
    /// @brief 基于当前日期添加指定年数
    /// @param years 添加年数
    /// @return 返回叠加后的新日期对象
    tpDate addYears(const uint32_t &years) const;

    /// @brief 将日期对象转换为字符串
    /// @param format 格式化字符串 例如yyyy-MM-dd
    /// @return 日期对象
    tpString toString(const tpString &format) const;

    /// @brief 转换为儒略日
    /// @return 儒略日
    int64_t toJulianDay() const;

public:
    tpDate &operator=(const tpDate &other) noexcept;

    bool operator==(const tpDate &other) const;
    bool operator<(const tpDate &other) const;

    bool operator!=(const tpDate &other) const { return !(*this == other); }
    bool operator<=(const tpDate &other) const { return !(other < *this); }
    bool operator>(const tpDate &other) const { return other < *this; }
    bool operator>=(const tpDate &other) const { return !(*this < other); }

private:
    ItpDateData *data_;
};

#endif
