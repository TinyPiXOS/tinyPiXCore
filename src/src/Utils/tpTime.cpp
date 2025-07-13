#include "tpTime.h"
#include <ctime>
#include <sys/time.h>
#include <sstream>
#include <cstdio>
#include <stdexcept>
#include <algorithm>

struct tpTimeData
{
    int32_t hour;
    int32_t minute;
    int32_t second;
    int32_t msec;

    tpTimeData(int32_t h = 0, int32_t m = 0, int32_t s = 0, int32_t ms = 0)
        : hour(h), minute(m), second(s), msec(ms) {}
};

tpTime::tpTime()
{
    tpTimeData *timeData = new tpTimeData();
    data_ = timeData;
}

tpTime::tpTime(int32_t h, int32_t m, int32_t s, int32_t ms)
{
    tpTimeData *timeData = new tpTimeData();
    data_ = timeData;
    setHMS(h, m, s, ms);
}

tpTime::~tpTime()
{
    tpTimeData *timeData = static_cast<tpTimeData *>(data_);
    if (timeData)
    {
        delete timeData;
        timeData = nullptr;
        data_ = nullptr;
    }
}

tpTime tpTime::currentTime()
{
    struct timeval tv;
    gettimeofday(&tv, nullptr);

    struct tm tm;
    localtime_r(&tv.tv_sec, &tm);
    return tpTime(tm.tm_hour, tm.tm_min, tm.tm_sec, tv.tv_usec / 1000);
}

tpTime tpTime::fromString(const tpString &s, const tpString &format)
{
    tpTimeData td(0, 0, 0, 0);
    const char *str = s.c_str();
    const char *fmt = format.c_str();
    int ms = 0;

    // 解析主要部分
    std::string converted_fmt = format.replace("HH", "%H")
                                    .replace("mm", "%M")
                                    .replace("ss", "%S");

    // 解析毫秒
    size_t zpos = format.find("zzz");
    if (zpos != tpString::npos)
    {
        converted_fmt.replace(zpos, 3, "%3d"); // 自定义3位数字占位符
    }

    // 使用sscanf解析
    int parsed = sscanf(str, converted_fmt.c_str(),
                        &td.hour, &td.minute, &td.second, &ms);

    // 验证结果
    tpTime checkTime;
    if (parsed < 3 || !checkTime.setHMS(td.hour, td.minute, td.second, ms))
    {
        throw std::invalid_argument("Invalid time string or format");
    }
    return tpTime(td.hour, td.minute, td.second, ms);
}

int32_t tpTime::hour() const
{
    tpTimeData *timeData = static_cast<tpTimeData *>(data_);
    return timeData->hour;
}

int32_t tpTime::minute() const
{
    tpTimeData *timeData = static_cast<tpTimeData *>(data_);
    return timeData->minute;
}

int32_t tpTime::second() const
{
    tpTimeData *timeData = static_cast<tpTimeData *>(data_);
    return timeData->second;
}

int32_t tpTime::msec() const
{
    tpTimeData *timeData = static_cast<tpTimeData *>(data_);
    return timeData->msec;
}

tpString tpTime::toString(const tpString &format) const
{
    char buf[64];
    std::string fmt = format.replace("HH", "%02d")
                          .replace("mm", "%02d")
                          .replace("ss", "%02d")
                          .replace("zzz", "%03d");

    snprintf(buf, sizeof(buf), fmt.c_str(),
             hour(), minute(), second(), msec());
    return tpString(buf);
}

bool tpTime::setHMS(int32_t h, int32_t m, int32_t s, int32_t ms)
{
    tpTimeData *timeData = static_cast<tpTimeData *>(data_);
    if (h < 0 || h > 23 || m < 0 || m > 59 || s < 0 || s > 59 || ms < 0 || ms > 999)
    {
        return false;
    }
    timeData->hour = h;
    timeData->minute = m;
    timeData->second = s;
    timeData->msec = ms;
    return true;
}

tpTime tpTime::addSecs(int32_t secs) const
{
    int total_secs = hour() * 3600 + minute() * 60 + second() + secs;
    total_secs %= 86400;
    if (total_secs < 0)
        total_secs += 86400;

    return tpTime(
        total_secs / 3600,
        (total_secs % 3600) / 60,
        total_secs % 60,
        msec());
}

int32_t tpTime::secsTo(const tpTime &t) const
{
    return (t.hour() - hour()) * 3600 +
           (t.minute() - minute()) * 60 +
           (t.second() - second());
}

tpTime tpTime::addMSecs(int32_t ms) const
{
    int64_t total_ms = hour() * 3600000 + minute() * 60000 + second() * 1000 + msec() + ms;
    total_ms %= 86400000LL;
    if (total_ms < 0)
        total_ms += 86400000LL;

    return tpTime(
        (total_ms / 3600000) % 24,
        (total_ms / 60000) % 60,
        (total_ms / 1000) % 60,
        total_ms % 1000);
}

int32_t tpTime::msecsTo(const tpTime &t) const
{
    return (t.hour() - hour()) * 3600000 +
           (t.minute() - minute()) * 60000 +
           (t.second() - second()) * 1000 +
           (t.msec() - msec());
}

tpTime &tpTime::operator=(const tpTime &other) noexcept
{
    auto d1 = static_cast<tpTimeData *>(data_);
    auto d2 = static_cast<tpTimeData *>(other.data_);

    d1->hour = d2->hour;
    d1->minute = d2->minute;
    d1->second = d2->second;
    d1->msec = d2->msec;

    return *this;
}

bool tpTime::operator==(const tpTime &other) const
{
    auto d1 = static_cast<tpTimeData *>(data_);
    auto d2 = static_cast<tpTimeData *>(other.data_);
    return std::tie(d1->hour, d1->minute, d1->second, d1->msec) ==
           std::tie(d2->hour, d2->minute, d2->second, d2->msec);
}

bool tpTime::operator<(const tpTime &other) const
{
    auto d1 = static_cast<tpTimeData *>(data_);
    auto d2 = static_cast<tpTimeData *>(other.data_);
    return std::tie(d1->hour, d1->minute, d1->second, d1->msec) <
           std::tie(d2->hour, d2->minute, d2->second, d2->msec);
}