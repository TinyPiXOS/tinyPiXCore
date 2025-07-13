#ifndef __TP_VARIANT_H
#define __TP_VARIANT_H

#include <iostream>
#include <stdint.h>
#include <set>
#include <vector>
#include <cstring>
#include <assert.h>

#include "tpUtils.h"

/// @brief 泛类型
class tpVariant
{
public:
    enum class VariantType : uint16_t
    {
        tpEmpty = 0, //!< 类型为定义
        tpBool = 1,  //!< bool类型
        tpInt4 = 2,  //!< int32_t 类型
        tpUint4 = 3, //!< uint32_t 类型
        tpInt8 = 4,  //!< int64_t 类型
        tpUint8 = 5, //!< uint64_t 类型
        tpReal4 = 6, //!< float 类型
        tpReal8 = 7, //!< double 类型
        tpBstr = 8,  //!< const char* 类型
        tpRect,
        tpSize,
        tpPoint,
        tpSet = 1 << 8 // 0x1000, //!< 集合类型，该类型的值需要用过特定函数获取，不能直接通过成员变量获取
    };

    struct VariantValue
    {
        uint16_t m_vt; //!< 值得类型,取值参看枚举VariantType，其中vt_set可以与其他值联合使用
        union InnerUnion
        {
            bool m_bVal;
            int32_t m_i4Val;
            uint32_t m_ui4Val;
            int64_t m_i8Val;
            uint64_t m_ui8Val;
            float m_r4Val;
            double m_r8Val;

            ItpRect tpRectValue;
            ItpSize tpSizeValue;
            ItpPoint tpPointValue;

            char *m_strVal;
            void *m_pSetVal;

            InnerUnion() {}
        } data;

        VariantValue()
        {
        }
    };

public:
    tpVariant();
    tpVariant(bool bValue);
    tpVariant(int32_t nValue);
    tpVariant(uint32_t uValue);
    tpVariant(int64_t nValue);
    tpVariant(uint64_t uValue);
    tpVariant(float fValue);
    tpVariant(double dValue);
    tpVariant(const char *pChar);
    tpVariant(const std::string &strChar);

    tpVariant(const ItpRect &value);
    tpVariant(const ItpSize &value);
    tpVariant(const ItpPoint &value);

    tpVariant(const std::vector<bool> &valueVector);
    tpVariant(const std::vector<int32_t> &valueVector);
    tpVariant(const std::vector<uint32_t> &valueVector);
    tpVariant(const std::vector<int64_t> &valueVector);
    tpVariant(const std::vector<uint64_t> &valueVector);
    tpVariant(const std::vector<float> &valueVector);
    tpVariant(const std::vector<double> &valueVector);
    tpVariant(const std::vector<std::string> &valueVector);
    tpVariant(const std::set<bool> &valueSet);
    tpVariant(const std::set<int32_t> &valueSet);
    tpVariant(const std::set<uint32_t> &valueSet);
    tpVariant(const std::set<int64_t> &valueSet);
    tpVariant(const std::set<uint64_t> &valueSet);
    tpVariant(const std::set<float> &valueSet);
    tpVariant(const std::set<double> &valueSet);
    tpVariant(const std::set<std::string> &valueSet);

    tpVariant(const VariantValue &value);
    tpVariant(const tpVariant &other);

public:
    bool isNull();

    tpVariant &operator=(bool bValue);
    tpVariant &operator=(int32_t nValue);
    tpVariant &operator=(uint32_t uValue);
    tpVariant &operator=(int64_t nValue);
    tpVariant &operator=(uint64_t uValue);
    tpVariant &operator=(float fValue);
    tpVariant &operator=(double dValue);
    tpVariant &operator=(ItpRect value);
    tpVariant &operator=(ItpSize value);
    tpVariant &operator=(ItpPoint value);
    tpVariant &operator=(const char *pChar);
    tpVariant &operator=(const std::string &strChar);
    tpVariant &operator=(const std::vector<bool> &valueVector);
    tpVariant &operator=(const std::vector<int32_t> &valueVector);
    tpVariant &operator=(const std::vector<uint32_t> &valueVector);
    tpVariant &operator=(const std::vector<int64_t> &valueVector);
    tpVariant &operator=(const std::vector<uint64_t> &valueVector);
    tpVariant &operator=(const std::vector<float> &valueVector);
    tpVariant &operator=(const std::vector<double> &valueVector);
    tpVariant &operator=(const std::vector<std::string> &valueVector);
    tpVariant &operator=(const std::set<bool> &valueSet);
    tpVariant &operator=(const std::set<int32_t> &valueSet);
    tpVariant &operator=(const std::set<uint32_t> &valueSet);
    tpVariant &operator=(const std::set<int64_t> &valueSet);
    tpVariant &operator=(const std::set<uint64_t> &valueSet);
    tpVariant &operator=(const std::set<float> &valueSet);
    tpVariant &operator=(const std::set<double> &valueSet);
    tpVariant &operator=(const std::set<std::string> &valueSet);
    tpVariant &operator=(const VariantValue &value);
    tpVariant &operator=(const tpVariant &other);

    // bool operator==(const VariantValue &value);
    bool operator==(const tpVariant &value);

    bool operator!=(const VariantValue &value);
    bool operator!=(const tpVariant &value);

    operator bool() const
    {
        if (!isBool())
            return false;
        return data_.data.m_bVal;
    }

    operator int32_t() const
    {
        if (!isInt32())
            return 0;
        return data_.data.m_i4Val;
    }

    operator uint32_t() const
    {
        if (!isUint32())
            return 0;
        return data_.data.m_ui4Val;
    }

    operator int64_t() const
    {
        if (!isInt64())
            return 0;
        return data_.data.m_i8Val;
    }

    operator uint64_t() const
    {
        if (!isUint64())
            return 0;
        return data_.data.m_ui4Val;
    }

    operator float() const
    {
        if (!isFloat())
            return 0.0;
        return data_.data.m_r4Val;
    }

    operator double() const
    {
        if (!isDouble())
            return 0.0;
        return data_.data.m_r8Val;
    }

    operator const char *() const
    {
        if (!isConstChar())
            return "";
        return data_.data.m_strVal;
    }

    operator std::string() const
    {
        if (!isString())
            return "";
        return std::string(data_.data.m_strVal);
    }

    operator tpString() const
    {
        if (!isString())
            return "";
        return std::string(data_.data.m_strVal);
    }

    operator ItpRect() const
    {
        if (!isRect())
            return ItpRect();
        return ItpRect(data_.data.tpRectValue);
    }

    operator ItpSize() const
    {
        if (!isSize())
            return ItpSize();
        return ItpSize(data_.data.tpSizeValue);
    }

    operator ItpPoint() const
    {
        if (!isPoint())
            return ItpPoint();
        return ItpPoint(data_.data.tpPointValue);
    }

    bool isBool() const { return (uint16_t)VariantType::tpBool == data_.m_vt; }

    bool isInt32() const { return (uint16_t)VariantType::tpInt4 == data_.m_vt; }

    bool isUint32() const { return (uint16_t)VariantType::tpUint4 == data_.m_vt; }

    bool isInt64() const { return (uint16_t)VariantType::tpInt8 == data_.m_vt; }

    bool isUint64() const { return (uint16_t)VariantType::tpUint8 == data_.m_vt; }

    bool isFloat() const { return (uint16_t)VariantType::tpReal4 == data_.m_vt; }

    bool isDouble() const { return (uint16_t)VariantType::tpReal8 == data_.m_vt; }

    bool isConstChar() const { return (uint16_t)VariantType::tpBstr == data_.m_vt; }

    bool isString() const { return (uint16_t)VariantType::tpBstr == data_.m_vt; }

    bool isRect() const { return (uint16_t)VariantType::tpRect == data_.m_vt; }

    bool isSize() const { return (uint16_t)VariantType::tpSize == data_.m_vt; }

    bool isPoint() const { return (uint16_t)VariantType::tpPoint == data_.m_vt; }

    bool toBool(const bool &defaultValue = false) const
    {
        if (!isBool())
            return defaultValue;

        return bool(*this);
    }

    int32_t toInt32(const int32_t &defaultValue = 0) const
    {
        if (!isInt32())
            return defaultValue;
        return int32_t(*this);
    }

    uint32_t toUInt32(const uint32_t &defaultValue = 0) const
    {
        if (!isUint32())
            return defaultValue;
        return uint32_t(*this);
    }

    int64_t toInt64(const int64_t &defaultValue = 0) const
    {
        if (!isInt64())
            return defaultValue;
        return int64_t(*this);
    }

    uint64_t toUint64(const uint64_t &defaultValue = 0) const
    {
        if (!isUint64())
            return defaultValue;
        return uint64_t(*this);
    }

    float toFloat(const float &defaultValue = 0) const
    {
        if (!isFloat())
            return defaultValue;
        return float(*this);
    }

    double toDouble(const double &defaultValue = 0) const
    {
        if (!isDouble())
            return defaultValue;
        return double(*this);
    }

    tpString toString(const tpString &defaultValue = "") const
    {
        if (!isString())
            return defaultValue;

        return std::string(data_.data.m_strVal);
        // return static_cast<tpString>(*this);
    }

    ItpRect toRect(const ItpRect &defaultValue = ItpRect()) const
    {
        if (!isRect())
            return defaultValue;
        return ItpRect(*this);
    }

    ItpSize toSize(const ItpSize &defaultValue = ItpSize()) const
    {
        if (!isSize())
            return defaultValue;
        return ItpSize(*this);
    }

    ItpPoint toPoint(const ItpPoint &defaultValue = ItpPoint()) const
    {
        if (!isPoint())
            return defaultValue;
        return ItpPoint(*this);
    }

    std::vector<bool> ToBoolArray() const;
    std::vector<int32_t> ToInt32Array() const;
    std::vector<uint32_t> ToUint32Array() const;
    std::vector<int64_t> ToInt64Array() const;
    std::vector<uint64_t> ToUint64Array() const;
    std::vector<float> ToFloatArray() const;
    std::vector<double> ToDoubleArray() const;
    std::vector<std::string> ToStringArray() const;

    std::set<bool> &ToBoolSet();
    std::set<int32_t> &ToInt32Set();
    std::set<uint32_t> &ToUint32Set();
    std::set<int64_t> &ToInt64Set();
    std::set<uint64_t> &ToUint64Set();
    std::set<float> &ToFloatSet();
    std::set<double> &ToDoubleSet();
    std::set<std::string> &ToStringSet();

private:
    bool Compare(const VariantValue &value);

private:
    VariantValue data_;
};

#endif