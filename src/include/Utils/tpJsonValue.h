#ifndef TP_JSON_VALUE
#define TP_JSON_VALUE

#include <tpString.h>
#include <cstdint>

#include <JsonStructPackage/rapidjson/rapidjson.h>
#include <JsonStructPackage/rapidjson/document.h>
#include <JsonStructPackage/rapidjson/stringbuffer.h>
#include <JsonStructPackage/rapidjson/writer.h>

class tpJsonObject;
class tpJsonArray;

/// @brief JSON值类，表示JSON中的各种数据类型
class tpJsonValue
{
public:
    rapidjson::Value value_; ///< @brief 底层的rapidjson值对象

    /// @brief 默认构造函数，创建空值（Null）
    tpJsonValue() : value_(rapidjson::kNullType) {}

    /// @brief 从布尔值构造JSON值
    /// @param b 布尔值
    tpJsonValue(bool b) : value_(b) {}

    /// @brief 从无符号32位整数构造JSON值
    /// @param i 无符号32位整数
    tpJsonValue(uint32_t i) : value_(i) {}

    /// @brief 从有符号32位整数构造JSON值
    /// @param i 有符号32位整数
    tpJsonValue(int32_t i) : value_(i) {}

    /// @brief 从有符号64位整数构造JSON值
    /// @param i 有符号64位整数
    tpJsonValue(int64_t i) : value_(i) {}

    /// @brief 从无符号64位整数构造JSON值
    /// @param i 无符号64位整数
    tpJsonValue(uint64_t i) : value_(i) {}

    /// @brief 从双精度浮点数构造JSON值
    /// @param d 双精度浮点数
    tpJsonValue(double d) : value_(d) {}

    /// @brief 从C字符串构造JSON值
    /// @param str C风格字符串
    tpJsonValue(const char *str) : value_(str, strlen(str)) {}

    /// @brief 从字符串对象构造JSON值
    /// @param str tpString字符串对象
    tpJsonValue(const tpString &str) : value_(str.c_str(), str.length()) {}

    /// @brief 拷贝构造函数
    /// @param others 源JSON值
    tpJsonValue(const tpJsonValue &others) { *this = others; }

    // 类型判断函数组
    /// @brief 检查是否为空值(Null)
    inline bool isNull() const { return value_.IsNull(); }

    /// @brief 检查是否为布尔值
    inline bool isBool() const { return value_.IsBool(); }

    /// @brief 检查是否为32位有符号整数
    inline bool isInt() const { return value_.IsInt(); }

    /// @brief 检查是否为32位无符号整数
    inline bool isUint() const { return value_.IsUint(); }

    /// @brief 检查是否为64位有符号整数
    inline bool isInt64() const { return value_.IsInt64(); }

    /// @brief 检查是否为64位无符号整数
    inline bool isUint64() const { return value_.IsUint64(); }

    /// @brief 检查是否为双精度浮点数
    inline bool isDouble() const { return value_.IsDouble(); }

    /// @brief 检查是否为字符串
    inline bool isString() const { return value_.IsString(); }

    /// @brief 检查是否为对象
    inline bool isObject() const { return value_.IsObject(); }

    /// @brief 检查是否为数组
    inline bool isArray() const { return value_.IsArray(); }

    // 类型转换函数组
    /// @brief 转换为布尔值
    /// @return 转换后的布尔值
    /// @note 如果原始类型不是布尔值，转换结果可能不符合预期
    bool toBool() const;

    /// @brief 转换为32位有符号整数
    /// @return 转换后的整数
    /// @note 如果原始类型不是整数，转换结果可能不符合预期
    int32_t toInt() const;

    /// @brief 转换为32位无符号整数
    /// @return 转换后的整数
    /// @note 如果原始类型不是整数，转换结果可能不符合预期
    uint32_t toUint() const;

    /// @brief 转换为64位有符号整数
    /// @return 转换后的整数
    /// @note 如果原始类型不是整数，转换结果可能不符合预期
    int64_t toInt64() const;

    /// @brief 转换为64位无符号整数
    /// @return 转换后的整数
    /// @note 如果原始类型不是整数，转换结果可能不符合预期
    uint64_t toUint64() const;

    /// @brief 转换为双精度浮点数
    /// @return 转换后的浮点数
    /// @note 数值类型都会尝试转换，非数值类型会返回0.0
    double toDouble() const;

    /// @brief 转换为字符串对象
    /// @return 转换后的字符串
    /// @note 如果原始类型不是字符串，会尝试格式化输出
    tpString toString() const;

    /// @brief 转换为JSON对象
    /// @return 转换后的JSON对象
    /// @note 如果原始类型不是对象，返回空对象
    tpJsonObject toObject() const;

    /// @brief 转换为JSON数组
    /// @return 转换后的JSON数组
    /// @note 如果原始类型不是数组，返回空数组
    tpJsonArray toArray() const;

    /// @brief 赋值操作符
    /// @param others 要复制的源JSON值
    /// @return 当前JSON值的引用
    tpJsonValue &operator=(const tpJsonValue &others);
};

#endif