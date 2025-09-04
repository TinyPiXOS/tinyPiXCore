#ifndef __TP_VARIANT_H
#define __TP_VARIANT_H

#include <iostream>
#include <stdint.h>
#include <set>
#include <vector>
#include <cstring>
#include <assert.h>

#include "TpUtils.h"
#include "TpSize.h"
#include "TpRect.h"
#include "TpPoint.h"

/// @brief 泛类型
class TpVariant
{
public:
    enum class VariantType : uint16_t
    {
        EmptyVar = 0, //!< 类型为定义
        BoolVar = 1,  //!< bool类型
        Int4Var = 2,  //!< int32_t 类型
        Uint4Var = 3, //!< uint32_t 类型
        Int8Var = 4,  //!< int64_t 类型
        Uint8Var = 5, //!< uint64_t 类型
        Real4Var = 6, //!< float 类型
        Real8Var = 7, //!< double 类型
        BstrVar = 8,  //!< const char* 类型
        RectVar,
        SizeVar,
        PointVar,

        Int1Var,  //!< int8_t 类型
        Uint1Var, //!< uint8_t 类型
        Int2Var,  //!< int16_t 类型
        Uint2Var, //!< uint16_t 类型

        VectorVar, // 向量类型
        CustomVar, // 自定义类型

        SetVar = 1 << 8 // 0x1000, //!< 集合类型，该类型的值需要用过特定函数获取，不能直接通过成员变量获取
    };

    struct VariantValue
    {
        uint16_t m_vt; //!< 值的类型,取值参看枚举VariantType，其中vt_set可以与其他值联合使用
        union InnerUnion
        {
            bool m_bVal;
            int8_t m_i1Val;
            uint8_t m_ui1Val;
            int16_t m_i2Val;
            uint16_t m_ui2Val;
            int32_t m_i4Val;
            uint32_t m_ui4Val;
            int64_t m_i8Val;
            uint64_t m_ui8Val;
            float m_r4Val;
            double m_r8Val;

            TpRect mRectValue;
            TpSize mSizeValue;
            TpPoint mPointValue;

            char *m_strVal;
            void *m_pSetVal;

            std::vector<TpVariant> *m_vectorVal; // 向量指针
            struct CustomData
            {
                void *ptr;                    // 数据
                void (*destroy)(void *);      // 释放操作
                void *(*clone)(const void *); // 拷贝操作
            } custom;                         // 自定义类型数据保存

            InnerUnion() {}
            ~InnerUnion() {}
        } data;

        VariantValue()
        {
        }
        ~VariantValue()
        {

            switch ((VariantType)m_vt)
            {
            case VariantType::RectVar:
                data.mRectValue.~TpRect(); // 显式调用析构
                break;
            case VariantType::SizeVar:
                data.mSizeValue.~TpSize(); // 显式调用析构
                break;
            case VariantType::PointVar:
                data.mPointValue.~TpPoint(); // 显式调用析构
                break;
            default:
                break;
            }
        }
    };

public:
    TpVariant();
    TpVariant(bool bValue);
    TpVariant(int8_t nValue);
    TpVariant(uint8_t nValue);
    TpVariant(int16_t nValue);
    TpVariant(uint16_t nValue);
    TpVariant(int32_t nValue);
    TpVariant(uint32_t uValue);
    TpVariant(int64_t nValue);
    TpVariant(uint64_t uValue);
    TpVariant(float fValue);
    TpVariant(double dValue);
    TpVariant(const char *pChar);
    TpVariant(const std::string &strChar);

    TpVariant(const TpRect &value);
    TpVariant(const TpSize &value);
    TpVariant(const TpPoint &value);

    TpVariant(const std::vector<bool> &valueVector);
    TpVariant(const std::vector<int8_t> &valueVector);
    TpVariant(const std::vector<uint8_t> &valueVector);
    TpVariant(const std::vector<int16_t> &valueVector);
    TpVariant(const std::vector<uint16_t> &valueVector);
    TpVariant(const std::vector<int32_t> &valueVector);
    TpVariant(const std::vector<uint32_t> &valueVector);
    TpVariant(const std::vector<int64_t> &valueVector);
    TpVariant(const std::vector<uint64_t> &valueVector);
    TpVariant(const std::vector<float> &valueVector);
    TpVariant(const std::vector<double> &valueVector);
    TpVariant(const std::vector<std::string> &valueVector);
    TpVariant(const std::set<bool> &valueSet);
    TpVariant(const std::set<int8_t> &valueSet);
    TpVariant(const std::set<uint8_t> &valueSet);
    TpVariant(const std::set<int16_t> &valueSet);
    TpVariant(const std::set<uint16_t> &valueSet);
    TpVariant(const std::set<int32_t> &valueSet);
    TpVariant(const std::set<uint32_t> &valueSet);
    TpVariant(const std::set<int64_t> &valueSet);
    TpVariant(const std::set<uint64_t> &valueSet);
    TpVariant(const std::set<float> &valueSet);
    TpVariant(const std::set<double> &valueSet);
    TpVariant(const std::set<std::string> &valueSet);

    TpVariant(const VariantValue &value);
    TpVariant(const TpVariant &other);
    TpVariant(std::vector<TpVariant> *vectorVal);

    // 模板构造函数
    template <typename T>
    TpVariant(const T &value)
    {
        clear();
        data_.m_vt = static_cast<uint16_t>(VariantType::CustomVar);
        data_.data.custom.ptr = new T(value);
        data_.data.custom.destroy = [](void *p)
        {
            delete static_cast<T *>(p);
        };
        data_.data.custom.clone = [](const void *p) -> void *
        {
            return new T(*static_cast<const T *>(p));
        };
    }

    ~TpVariant();

public:
    bool isNull();

    TpVariant &operator=(bool bValue);
    TpVariant &operator=(int8_t nValue);
    TpVariant &operator=(uint8_t uValue);
    TpVariant &operator=(int16_t nValue);
    TpVariant &operator=(uint16_t uValue);
    TpVariant &operator=(int32_t nValue);
    TpVariant &operator=(uint32_t uValue);
    TpVariant &operator=(int64_t nValue);
    TpVariant &operator=(uint64_t uValue);
    TpVariant &operator=(float fValue);
    TpVariant &operator=(double dValue);
    TpVariant &operator=(TpRect value);
    TpVariant &operator=(TpSize value);
    TpVariant &operator=(TpPoint value);
    TpVariant &operator=(const char *pChar);
    TpVariant &operator=(const std::string &strChar);
    TpVariant &operator=(std::vector<TpVariant> *vectorVal);
    TpVariant &operator=(const std::vector<bool> &valueVector);
    TpVariant &operator=(const std::vector<int8_t> &valueVector);
    TpVariant &operator=(const std::vector<uint8_t> &valueVector);
    TpVariant &operator=(const std::vector<int16_t> &valueVector);
    TpVariant &operator=(const std::vector<uint16_t> &valueVector);
    TpVariant &operator=(const std::vector<int32_t> &valueVector);
    TpVariant &operator=(const std::vector<uint32_t> &valueVector);
    TpVariant &operator=(const std::vector<int64_t> &valueVector);
    TpVariant &operator=(const std::vector<uint64_t> &valueVector);
    TpVariant &operator=(const std::vector<float> &valueVector);
    TpVariant &operator=(const std::vector<double> &valueVector);
    TpVariant &operator=(const std::vector<std::string> &valueVector);
    TpVariant &operator=(const std::set<bool> &valueSet);
    TpVariant &operator=(const std::set<int8_t> &valueSet);
    TpVariant &operator=(const std::set<uint8_t> &valueSet);
    TpVariant &operator=(const std::set<int16_t> &valueSet);
    TpVariant &operator=(const std::set<uint16_t> &valueSet);
    TpVariant &operator=(const std::set<int32_t> &valueSet);
    TpVariant &operator=(const std::set<uint32_t> &valueSet);
    TpVariant &operator=(const std::set<int64_t> &valueSet);
    TpVariant &operator=(const std::set<uint64_t> &valueSet);
    TpVariant &operator=(const std::set<float> &valueSet);
    TpVariant &operator=(const std::set<double> &valueSet);
    TpVariant &operator=(const std::set<std::string> &valueSet);
    TpVariant &operator=(const VariantValue &value);
    TpVariant &operator=(const TpVariant &other);
    // 自定义类型
    template <typename T>
    TpVariant &operator=(const T &value)
    {
        // 清理现有值
        if (data_.m_vt == static_cast<uint16_t>(VariantType::CustomVar) &&
            data_.data.custom.ptr != nullptr)
        {
            // 使用存储的析构函数清理自定义类型
            if (data_.data.custom.destroy)
            {
                data_.data.custom.destroy(data_.data.custom.ptr);
            }
        }

        // 分配新值
        data_.m_vt = static_cast<uint16_t>(VariantType::CustomVar);
        data_.data.custom.ptr = new T(value);
        data_.data.custom.destroy = [](void *p)
        {
            delete static_cast<T *>(p);
        };
        data_.data.custom.clone = [](const void *p) -> void *
        {
            return new T(*static_cast<const T *>(p));
        };
        return *this;
    }

    // bool operator==(const VariantValue &value);
    bool operator==(const TpVariant &value);

    bool operator!=(const VariantValue &value);
    bool operator!=(const TpVariant &value);

    operator bool() const
    {
        if (!isBool())
            return false;
        return data_.data.m_bVal;
    }
    operator int8_t() const
    {
        if (!isInt8())
            return 0;
        return data_.data.m_i1Val;
    }

    operator uint8_t() const
    {
        if (!isUint8())
            return 0;
        return data_.data.m_ui1Val;
    }
    operator int16_t() const
    {
        if (!isInt16())
            return 0;
        return data_.data.m_i2Val;
    }

    operator uint16_t() const
    {
        if (!isUint16())
            return 0;
        return data_.data.m_ui2Val;
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

    operator TpString() const
    {
        if (!isString())
            return "";
        return std::string(data_.data.m_strVal);
    }

    operator TpRect() const
    {
        if (!isRect())
            return TpRect();
        return TpRect(data_.data.mRectValue);
    }

    operator TpSize() const
    {
        if (!isSize())
            return TpSize();
        return TpSize(data_.data.mSizeValue);
    }

    operator TpPoint() const
    {
        if (!isPoint())
            return TpPoint();
        return TpPoint(data_.data.mPointValue);
    }

    bool isVector() const { return data_.m_vt == static_cast<uint16_t>(VariantType::VectorVar); }
    const std::vector<TpVariant> *toVectorPtr() const;

    // 自定义类型检查
    template <typename T>
    bool isCustom() const { return data_.m_vt == static_cast<uint16_t>(VariantType::CustomVar) && data_.data.custom.ptr != nullptr; }
    // 获取自定义类型值
    template <typename T>
    T toCustom() const
    {
        if (isCustom<T>())
        {
            return *static_cast<T *>(data_.data.custom.ptr);
        }
        throw std::bad_cast();
    }

    bool isBool() const { return (uint16_t)VariantType::BoolVar == data_.m_vt; }

    bool isInt8() const { return (uint16_t)VariantType::Int1Var == data_.m_vt; }

    bool isUint8() const { return (uint16_t)VariantType::Uint1Var == data_.m_vt; }

    bool isInt16() const { return (uint16_t)VariantType::Int2Var == data_.m_vt; }

    bool isUint16() const { return (uint16_t)VariantType::Uint2Var == data_.m_vt; }

    bool isInt32() const { return (uint16_t)VariantType::Int4Var == data_.m_vt; }

    bool isUint32() const { return (uint16_t)VariantType::Uint4Var == data_.m_vt; }

    bool isInt64() const { return (uint16_t)VariantType::Int8Var == data_.m_vt; }

    bool isUint64() const { return (uint16_t)VariantType::Uint8Var == data_.m_vt; }

    bool isFloat() const { return (uint16_t)VariantType::Real4Var == data_.m_vt; }

    bool isDouble() const { return (uint16_t)VariantType::Real8Var == data_.m_vt; }

    bool isConstChar() const { return (uint16_t)VariantType::BstrVar == data_.m_vt; }

    bool isString() const { return (uint16_t)VariantType::BstrVar == data_.m_vt; }

    bool isRect() const { return (uint16_t)VariantType::RectVar == data_.m_vt; }

    bool isSize() const { return (uint16_t)VariantType::SizeVar == data_.m_vt; }

    bool isPoint() const { return (uint16_t)VariantType::PointVar == data_.m_vt; }

    bool toBool(const bool &defaultValue = false) const
    {
        if (!isBool())
            return defaultValue;

        return bool(*this);
    }

    int8_t toInt8(const int8_t &defaultValue = 0) const
    {
        if (!isInt8())
            return defaultValue;
        return int8_t(*this);
    }

    uint8_t toUInt8(const uint8_t &defaultValue = 0) const
    {
        if (!isUint8())
            return defaultValue;
        return uint8_t(*this);
    }

    int16_t toInt16(const int16_t &defaultValue = 0) const
    {
        if (!isInt16())
            return defaultValue;
        return int16_t(*this);
    }

    uint16_t toUInt16(const uint16_t &defaultValue = 0) const
    {
        if (!isUint16())
            return defaultValue;
        return uint16_t(*this);
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

    TpString toString(const TpString &defaultValue = "") const
    {
        if (!isString())
            return defaultValue;

        return std::string(data_.data.m_strVal);
        // return static_cast<TpString>(*this);
    }

    TpRect toRect(const TpRect &defaultValue = TpRect()) const
    {
        if (!isRect())
            return defaultValue;
        return TpRect(*this);
    }

    TpSize toSize(const TpSize &defaultValue = TpSize()) const
    {
        if (!isSize())
            return defaultValue;
        return TpSize(*this);
    }

    TpPoint toPoint(const TpPoint &defaultValue = TpPoint()) const
    {
        if (!isPoint())
            return defaultValue;
        return TpPoint(*this);
    }

    std::vector<bool> ToBoolArray() const;
    std::vector<int8_t> ToInt8Array() const;
    std::vector<uint8_t> ToUint8Array() const;
    std::vector<int16_t> ToInt16Array() const;
    std::vector<uint16_t> ToUint16Array() const;
    std::vector<int32_t> ToInt32Array() const;
    std::vector<uint32_t> ToUint32Array() const;
    std::vector<int64_t> ToInt64Array() const;
    std::vector<uint64_t> ToUint64Array() const;
    std::vector<float> ToFloatArray() const;
    std::vector<double> ToDoubleArray() const;
    std::vector<std::string> ToStringArray() const;

    std::set<bool> &ToBoolSet();
    std::set<int8_t> &ToInt8Set();
    std::set<uint8_t> &ToUint8Set();
    std::set<int16_t> &ToInt16Set();
    std::set<uint16_t> &ToUint16Set();
    std::set<int32_t> &ToInt32Set();
    std::set<uint32_t> &ToUint32Set();
    std::set<int64_t> &ToInt64Set();
    std::set<uint64_t> &ToUint64Set();
    std::set<float> &ToFloatSet();
    std::set<double> &ToDoubleSet();
    std::set<std::string> &ToStringSet();

    uint16_t getVariantType() const;

    const char *variantTypeName() const;

private:
    bool Compare(const VariantValue &value);
    void clear();

private:
    VariantValue data_;
};

#endif