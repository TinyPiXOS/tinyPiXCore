#ifndef __TP_PROPERTY_H
#define __TP_PROPERTY_H

#include <functional>
#include <unordered_map>
#include <TpVariant.h>

typedef std::function<TpVariant()> TpPropertyReadCallFunc;
typedef std::function<void(const TpVariant &)> TpPropertyWriteCallFunc;

// 自定义类型
template <typename T>
inline T parsePropertyValue(const TpVariant &value)
{
    return value.toCustom<T>();
}

// 基础类型特化
template <>
inline bool parsePropertyValue<bool>(const TpVariant &value)
{
    return value.toBool();
}

template <>
inline int8_t parsePropertyValue<int8_t>(const TpVariant &value)
{
    return value.toInt8();
}

template <>
inline uint8_t parsePropertyValue<uint8_t>(const TpVariant &value)
{
    return value.toUInt8();
}

template <>
inline int16_t parsePropertyValue<int16_t>(const TpVariant &value)
{
    return value.toInt16();
}

template <>
inline uint16_t parsePropertyValue<uint16_t>(const TpVariant &value)
{
    return value.toUInt16();
}

template <>
inline int32_t parsePropertyValue<int32_t>(const TpVariant &value)
{
    return value.toInt32();
}

template <>
inline uint32_t parsePropertyValue<uint32_t>(const TpVariant &value)
{
    return value.toUInt32();
}

template <>
inline int64_t parsePropertyValue<int64_t>(const TpVariant &value)
{
    return value.toInt64();
}

template <>
inline uint64_t parsePropertyValue<uint64_t>(const TpVariant &value)
{
    return value.toUint64();
}

template <>
inline float parsePropertyValue<float>(const TpVariant &value)
{
    return value.toFloat();
}

template <>
inline double parsePropertyValue<double>(const TpVariant &value)
{
    return value.toDouble();
}

// 字符串类型特化
template <>
inline const char *parsePropertyValue<const char *>(const TpVariant &value)
{
    return value.toString().c_str();
}

template <>
inline std::string parsePropertyValue<std::string>(const TpVariant &value)
{
    return value.toString();
}

template <>
inline TpString parsePropertyValue<TpString>(const TpVariant &value)
{
    return value.toString();
}

// 几何类型特化
template <>
inline TpRect parsePropertyValue<TpRect>(const TpVariant &value)
{
    return value.toRect();
}

template <>
inline TpSize parsePropertyValue<TpSize>(const TpVariant &value)
{
    return value.toSize();
}

template <>
inline TpPoint parsePropertyValue<TpPoint>(const TpVariant &value)
{
    return value.toPoint();
}

// 向量类型特化
template <>
inline TpVector<bool> parsePropertyValue<TpVector<bool>>(const TpVariant &value)
{
    return value.ToBoolArray();
}

template <>
inline TpVector<int8_t> parsePropertyValue<TpVector<int8_t>>(const TpVariant &value)
{
    return value.ToInt8Array();
}

template <>
inline TpVector<uint8_t> parsePropertyValue<TpVector<uint8_t>>(const TpVariant &value)
{
    return value.ToUint8Array();
}

template <>
inline TpVector<int16_t> parsePropertyValue<TpVector<int16_t>>(const TpVariant &value)
{
    return value.ToInt16Array();
}

template <>
inline TpVector<uint16_t> parsePropertyValue<TpVector<uint16_t>>(const TpVariant &value)
{
    return value.ToUint16Array();
}

template <>
inline TpVector<int32_t> parsePropertyValue<TpVector<int32_t>>(const TpVariant &value)
{
    return value.ToInt32Array();
}

template <>
inline TpVector<uint32_t> parsePropertyValue<TpVector<uint32_t>>(const TpVariant &value)
{
    return value.ToUint32Array();
}

template <>
inline TpVector<int64_t> parsePropertyValue<TpVector<int64_t>>(const TpVariant &value)
{
    return value.ToInt64Array();
}

template <>
inline TpVector<uint64_t> parsePropertyValue<TpVector<uint64_t>>(const TpVariant &value)
{
    return value.ToUint64Array();
}

template <>
inline TpVector<float> parsePropertyValue<TpVector<float>>(const TpVariant &value)
{
    return value.ToFloatArray();
}

template <>
inline TpVector<double> parsePropertyValue<TpVector<double>>(const TpVariant &value)
{
    return value.ToDoubleArray();
}

template <>
inline TpVector<std::string> parsePropertyValue<TpVector<std::string>>(const TpVariant &value)
{
    return value.ToStringArray();
}

// 集合类型特化
template <>
inline std::set<bool> parsePropertyValue<std::set<bool>>(const TpVariant &value)
{
    return value.ToBoolSet();
}

template <>
inline std::set<int8_t> parsePropertyValue<std::set<int8_t>>(const TpVariant &value)
{
    return value.ToInt8Set();
}

template <>
inline std::set<uint8_t> parsePropertyValue<std::set<uint8_t>>(const TpVariant &value)
{
    return value.ToUint8Set();
}

template <>
inline std::set<int16_t> parsePropertyValue<std::set<int16_t>>(const TpVariant &value)
{
    return value.ToInt16Set();
}

template <>
inline std::set<uint16_t> parsePropertyValue<std::set<uint16_t>>(const TpVariant &value)
{
    return value.ToUint16Set();
}

template <>
inline std::set<int32_t> parsePropertyValue<std::set<int32_t>>(const TpVariant &value)
{
    return value.ToInt32Set();
}

template <>
inline std::set<uint32_t> parsePropertyValue<std::set<uint32_t>>(const TpVariant &value)
{
    return value.ToUint32Set();
}

template <>
inline std::set<int64_t> parsePropertyValue<std::set<int64_t>>(const TpVariant &value)
{
    return value.ToInt64Set();
}

template <>
inline std::set<uint64_t> parsePropertyValue<std::set<uint64_t>>(const TpVariant &value)
{
    return value.ToUint64Set();
}

template <>
inline std::set<float> parsePropertyValue<std::set<float>>(const TpVariant &value)
{
    return value.ToFloatSet();
}

template <>
inline std::set<double> parsePropertyValue<std::set<double>>(const TpVariant &value)
{
    return value.ToDoubleSet();
}

template <>
inline std::set<std::string> parsePropertyValue<std::set<std::string>>(const TpVariant &value)
{
    return value.ToStringSet();
}

// 泛型向量类型（使用 TpVector<TpVariant>）
template <>
inline TpVector<TpVariant> parsePropertyValue<TpVector<TpVariant>>(const TpVariant &value)
{
    if (value.isVector())
    {
        const TpVector<TpVariant> *vec = value.toVectorPtr();
        if (vec)
        {
            return *vec;
        }
    }
    return TpVector<TpVariant>();
}

#define TP_PROPERTY(T, name, readFunc, writeFunc) \
    TpObject::registerProperty(#name, [=]() { return readFunc(); }, [=](const TpVariant &input) { writeFunc(parsePropertyValue<T>(input)); });

#endif