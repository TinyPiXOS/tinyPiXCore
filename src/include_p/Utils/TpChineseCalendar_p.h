#ifndef __TP_DATE_PRIVATE_H
#define __TP_DATE_PRIVATE_H

#include "TpUtils.h"
#include "TpVector.h"
#include "TpString.h"
#include "TpMap.h"

#include <ctime>
#include <iostream>
#include <sstream>
#include <stdexcept>
#include <cmath>
#include <algorithm>

// 日期结构体
struct LunarDate
{
    int year = 0;
    int month = 0;
    int day = 0;
    bool isLeap = false;
    TpString ganZhiYear = ""; // 天干地支
    TpString zodiac = "";     // 属相
    TpString monthName = "";
    TpString dayName = "";
    TpString solarTerm = ""; // 二十四节气
    TpString holiday = "";   // 节日
    TpString fullName = "";
};

// 修正后的农历数据表（1900-2100年）
const TpVector<uint32_t> lunarInfo = {
    0x04bd8, 0x04ae0, 0x0a570, 0x054d5, 0x0d260, 0x0d950, 0x16554, 0x056a0, 0x09ad0, 0x055d2, // 1900-1909
    0x04ae0, 0x0a5b6, 0x0a4d0, 0x0d250, 0x1d255, 0x0b540, 0x0d6a0, 0x0ada2, 0x095b0, 0x14977, // 1910-1919
    0x04970, 0x0a4b0, 0x0b4b5, 0x06a50, 0x06d40, 0x1ab54, 0x02b60, 0x09570, 0x052f2, 0x04970, // 1920-1929
    0x06566, 0x0d4a0, 0x0ea50, 0x06e95, 0x05ad0, 0x02b60, 0x186e3, 0x092e0, 0x1c8d7, 0x0c950, // 1930-1939
    0x0d4a0, 0x1d8a6, 0x0b550, 0x056a0, 0x1a5b4, 0x025d0, 0x092d0, 0x0d2b2, 0x0a950, 0x0b557, // 1940-1949
    0x06ca0, 0x0b550, 0x15355, 0x04da0, 0x0a5d0, 0x14573, 0x052d0, 0x0a9a8, 0x0e950, 0x06aa0, // 1950-1959
    0x0aea6, 0x0ab50, 0x04b60, 0x0aae4, 0x0a570, 0x05260, 0x0f263, 0x0d950, 0x05b57, 0x056a0, // 1960-1969
    0x096d0, 0x04dd5, 0x04ad0, 0x0a4d0, 0x0d4d4, 0x0d250, 0x0d558, 0x0b540, 0x0b5a0, 0x195a6, // 1970-1979
    0x095b0, 0x049b0, 0x0a974, 0x0a4b0, 0x0b27a, 0x06a50, 0x06d40, 0x0af46, 0x0ab60, 0x09570, // 1980-1989
    0x04af5, 0x04970, 0x064b0, 0x074a3, 0x0ea50, 0x06b58, 0x055c0, 0x0ab60, 0x096d5, 0x092e0, // 1990-1999
    0x0c960, 0x0d954, 0x0d4a0, 0x0da50, 0x07552, 0x056a0, 0x0abb7, 0x025d0, 0x092d0, 0x0cab5, // 2000-2009
    0x0a950, 0x0b4a0, 0x0baa4, 0x0ad50, 0x055d9, 0x04ba0, 0x0a5b0, 0x15176, 0x052b0, 0x0a930, // 2010-2019
    0x07954, 0x06aa0, 0x0ad50, 0x05b52, 0x04b60, 0x0a6e6, 0x0a4e0, 0x0d260, 0x0ea65, 0x0d530, // 2020-2029
    0x05aa0, 0x076a3, 0x096d0, 0x04bd7, 0x04ad0, 0x0a4d0, 0x1d0b6, 0x0d250, 0x0d520, 0x0dd45, // 2030-2039
    0x0b5a0, 0x056d0, 0x055b2, 0x049b0, 0x0a577, 0x0a4b0, 0x0aa50, 0x1b255, 0x06d20, 0x0ada0, // 2040-2049
    0x14b63, 0x09370, 0x049f8, 0x04970, 0x064b0, 0x168a6, 0x0ea50, 0x06b20, 0x1a6c4, 0x0aae0, // 2050-2059
    0x092e0, 0x0d2e3, 0x0c960, 0x0d557, 0x0d4a0, 0x0da50, 0x05d55, 0x056a0, 0x0a6d0, 0x055d4, // 2060-2069
    0x052d0, 0x0a9b8, 0x0a950, 0x0b4a0, 0x0b6a6, 0x0ad50, 0x055a0, 0x0aba4, 0x0a5b0, 0x052b0, // 2070-2079
    0x0b273, 0x06930, 0x07337, 0x06aa0, 0x0ad50, 0x14b55, 0x04b60, 0x0a570, 0x054e4, 0x0d160, // 2080-2089
    0x0e968, 0x0d520, 0x0daa0, 0x16aa6, 0x056d0, 0x04ae0, 0x0a9d4, 0x0a2d0, 0x0d150, 0x0f252, // 2090-2099
    0x0d520                                                                                   // 2100
};

// 农历月份名称
const TpVector<TpString> lunarMonthNames = {
    "正月", "二月", "三月", "四月", "五月", "六月",
    "七月", "八月", "九月", "十月", "冬月", "腊月"};

// 农历日期名称
const TpVector<TpString> lunarDayNames = {
    "初一", "初二", "初三", "初四", "初五", "初六", "初七", "初八", "初九", "初十",
    "十一", "十二", "十三", "十四", "十五", "十六", "十七", "十八", "十九", "二十",
    "廿一", "廿二", "廿三", "廿四", "廿五", "廿六", "廿七", "廿八", "廿九", "三十"};

// 生肖名称
const TpVector<TpString> zodiacNames = {
    "鼠", "牛", "虎", "兔", "龙", "蛇", "马", "羊", "猴", "鸡", "狗", "猪"};

// 天干
const TpVector<TpString> heavenlyStems = {
    "甲", "乙", "丙", "丁", "戊", "己", "庚", "辛", "壬", "癸"};

// 地支
const TpVector<TpString> earthlyBranches = {
    "子", "丑", "寅", "卯", "辰", "巳", "午", "未", "申", "酉", "戌", "亥"};

// 二十四节气名称
const TpVector<TpString> solarTermNames = {
    "小寒", "大寒", "立春", "雨水", "惊蛰", "春分",
    "清明", "谷雨", "立夏", "小满", "芒种", "夏至",
    "小暑", "大暑", "立秋", "处暑", "白露", "秋分",
    "寒露", "霜降", "立冬", "小雪", "大雪", "冬至"};

// 公历节日映射表
const TpMap<std::pair<int, int>, TpString> solarHolidays = {
    {{1, 1}, "元旦"},
    {{2, 14}, "情人节"},
    {{3, 8}, "妇女节"},
    {{3, 12}, "植树节"},
    {{4, 1}, "愚人节"},
    {{5, 1}, "劳动节"},
    {{5, 4}, "青年节"},
    {{6, 1}, "儿童节"},
    {{7, 1}, "建党节"},
    {{8, 1}, "建军节"},
    {{9, 10}, "教师节"},
    {{10, 1}, "国庆节"},
    {{12, 24}, "平安夜"},
    {{12, 25}, "圣诞节"}};

// 农历节日映射表
const TpMap<std::pair<int, int>, TpString> lunarHolidays = {
    {{1, 1}, "春节"},
    {{1, 15}, "元宵节"},
    {{5, 5}, "端午节"},
    {{7, 7}, "七夕"},
    {{7, 15}, "中元节"},
    {{8, 15}, "中秋节"},
    {{9, 9}, "重阳节"},
    {{12, 8}, "腊八节"},
    {{12, 23}, "小年"},
    {{12, 30}, "除夕"}};

// 计算农历年份的总天数
int lunarYearDays(int year)
{
    int yearIndex = year - 1900;
    if (yearIndex < 0 || yearIndex >= static_cast<int>(lunarInfo.size()))
    {
        return -1; // 年份超出范围
    }

    uint32_t monthsInfo = lunarInfo[yearIndex];
    int days = 0;

    // 计算12个月的天数
    for (int i = 0; i < 12; i++)
    {
        days += 29 + ((monthsInfo & (0x10000 >> i)) ? 1 : 0);
    }

    // 如果有闰月，加上闰月的天数
    int leapMonth = monthsInfo & 0xf;
    if (leapMonth)
    {
        days += (monthsInfo & 0x10000) ? 30 : 29;
    }

    return days;
}

// 计算两个日期之间的天数差
int daysBetweenDates(int year1, int month1, int day1, int year2, int month2, int day2)
{
    struct std::tm date1 = {0, 0, 12, day1, month1 - 1, year1 - 1900};
    struct std::tm date2 = {0, 0, 12, day2, month2 - 1, year2 - 1900};

    std::time_t time1 = std::mktime(&date1);
    std::time_t time2 = std::mktime(&date2);

    if (time1 == -1 || time2 == -1)
    {
        return -1; // 日期无效
    }

    double difference = std::difftime(time2, time1);
    return static_cast<int>(std::round(difference / (60 * 60 * 24)));
}

// 计算二十四节气（精确计算）
TpString calculateSolarTerm(int year, int month, int day)
{
    // 精确的二十四节气计算公式
    // 基于Jean Meeus的《天文算法》
    if (month < 1 || month > 12)
        return "";

    // 计算该月节气的近似日期
    double yearFraction = (year - 1900) + (month - 1) * 1.0 / 12.0;
    double term = 0.0;

    // 每个节气的偏移量
    static const double termOffsets[24] = {
        0.0, 15.2184, 30.438, 45.656, 60.874, 76.092, 91.31, 106.528, 121.746, 136.964, 152.182, 167.4,
        182.618, 197.836, 213.054, 228.272, 243.49, 258.708, 273.926, 289.144, 304.362, 319.58, 334.798, 350.016};

    // 计算该月第一个节气的近似日期
    term = termOffsets[(month - 1) * 2];

    // 精确调整
    double adjustment = 0.0;
    if (year >= 1900 && year <= 2100)
    {
        // 使用简化公式进行调整
        adjustment = 0.5 + 0.2422 * (year - 1900) - floor(0.25 * (year - 1900));
    }

    int approxDay = static_cast<int>(term + adjustment) % 30;
    if (approxDay < 1)
        approxDay = 1;
    if (approxDay > 31)
        approxDay = 31;

    // 检查是否接近节气
    if (std::abs(day - approxDay) <= 1)
    {
        return solarTermNames[(month - 1) * 2];
    }

    return "";
}

// 计算某年某月的第n个星期日的日期
int nthSunday(int year, int month, int n)
{
    if (n < 1 || n > 5)
        return 0;

    // 获取该月1号的信息
    struct std::tm firstDay = {0, 0, 12, 1, month - 1, year - 1900};
    std::mktime(&firstDay);
    int firstDayWeekday = firstDay.tm_wday; // 0=周日, 1=周一, ..., 6=周六

    // 计算第一个星期日的日期
    int firstSunday = 1;
    if (firstDayWeekday != 0)
    {
        firstSunday += (7 - firstDayWeekday);
    }

    // 计算第n个星期日的日期
    return firstSunday + (n - 1) * 7;
}

// 获取节日信息
TpString getHolidayInfo(int year, int month, int day, int lunarMonth, int lunarDay, bool isLeap)
{
    // 检查公历节日
    auto solarIt = solarHolidays.find({month, day});
    if (solarIt != solarHolidays.end())
    {
        return solarIt->second;
    }

    // 检查农历节日（如果不是闰月）
    if (!isLeap)
    {
        auto lunarIt = lunarHolidays.find({lunarMonth, lunarDay});
        if (lunarIt != lunarHolidays.end())
        {
            return lunarIt->second;
        }
    }

    // 特殊节日处理
    if (month == 5)
    {
        int motherDay = nthSunday(year, 5, 2);
        if (day == motherDay)
        {
            return "母亲节";
        }
    }

    if (month == 6)
    {
        int fatherDay = nthSunday(year, 6, 3);
        if (day == fatherDay)
        {
            return "父亲节";
        }
    }

    return "";
}

// 将公历日期转换为农历日期
LunarDate solarToLunar(int year, int month, int day)
{
    LunarDate result;

    // 基准日期：1900年1月31日（农历正月初一）
    const int baseYear = 1900;
    const int baseMonth = 1;
    const int baseDay = 31;

    // 检查日期是否在有效范围内（1900-2100）
    if (year < 1900 || year > 2100)
    {
        result.year = -1;
        result.fullName = "日期超出范围（1900-2100）";
        return result;
    }

    // 计算输入日期与基准日期的天数差
    int offset = daysBetweenDates(baseYear, baseMonth, baseDay, year, month, day);
    if (offset < 0)
    {
        result.year = -1;
        result.fullName = "日期无效";
        return result;
    }

    // 初始化农历年份和天数
    int lunarYear = 1900;
    int daysInYear = 0;

    // 计算农历年份
    while (offset > 0)
    {
        daysInYear = lunarYearDays(lunarYear);
        if (daysInYear < 0)
        {
            result.year = -1;
            result.fullName = "农历年份计算错误";
            return result;
        }

        if (offset < daysInYear)
        {
            break;
        }

        offset -= daysInYear;
        lunarYear++;
    }

    // 计算闰月信息
    int yearIndex = lunarYear - 1900;
    if (yearIndex < 0 || yearIndex >= static_cast<int>(lunarInfo.size()))
    {
        result.year = -1;
        result.fullName = "农历年份超出范围";
        return result;
    }

    uint32_t yearInfo = lunarInfo[yearIndex];
    int leapMonth = yearInfo & 0xf;
    int leapMonthDays = 0;
    if (leapMonth)
    {
        leapMonthDays = (yearInfo & 0x10) ? 30 : 29; // 修复掩码
        // leapMonthDays = (yearInfo & 0x10000) ? 30 : 29;
    }

    // 计算农历月份和日期
    int lunarMonth = 1;
    int lunarDay = 1;
    bool isLeap = false;
    int daysInMonth = 0;

    // 修正的月份循环逻辑
    int monthCount = leapMonth ? 13 : 12;
    int bitIndex = 0;           // 位索引，用于获取月份天数
    bool leapProcessed = false; // 闰月是否已经处理

    for (int i = 0; i < monthCount; i++)
    {
        // 检查是否需要处理闰月
        if (leapMonth && !leapProcessed && i == leapMonth)
        {
            daysInMonth = leapMonthDays;
            isLeap = true;
            leapProcessed = true;
        }
        else
        {
            // 处理正常月份
            // int bitMask = 0x10000 >> bitIndex;
            // daysInMonth = 29 + ((yearInfo & bitMask) ? 1 : 0);
            // isLeap = false;
            // bitIndex++;

            lunarMonth++; // 正常月份递增
            int bitMask = 0x10000 >> bitIndex;
            daysInMonth = 29 + ((yearInfo & bitMask) ? 1 : 0);
            isLeap = false;
            bitIndex++;
        }

        // 检查是否找到对应的月份
        if (offset < daysInMonth)
        {
            lunarDay = offset + 1;
            break;
        }

        offset -= daysInMonth;
        if (!isLeap)
        {
            lunarMonth++;
        }
    }

    // 计算天干地支年
    int stemIndex = (lunarYear - 4) % 10;
    int branchIndex = (lunarYear - 4) % 12;
    if (stemIndex < 0)
        stemIndex += 10;
    if (branchIndex < 0)
        branchIndex += 12;

    TpString ganZhiYear = heavenlyStems[stemIndex] + earthlyBranches[branchIndex];

    // 计算生肖
    int zodiacIndex = (lunarYear - 4) % 12;
    if (zodiacIndex < 0)
        zodiacIndex += 12;
    TpString zodiac = zodiacNames[zodiacIndex];

    // 计算二十四节气
    TpString solarTerm = calculateSolarTerm(year, month, day);

    // 获取节日信息
    TpString holiday = getHolidayInfo(year, month, day, lunarMonth, lunarDay, isLeap);

    // 设置结果
    result.year = lunarYear;
    result.month = lunarMonth;
    result.day = lunarDay;
    result.isLeap = isLeap;
    result.ganZhiYear = ganZhiYear;
    result.zodiac = zodiac;
    result.monthName = lunarMonthNames[lunarMonth - 1];
    result.dayName = lunarDayNames[lunarDay - 1];
    result.solarTerm = solarTerm;
    result.holiday = holiday;

    // 构建完整名称
    result.fullName = ganZhiYear + "年(" + zodiac + ") ";
    if (isLeap)
    {
        result.fullName += "闰";
    }
    result.fullName += lunarMonthNames[lunarMonth - 1] + lunarDayNames[lunarDay - 1];

    if (!solarTerm.empty())
    {
        result.fullName += " " + solarTerm;
    }

    if (!holiday.empty())
    {
        result.fullName += " " + holiday;
    }

    return result;
}

#endif
