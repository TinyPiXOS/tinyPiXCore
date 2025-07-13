#include "tpDateTime.h"
#include <ctime>
#include <sstream>
#include <cstring>
#include <stdexcept>
#include <sys/time.h>
#include <cmath>

struct tpDateTimeData
{
    int64_t msecsSinceEpoch; // UTC时间戳（毫秒精度）

    tpDateTimeData(int64_t msecs = 0) : msecsSinceEpoch(msecs) {}

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

tpDateTime::tpDateTime()
{
    tpDateTimeData *timeData = new tpDateTimeData();
    data_ = timeData;
}

tpDateTime::~tpDateTime()
{
    tpDateTimeData *timeData = static_cast<tpDateTimeData *>(data_);
    if (timeData)
    {
        delete timeData;
        timeData = nullptr;
        data_ = nullptr;
    }
}

tpDateTime tpDateTime::currentDateTime()
{
    return tpDateTime::fromMSecsSinceEpoch(systemCurrentMSecs());
}

tpDateTime tpDateTime::fromString(const tpString &s, const tpString &format)
{
    // 分割日期时间部分
    auto parts = s.split(' ');
    tpDate d = tpDate::fromString(parts[0], format.substr(0, format.find(' ')));
    tpTime t = tpTime::fromString(parts[1], format.substr(format.find(' ') + 1));

    std::tm tm = {};
    tm.tm_year = d.year() - 1900;
    tm.tm_mon = d.month() - 1;
    tm.tm_mday = d.day();
    tm.tm_hour = t.hour();
    tm.tm_min = t.minute();
    tm.tm_sec = t.second();

    return tpDateTime::fromMSecsSinceEpoch(tpDateTimeData::fromLocalTm(tm, t.msec()));
}

tpDateTime tpDateTime::fromMSecsSinceEpoch(int64_t msecs)
{
    tpDateTime dt;
    dt.setMSecsSinceEpoch(msecs);
    return dt;
}

tpDateTime tpDateTime::fromSecsSinceEpoch(int64_t secs)
{
    return fromMSecsSinceEpoch(secs * 1000);
}

int64_t tpDateTime::currentMSecsSinceEpoch() noexcept
{
    return systemCurrentMSecs();
}

int64_t tpDateTime::currentSecsSinceEpoch() noexcept
{
    return systemCurrentMSecs() / 1000;
}

tpDate tpDateTime::date() const
{
    auto tm = static_cast<tpDateTimeData *>(data_)->localTm();
    return tpDate(tm.tm_year + 1900, tm.tm_mon + 1, tm.tm_mday);
}

tpTime tpDateTime::time() const
{
    auto tm = static_cast<tpDateTimeData *>(data_)->localTm();
    int ms = static_cast<tpDateTimeData *>(data_)->msecsSinceEpoch % 1000;
    return tpTime(tm.tm_hour, tm.tm_min, tm.tm_sec, ms);
}

int64_t tpDateTime::toMSecsSinceEpoch() const
{
    return static_cast<tpDateTimeData *>(data_)->msecsSinceEpoch;
}

int64_t tpDateTime::toSecsSinceEpoch() const
{
    return toMSecsSinceEpoch() / 1000;
}

void tpDateTime::setMSecsSinceEpoch(int64_t msecs)
{
    tpDateTimeData *timeData = static_cast<tpDateTimeData *>(data_);
    timeData->msecsSinceEpoch = msecs;
}

void tpDateTime::setSecsSinceEpoch(int64_t secs)
{
    setMSecsSinceEpoch(secs * 1000);
}

void tpDateTime::setDate(const tpDate &date)
{
    tpTime t = time();
    std::tm tm = {};
    tm.tm_year = date.year() - 1900;
    tm.tm_mon = date.month() - 1;
    tm.tm_mday = date.day();
    tm.tm_hour = t.hour();
    tm.tm_min = t.minute();
    tm.tm_sec = t.second();
    setMSecsSinceEpoch(tpDateTimeData::fromLocalTm(tm, t.msec()));
}

void tpDateTime::setTime(const tpTime &time)
{
    tpDate d = date();
    std::tm tm = {};
    tm.tm_year = d.year() - 1900;
    tm.tm_mon = d.month() - 1;
    tm.tm_mday = d.day();
    tm.tm_hour = time.hour();
    tm.tm_min = time.minute();
    tm.tm_sec = time.second();
    setMSecsSinceEpoch(tpDateTimeData::fromLocalTm(tm, time.msec()));
}

tpString tpDateTime::toString(const tpString &format) const
{
    tpDate d = date();
    tpTime t = time();
    return d.toString(format.substr(0, format.find(' '))) + " " +
           t.toString(format.substr(format.find(' ') + 1));
}

tpDateTime tpDateTime::addDays(int64_t days) const
{
    return fromMSecsSinceEpoch(toMSecsSinceEpoch() + days * 86400000LL);
}

tpDateTime tpDateTime::addMonths(int32_t months) const
{
    tpDate d = date().addMonths(months);
    return tpDateTime::fromString(
        d.toString("yyyy-MM-dd") + " " + time().toString("HH:mm:ss.zzz"),
        "yyyy-MM-dd HH:mm:ss.zzz");
}

tpDateTime tpDateTime::addYears(int32_t years) const
{
    return addMonths(years * 12);
}

tpDateTime tpDateTime::addSecs(int64_t secs) const
{
    return tpDateTime::fromMSecsSinceEpoch(toMSecsSinceEpoch() + secs * 1000);
}

tpDateTime tpDateTime::addMSecs(int64_t msecs) const
{
    return fromMSecsSinceEpoch(toMSecsSinceEpoch() + msecs);
}

int64_t tpDateTime::daysTo(const tpDateTime &other) const
{
    return (other.date().toJulianDay() - date().toJulianDay());
}

int64_t tpDateTime::secsTo(const tpDateTime &other) const
{
    return (other.toMSecsSinceEpoch() - toMSecsSinceEpoch()) / 1000;
}

int64_t tpDateTime::msecsTo(const tpDateTime &other) const
{
    return other.toMSecsSinceEpoch() - toMSecsSinceEpoch();
}

tpDateTime &tpDateTime::operator=(const tpDateTime &other) noexcept
{
    if (this != &other)
    {
        setMSecsSinceEpoch(other.toMSecsSinceEpoch());
    }
    return *this;
}

bool tpDateTime::operator==(const tpDateTime &other) const
{
    return toMSecsSinceEpoch() == other.toMSecsSinceEpoch();
}

bool tpDateTime::operator<(const tpDateTime &other) const
{
    return toMSecsSinceEpoch() < other.toMSecsSinceEpoch();
}
