
#ifndef PATH_MAX
#define PATH_MAX 2048
#endif

#ifndef STDCALL
#define STDCALL
#endif

#define TP_DEF_VOID_TYPE_VAR(v) \
    typedef void v

#define TP_INVALIDATE_VALUE -1

#ifndef __TP_CORE_H
#define __TP_CORE_H

#include "typesDef.h"

#include <TpString.h>
#include <unistd.h>
#include <atomic>
#include <limits>
#include <cmath>

/// @brief 获取当前应用程序可执行程序所在绝对路径
/// @return 可执行程序绝对路径
static TpString applicationDirPath()
{
    char exePath[PATH_MAX] = {0};
    size_t len = readlink("/proc/self/exe", exePath, sizeof(exePath) - 1); // 读取符号链接

    // 错误处理
    if (len == -1)
    {
        throw std::runtime_error("Failed to read /proc/self/exe");
    }
    exePath[len] = '\0'; // 确保字符串终止

    // 提取父目录路径
    char *lastSlash = strrchr(exePath, '/');
    if (!lastSlash)
    {
        return TpString(); // 无效路径
    }

    // 处理根目录的特殊情况（如 "/usr" → "/"）
    if (lastSlash == exePath)
    {
        return TpString("/");
    }

    // 截取目录部分
    *lastSlash = '\0'; // 将最后一个 '/' 替换为终止符
    return TpString(exePath);
}

static uint32_t generateTimerId()
{
    static std::atomic<uint32_t> uniqueIdGenerator(0);
    return uniqueIdGenerator.fetch_add(1);
}

/// @brief 对两个浮点数判断是否相等
/// @param p1 浮点数1
/// @param p2 浮点数2
/// @return 相等返回true，否则返回false
static bool tpFuzzyCompare(double p1, double p2)
{
    // 使用机器精度的倍数作为容差
    static constexpr double epsilon = 10 * std::numeric_limits<double>::epsilon();

    // 处理无穷大和NaN的情况
    if (std::isinf(p1) || std::isinf(p2))
        return p1 == p2;

    if (std::isnan(p1) || std::isnan(p2))
        return false;

    // 处理两个数都非常接近零的情况
    if (std::fabs(p1 - p2) <= std::numeric_limits<double>::min())
        return true;

    // 使用相对误差进行比较
    double absMax = std::max(std::fabs(p1), std::fabs(p2));
    return std::fabs(p1 - p2) <= absMax * epsilon;
}

/// @brief 对两个浮点数判断是否相等
/// @param p1 浮点数1
/// @param p2 浮点数2
/// @return 相等返回true，否则返回false
static bool tpFuzzyCompare(float p1, float p2)
{
    // 使用机器精度的倍数作为容差
    static constexpr float epsilon = 10 * std::numeric_limits<float>::epsilon();

    // 处理无穷大和NaN的情况
    if (std::isinf(p1) || std::isinf(p2))
        return p1 == p2;

    if (std::isnan(p1) || std::isnan(p2))
        return false;

    // 处理两个数都非常接近零的情况
    if (std::fabs(p1 - p2) <= std::numeric_limits<float>::min())
        return true;

    // 使用相对误差进行比较
    float absMax = std::max(std::fabs(p1), std::fabs(p2));
    return std::fabs(p1 - p2) <= absMax * epsilon;
}

/// @brief 判断一个浮点数是否为0
/// @param d 浮点数
/// @return 为0返回true，否则返回false
static bool tpFuzzyIsNull(double d)
{
    // 定义容差值，可以根据需要调整
    static constexpr double epsilon = 1e-12;

    // 处理特殊值
    if (std::isnan(d))
        return false; // NaN 不被认为是零

    if (std::isinf(d))
        return false; // 无穷大不被认为是零

    // 判断是否近似为零
    return std::fabs(d) <= epsilon;
}

/// @brief 判断一个浮点数是否为0
/// @param d 浮点数
/// @return 为0返回true，否则返回false
static bool tpFuzzyIsNull(float f)
{
    // 定义容差值，可以根据需要调整
    static constexpr float epsilon = 1e-6f;

    // 处理特殊值
    if (std::isnan(f))
        return false; // NaN 不被认为是零

    if (std::isinf(f))
        return false; // 无穷大不被认为是零

    // 判断是否近似为零
    return std::fabs(f) <= epsilon;
}

#endif
