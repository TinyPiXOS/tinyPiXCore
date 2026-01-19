#include "TpDateTime.h"
#include <ctime>
#include <sstream>
#include <cstring>
#include <stdexcept>
#include <sys/time.h>
#include <cmath>

struct TpDateTimeData
{
    int64_t msecsSinceEpoch; // UTC时间戳（毫秒精度）

    TpDateTimeData(int64_t msecs = 0) : msecsSinceEpoch(msecs) {}

    // 转换为本地时间分解结构
    std::tm localTm() const
    {
        std::time_t secs = msecsSinceEpoch / 1000;
        std::tm tm;
        localtime_r(&secs, &tm);
        return tm;
    }

    // 从分解结构构造
    static int64_t fromLocalTm(const std::tm &tm, int ms)
    {
        return mktime(const_cast<std::tm *>(&tm)) * 1000LL + ms;
    }
};

// 辅助函数：获取当前UTC时间戳（毫秒）
static int64_t systemCurrentMSecs()
{
    struct timeval tv;
    gettimeofday(&tv, nullptr);
    return tv.tv_sec * 1000LL + tv.tv_usec / 1000;
}

TpDateTime::TpDateTime()
{
    TpDateTimeData *timeData = new TpDateTimeData();
    data_ = timeData;
}

TpDateTime::~TpDateTime()
{
    TpDateTimeData *timeData = static_cast<TpDateTimeData *>(data_);
    if (timeData)
    {
        delete timeData;
        timeData = nullptr;
        data_ = nullptr;
    }
}

TpDateTime TpDateTime::currentDateTime()
{
    return TpDateTime::fromMSecsSinceEpoch(systemCurrentMSecs());
}

TpDateTime TpDateTime::fromString(const TpString &s, const TpString &format)
{
    // 分割日期时间部分
    auto parts = s.split(' ');
    TpDate d = TpDate::fromString(parts[0], format.substr(0, format.find(' ')));
    TpTime t = TpTime::fromString(parts[1], format.substr(format.find(' ') + 1));

    std::tm tm = {};
    tm.tm_year = d.year() - 1900;
    tm.tm_mon = d.month() - 1;
    tm.tm_mday = d.day();
    tm.tm_hour = t.hour();
    tm.tm_min = t.minute();
    tm.tm_sec = t.second();

    return TpDateTime::fromMSecsSinceEpoch(TpDateTimeData::fromLocalTm(tm, t.msec()));
}

TpDateTime TpDateTime::fromMSecsSinceEpoch(int64_t msecs)
{
    TpDateTime dt;
    dt.setMSecsSinceEpoch(msecs);
    return dt;
}

TpDateTime TpDateTime::fromSecsSinceEpoch(int64_t secs)
{
    return fromMSecsSinceEpoch(secs * 1000);
}

int64_t TpDateTime::currentMSecsSinceEpoch() noexcept
{
    return systemCurrentMSecs();
}

int64_t TpDateTime::currentSecsSinceEpoch() noexcept
{
    return systemCurrentMSecs() / 1000;
}

TpDate TpDateTime::date() const
{
    auto tm = static_cast<TpDateTimeData *>(data_)->localTm();
    return TpDate(tm.tm_year + 1900, tm.tm_mon + 1, tm.tm_mday);
}

TpTime TpDateTime::time() const
{
    auto tm = static_cast<TpDateTimeData *>(data_)->localTm();
    int ms = static_cast<TpDateTimeData *>(data_)->msecsSinceEpoch % 1000;
    return TpTime(tm.tm_hour, tm.tm_min, tm.tm_sec, ms);
}

int64_t TpDateTime::toMSecsSinceEpoch() const
{
    return static_cast<TpDateTimeData *>(data_)->msecsSinceEpoch;
}

int64_t TpDateTime::toSecsSinceEpoch() const
{
    return toMSecsSinceEpoch() / 1000;
}

void TpDateTime::setMSecsSinceEpoch(int64_t msecs)
{
    TpDateTimeData *timeData = static_cast<TpDateTimeData *>(data_);
    timeData->msecsSinceEpoch = msecs;
}

void TpDateTime::setSecsSinceEpoch(int64_t secs)
{
    setMSecsSinceEpoch(secs * 1000);
}

void TpDateTime::setDate(const TpDate &date)
{
    TpTime t = time();
    std::tm tm = {};
    tm.tm_year = date.year() - 1900;
    tm.tm_mon = date.month() - 1;
    tm.tm_mday = date.day();
    tm.tm_hour = t.hour();
    tm.tm_min = t.minute();
    tm.tm_sec = t.second();
    setMSecsSinceEpoch(TpDateTimeData::fromLocalTm(tm, t.msec()));
}

void TpDateTime::setTime(const TpTime &time)
{
    TpDate d = date();
    std::tm tm = {};
    tm.tm_year = d.year() - 1900;
    tm.tm_mon = d.month() - 1;
    tm.tm_mday = d.day();
    tm.tm_hour = time.hour();
    tm.tm_min = time.minute();
    tm.tm_sec = time.second();
    setMSecsSinceEpoch(TpDateTimeData::fromLocalTm(tm, time.msec()));
}

TpString TpDateTime::toString(const TpString &format) const
{
    TpDate d = date();
    TpTime t = time();
    return d.toString(format.substr(0, format.find(' '))) + " " +
           t.toString(format.substr(format.find(' ') + 1));
}

TpDateTime TpDateTime::addDays(int64_t days) const
{
    return fromMSecsSinceEpoch(toMSecsSinceEpoch() + days * 86400000LL);
}

TpDateTime TpDateTime::addMonths(int32_t months) const
{
    TpDate d = date().addMonths(months);
    return TpDateTime::fromString(
        d.toString("yyyy-MM-dd") + " " + time().toString("HH:mm:ss.zzz"),
        "yyyy-MM-dd HH:mm:ss.zzz");
}

TpDateTime TpDateTime::addYears(int32_t years) const
{
    return addMonths(years * 12);
}

TpDateTime TpDateTime::addSecs(int64_t secs) const
{
    return TpDateTime::fromMSecsSinceEpoch(toMSecsSinceEpoch() + secs * 1000);
}

TpDateTime TpDateTime::addMSecs(int64_t msecs) const
{
    return fromMSecsSinceEpoch(toMSecsSinceEpoch() + msecs);
}

int64_t TpDateTime::daysTo(const TpDateTime &other) const
{
    return (other.date().toJulianDay() - date().toJulianDay());
}

int64_t TpDateTime::secsTo(const TpDateTime &other) const
{
    return (other.toMSecsSinceEpoch() - toMSecsSinceEpoch()) / 1000;
}

int64_t TpDateTime::msecsTo(const TpDateTime &other) const
{
    return other.toMSecsSinceEpoch() - toMSecsSinceEpoch();
}

TpDateTime &TpDateTime::operator=(const TpDateTime &other) noexcept
{
    if (this != &other)
    {
        setMSecsSinceEpoch(other.toMSecsSinceEpoch());
    }
    return *this;
}

bool TpDateTime::operator==(const TpDateTime &other) const
{
    return toMSecsSinceEpoch() == other.toMSecsSinceEpoch();
}

bool TpDateTime::operator<(const TpDateTime &other) const
{
    return toMSecsSinceEpoch() < other.toMSecsSinceEpoch();
}
