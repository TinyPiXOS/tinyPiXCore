#ifndef __TP_TIME_H
#define __TP_TIME_H

#include <tpUtils.h>
#include <tpString.h>

TP_DEF_VOID_TYPE_VAR(ItpTimeData);
/// @brief 时间类，提供时间相关操作
class tpTime
{
public:
    tpTime();
    explicit tpTime(int32_t h, int32_t m, int32_t s, int32_t ms = 0);
    ~tpTime();

    /// @brief 获取当前时间
    /// @return 事件对象
    static tpTime currentTime();
    /// @brief 将时间字符串转换为时间对象
    /// @param s 时间字符串
    /// @param format 格式化字符串 例如HH:mm
    /// @return 时间对象
    static tpTime fromString(const tpString &s, const tpString &format);

    /// @brief 获取时间小时值
    /// @return 小时
    int32_t hour() const;
    /// @brief 获取时间分钟值
    /// @return 分钟
    int32_t minute() const;
    /// @brief 获取时间秒值
    /// @return 秒
    int32_t second() const;
    /// @brief 获取时间毫秒值
    /// @return 毫秒
    int32_t msec() const;

    /// @brief 将时间对象转换为字符串
    /// @param format 格式化字符串 例如"HH:mm"
    /// @return 时间字符串
    tpString toString(const tpString &format) const;

    /// @brief 设置时间的时分秒毫秒
    /// @param h 小时
    /// @param m 分钟
    /// @param s 秒
    /// @param ms 毫秒
    /// @return 返回设置结果；如果时间取值错误，则返回false
    bool setHMS(int32_t h, int32_t m, int32_t s, int32_t ms = 0);

    /// @brief 对当前时间添加指定秒数
    /// @param secs 秒数
    /// @return 返回添加指定秒数后的事件对象
    tpTime addSecs(int32_t secs) const;
    /// @brief 从当前时间到指定时间的时间间隔 秒
    /// @param t 指定时间
    /// @return 指定时间在当前时间之后为正，反之为负值
    int32_t secsTo(const tpTime &t) const;

    /// @brief 对当前时间添加指定毫秒数
    /// @param ms 毫秒数
    /// @return 返回添加指定毫秒数后的事件对象
    tpTime addMSecs(int32_t ms) const;
    /// @brief 从当前时间到指定时间的时间间隔 毫秒
    /// @param t 指定时间
    /// @return 指定时间在当前时间之后为正，反之为负值
    int32_t msecsTo(const tpTime &t) const;

public:
    tpTime &operator=(const tpTime &other) noexcept;

    bool operator==(const tpTime &other) const;
    bool operator<(const tpTime &other) const;

    bool operator!=(const tpTime &other) const { return !(*this == other); }
    bool operator<=(const tpTime &other) const { return !(other < *this); }
    bool operator>(const tpTime &other) const { return other < *this; }
    bool operator>=(const tpTime &other) const { return !(*this < other); }

private:
    ItpTimeData *data_;
};

#endif
