#include "TpChineseCalendar.h"
#include "TpChineseCalendar_p.h"

struct TpChineseCalendarDate
{
    int32_t year = 0;
    int32_t month = 0;
    int32_t day = 0;

    LunarDate chineseCalendarDate;
};

TpChineseCalendar::TpChineseCalendar()
{
    TpChineseCalendarDate *calendarDate = new TpChineseCalendarDate();
    data_ = calendarDate;
}

TpChineseCalendar::TpChineseCalendar(int32_t year, int32_t month, int32_t day)
{
    TpChineseCalendarDate *calendarDate = new TpChineseCalendarDate();
    calendarDate->year = year;
    calendarDate->month = month;
    calendarDate->day = day;
    calendarDate->chineseCalendarDate = solarToLunar(year, month, day);
    data_ = calendarDate;
}

TpChineseCalendar::TpChineseCalendar(const TpDate &date)
{
    TpChineseCalendarDate *calendarDate = new TpChineseCalendarDate();
    calendarDate->year = date.year();
    calendarDate->month = date.month();
    calendarDate->day = date.day();
    calendarDate->chineseCalendarDate = solarToLunar(calendarDate->year, calendarDate->month, calendarDate->day);
    data_ = calendarDate;
}

TpChineseCalendar::~TpChineseCalendar()
{
    TpChineseCalendarDate *calendarDate = static_cast<TpChineseCalendarDate *>(data_);
    if (calendarDate)
    {
        delete calendarDate;
        calendarDate = nullptr;
        data_ = nullptr;
    }
}

int32_t TpChineseCalendar::year() const
{
    TpChineseCalendarDate *calendarDate = static_cast<TpChineseCalendarDate *>(data_);
    return calendarDate->year;
}

int32_t TpChineseCalendar::month() const
{
    TpChineseCalendarDate *calendarDate = static_cast<TpChineseCalendarDate *>(data_);
    return calendarDate->month;
}

int32_t TpChineseCalendar::day() const
{
    TpChineseCalendarDate *calendarDate = static_cast<TpChineseCalendarDate *>(data_);
    return calendarDate->day;
}

void TpChineseCalendar::setYear(const int32_t &year)
{
    TpChineseCalendarDate *calendarDate = static_cast<TpChineseCalendarDate *>(data_);
    calendarDate->year = year;
}

void TpChineseCalendar::setMonth(const int32_t &month)
{
    TpChineseCalendarDate *calendarDate = static_cast<TpChineseCalendarDate *>(data_);
    calendarDate->month = month;
}

void TpChineseCalendar::setDay(const int32_t &day)
{
    TpChineseCalendarDate *calendarDate = static_cast<TpChineseCalendarDate *>(data_);
    calendarDate->day = day;
}

bool TpChineseCalendar::isLeap()
{
    TpChineseCalendarDate *calendarDate = static_cast<TpChineseCalendarDate *>(data_);
    return calendarDate->chineseCalendarDate.isLeap;
}

TpString TpChineseCalendar::ganZhiYear()
{
    TpChineseCalendarDate *calendarDate = static_cast<TpChineseCalendarDate *>(data_);
    return calendarDate->chineseCalendarDate.ganZhiYear;
}

TpString TpChineseCalendar::zodiac()
{
    TpChineseCalendarDate *calendarDate = static_cast<TpChineseCalendarDate *>(data_);
    return calendarDate->chineseCalendarDate.zodiac;
}

TpString TpChineseCalendar::monthName()
{
   TpChineseCalendarDate *calendarDate = static_cast<TpChineseCalendarDate *>(data_);
    return calendarDate->chineseCalendarDate.monthName;
}

TpString TpChineseCalendar::dayName()
{
    TpChineseCalendarDate *calendarDate = static_cast<TpChineseCalendarDate *>(data_);
    return calendarDate->chineseCalendarDate.dayName;
}

TpString TpChineseCalendar::fullName()
{
    TpChineseCalendarDate *calendarDate = static_cast<TpChineseCalendarDate *>(data_);
    return calendarDate->chineseCalendarDate.fullName;
}

TpString TpChineseCalendar::solarTerm()
{
    TpChineseCalendarDate *calendarDate = static_cast<TpChineseCalendarDate *>(data_);
    return calendarDate->chineseCalendarDate.solarTerm;
}

TpString TpChineseCalendar::holiday()
{
    TpChineseCalendarDate *calendarDate = static_cast<TpChineseCalendarDate *>(data_);
    return calendarDate->chineseCalendarDate.holiday;
}
