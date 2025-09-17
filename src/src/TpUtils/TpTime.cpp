#include "TpTime.h"
#include <ctime>
#include <sys/time.h>
#include <sstream>
#include <cstdio>
#include <stdexcept>
#include <algorithm>

struct TpTimeData
{
    int32_t hour;
    int32_t minute;
    int32_t second;
    int32_t msec;

    TpTimeData(int32_t h = 0, int32_t m = 0, int32_t s = 0, int32_t ms = 0)
        : hour(h), minute(m), second(s), msec(ms) {}
};

TpTime::TpTime()
{
    TpTimeData *timeData = new TpTimeData();
    data_ = timeData;
}

TpTime::TpTime(int32_t h, int32_t m, int32_t s, int32_t ms)
{
    TpTimeData *timeData = new TpTimeData();
    data_ = timeData;
    setHMS(h, m, s, ms);
}

TpTime::~TpTime()
{
    TpTimeData *timeData = static_cast<TpTimeData *>(data_);
    if (timeData)
    {
        delete timeData;
        timeData = nullptr;
        data_ = nullptr;
    }
}

TpTime TpTime::currentTime()
{
    struct timeval tv;
    gettimeofday(&tv, nullptr);

    struct tm tm;
    localtime_r(&tv.tv_sec, &tm);
    return TpTime(tm.tm_hour, tm.tm_min, tm.tm_sec, tv.tv_usec / 1000);
}

TpTime TpTime::fromString(const TpString &s, const TpString &format)
{
    TpTimeData td(0, 0, 0, 0);
    const char *str = s.c_str();
    const char *fmt = format.c_str();
    int ms = 0;

    // 解析主要部分
    std::string converted_fmt = format.replace("HH", "%H")
                                    .replace("mm", "%M")
                                    .replace("ss", "%S");

    // 解析毫秒
    size_t zpos = format.find("zzz");
    if (zpos != TpString::npos)
    {
        converted_fmt.replace(zpos, 3, "%3d"); // 自定义3位数字占位符
    }

    // 使用sscanf解析
    int parsed = sscanf(str, converted_fmt.c_str(),
                        &td.hour, &td.minute, &td.second, &ms);

    // 验证结果
    TpTime checkTime;
    if (parsed < 3 || !checkTime.setHMS(td.hour, td.minute, td.second, ms))
    {
        throw std::invalid_argument("Invalid time string or format");
    }
    return TpTime(td.hour, td.minute, td.second, ms);
}

int32_t TpTime::hour() const
{
    TpTimeData *timeData = static_cast<TpTimeData *>(data_);
    return timeData->hour;
}

int32_t TpTime::minute() const
{
    TpTimeData *timeData = static_cast<TpTimeData *>(data_);
    return timeData->minute;
}

int32_t TpTime::second() const
{
    TpTimeData *timeData = static_cast<TpTimeData *>(data_);
    return timeData->second;
}

int32_t TpTime::msec() const
{
    TpTimeData *timeData = static_cast<TpTimeData *>(data_);
    return timeData->msec;
}

TpString TpTime::toString(const TpString &format) const
{
    char buf[64];
    std::string fmt = format.replace("HH", "%02d")
                          .replace("mm", "%02d")
                          .replace("ss", "%02d")
                          .replace("zzz", "%03d");

    snprintf(buf, sizeof(buf), fmt.c_str(),
             hour(), minute(), second(), msec());
    return TpString(buf);
}

bool TpTime::setHMS(int32_t h, int32_t m, int32_t s, int32_t ms)
{
    TpTimeData *timeData = static_cast<TpTimeData *>(data_);
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

TpTime TpTime::addSecs(int32_t secs) const
{
    int total_secs = hour() * 3600 + minute() * 60 + second() + secs;
    total_secs %= 86400;
    if (total_secs < 0)
        total_secs += 86400;

    return TpTime(
        total_secs / 3600,
        (total_secs % 3600) / 60,
        total_secs % 60,
        msec());
}

int32_t TpTime::secsTo(const TpTime &t) const
{
    return (t.hour() - hour()) * 3600 +
           (t.minute() - minute()) * 60 +
           (t.second() - second());
}

TpTime TpTime::addMSecs(int64_t ms) const
{
    int64_t total_ms = hour() * 3600000 + minute() * 60000 + second() * 1000 + msec() + ms;
    total_ms %= 86400000LL;
    if (total_ms < 0)
        total_ms += 86400000LL;

    return TpTime(
        (total_ms / 3600000) % 24,
        (total_ms / 60000) % 60,
        (total_ms / 1000) % 60,
        total_ms % 1000);
}

int64_t TpTime::msecsTo(const TpTime &t) const
{
    return (t.hour() - hour()) * 3600000 +
           (t.minute() - minute()) * 60000 +
           (t.second() - second()) * 1000 +
           (t.msec() - msec());
}

TpTime &TpTime::operator=(const TpTime &other) noexcept
{
    auto d1 = static_cast<TpTimeData *>(data_);
    auto d2 = static_cast<TpTimeData *>(other.data_);

    d1->hour = d2->hour;
    d1->minute = d2->minute;
    d1->second = d2->second;
    d1->msec = d2->msec;

    return *this;
}

bool TpTime::operator==(const TpTime &other) const
{
    auto d1 = static_cast<TpTimeData *>(data_);
    auto d2 = static_cast<TpTimeData *>(other.data_);
    return std::tie(d1->hour, d1->minute, d1->second, d1->msec) ==
           std::tie(d2->hour, d2->minute, d2->second, d2->msec);
}

bool TpTime::operator<(const TpTime &other) const
{
    auto d1 = static_cast<TpTimeData *>(data_);
    auto d2 = static_cast<TpTimeData *>(other.data_);
    return std::tie(d1->hour, d1->minute, d1->second, d1->msec) <
           std::tie(d2->hour, d2->minute, d2->second, d2->msec);
}