#ifndef __TP_CHINESE_CALENDAR_H
#define __TP_CHINESE_CALENDAR_H

#include <TpCore.h>
#include <TpString.h>
#include <TpDate.h>

TP_DEF_VOID_TYPE_VAR(ITpChineseCalendarDate);
/// @brief 农历日期类，提供农历日期操作方法
class TpChineseCalendar
{
public:
    TpChineseCalendar();
    /// @brief 传入公历日期构造农历对象
    TpChineseCalendar(int32_t year, int32_t month, int32_t day);
    /// @brief 传入 TpDate 对象构造农历对象
    /// @param date
    TpChineseCalendar(const TpDate &date);
    ~TpChineseCalendar();

    /// @brief 获取日期的公历年份
    /// @return 年份
    int32_t year() const;
    /// @brief 获取日期的公历月份
    /// @return 月份
    int32_t month() const;
    /// @brief 获取日期的公历天
    /// @return 天数
    int32_t day() const;

    /// @brief 设置公历年份
    /// @param year 年份
    void setYear(const int32_t &year);
    /// @brief 设置公历月份
    /// @param month 月份
    void setMonth(const int32_t &month);
    /// @brief 设置公历日期
    /// @param day 日期
    void setDay(const int32_t &day);

    /// @brief 是否闰年
    /// @return 闰年返回tue；解析失败；不是闰年返回false
    bool isLeap();
    /// @brief 天干地支
    /// @return 解析失败返回空
    TpString ganZhiYear();

    /// @brief 当前年份属相
    /// @return 解析失败返回空
    TpString zodiac();
    /// @brief 农历月份
    /// @return 解析失败返回空
    TpString monthName();
    /// @brief 农历日期
    /// @return 解析失败返回空
    TpString dayName();
    /// @brief 农历日期全称
    /// @return 解析失败返回空
    TpString fullName();
    /// @brief 二十四节气
    /// @return 无则返回空
    TpString solarTerm();
    /// @brief 农历日期节日
    /// @return 无则返回空
    TpString holiday();

private:
    ITpChineseCalendarDate *data_;
};

#endif
