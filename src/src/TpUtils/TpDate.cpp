#include "TpDate.h"
#include <ctime>
#include <iostream>
#include <sstream>
#include <stdexcept>
#include <cmath>

struct TpDateData
{
    int32_t year;
    int32_t month;
    int32_t day;

    TpDateData(int32_t y = 1970, int32_t m = 1, int32_t d = 1)
        : year(y), month(m), day(d)
    {
    }
};

std::tm toTm(const TpDateData *d)
{
    std::tm tm = {};
    tm.tm_year = d->year - 1900;
    tm.tm_mon = d->month - 1;
    tm.tm_mday = d->day;
    return tm;
}

bool isLeapYear(int32_t year)
{
    return (year % 4 == 0 && year % 100 != 0) || (year % 400 == 0);
}

int32_t monthDays(int32_t year, int32_t month)
{
    static const int32_t days[] = {31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31};
    return days[month - 1] + (month == 2 && isLeapYear(year));
}

TpDate::TpDate()
{
    TpDateData *dateData = new TpDateData();
    data_ = dateData;
}

TpDate::TpDate(const int32_t &y, const int32_t &m, const int32_t &d)
{
    TpDateData *dateData = new TpDateData();
    data_ = dateData;

    dateData->year = y;
    dateData->month = m;
    dateData->day = d;
}

TpDate::~TpDate()
{
    TpDateData *dateData = static_cast<TpDateData *>(data_);
    if (dateData)
    {
        delete dateData;
        dateData = nullptr;
        data_ = nullptr;
    }
}

TpDate TpDate::fromString(const TpString &s, const TpString &format)
{
    std::tm tm = {};
    const char *str = s.c_str();
    const char *fmt = format.c_str();

    // 转换格式符 (yyyy -> %Y, MM -> %m等)
    TpString converted_fmt = format.replace("yyyy", "%Y")
                                 .replace("MM", "%m")
                                 .replace("dd", "%d");

    if (!strptime(str, converted_fmt.c_str(), &tm))
    {
        throw std::invalid_argument("Invalid date string or format");
    }
    return TpDate(tm.tm_year + 1900, tm.tm_mon + 1, tm.tm_mday);
}

TpDate TpDate::currentDate()
{
    std::time_t t = std::time(nullptr);
    std::tm tm;
    localtime_r(&t, &tm);
    return TpDate(tm.tm_year + 1900, tm.tm_mon + 1, tm.tm_mday);
}

int32_t TpDate::year() const
{
    TpDateData *dateData = static_cast<TpDateData *>(data_);
    return dateData->year;
}

int32_t TpDate::month() const
{
    TpDateData *dateData = static_cast<TpDateData *>(data_);
    return dateData->month;
}

int32_t TpDate::day() const
{
    TpDateData *dateData = static_cast<TpDateData *>(data_);
    return dateData->day;
}

void TpDate::setYear(const int32_t &year)
{
    TpDateData *dateData = static_cast<TpDateData *>(data_);
    dateData->year = year;
}

void TpDate::setMonth(const int32_t &month)
{
    TpDateData *dateData = static_cast<TpDateData *>(data_);
    dateData->month = month;
}

void TpDate::setDay(const int32_t &day)
{
    TpDateData *dateData = static_cast<TpDateData *>(data_);
    dateData->day = day;
}

int32_t TpDate::dayOfWeek() const
{
    TpDateData *dateData = static_cast<TpDateData *>(data_);

    std::tm tm = toTm(dateData);
    // 自动填充tm_wday
    std::mktime(&tm);
    // 1=Mon...7=Sun
    return (tm.tm_wday == 0) ? 7 : tm.tm_wday;
}

int32_t TpDate::dayOfYear() const
{
    TpDateData *dateData = static_cast<TpDateData *>(data_);

    std::tm tm = toTm(dateData);
    std::mktime(&tm);
    return tm.tm_yday + 1;
}

int32_t TpDate::daysInMonth() const
{
    TpDateData *dateData = static_cast<TpDateData *>(data_);
    return monthDays(dateData->year, dateData->month);
}

int32_t TpDate::daysInYear() const
{
    return isLeapYear(year()) ? 366 : 365;
}

TpDate TpDate::addDays(const int64_t &days) const
{
    TpDateData *dateData = static_cast<TpDateData *>(data_);

    std::tm tm = toTm(dateData);
    std::time_t t = std::mktime(&tm);
    // 精确计算需考虑夏令时
    t += days * 86400;
    localtime_r(&t, &tm);
    return TpDate(tm.tm_year + 1900, tm.tm_mon + 1, tm.tm_mday);
}

TpDate TpDate::addMonths(const int32_t &months) const
{
    TpDateData *dateData = static_cast<TpDateData *>(data_);

    int totalMonths = dateData->year * 12 + dateData->month - 1 + months;
    int newYear = totalMonths / 12;
    int newMonth = totalMonths % 12 + 1;
    int newDay = std::min(dateData->day, monthDays(newYear, newMonth));
    return TpDate(newYear, newMonth, newDay);
}

TpDate TpDate::addYears(const int32_t &years) const
{
    TpDateData *dateData = static_cast<TpDateData *>(data_);

    int newYear = dateData->year + years;
    int newDay = std::min(dateData->day, monthDays(newYear, dateData->month));
    return TpDate(newYear, dateData->month, newDay);
}

TpString TpDate::toString(const TpString &format) const
{
    TpDateData *dateData = static_cast<TpDateData *>(data_);

    std::tm tm = toTm(dateData);
    char buf[64];

    TpString converted_fmt = format.replace("yyyy", "%Y")
                                 .replace("MM", "%m")
                                 .replace("dd", "%d");

    strftime(buf, sizeof(buf), converted_fmt.c_str(), &tm);
    return TpString(buf);
}

int64_t TpDate::toJulianDay() const
{
    int a = (14 - month()) / 12;
    int y = year() + 4800 - a;
    int m = month() + 12 * a - 3;
    return day() + (153 * m + 2) / 5 + 365 * y + y / 4 - y / 100 + y / 400 - 32045;
}

TpDate &TpDate::operator=(const TpDate &other) noexcept
{
    auto d1 = static_cast<TpDateData *>(data_);
    auto d2 = static_cast<TpDateData *>(other.data_);

    d1->day = d2->day;
    d1->month = d2->month;
    d1->year = d2->year;

    return *this;
}

bool TpDate::operator==(const TpDate &other) const
{
    auto d1 = static_cast<TpDateData *>(data_);
    auto d2 = static_cast<TpDateData *>(other.data_);
    return d1->year == d2->year &&
           d1->month == d2->month &&
           d1->day == d2->day;
}

bool TpDate::operator<(const TpDate &other) const
{
    auto d1 = static_cast<TpDateData *>(data_);
    auto d2 = static_cast<TpDateData *>(other.data_);
    return std::tie(d1->year, d1->month, d1->day) <
           std::tie(d2->year, d2->month, d2->day);
}
