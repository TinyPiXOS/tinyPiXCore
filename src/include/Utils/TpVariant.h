#ifndef __TP_VARIANT_H
#define __TP_VARIANT_H

#include <iostream>
#include <stdint.h>
#include <set>
#include <vector>
#include <cstring>
#include <assert.h>

#include "TpUtils.h"
class BluetoothService;
/// @brief 泛类型
class TpVariant
{
public:
    enum class VariantType : uint16_t
    {
        TpEmpty = 0, //!< 类型为定义
        tpBool = 1,  //!< bool类型
        TpInt4 = 2,  //!< int32_t 类型
        TpUint4 = 3, //!< uint32_t 类型
        tpInt8 = 4,  //!< int64_t 类型
        TpUint8 = 5, //!< uint64_t 类型
        TpReal4 = 6, //!< float 类型
        TpReal8 = 7, //!< double 类型
        TpBstr = 8,  //!< const char* 类型
        TpRect,
        TpSize,
        TpPoint,

		TpInt1,  //!< int8_t 类型
        TpUint1, //!< uint8_t 类型
		TpInt2,  //!< int16_t 类型
        TpUint2 , //!< uint16_t 类型

		TpVector, // 向量类型
		tpCustom, // 自定义类型

        TpSet = 1 << 8 // 0x1000, //!< 集合类型，该类型的值需要用过特定函数获取，不能直接通过成员变量获取
    };

    struct VariantValue
    {
        uint16_t m_vt; //!< 值得类型,取值参看枚举VariantType，其中vt_set可以与其他值联合使用
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

            ItpRect TpRectValue;
            ItpSize TpSizeValue;
            ItpPoint TpPointValue;

            char *m_strVal;
            void *m_pSetVal;
			
			std::vector<TpVariant>* m_vectorVal; // 向量指针
			void* m_customVal;

            InnerUnion() {}
        } data;

        VariantValue()
        {
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

    TpVariant(const ItpRect &value);
    TpVariant(const ItpSize &value);
    TpVariant(const ItpPoint &value);

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
	TpVariant(std::vector<TpVariant>* vectorVal);

	template <typename T>
    TpVariant(const T& value) {
        // 分配内存存储自定义类型
        T* copy = new T(value);
        data_.m_vt = static_cast<uint16_t>(VariantType::tpCustom);
        data_.data.m_customVal = copy;
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
    TpVariant &operator=(ItpRect value);
    TpVariant &operator=(ItpSize value);
    TpVariant &operator=(ItpPoint value);
    TpVariant &operator=(const char *pChar);
    TpVariant &operator=(const std::string &strChar);
	TpVariant &operator=(std::vector<TpVariant>* vectorVal);
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
	//自定义类型
	template <typename T>
    TpVariant& operator=(const T& value) {
        // 清理现有值
        if (data_.m_vt == static_cast<uint16_t>(VariantType::tpCustom) && 
            data_.data.m_customVal != nullptr) {
            delete static_cast<T*>(data_.data.m_customVal);
        }
        
        // 分配新值
        T* copy = new T(value);
        data_.m_vt = static_cast<uint16_t>(VariantType::tpCustom);
        data_.data.m_customVal = copy;
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

    operator ItpRect() const
    {
        if (!isRect())
            return ItpRect();
        return ItpRect(data_.data.TpRectValue);
    }

    operator ItpSize() const
    {
        if (!isSize())
            return ItpSize();
        return ItpSize(data_.data.TpSizeValue);
    }

    operator ItpPoint() const
    {
        if (!isPoint())
            return ItpPoint();
        return ItpPoint(data_.data.TpPointValue);
    }

	bool isVector() const { return data_.m_vt == static_cast<uint16_t>(VariantType::TpVector);}
	const std::vector<TpVariant>* toVectorPtr() const;
	
	// 自定义类型检查
    template <typename T>
    bool isCustom() const {return data_.m_vt == static_cast<uint16_t>(VariantType::tpCustom) && data_.data.m_customVal != nullptr;}
    
    // 获取自定义类型值
    template <typename T>
    T toCustom() const {
        if (isCustom<T>()) {
            return *static_cast<T*>(data_.data.m_customVal);
        }
        throw std::bad_cast();
    }

    bool isBool() const { return (uint16_t)VariantType::tpBool == data_.m_vt; }

	bool isInt8() const { return (uint16_t)VariantType::TpInt1 == data_.m_vt; }

    bool isUint8() const { return (uint16_t)VariantType::TpUint1 == data_.m_vt; }

	bool isInt16() const { return (uint16_t)VariantType::TpInt2 == data_.m_vt; }

    bool isUint16() const { return (uint16_t)VariantType::TpUint2 == data_.m_vt; }

    bool isInt32() const { return (uint16_t)VariantType::TpInt4 == data_.m_vt; }

    bool isUint32() const { return (uint16_t)VariantType::TpUint4 == data_.m_vt; }

    bool isInt64() const { return (uint16_t)VariantType::tpInt8 == data_.m_vt; }

    bool isUint64() const { return (uint16_t)VariantType::TpUint8 == data_.m_vt; }

    bool isFloat() const { return (uint16_t)VariantType::TpReal4 == data_.m_vt; }

    bool isDouble() const { return (uint16_t)VariantType::TpReal8 == data_.m_vt; }

    bool isConstChar() const { return (uint16_t)VariantType::TpBstr == data_.m_vt; }

    bool isString() const { return (uint16_t)VariantType::TpBstr == data_.m_vt; }

    bool isRect() const { return (uint16_t)VariantType::TpRect == data_.m_vt; }

    bool isSize() const { return (uint16_t)VariantType::TpSize == data_.m_vt; }

    bool isPoint() const { return (uint16_t)VariantType::TpPoint == data_.m_vt; }

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

	uint16_t getVariantType() const ;

	const char* variantTypeName() const;

private:
    bool Compare(const VariantValue &value);

private:
    VariantValue data_;
};

#endif