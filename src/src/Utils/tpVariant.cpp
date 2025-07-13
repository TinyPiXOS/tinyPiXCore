/***
 * @Author: hywang
 * @Date: 2024-05-22 09:41:10
 * @LastEditors: hywang
 * @LastEditTime: 2024-05-23 16:28:20
 * @FilePath: /pix-singlegui/PixSingleGUI/src/Utils/tpVariant.cpp
 * @Description:
 * @
 * @PiXOS
 */

#include "tpVariant.h"
#include "tpVariant_p.h"

#include "tpRect.h"
#include "tpColors.h"
#include "tpSize.h"
#include "tpFont.h"

namespace
{
    template <typename T>
    bool VariantSetValueComplie(std::set<T> *pSetValue1, std::set<T> *pSetValue2)
    {
        if ((nullptr == pSetValue1) && (nullptr == pSetValue2))
            return true;

        if ((nullptr == pSetValue1) || (nullptr == pSetValue2))
            return false;

        return (*pSetValue1) == (*pSetValue2);
    }
}

tpVariant::tpVariant()
{
    VariantValueInit(data_);
}

tpVariant::tpVariant(bool bValue)
{
    data_.m_vt = (uint16_t)VariantType::tpBool;
    data_.data.m_bVal = bValue;
}

tpVariant::tpVariant(int32_t nValue)
{
    data_.m_vt = (uint16_t)VariantType::tpInt4;
    data_.data.m_i4Val = nValue;
}

tpVariant::tpVariant(uint32_t uValue)
{
    data_.m_vt = (uint16_t)VariantType::tpUint4;
    data_.data.m_ui4Val = uValue;
}

tpVariant::tpVariant(int64_t nValue)
{
    data_.m_vt = (uint16_t)VariantType::tpInt8;
    data_.data.m_i8Val = nValue;
}

tpVariant::tpVariant(uint64_t uValue)
{
    data_.m_vt = (uint16_t)VariantType::tpUint8;
    data_.data.m_ui8Val = uValue;
}

tpVariant::tpVariant(float fValue)
{
    data_.m_vt = (uint16_t)VariantType::tpReal4;
    data_.data.m_r4Val = fValue;
}

tpVariant::tpVariant(double dValue)
{
    data_.m_vt = (uint16_t)VariantType::tpReal8;
    data_.data.m_r8Val = dValue;
}

tpVariant::tpVariant(const char *pChar)
{
    data_.m_vt = (uint16_t)VariantType::tpBstr;
    if (nullptr != pChar)
    {
        uint32_t uLen = strlen(pChar);

        data_.data.m_strVal = new char[uLen + 1];
        memcpy(data_.data.m_strVal, pChar, uLen);
        data_.data.m_strVal[uLen] = '\0';
    }
    else
    {
        data_.data.m_strVal = nullptr;
    }
}

tpVariant::tpVariant(const std::string &strChar)
{
    uint32_t uLen = static_cast<uint32_t>(strChar.length());

    data_.m_vt = (uint16_t)VariantType::tpBstr;
    if (0 != uLen)
    {
        data_.data.m_strVal = new char[uLen + 1];
        memcpy(data_.data.m_strVal, strChar.c_str(), uLen);
        data_.data.m_strVal[uLen] = '\0';
    }
    else
    {
        data_.data.m_strVal = nullptr;
    }
}

tpVariant::tpVariant(const ItpRect &value)
{
    data_.m_vt = (uint16_t)VariantType::tpRect;
    data_.data.tpRectValue = value;
}

tpVariant::tpVariant(const ItpSize &value)
{
    data_.m_vt = (uint16_t)VariantType::tpSize;
    data_.data.tpSizeValue = value;
}

tpVariant::tpVariant(const ItpPoint &value)
{
    data_.m_vt = (uint16_t)VariantType::tpPoint;
    data_.data.tpPointValue = value;
}

tpVariant::tpVariant(const std::vector<bool> &valueVector)
{
    data_.m_vt = (uint16_t)VariantType::tpBool | (uint16_t)VariantType::tpSet;
    auto pSetVal = new std::set<bool>();
    for (auto value : valueVector)
        pSetVal->insert(value);
    data_.data.m_pSetVal = pSetVal;
}

tpVariant::tpVariant(const std::vector<int32_t> &valueVector)
{
    data_.m_vt = (uint16_t)VariantType::tpInt4 | (uint16_t)VariantType::tpSet;
    auto pSetVal = new std::set<int32_t>();
    for (auto value : valueVector)
        pSetVal->insert(value);
    data_.data.m_pSetVal = pSetVal;
}

tpVariant::tpVariant(const std::vector<uint32_t> &valueVector)
{
    data_.m_vt = (uint16_t)VariantType::tpUint4 | (uint16_t)VariantType::tpSet;
    auto pSetVal = new std::set<uint32_t>();
    for (auto value : valueVector)
        pSetVal->insert(value);
    data_.data.m_pSetVal = pSetVal;
}

tpVariant::tpVariant(const std::vector<int64_t> &valueVector)
{
    data_.m_vt = (uint16_t)VariantType::tpInt8 | (uint16_t)VariantType::tpSet;
    auto pSetVal = new std::set<int64_t>();
    for (auto value : valueVector)
        pSetVal->insert(value);
    data_.data.m_pSetVal = pSetVal;
}

tpVariant::tpVariant(const std::vector<uint64_t> &valueVector)
{
    data_.m_vt = (uint16_t)VariantType::tpUint8 | (uint16_t)VariantType::tpSet;
    auto pSetVal = new std::set<uint64_t>();
    for (auto value : valueVector)
        pSetVal->insert(value);
    data_.data.m_pSetVal = pSetVal;
}

tpVariant::tpVariant(const std::vector<float> &valueVector)
{
    data_.m_vt = (uint16_t)VariantType::tpReal4 | (uint16_t)VariantType::tpSet;
    auto pSetVal = new std::set<float>();
    for (auto value : valueVector)
        pSetVal->insert(value);
    data_.data.m_pSetVal = pSetVal;
}

tpVariant::tpVariant(const std::vector<double> &valueVector)
{
    data_.m_vt = (uint16_t)VariantType::tpReal8 | (uint16_t)VariantType::tpSet;
    auto pSetVal = new std::set<double>();
    for (auto value : valueVector)
        pSetVal->insert(value);
    data_.data.m_pSetVal = pSetVal;
}

tpVariant::tpVariant(const std::vector<std::string> &valueVector)
{
    data_.m_vt = (uint16_t)VariantType::tpBstr | (uint16_t)VariantType::tpSet;
    auto pSetVal = new std::set<std::string>();
    for (auto value : valueVector)
        pSetVal->insert(value);
    data_.data.m_pSetVal = pSetVal;
}

tpVariant::tpVariant(const std::set<bool> &valueSet)
{
    data_.m_vt = (uint16_t)VariantType::tpBool | (uint16_t)VariantType::tpSet;
    data_.data.m_pSetVal = new std::set<bool>(valueSet);
}

tpVariant::tpVariant(const std::set<int32_t> &valueSet)
{
    data_.m_vt = (uint16_t)VariantType::tpInt4 | (uint16_t)VariantType::tpSet;
    data_.data.m_pSetVal = new std::set<int32_t>(valueSet);
}

tpVariant::tpVariant(const std::set<uint32_t> &valueSet)
{
    data_.m_vt = (uint16_t)VariantType::tpUint4 | (uint16_t)VariantType::tpSet;
    data_.data.m_pSetVal = new std::set<uint32_t>(valueSet);
}

tpVariant::tpVariant(const std::set<int64_t> &valueSet)
{
    data_.m_vt = (uint16_t)VariantType::tpInt8 | (uint16_t)VariantType::tpSet;
    data_.data.m_pSetVal = new std::set<int64_t>(valueSet);
}

tpVariant::tpVariant(const std::set<uint64_t> &valueSet)
{
    data_.m_vt = (uint16_t)VariantType::tpUint8 | (uint16_t)VariantType::tpSet;
    data_.data.m_pSetVal = new std::set<uint64_t>(valueSet);
}

tpVariant::tpVariant(const std::set<float> &valueSet)
{
    data_.m_vt = (uint16_t)VariantType::tpReal4 | (uint16_t)VariantType::tpSet;
    data_.data.m_pSetVal = new std::set<float>(valueSet);
}

tpVariant::tpVariant(const std::set<double> &valueSet)
{
    data_.m_vt = (uint16_t)VariantType::tpReal8 | (uint16_t)VariantType::tpSet;
    data_.data.m_pSetVal = new std::set<double>(valueSet);
}

tpVariant::tpVariant(const std::set<std::string> &valueSet)
{
    data_.m_vt = (uint16_t)VariantType::tpBstr | (uint16_t)VariantType::tpSet;
    data_.data.m_pSetVal = new std::set<std::string>(valueSet);
}

tpVariant::tpVariant(const VariantValue &value)
{
    VariantValueInit(data_);
    VariantValueCopy(data_, value);
}

tpVariant::tpVariant(const tpVariant &other)
{
    VariantValueInit(data_);
    VariantValueCopy(data_, other.data_);
}

bool tpVariant::isNull()
{
    return (data_.m_vt == (uint16_t)VariantType::tpEmpty);
}

tpVariant &tpVariant::operator=(bool bValue)
{
    VariantValueClear(data_);
    data_.m_vt = (uint16_t)VariantType::tpBool;
    data_.data.m_bVal = bValue;
    return *this;
}

tpVariant &tpVariant::operator=(int32_t nValue)
{
    VariantValueClear(data_);
    data_.m_vt = (uint16_t)VariantType::tpInt4;
    data_.data.m_i4Val = nValue;
    return *this;
}

tpVariant &tpVariant::operator=(uint32_t uValue)
{
    VariantValueClear(data_);
    data_.m_vt = (uint16_t)VariantType::tpUint4;
    data_.data.m_ui4Val = uValue;
    return *this;
}

tpVariant &tpVariant::operator=(int64_t nValue)
{
    VariantValueClear(data_);
    data_.m_vt = (uint16_t)VariantType::tpInt8;
    data_.data.m_i8Val = nValue;
    return *this;
}

tpVariant &tpVariant::operator=(uint64_t uValue)
{
    VariantValueClear(data_);
    data_.m_vt = (uint16_t)VariantType::tpUint8;
    data_.data.m_ui8Val = uValue;
    return *this;
}

tpVariant &tpVariant::operator=(float fValue)
{
    VariantValueClear(data_);
    data_.m_vt = (uint16_t)VariantType::tpReal4;
    data_.data.m_r4Val = fValue;
    return *this;
}

tpVariant &tpVariant::operator=(double dValue)
{
    VariantValueClear(data_);
    data_.m_vt = (uint16_t)VariantType::tpReal8;
    data_.data.m_r8Val = dValue;
    return *this;
}

tpVariant &tpVariant::operator=(ItpRect value)
{
    VariantValueClear(data_);
    data_.m_vt = (uint16_t)VariantType::tpRect;
    data_.data.tpRectValue = value;
    return *this;
}

tpVariant &tpVariant::operator=(ItpSize value)
{
    VariantValueClear(data_);
    data_.m_vt = (uint16_t)VariantType::tpSize;
    data_.data.tpSizeValue = value;
    return *this;
}

tpVariant &tpVariant::operator=(ItpPoint value)
{
    VariantValueClear(data_);
    data_.m_vt = (uint16_t)VariantType::tpPoint;
    data_.data.tpPointValue = value;
    return *this;
}

tpVariant &tpVariant::operator=(const char *pChar)
{
    VariantValueClear(data_);
    data_.m_vt = (uint16_t)VariantType::tpBstr;
    if (nullptr != pChar)
    {
        uint32_t uLen = strlen(pChar);

        data_.data.m_strVal = new char[uLen + 1];
        memcpy(data_.data.m_strVal, pChar, uLen);
        data_.data.m_strVal[uLen] = '\0';
    }
    else
    {
        data_.data.m_strVal = nullptr;
    }

    return *this;
}

tpVariant &tpVariant::operator=(const std::string &strChar)
{
    VariantValueClear(data_);

    uint32_t uLen = static_cast<uint32_t>(strChar.length());

    data_.m_vt = (uint16_t)VariantType::tpBstr;
    if (0 != uLen)
    {
        data_.data.m_strVal = new char[uLen + 1];
        memcpy(data_.data.m_strVal, strChar.c_str(), uLen);
        data_.data.m_strVal[uLen] = '\0';
    }
    else
    {
        data_.data.m_strVal = nullptr;
    }

    return *this;
}

tpVariant &tpVariant::operator=(const std::vector<bool> &valueVector)
{
    VariantValueClear(data_);
    data_.m_vt = (uint16_t)VariantType::tpBool | (uint16_t)VariantType::tpSet;
    auto pSetVal = new std::set<bool>();
    for (auto value : valueVector)
        pSetVal->insert(value);
    data_.data.m_pSetVal = pSetVal;

    return *this;
}

tpVariant &tpVariant::operator=(const std::vector<int32_t> &valueVector)
{
    VariantValueClear(data_);
    data_.m_vt = (uint16_t)VariantType::tpInt4 | (uint16_t)VariantType::tpSet;
    auto pSetVal = new std::set<int32_t>();
    for (auto value : valueVector)
        pSetVal->insert(value);
    data_.data.m_pSetVal = pSetVal;

    return *this;
}

tpVariant &tpVariant::operator=(const std::vector<uint32_t> &valueVector)
{
    VariantValueClear(data_);
    data_.m_vt = (uint16_t)VariantType::tpUint4 | (uint16_t)VariantType::tpSet;
    auto pSetVal = new std::set<uint32_t>();
    for (auto value : valueVector)
        pSetVal->insert(value);
    data_.data.m_pSetVal = pSetVal;

    return *this;
}

tpVariant &tpVariant::operator=(const std::vector<int64_t> &valueVector)
{
    VariantValueClear(data_);
    data_.m_vt = (uint16_t)VariantType::tpInt8 | (uint16_t)VariantType::tpSet;
    auto pSetVal = new std::set<int64_t>();
    for (auto value : valueVector)
        pSetVal->insert(value);
    data_.data.m_pSetVal = pSetVal;

    return *this;
}

tpVariant &tpVariant::operator=(const std::vector<uint64_t> &valueVector)
{
    VariantValueClear(data_);
    data_.m_vt = (uint16_t)VariantType::tpUint8 | (uint16_t)VariantType::tpSet;
    auto pSetVal = new std::set<uint64_t>();
    for (auto value : valueVector)
        pSetVal->insert(value);
    data_.data.m_pSetVal = pSetVal;

    return *this;
}

tpVariant &tpVariant::operator=(const std::vector<float> &valueVector)
{
    VariantValueClear(data_);
    data_.m_vt = (uint16_t)VariantType::tpReal4 | (uint16_t)VariantType::tpSet;
    auto pSetVal = new std::set<float>();
    for (auto value : valueVector)
        pSetVal->insert(value);
    data_.data.m_pSetVal = pSetVal;

    return *this;
}

tpVariant &tpVariant::operator=(const std::vector<double> &valueVector)
{
    VariantValueClear(data_);
    data_.m_vt = (uint16_t)VariantType::tpReal8 | (uint16_t)VariantType::tpSet;
    auto pSetVal = new std::set<double>();
    for (auto value : valueVector)
        pSetVal->insert(value);
    data_.data.m_pSetVal = pSetVal;

    return *this;
}

tpVariant &tpVariant::operator=(const std::vector<std::string> &valueVector)
{
    VariantValueClear(data_);
    data_.m_vt = (uint16_t)VariantType::tpBstr | (uint16_t)VariantType::tpSet;
    auto pSetVal = new std::set<std::string>();
    for (auto value : valueVector)
        pSetVal->insert(value);
    data_.data.m_pSetVal = pSetVal;

    return *this;
}

tpVariant &tpVariant::operator=(const std::set<bool> &valueSet)
{
    VariantValueClear(data_);
    data_.m_vt = (uint16_t)VariantType::tpBool | (uint16_t)VariantType::tpSet;
    data_.data.m_pSetVal = new std::set<bool>(valueSet);
    return *this;
}

tpVariant &tpVariant::operator=(const std::set<int32_t> &valueSet)
{
    VariantValueClear(data_);
    data_.m_vt = (uint16_t)VariantType::tpInt4 | (uint16_t)VariantType::tpSet;
    data_.data.m_pSetVal = new std::set<int32_t>(valueSet);
    return *this;
}

tpVariant &tpVariant::operator=(const std::set<uint32_t> &valueSet)
{
    VariantValueClear(data_);
    data_.m_vt = (uint16_t)VariantType::tpUint4 | (uint16_t)VariantType::tpSet;
    data_.data.m_pSetVal = new std::set<uint32_t>(valueSet);
    return *this;
}

tpVariant &tpVariant::operator=(const std::set<int64_t> &valueSet)
{
    VariantValueClear(data_);
    data_.m_vt = (uint16_t)VariantType::tpInt8 | (uint16_t)VariantType::tpSet;
    data_.data.m_pSetVal = new std::set<int64_t>(valueSet);
    return *this;
}

tpVariant &tpVariant::operator=(const std::set<uint64_t> &valueSet)
{
    VariantValueClear(data_);
    data_.m_vt = (uint16_t)VariantType::tpUint8 | (uint16_t)VariantType::tpSet;
    data_.data.m_pSetVal = new std::set<uint64_t>(valueSet);
    return *this;
}

tpVariant &tpVariant::operator=(const std::set<float> &valueSet)
{
    VariantValueClear(data_);
    data_.m_vt = (uint16_t)VariantType::tpReal4 | (uint16_t)VariantType::tpSet;
    data_.data.m_pSetVal = new std::set<float>(valueSet);
    return *this;
}

tpVariant &tpVariant::operator=(const std::set<double> &valueSet)
{
    VariantValueClear(data_);
    data_.m_vt = (uint16_t)VariantType::tpReal8 | (uint16_t)VariantType::tpSet;
    data_.data.m_pSetVal = new std::set<double>(valueSet);
    return *this;
}

tpVariant &tpVariant::operator=(const std::set<std::string> &valueSet)
{
    VariantValueClear(data_);
    data_.m_vt = (uint16_t)VariantType::tpBstr | (uint16_t)VariantType::tpSet;
    data_.data.m_pSetVal = new std::set<std::string>(valueSet);
    return *this;
}

tpVariant &tpVariant::operator=(const tpVariant::VariantValue &value)
{
    if (&this->data_ != &value)
    {
        VariantValueClear(data_);
        VariantValueCopy(data_, value);
    }

    return *this;
}

tpVariant &tpVariant::operator=(const tpVariant &other)
{
    if (this != &other)
    {
        VariantValueClear(data_);
        VariantValueCopy(data_, other.data_);
    }

    return *this;
}

// bool tpVariant::operator==(const tpVariant::VariantValue &value)
// {
//     return Compare(value);
// }

bool tpVariant::operator==(const tpVariant &value)
{
    return Compare(value.data_);
}

bool tpVariant::operator!=(const VariantValue &value)
{
    return !Compare(value);
}

bool tpVariant::operator!=(const tpVariant &value)
{
    return !Compare(value.data_);
}

std::vector<bool> tpVariant::ToBoolArray() const
{
    std::vector<bool> arrayValue;
    std::set<bool> *pSetVal = static_cast<std::set<bool> *>(data_.data.m_pSetVal);

    if (pSetVal)
    {
        uint32_t i = 0;

        assert(data_.m_vt == (((uint16_t)VariantType::tpBool) | (uint16_t)VariantType::tpSet));
        arrayValue.resize(pSetVal->size());
        for (auto subValue : *pSetVal)
            arrayValue[i++] = subValue;
    }
    else
    {
        assert(data_.m_vt == (uint16_t)VariantType::tpEmpty);
    }

    return arrayValue;
}

std::vector<int32_t> tpVariant::ToInt32Array() const
{
    std::vector<int32_t> arrayValue;
    std::set<int32_t> *pSetVal = static_cast<std::set<int32_t> *>(data_.data.m_pSetVal);

    if (pSetVal)
    {
        uint32_t i = 0;

        assert(data_.m_vt == (((uint16_t)VariantType::tpInt4) | (uint16_t)VariantType::tpSet));
        arrayValue.resize(pSetVal->size());
        for (auto subValue : *pSetVal)
            arrayValue[i++] = subValue;
    }
    else
    {
        assert(data_.m_vt == (uint16_t)VariantType::tpEmpty);
    }

    return arrayValue;
}

std::vector<uint32_t> tpVariant::ToUint32Array() const
{
    std::vector<uint32_t> arrayValue;
    std::set<uint32_t> *pSetVal = static_cast<std::set<uint32_t> *>(data_.data.m_pSetVal);

    if (pSetVal)
    {
        uint32_t i = 0;

        assert(data_.m_vt == (((uint16_t)VariantType::tpUint4) | (uint16_t)VariantType::tpSet));
        arrayValue.resize(pSetVal->size());
        for (auto subValue : *pSetVal)
            arrayValue[i++] = subValue;
    }
    else
    {
        assert(data_.m_vt == (uint16_t)VariantType::tpEmpty);
    }

    return arrayValue;
}

std::vector<int64_t> tpVariant::ToInt64Array() const
{
    std::vector<int64_t> arrayValue;
    std::set<int64_t> *pSetVal = static_cast<std::set<int64_t> *>(data_.data.m_pSetVal);

    if (pSetVal)
    {
        uint32_t i = 0;

        assert(data_.m_vt == (((uint16_t)VariantType::tpInt8) | (uint16_t)VariantType::tpSet));
        arrayValue.resize(pSetVal->size());
        for (auto subValue : *pSetVal)
            arrayValue[i++] = subValue;
    }
    else
    {
        assert(data_.m_vt == (uint16_t)VariantType::tpEmpty);
    }

    return arrayValue;
}

std::vector<uint64_t> tpVariant::ToUint64Array() const
{
    std::vector<uint64_t> arrayValue;
    std::set<uint64_t> *pSetVal = static_cast<std::set<uint64_t> *>(data_.data.m_pSetVal);

    if (pSetVal)
    {
        uint32_t i = 0;

        assert(data_.m_vt == (((uint16_t)VariantType::tpUint8) | (uint16_t)VariantType::tpSet));
        arrayValue.resize(pSetVal->size());
        for (auto subValue : *pSetVal)
            arrayValue[i++] = subValue;
    }
    else
    {
        assert(data_.m_vt == (uint16_t)VariantType::tpEmpty);
    }

    return arrayValue;
}

std::vector<float> tpVariant::ToFloatArray() const
{
    std::vector<float> arrayValue;
    std::set<float> *pSetVal = static_cast<std::set<float> *>(data_.data.m_pSetVal);

    if (pSetVal)
    {
        uint32_t i = 0;

        assert(data_.m_vt == (((uint16_t)VariantType::tpReal4) | (uint16_t)VariantType::tpSet));
        arrayValue.resize(pSetVal->size());
        for (auto subValue : *pSetVal)
            arrayValue[i++] = subValue;
    }
    else
    {
        assert(data_.m_vt == (uint16_t)VariantType::tpEmpty);
    }

    return arrayValue;
}

std::vector<double> tpVariant::ToDoubleArray() const
{
    std::vector<double> arrayValue;
    std::set<double> *pSetVal = static_cast<std::set<double> *>(data_.data.m_pSetVal);

    if (pSetVal)
    {
        uint32_t i = 0;

        assert(data_.m_vt == (((uint16_t)VariantType::tpReal8) | (uint16_t)VariantType::tpSet));
        arrayValue.resize(pSetVal->size());
        for (auto subValue : *pSetVal)
            arrayValue[i++] = subValue;
    }
    else
    {
        assert(data_.m_vt == (uint16_t)VariantType::tpEmpty);
    }

    return arrayValue;
}

std::vector<std::string> tpVariant::ToStringArray() const
{
    std::vector<std::string> arrayValue;
    std::set<std::string> *pSetVal = static_cast<std::set<std::string> *>(data_.data.m_pSetVal);

    if (pSetVal)
    {
        uint32_t i = 0;

        assert(data_.m_vt == (((uint16_t)VariantType::tpBstr) | (uint16_t)VariantType::tpSet));
        arrayValue.resize(pSetVal->size());
        for (auto subValue : *pSetVal)
            arrayValue[i++] = subValue;
    }
    else
    {
        assert(data_.m_vt == (uint16_t)VariantType::tpEmpty);
    }

    return arrayValue;
}

std::set<bool> &tpVariant::ToBoolSet()
{
    if (nullptr == data_.data.m_pSetVal)
    {
        assert(data_.m_vt == (uint16_t)VariantType::tpEmpty);
        data_.m_vt = (((uint16_t)VariantType::tpBool) | (uint16_t)VariantType::tpSet);
        data_.data.m_pSetVal = new std::set<bool>();
    }
    else
    {
        assert(data_.m_vt == (((uint16_t)VariantType::tpBool) | (uint16_t)VariantType::tpSet));
    }

    return *static_cast<std::set<bool> *>(data_.data.m_pSetVal);
}

std::set<int32_t> &tpVariant::ToInt32Set()
{
    if (nullptr == data_.data.m_pSetVal)
    {
        assert(data_.m_vt == (uint16_t)VariantType::tpEmpty);
        data_.m_vt = (((uint16_t)VariantType::tpInt4) | (uint16_t)VariantType::tpSet);
        data_.data.m_pSetVal = new std::set<int32_t>();
    }
    else
    {
        assert(data_.m_vt == (((uint16_t)VariantType::tpInt4) | (uint16_t)VariantType::tpSet));
    }

    return *static_cast<std::set<int32_t> *>(data_.data.m_pSetVal);
}

std::set<uint32_t> &tpVariant::ToUint32Set()
{
    if (nullptr == data_.data.m_pSetVal)
    {
        assert(data_.m_vt == (uint16_t)VariantType::tpEmpty);
        data_.m_vt = (((uint16_t)VariantType::tpUint4) | (uint16_t)VariantType::tpSet);
        data_.data.m_pSetVal = new std::set<uint32_t>();
    }
    else
    {
        assert((data_.m_vt == (((uint16_t)VariantType::tpUint4) | (uint16_t)VariantType::tpSet)));
    }

    return *static_cast<std::set<uint32_t> *>(data_.data.m_pSetVal);
}

std::set<int64_t> &tpVariant::ToInt64Set()
{
    if (nullptr == data_.data.m_pSetVal)
    {
        assert(data_.m_vt == (uint16_t)VariantType::tpEmpty);
        data_.m_vt = (((uint16_t)VariantType::tpInt8) | (uint16_t)VariantType::tpSet);
        data_.data.m_pSetVal = new std::set<int64_t>();
    }
    else
    {
        assert(data_.m_vt == (((uint16_t)VariantType::tpInt8) | (uint16_t)VariantType::tpSet));
    }

    return *static_cast<std::set<int64_t> *>(data_.data.m_pSetVal);
}

std::set<uint64_t> &tpVariant::ToUint64Set()
{
    if (nullptr == data_.data.m_pSetVal)
    {
        assert(data_.m_vt == (uint16_t)VariantType::tpEmpty);
        data_.m_vt = (((uint16_t)VariantType::tpUint8) | (uint16_t)VariantType::tpSet);
        data_.data.m_pSetVal = new std::set<uint64_t>();
    }
    else
    {
        assert(data_.m_vt == (((uint16_t)VariantType::tpUint8) | (uint16_t)VariantType::tpSet));
    }
    return *static_cast<std::set<uint64_t> *>(data_.data.m_pSetVal);
}

std::set<float> &tpVariant::ToFloatSet()
{
    if (nullptr == data_.data.m_pSetVal)
    {
        assert(data_.m_vt == (uint16_t)VariantType::tpEmpty);
        data_.m_vt = (((uint16_t)VariantType::tpReal4) | (uint16_t)VariantType::tpSet);
        data_.data.m_pSetVal = new std::set<float>();
    }
    else
    {
        assert(data_.m_vt == (((uint16_t)VariantType::tpReal4) | (uint16_t)VariantType::tpSet));
    }

    return *static_cast<std::set<float> *>(data_.data.m_pSetVal);
}

std::set<double> &tpVariant::ToDoubleSet()
{
    if (nullptr == data_.data.m_pSetVal)
    {
        assert(data_.m_vt == (uint16_t)VariantType::tpEmpty);
        data_.m_vt = (((uint16_t)VariantType::tpReal8) | (uint16_t)VariantType::tpSet);
        data_.data.m_pSetVal = new std::set<double>();
    }
    else
    {
        assert(data_.m_vt == (((uint16_t)VariantType::tpReal8) | (uint16_t)VariantType::tpSet));
    }

    return *static_cast<std::set<double> *>(data_.data.m_pSetVal);
}

std::set<std::string> &tpVariant::ToStringSet()
{
    if (nullptr == data_.data.m_pSetVal)
    {
        assert(data_.m_vt == (uint16_t)VariantType::tpEmpty);
        data_.m_vt = (((uint16_t)VariantType::tpBstr) | (uint16_t)VariantType::tpSet);
        data_.data.m_pSetVal = new std::set<std::string>();
    }
    else
    {
        assert(data_.m_vt == (((uint16_t)VariantType::tpBstr) | (uint16_t)VariantType::tpSet));
    }
    return *static_cast<std::set<std::string> *>(data_.data.m_pSetVal);
}

bool tpVariant::Compare(const VariantValue &value)
{
    if (data_.m_vt != value.m_vt)
        return false;

    if (data_.m_vt & (uint16_t)VariantType::tpSet)
    {
        uint16_t uType = (uint16_t)value.m_vt & 0xff;
        bool bResult = false;

        switch ((VariantType)uType)
        {
        case VariantType::tpBool:
            bResult = VariantSetValueComplie(reinterpret_cast<std::set<bool> *>(data_.data.m_pSetVal), reinterpret_cast<std::set<bool> *>(value.data.m_pSetVal));
            break;
        case VariantType::tpInt4:
            bResult = VariantSetValueComplie(reinterpret_cast<std::set<int32_t> *>(data_.data.m_pSetVal), reinterpret_cast<std::set<int32_t> *>(value.data.m_pSetVal));
            break;
        case VariantType::tpUint4:
            bResult = VariantSetValueComplie(reinterpret_cast<std::set<uint32_t> *>(data_.data.m_pSetVal), reinterpret_cast<std::set<uint32_t> *>(value.data.m_pSetVal));
            break;
        case VariantType::tpInt8:
            bResult = VariantSetValueComplie(reinterpret_cast<std::set<int64_t> *>(data_.data.m_pSetVal), reinterpret_cast<std::set<int64_t> *>(value.data.m_pSetVal));
            break;
        case VariantType::tpUint8:
            bResult = VariantSetValueComplie(reinterpret_cast<std::set<uint64_t> *>(data_.data.m_pSetVal), reinterpret_cast<std::set<uint64_t> *>(value.data.m_pSetVal));
            break;
        case VariantType::tpReal4:
            bResult = VariantSetValueComplie(reinterpret_cast<std::set<float> *>(data_.data.m_pSetVal), reinterpret_cast<std::set<float> *>(value.data.m_pSetVal));
            break;
        case VariantType::tpReal8:
            bResult = VariantSetValueComplie(reinterpret_cast<std::set<double> *>(data_.data.m_pSetVal), reinterpret_cast<std::set<double> *>(value.data.m_pSetVal));
            break;
        case VariantType::tpBstr:
            bResult = VariantSetValueComplie(reinterpret_cast<std::set<std::string> *>(data_.data.m_pSetVal), reinterpret_cast<std::set<std::string> *>(value.data.m_pSetVal));
            break;
        default:
            assert(false);
            break;
        }

        return bResult;
    }
    else
    {
        if (data_.m_vt != (uint16_t)VariantType::tpBstr)
        {
            return 0 == memcmp(this, &value, sizeof(value));
        }
        else
        {
            if ((nullptr == data_.data.m_strVal) && (nullptr == value.data.m_strVal))
                return true;

            if ((nullptr == data_.data.m_strVal) || (nullptr == value.data.m_strVal))
                return false;

            return 0 == strcmp(data_.data.m_strVal, value.data.m_strVal);
        }
    }
}