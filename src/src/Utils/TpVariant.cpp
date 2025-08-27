/***
 * @Author: hywang
 * @Date: 2024-05-22 09:41:10
 * @LastEditors: hywang
 * @LastEditTime: 2024-05-23 16:28:20
 * @FilePath: /pix-singlegui/PixSingleGUI/src/Utils/TpVariant.cpp
 * @Description:
 * @
 * @PiXOS
 */

#include "TpVariant.h"
#include "TpVariant_p.h"

#include "TpRect.h"
#include "TpColors.h"
#include "TpSize.h"
#include "TpFont.h"

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

TpVariant::TpVariant()
{
    VariantValueInit(data_);
}

TpVariant::TpVariant(bool bValue)
{
    data_.m_vt = (uint16_t)VariantType::tpBool;
    data_.data.m_bVal = bValue;
}


TpVariant::TpVariant(int8_t nValue)
{
    data_.m_vt = (uint16_t)VariantType::TpInt1;
    data_.data.m_i1Val = nValue;
}

TpVariant::TpVariant(uint8_t nValue)
{
	data_.m_vt = (uint16_t)VariantType::TpUint1;
    data_.data.m_ui1Val = nValue;
}

TpVariant::TpVariant(int16_t nValue)
{
    data_.m_vt = (uint16_t)VariantType::TpInt2;
    data_.data.m_i2Val = nValue;
}

TpVariant::TpVariant(uint16_t nValue)
{
	data_.m_vt = (uint16_t)VariantType::TpUint2;
    data_.data.m_ui2Val = nValue;
}

TpVariant::TpVariant(int32_t nValue)
{
    data_.m_vt = (uint16_t)VariantType::TpInt4;
    data_.data.m_i4Val = nValue;
}

TpVariant::TpVariant(uint32_t uValue)
{
    data_.m_vt = (uint16_t)VariantType::TpUint4;
    data_.data.m_ui4Val = uValue;
}

TpVariant::TpVariant(int64_t nValue)
{
    data_.m_vt = (uint16_t)VariantType::tpInt8;
    data_.data.m_i8Val = nValue;
}

TpVariant::TpVariant(uint64_t uValue)
{
    data_.m_vt = (uint16_t)VariantType::TpUint8;
    data_.data.m_ui8Val = uValue;
}

TpVariant::TpVariant(float fValue)
{
    data_.m_vt = (uint16_t)VariantType::TpReal4;
    data_.data.m_r4Val = fValue;
}

TpVariant::TpVariant(double dValue)
{
    data_.m_vt = (uint16_t)VariantType::TpReal8;
    data_.data.m_r8Val = dValue;
}

TpVariant::TpVariant(const char *pChar)
{
    data_.m_vt = (uint16_t)VariantType::TpBstr;
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

TpVariant::TpVariant(const std::string &strChar)
{
    uint32_t uLen = static_cast<uint32_t>(strChar.length());

    data_.m_vt = (uint16_t)VariantType::TpBstr;
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

TpVariant::TpVariant(const ItpRect &value)
{
    data_.m_vt = (uint16_t)VariantType::TpRect;
    data_.data.TpRectValue = value;
}

TpVariant::TpVariant(const ItpSize &value)
{
    data_.m_vt = (uint16_t)VariantType::TpSize;
    data_.data.TpSizeValue = value;
}

TpVariant::TpVariant(const ItpPoint &value)
{
    data_.m_vt = (uint16_t)VariantType::TpPoint;
    data_.data.TpPointValue = value;
}

TpVariant::TpVariant(const std::vector<bool> &valueVector)
{
    data_.m_vt = (uint16_t)VariantType::tpBool | (uint16_t)VariantType::TpSet;
    auto pSetVal = new std::set<bool>();
    for (auto value : valueVector)
        pSetVal->insert(value);
    data_.data.m_pSetVal = pSetVal;
}

TpVariant::TpVariant(const std::vector<int8_t> &valueVector)
{
    data_.m_vt = (uint16_t)VariantType::TpInt1 | (uint16_t)VariantType::TpSet;
    auto pSetVal = new std::set<int8_t>();
    for (auto value : valueVector)
        pSetVal->insert(value);
    data_.data.m_pSetVal = pSetVal;
}

TpVariant::TpVariant(const std::vector<uint8_t> &valueVector)
{
    data_.m_vt = (uint16_t)VariantType::TpUint1 | (uint16_t)VariantType::TpSet;
    auto pSetVal = new std::set<uint8_t>();
    for (auto value : valueVector)
        pSetVal->insert(value);
    data_.data.m_pSetVal = pSetVal;
}

TpVariant::TpVariant(const std::vector<int16_t> &valueVector)
{
    data_.m_vt = (uint16_t)VariantType::TpInt2 | (uint16_t)VariantType::TpSet;
    auto pSetVal = new std::set<int16_t>();
    for (auto value : valueVector)
        pSetVal->insert(value);
    data_.data.m_pSetVal = pSetVal;
}

TpVariant::TpVariant(const std::vector<uint16_t> &valueVector)
{
    data_.m_vt = (uint16_t)VariantType::TpUint2 | (uint16_t)VariantType::TpSet;
    auto pSetVal = new std::set<uint16_t>();
    for (auto value : valueVector)
        pSetVal->insert(value);
    data_.data.m_pSetVal = pSetVal;
}

TpVariant::TpVariant(const std::vector<int32_t> &valueVector)
{
    data_.m_vt = (uint16_t)VariantType::TpInt4 | (uint16_t)VariantType::TpSet;
    auto pSetVal = new std::set<int32_t>();
    for (auto value : valueVector)
        pSetVal->insert(value);
    data_.data.m_pSetVal = pSetVal;
}

TpVariant::TpVariant(const std::vector<uint32_t> &valueVector)
{
    data_.m_vt = (uint16_t)VariantType::TpUint4 | (uint16_t)VariantType::TpSet;
    auto pSetVal = new std::set<uint32_t>();
    for (auto value : valueVector)
        pSetVal->insert(value);
    data_.data.m_pSetVal = pSetVal;
}

TpVariant::TpVariant(const std::vector<int64_t> &valueVector)
{
    data_.m_vt = (uint16_t)VariantType::tpInt8 | (uint16_t)VariantType::TpSet;
    auto pSetVal = new std::set<int64_t>();
    for (auto value : valueVector)
        pSetVal->insert(value);
    data_.data.m_pSetVal = pSetVal;
}

TpVariant::TpVariant(const std::vector<uint64_t> &valueVector)
{
    data_.m_vt = (uint16_t)VariantType::TpUint8 | (uint16_t)VariantType::TpSet;
    auto pSetVal = new std::set<uint64_t>();
    for (auto value : valueVector)
        pSetVal->insert(value);
    data_.data.m_pSetVal = pSetVal;
}

TpVariant::TpVariant(const std::vector<float> &valueVector)
{
    data_.m_vt = (uint16_t)VariantType::TpReal4 | (uint16_t)VariantType::TpSet;
    auto pSetVal = new std::set<float>();
    for (auto value : valueVector)
        pSetVal->insert(value);
    data_.data.m_pSetVal = pSetVal;
}

TpVariant::TpVariant(const std::vector<double> &valueVector)
{
    data_.m_vt = (uint16_t)VariantType::TpReal8 | (uint16_t)VariantType::TpSet;
    auto pSetVal = new std::set<double>();
    for (auto value : valueVector)
        pSetVal->insert(value);
    data_.data.m_pSetVal = pSetVal;
}

TpVariant::TpVariant(const std::vector<std::string> &valueVector)
{
    data_.m_vt = (uint16_t)VariantType::TpBstr | (uint16_t)VariantType::TpSet;
    auto pSetVal = new std::set<std::string>();
    for (auto value : valueVector)
        pSetVal->insert(value);
    data_.data.m_pSetVal = pSetVal;
}

TpVariant::TpVariant(const std::set<bool> &valueSet)
{
    data_.m_vt = (uint16_t)VariantType::tpBool | (uint16_t)VariantType::TpSet;
    data_.data.m_pSetVal = new std::set<bool>(valueSet);
}

TpVariant::TpVariant(const std::set<int8_t> &valueSet)
{
    data_.m_vt = (uint16_t)VariantType::TpInt1 | (uint16_t)VariantType::TpSet;
    data_.data.m_pSetVal = new std::set<int8_t>(valueSet);
}

TpVariant::TpVariant(const std::set<uint8_t> &valueSet)
{
    data_.m_vt = (uint16_t)VariantType::TpUint1 | (uint16_t)VariantType::TpSet;
    data_.data.m_pSetVal = new std::set<uint8_t>(valueSet);
}

TpVariant::TpVariant(const std::set<int16_t> &valueSet)
{
    data_.m_vt = (uint16_t)VariantType::TpInt2 | (uint16_t)VariantType::TpSet;
    data_.data.m_pSetVal = new std::set<int16_t>(valueSet);
}

TpVariant::TpVariant(const std::set<uint16_t> &valueSet)
{
    data_.m_vt = (uint16_t)VariantType::TpUint2 | (uint16_t)VariantType::TpSet;
	data_.data.m_pSetVal = new std::set<uint16_t>(valueSet);
}

TpVariant::TpVariant(const std::set<int32_t> &valueSet)
{
    data_.m_vt = (uint16_t)VariantType::TpInt4 | (uint16_t)VariantType::TpSet;
    data_.data.m_pSetVal = new std::set<int32_t>(valueSet);
}

TpVariant::TpVariant(const std::set<uint32_t> &valueSet)
{
    data_.m_vt = (uint16_t)VariantType::TpUint4 | (uint16_t)VariantType::TpSet;
    data_.data.m_pSetVal = new std::set<uint32_t>(valueSet);
}

TpVariant::TpVariant(const std::set<int64_t> &valueSet)
{
    data_.m_vt = (uint16_t)VariantType::tpInt8 | (uint16_t)VariantType::TpSet;
    data_.data.m_pSetVal = new std::set<int64_t>(valueSet);
}

TpVariant::TpVariant(const std::set<uint64_t> &valueSet)
{
    data_.m_vt = (uint16_t)VariantType::TpUint8 | (uint16_t)VariantType::TpSet;
    data_.data.m_pSetVal = new std::set<uint64_t>(valueSet);
}

TpVariant::TpVariant(const std::set<float> &valueSet)
{
    data_.m_vt = (uint16_t)VariantType::TpReal4 | (uint16_t)VariantType::TpSet;
    data_.data.m_pSetVal = new std::set<float>(valueSet);
}

TpVariant::TpVariant(const std::set<double> &valueSet)
{
    data_.m_vt = (uint16_t)VariantType::TpReal8 | (uint16_t)VariantType::TpSet;
    data_.data.m_pSetVal = new std::set<double>(valueSet);
}

TpVariant::TpVariant(const std::set<std::string> &valueSet)
{
    data_.m_vt = (uint16_t)VariantType::TpBstr | (uint16_t)VariantType::TpSet;
    data_.data.m_pSetVal = new std::set<std::string>(valueSet);
}

TpVariant::TpVariant(const VariantValue &value)
{
    VariantValueInit(data_);
    VariantValueCopy(data_, value);
}

TpVariant::TpVariant(const TpVariant &other)
{
    VariantValueInit(data_);
    VariantValueCopy(data_, other.data_);
}

TpVariant::TpVariant(std::vector<TpVariant>* vectorVal) {
	VariantValueInit(data_); // 确保初始化
    data_.m_vt = static_cast<uint16_t>(VariantType::TpVector);
    data_.data.m_vectorVal = vectorVal;
}

TpVariant::~TpVariant() {
    VariantValueClear(data_);
}


bool TpVariant::isNull()
{
    return (data_.m_vt == (uint16_t)VariantType::TpEmpty);
}

TpVariant &TpVariant::operator=(bool bValue)
{
    VariantValueClear(data_);
    data_.m_vt = (uint16_t)VariantType::tpBool;
    data_.data.m_bVal = bValue;
    return *this;
}
TpVariant &TpVariant::operator=(int8_t nValue)
{
    VariantValueClear(data_);
    data_.m_vt = (uint16_t)VariantType::TpInt1;
    data_.data.m_i1Val = nValue;
    return *this;
}

TpVariant &TpVariant::operator=(uint8_t uValue)
{
    VariantValueClear(data_);
    data_.m_vt = (uint16_t)VariantType::TpUint1;
    data_.data.m_ui1Val = uValue;
    return *this;
}
TpVariant &TpVariant::operator=(int16_t nValue)
{
    VariantValueClear(data_);
    data_.m_vt = (uint16_t)VariantType::TpInt2;
    data_.data.m_i2Val = nValue;
    return *this;
}

TpVariant &TpVariant::operator=(uint16_t uValue)
{
    VariantValueClear(data_);
    data_.m_vt = (uint16_t)VariantType::TpUint2;
    data_.data.m_ui2Val = uValue;
    return *this;
}

TpVariant &TpVariant::operator=(int32_t nValue)
{
    VariantValueClear(data_);
    data_.m_vt = (uint16_t)VariantType::TpInt4;
    data_.data.m_i4Val = nValue;
    return *this;
}

TpVariant &TpVariant::operator=(uint32_t uValue)
{
    VariantValueClear(data_);
    data_.m_vt = (uint16_t)VariantType::TpUint4;
    data_.data.m_ui4Val = uValue;
    return *this;
}

TpVariant &TpVariant::operator=(int64_t nValue)
{
    VariantValueClear(data_);
    data_.m_vt = (uint16_t)VariantType::tpInt8;
    data_.data.m_i8Val = nValue;
    return *this;
}

TpVariant &TpVariant::operator=(uint64_t uValue)
{
    VariantValueClear(data_);
    data_.m_vt = (uint16_t)VariantType::TpUint8;
    data_.data.m_ui8Val = uValue;
    return *this;
}

TpVariant &TpVariant::operator=(float fValue)
{
    VariantValueClear(data_);
    data_.m_vt = (uint16_t)VariantType::TpReal4;
    data_.data.m_r4Val = fValue;
    return *this;
}

TpVariant &TpVariant::operator=(double dValue)
{
    VariantValueClear(data_);
    data_.m_vt = (uint16_t)VariantType::TpReal8;
    data_.data.m_r8Val = dValue;
    return *this;
}

TpVariant &TpVariant::operator=(ItpRect value)
{
    VariantValueClear(data_);
    data_.m_vt = (uint16_t)VariantType::TpRect;
    data_.data.TpRectValue = value;
    return *this;
}

TpVariant &TpVariant::operator=(ItpSize value)
{
    VariantValueClear(data_);
    data_.m_vt = (uint16_t)VariantType::TpSize;
    data_.data.TpSizeValue = value;
    return *this;
}

TpVariant &TpVariant::operator=(ItpPoint value)
{
    VariantValueClear(data_);
    data_.m_vt = (uint16_t)VariantType::TpPoint;
    data_.data.TpPointValue = value;
    return *this;
}

TpVariant &TpVariant::operator=(const char *pChar)
{
    VariantValueClear(data_);
    data_.m_vt = (uint16_t)VariantType::TpBstr;
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

TpVariant &TpVariant::operator=(const std::string &strChar)
{
    VariantValueClear(data_);

    uint32_t uLen = static_cast<uint32_t>(strChar.length());

    data_.m_vt = (uint16_t)VariantType::TpBstr;
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

TpVariant &TpVariant::operator=(std::vector<TpVariant>* vectorVal) {
    VariantValueClear(data_);
    data_.m_vt = static_cast<uint16_t>(VariantType::TpVector);
    data_.data.m_vectorVal = vectorVal;
    return *this;
}

TpVariant &TpVariant::operator=(const std::vector<bool> &valueVector)
{
    VariantValueClear(data_);
    data_.m_vt = (uint16_t)VariantType::tpBool | (uint16_t)VariantType::TpSet;
    auto pSetVal = new std::set<bool>();
    for (auto value : valueVector)
        pSetVal->insert(value);
    data_.data.m_pSetVal = pSetVal;

    return *this;
}

TpVariant &TpVariant::operator=(const std::vector<int8_t> &valueVector)
{
    VariantValueClear(data_);
    data_.m_vt = (uint16_t)VariantType::TpInt1 | (uint16_t)VariantType::TpSet;
    auto pSetVal = new std::set<int8_t>();
    for (auto value : valueVector)
        pSetVal->insert(value);
    data_.data.m_pSetVal = pSetVal;
    return *this;
}

TpVariant &TpVariant::operator=(const std::vector<uint8_t> &valueVector)
{
    VariantValueClear(data_);
    data_.m_vt = (uint16_t)VariantType::TpUint1 | (uint16_t)VariantType::TpSet;
    auto pSetVal = new std::set<uint8_t>();
    for (auto value : valueVector)
        pSetVal->insert(value);
    data_.data.m_pSetVal = pSetVal;
    return *this;
}
TpVariant &TpVariant::operator=(const std::vector<int16_t> &valueVector)
{
    VariantValueClear(data_);
    data_.m_vt = (uint16_t)VariantType::TpInt2 | (uint16_t)VariantType::TpSet;
    auto pSetVal = new std::set<int16_t>();
    for (auto value : valueVector)
        pSetVal->insert(value);
    data_.data.m_pSetVal = pSetVal;
    return *this;
}

TpVariant &TpVariant::operator=(const std::vector<uint16_t> &valueVector)
{
    VariantValueClear(data_);
    data_.m_vt = (uint16_t)VariantType::TpUint2 | (uint16_t)VariantType::TpSet;
    auto pSetVal = new std::set<uint16_t>();
    for (auto value : valueVector)
        pSetVal->insert(value);
    data_.data.m_pSetVal = pSetVal;
    return *this;
}

TpVariant &TpVariant::operator=(const std::vector<int32_t> &valueVector)
{
    VariantValueClear(data_);
    data_.m_vt = (uint16_t)VariantType::TpInt4 | (uint16_t)VariantType::TpSet;
    auto pSetVal = new std::set<int32_t>();
    for (auto value : valueVector)
        pSetVal->insert(value);
    data_.data.m_pSetVal = pSetVal;

    return *this;
}

TpVariant &TpVariant::operator=(const std::vector<uint32_t> &valueVector)
{
    VariantValueClear(data_);
    data_.m_vt = (uint16_t)VariantType::TpUint4 | (uint16_t)VariantType::TpSet;
    auto pSetVal = new std::set<uint32_t>();
    for (auto value : valueVector)
        pSetVal->insert(value);
    data_.data.m_pSetVal = pSetVal;

    return *this;
}

TpVariant &TpVariant::operator=(const std::vector<int64_t> &valueVector)
{
    VariantValueClear(data_);
    data_.m_vt = (uint16_t)VariantType::tpInt8 | (uint16_t)VariantType::TpSet;
    auto pSetVal = new std::set<int64_t>();
    for (auto value : valueVector)
        pSetVal->insert(value);
    data_.data.m_pSetVal = pSetVal;

    return *this;
}

TpVariant &TpVariant::operator=(const std::vector<uint64_t> &valueVector)
{
    VariantValueClear(data_);
    data_.m_vt = (uint16_t)VariantType::TpUint8 | (uint16_t)VariantType::TpSet;
    auto pSetVal = new std::set<uint64_t>();
    for (auto value : valueVector)
        pSetVal->insert(value);
    data_.data.m_pSetVal = pSetVal;

    return *this;
}

TpVariant &TpVariant::operator=(const std::vector<float> &valueVector)
{
    VariantValueClear(data_);
    data_.m_vt = (uint16_t)VariantType::TpReal4 | (uint16_t)VariantType::TpSet;
    auto pSetVal = new std::set<float>();
    for (auto value : valueVector)
        pSetVal->insert(value);
    data_.data.m_pSetVal = pSetVal;

    return *this;
}

TpVariant &TpVariant::operator=(const std::vector<double> &valueVector)
{
    VariantValueClear(data_);
    data_.m_vt = (uint16_t)VariantType::TpReal8 | (uint16_t)VariantType::TpSet;
    auto pSetVal = new std::set<double>();
    for (auto value : valueVector)
        pSetVal->insert(value);
    data_.data.m_pSetVal = pSetVal;

    return *this;
}

TpVariant &TpVariant::operator=(const std::vector<std::string> &valueVector)
{
    VariantValueClear(data_);
    data_.m_vt = (uint16_t)VariantType::TpBstr | (uint16_t)VariantType::TpSet;
    auto pSetVal = new std::set<std::string>();
    for (auto value : valueVector)
        pSetVal->insert(value);
    data_.data.m_pSetVal = pSetVal;

    return *this;
}

TpVariant &TpVariant::operator=(const std::set<bool> &valueSet)
{
    VariantValueClear(data_);
    data_.m_vt = (uint16_t)VariantType::tpBool | (uint16_t)VariantType::TpSet;
    data_.data.m_pSetVal = new std::set<bool>(valueSet);
    return *this;
}

TpVariant &TpVariant::operator=(const std::set<int8_t> &valueSet)
{
    VariantValueClear(data_);
    data_.m_vt = (uint16_t)VariantType::TpInt1 | (uint16_t)VariantType::TpSet;
    data_.data.m_pSetVal = new std::set<int8_t>(valueSet);
    return *this;
}

TpVariant &TpVariant::operator=(const std::set<uint8_t> &valueSet)
{
    VariantValueClear(data_);
    data_.m_vt = (uint16_t)VariantType::TpUint1 | (uint16_t)VariantType::TpSet;
    data_.data.m_pSetVal = new std::set<uint8_t>(valueSet);
    return *this;
}

TpVariant &TpVariant::operator=(const std::set<int16_t> &valueSet)
{
    VariantValueClear(data_);
    data_.m_vt = (uint16_t)VariantType::TpInt2 | (uint16_t)VariantType::TpSet;
    data_.data.m_pSetVal = new std::set<int16_t>(valueSet);
    return *this;
}

TpVariant &TpVariant::operator=(const std::set<uint16_t> &valueSet)
{
    VariantValueClear(data_);
    data_.m_vt = (uint16_t)VariantType::TpUint2 | (uint16_t)VariantType::TpSet;
    data_.data.m_pSetVal = new std::set<uint16_t>(valueSet);
    return *this;
}

TpVariant &TpVariant::operator=(const std::set<int32_t> &valueSet)
{
    VariantValueClear(data_);
    data_.m_vt = (uint16_t)VariantType::TpInt4 | (uint16_t)VariantType::TpSet;
    data_.data.m_pSetVal = new std::set<int32_t>(valueSet);
    return *this;
}

TpVariant &TpVariant::operator=(const std::set<uint32_t> &valueSet)
{
    VariantValueClear(data_);
    data_.m_vt = (uint16_t)VariantType::TpUint4 | (uint16_t)VariantType::TpSet;
    data_.data.m_pSetVal = new std::set<uint32_t>(valueSet);
    return *this;
}

TpVariant &TpVariant::operator=(const std::set<int64_t> &valueSet)
{
    VariantValueClear(data_);
    data_.m_vt = (uint16_t)VariantType::tpInt8 | (uint16_t)VariantType::TpSet;
    data_.data.m_pSetVal = new std::set<int64_t>(valueSet);
    return *this;
}

TpVariant &TpVariant::operator=(const std::set<uint64_t> &valueSet)
{
    VariantValueClear(data_);
    data_.m_vt = (uint16_t)VariantType::TpUint8 | (uint16_t)VariantType::TpSet;
    data_.data.m_pSetVal = new std::set<uint64_t>(valueSet);
    return *this;
}

TpVariant &TpVariant::operator=(const std::set<float> &valueSet)
{
    VariantValueClear(data_);
    data_.m_vt = (uint16_t)VariantType::TpReal4 | (uint16_t)VariantType::TpSet;
    data_.data.m_pSetVal = new std::set<float>(valueSet);
    return *this;
}

TpVariant &TpVariant::operator=(const std::set<double> &valueSet)
{
    VariantValueClear(data_);
    data_.m_vt = (uint16_t)VariantType::TpReal8 | (uint16_t)VariantType::TpSet;
    data_.data.m_pSetVal = new std::set<double>(valueSet);
    return *this;
}

TpVariant &TpVariant::operator=(const std::set<std::string> &valueSet)
{
    VariantValueClear(data_);
    data_.m_vt = (uint16_t)VariantType::TpBstr | (uint16_t)VariantType::TpSet;
    data_.data.m_pSetVal = new std::set<std::string>(valueSet);
    return *this;
}

TpVariant &TpVariant::operator=(const TpVariant::VariantValue &value)
{
    if (&this->data_ != &value)
    {
        VariantValueClear(data_);
        VariantValueCopy(data_, value);
    }

    return *this;
}

TpVariant &TpVariant::operator=(const TpVariant &other)
{
    if (this != &other)
    {
        VariantValueClear(data_);
        VariantValueCopy(data_, other.data_);
    }

    return *this;
}

// bool TpVariant::operator==(const TpVariant::VariantValue &value)
// {
//     return Compare(value);
// }

bool TpVariant::operator==(const TpVariant &value)
{
    return Compare(value.data_);
}

bool TpVariant::operator!=(const VariantValue &value)
{
    return !Compare(value);
}

bool TpVariant::operator!=(const TpVariant &value)
{
    return !Compare(value.data_);
}

std::vector<bool> TpVariant::ToBoolArray() const
{
    std::vector<bool> arrayValue;
    std::set<bool> *pSetVal = static_cast<std::set<bool> *>(data_.data.m_pSetVal);

    if (pSetVal)
    {
        uint32_t i = 0;

        assert(data_.m_vt == (((uint16_t)VariantType::tpBool) | (uint16_t)VariantType::TpSet));
        arrayValue.resize(pSetVal->size());
        for (auto subValue : *pSetVal)
            arrayValue[i++] = subValue;
    }
    else
    {
        assert(data_.m_vt == (uint16_t)VariantType::TpEmpty);
    }

    return arrayValue;
}

std::vector<int8_t> TpVariant::ToInt8Array() const
{
    std::vector<int8_t> arrayValue;
    std::set<int8_t> *pSetVal = static_cast<std::set<int8_t> *>(data_.data.m_pSetVal);

    if (pSetVal)
    {
        uint32_t i = 0;

        assert(data_.m_vt == (((uint16_t)VariantType::TpInt1) | (uint16_t)VariantType::TpSet));
        arrayValue.resize(pSetVal->size());
        for (auto subValue : *pSetVal)
            arrayValue[i++] = subValue;
    }
    else
    {
        assert(data_.m_vt == (uint16_t)VariantType::TpEmpty);
    }

    return arrayValue;
}

std::vector<uint8_t> TpVariant::ToUint8Array() const
{
    std::vector<uint8_t> arrayValue;
    std::set<uint8_t> *pSetVal = static_cast<std::set<uint8_t> *>(data_.data.m_pSetVal);

    if (pSetVal)
    {
        uint32_t i = 0;

        assert(data_.m_vt == (((uint16_t)VariantType::TpUint1) | (uint16_t)VariantType::TpSet));
        arrayValue.resize(pSetVal->size());
        for (auto subValue : *pSetVal)
            arrayValue[i++] = subValue;
    }
    else
    {
        assert(data_.m_vt == (uint16_t)VariantType::TpEmpty);
    }

    return arrayValue;
}

std::vector<int16_t> TpVariant::ToInt16Array() const
{
    std::vector<int16_t> arrayValue;
    std::set<int16_t> *pSetVal = static_cast<std::set<int16_t> *>(data_.data.m_pSetVal);

    if (pSetVal)
    {
        uint32_t i = 0;

        assert(data_.m_vt == (((uint16_t)VariantType::TpInt2) | (uint16_t)VariantType::TpSet));
        arrayValue.resize(pSetVal->size());
        for (auto subValue : *pSetVal)
            arrayValue[i++] = subValue;
    }
    else
    {
        assert(data_.m_vt == (uint16_t)VariantType::TpEmpty);
    }

    return arrayValue;
}

std::vector<uint16_t> TpVariant::ToUint16Array() const
{
    std::vector<uint16_t> arrayValue;
    std::set<uint16_t> *pSetVal = static_cast<std::set<uint16_t> *>(data_.data.m_pSetVal);

    if (pSetVal)
    {
        uint32_t i = 0;

        assert(data_.m_vt == (((uint16_t)VariantType::TpUint2) | (uint16_t)VariantType::TpSet));
        arrayValue.resize(pSetVal->size());
        for (auto subValue : *pSetVal)
            arrayValue[i++] = subValue;
    }
    else
    {
        assert(data_.m_vt == (uint16_t)VariantType::TpEmpty);
    }

    return arrayValue;
}


std::vector<int32_t> TpVariant::ToInt32Array() const
{
    std::vector<int32_t> arrayValue;
    std::set<int32_t> *pSetVal = static_cast<std::set<int32_t> *>(data_.data.m_pSetVal);

    if (pSetVal)
    {
        uint32_t i = 0;

        assert(data_.m_vt == (((uint16_t)VariantType::TpInt4) | (uint16_t)VariantType::TpSet));
        arrayValue.resize(pSetVal->size());
        for (auto subValue : *pSetVal)
            arrayValue[i++] = subValue;
    }
    else
    {
        assert(data_.m_vt == (uint16_t)VariantType::TpEmpty);
    }

    return arrayValue;
}

std::vector<uint32_t> TpVariant::ToUint32Array() const
{
    std::vector<uint32_t> arrayValue;
    std::set<uint32_t> *pSetVal = static_cast<std::set<uint32_t> *>(data_.data.m_pSetVal);

    if (pSetVal)
    {
        uint32_t i = 0;

        assert(data_.m_vt == (((uint16_t)VariantType::TpUint4) | (uint16_t)VariantType::TpSet));
        arrayValue.resize(pSetVal->size());
        for (auto subValue : *pSetVal)
            arrayValue[i++] = subValue;
    }
    else
    {
        assert(data_.m_vt == (uint16_t)VariantType::TpEmpty);
    }

    return arrayValue;
}

std::vector<int64_t> TpVariant::ToInt64Array() const
{
    std::vector<int64_t> arrayValue;
    std::set<int64_t> *pSetVal = static_cast<std::set<int64_t> *>(data_.data.m_pSetVal);

    if (pSetVal)
    {
        uint32_t i = 0;

        assert(data_.m_vt == (((uint16_t)VariantType::tpInt8) | (uint16_t)VariantType::TpSet));
        arrayValue.resize(pSetVal->size());
        for (auto subValue : *pSetVal)
            arrayValue[i++] = subValue;
    }
    else
    {
        assert(data_.m_vt == (uint16_t)VariantType::TpEmpty);
    }

    return arrayValue;
}

std::vector<uint64_t> TpVariant::ToUint64Array() const
{
    std::vector<uint64_t> arrayValue;
    std::set<uint64_t> *pSetVal = static_cast<std::set<uint64_t> *>(data_.data.m_pSetVal);

    if (pSetVal)
    {
        uint32_t i = 0;

        assert(data_.m_vt == (((uint16_t)VariantType::TpUint8) | (uint16_t)VariantType::TpSet));
        arrayValue.resize(pSetVal->size());
        for (auto subValue : *pSetVal)
            arrayValue[i++] = subValue;
    }
    else
    {
        assert(data_.m_vt == (uint16_t)VariantType::TpEmpty);
    }

    return arrayValue;
}

std::vector<float> TpVariant::ToFloatArray() const
{
    std::vector<float> arrayValue;
    std::set<float> *pSetVal = static_cast<std::set<float> *>(data_.data.m_pSetVal);

    if (pSetVal)
    {
        uint32_t i = 0;

        assert(data_.m_vt == (((uint16_t)VariantType::TpReal4) | (uint16_t)VariantType::TpSet));
        arrayValue.resize(pSetVal->size());
        for (auto subValue : *pSetVal)
            arrayValue[i++] = subValue;
    }
    else
    {
        assert(data_.m_vt == (uint16_t)VariantType::TpEmpty);
    }

    return arrayValue;
}

std::vector<double> TpVariant::ToDoubleArray() const
{
    std::vector<double> arrayValue;
    std::set<double> *pSetVal = static_cast<std::set<double> *>(data_.data.m_pSetVal);

    if (pSetVal)
    {
        uint32_t i = 0;

        assert(data_.m_vt == (((uint16_t)VariantType::TpReal8) | (uint16_t)VariantType::TpSet));
        arrayValue.resize(pSetVal->size());
        for (auto subValue : *pSetVal)
            arrayValue[i++] = subValue;
    }
    else
    {
        assert(data_.m_vt == (uint16_t)VariantType::TpEmpty);
    }

    return arrayValue;
}

std::vector<std::string> TpVariant::ToStringArray() const
{
    std::vector<std::string> arrayValue;
    std::set<std::string> *pSetVal = static_cast<std::set<std::string> *>(data_.data.m_pSetVal);

    if (pSetVal)
    {
        uint32_t i = 0;

        assert(data_.m_vt == (((uint16_t)VariantType::TpBstr) | (uint16_t)VariantType::TpSet));
        arrayValue.resize(pSetVal->size());
        for (auto subValue : *pSetVal)
            arrayValue[i++] = subValue;
    }
    else
    {
        assert(data_.m_vt == (uint16_t)VariantType::TpEmpty);
    }

    return arrayValue;
}

std::set<bool> &TpVariant::ToBoolSet()
{
    if (nullptr == data_.data.m_pSetVal)
    {
        assert(data_.m_vt == (uint16_t)VariantType::TpEmpty);
        data_.m_vt = (((uint16_t)VariantType::tpBool) | (uint16_t)VariantType::TpSet);
        data_.data.m_pSetVal = new std::set<bool>();
    }
    else
    {
        assert(data_.m_vt == (((uint16_t)VariantType::tpBool) | (uint16_t)VariantType::TpSet));
    }

    return *static_cast<std::set<bool> *>(data_.data.m_pSetVal);
}

std::set<int8_t> &TpVariant::ToInt8Set()
{
    if (nullptr == data_.data.m_pSetVal)
    {
        assert(data_.m_vt == (uint16_t)VariantType::TpEmpty);
        data_.m_vt = (((uint16_t)VariantType::TpInt1) | (uint16_t)VariantType::TpSet);
        data_.data.m_pSetVal = new std::set<int8_t>();
    }
    else
    {
        assert(data_.m_vt == (((uint16_t)VariantType::TpInt1) | (uint16_t)VariantType::TpSet));
    }

    return *static_cast<std::set<int8_t> *>(data_.data.m_pSetVal);
}

std::set<uint8_t> &TpVariant::ToUint8Set()
{
    if (nullptr == data_.data.m_pSetVal)
    {
        assert(data_.m_vt == (uint16_t)VariantType::TpEmpty);
        data_.m_vt = (((uint16_t)VariantType::TpUint1) | (uint16_t)VariantType::TpSet);
        data_.data.m_pSetVal = new std::set<uint8_t>();
    }
    else
    {
        assert((data_.m_vt == (((uint16_t)VariantType::TpUint1) | (uint16_t)VariantType::TpSet)));
    }

    return *static_cast<std::set<uint8_t> *>(data_.data.m_pSetVal);
}

std::set<int16_t> &TpVariant::ToInt16Set()
{
    if (nullptr == data_.data.m_pSetVal)
    {
        assert(data_.m_vt == (uint16_t)VariantType::TpEmpty);
        data_.m_vt = (((uint16_t)VariantType::TpInt2) | (uint16_t)VariantType::TpSet);
        data_.data.m_pSetVal = new std::set<int16_t>();
    }
    else
    {
        assert(data_.m_vt == (((uint16_t)VariantType::TpInt2) | (uint16_t)VariantType::TpSet));
    }

    return *static_cast<std::set<int16_t> *>(data_.data.m_pSetVal);
}

std::set<uint16_t> &TpVariant::ToUint16Set()
{
    if (nullptr == data_.data.m_pSetVal)
    {
        assert(data_.m_vt == (uint16_t)VariantType::TpEmpty);
        data_.m_vt = (((uint16_t)VariantType::TpUint2) | (uint16_t)VariantType::TpSet);
        data_.data.m_pSetVal = new std::set<uint16_t>();
    }
    else
    {
        assert((data_.m_vt == (((uint16_t)VariantType::TpUint2) | (uint16_t)VariantType::TpSet)));
    }

    return *static_cast<std::set<uint16_t> *>(data_.data.m_pSetVal);
}

std::set<int32_t> &TpVariant::ToInt32Set()
{
    if (nullptr == data_.data.m_pSetVal)
    {
        assert(data_.m_vt == (uint16_t)VariantType::TpEmpty);
        data_.m_vt = (((uint16_t)VariantType::TpInt4) | (uint16_t)VariantType::TpSet);
        data_.data.m_pSetVal = new std::set<int32_t>();
    }
    else
    {
        assert(data_.m_vt == (((uint16_t)VariantType::TpInt4) | (uint16_t)VariantType::TpSet));
    }

    return *static_cast<std::set<int32_t> *>(data_.data.m_pSetVal);
}

std::set<uint32_t> &TpVariant::ToUint32Set()
{
    if (nullptr == data_.data.m_pSetVal)
    {
        assert(data_.m_vt == (uint16_t)VariantType::TpEmpty);
        data_.m_vt = (((uint16_t)VariantType::TpUint4) | (uint16_t)VariantType::TpSet);
        data_.data.m_pSetVal = new std::set<uint32_t>();
    }
    else
    {
        assert((data_.m_vt == (((uint16_t)VariantType::TpUint4) | (uint16_t)VariantType::TpSet)));
    }

    return *static_cast<std::set<uint32_t> *>(data_.data.m_pSetVal);
}

std::set<int64_t> &TpVariant::ToInt64Set()
{
    if (nullptr == data_.data.m_pSetVal)
    {
        assert(data_.m_vt == (uint16_t)VariantType::TpEmpty);
        data_.m_vt = (((uint16_t)VariantType::tpInt8) | (uint16_t)VariantType::TpSet);
        data_.data.m_pSetVal = new std::set<int64_t>();
    }
    else
    {
        assert(data_.m_vt == (((uint16_t)VariantType::tpInt8) | (uint16_t)VariantType::TpSet));
    }

    return *static_cast<std::set<int64_t> *>(data_.data.m_pSetVal);
}

std::set<uint64_t> &TpVariant::ToUint64Set()
{
    if (nullptr == data_.data.m_pSetVal)
    {
        assert(data_.m_vt == (uint16_t)VariantType::TpEmpty);
        data_.m_vt = (((uint16_t)VariantType::TpUint8) | (uint16_t)VariantType::TpSet);
        data_.data.m_pSetVal = new std::set<uint64_t>();
    }
    else
    {
        assert(data_.m_vt == (((uint16_t)VariantType::TpUint8) | (uint16_t)VariantType::TpSet));
    }
    return *static_cast<std::set<uint64_t> *>(data_.data.m_pSetVal);
}

std::set<float> &TpVariant::ToFloatSet()
{
    if (nullptr == data_.data.m_pSetVal)
    {
        assert(data_.m_vt == (uint16_t)VariantType::TpEmpty);
        data_.m_vt = (((uint16_t)VariantType::TpReal4) | (uint16_t)VariantType::TpSet);
        data_.data.m_pSetVal = new std::set<float>();
    }
    else
    {
        assert(data_.m_vt == (((uint16_t)VariantType::TpReal4) | (uint16_t)VariantType::TpSet));
    }

    return *static_cast<std::set<float> *>(data_.data.m_pSetVal);
}

std::set<double> &TpVariant::ToDoubleSet()
{
    if (nullptr == data_.data.m_pSetVal)
    {
        assert(data_.m_vt == (uint16_t)VariantType::TpEmpty);
        data_.m_vt = (((uint16_t)VariantType::TpReal8) | (uint16_t)VariantType::TpSet);
        data_.data.m_pSetVal = new std::set<double>();
    }
    else
    {
        assert(data_.m_vt == (((uint16_t)VariantType::TpReal8) | (uint16_t)VariantType::TpSet));
    }

    return *static_cast<std::set<double> *>(data_.data.m_pSetVal);
}

std::set<std::string> &TpVariant::ToStringSet()
{
    if (nullptr == data_.data.m_pSetVal)
    {
        assert(data_.m_vt == (uint16_t)VariantType::TpEmpty);
        data_.m_vt = (((uint16_t)VariantType::TpBstr) | (uint16_t)VariantType::TpSet);
        data_.data.m_pSetVal = new std::set<std::string>();
    }
    else
    {
        assert(data_.m_vt == (((uint16_t)VariantType::TpBstr) | (uint16_t)VariantType::TpSet));
    }
    return *static_cast<std::set<std::string> *>(data_.data.m_pSetVal);
}

bool TpVariant::Compare(const VariantValue &value)
{
    if (data_.m_vt != value.m_vt)
        return false;

    if (data_.m_vt & (uint16_t)VariantType::TpSet)
    {
        uint16_t uType = (uint16_t)value.m_vt & 0xff;
        bool bResult = false;

        switch ((VariantType)uType)
        {
        case VariantType::tpBool:
            bResult = VariantSetValueComplie(reinterpret_cast<std::set<bool> *>(data_.data.m_pSetVal), reinterpret_cast<std::set<bool> *>(value.data.m_pSetVal));
            break;
		case VariantType::TpInt1:
            bResult = VariantSetValueComplie(reinterpret_cast<std::set<int8_t> *>(data_.data.m_pSetVal), reinterpret_cast<std::set<int8_t> *>(value.data.m_pSetVal));
            break;
        case VariantType::TpUint1:
            bResult = VariantSetValueComplie(reinterpret_cast<std::set<uint8_t> *>(data_.data.m_pSetVal), reinterpret_cast<std::set<uint8_t> *>(value.data.m_pSetVal));
            break;
        case VariantType::TpInt2:
            bResult = VariantSetValueComplie(reinterpret_cast<std::set<int16_t> *>(data_.data.m_pSetVal), reinterpret_cast<std::set<int16_t> *>(value.data.m_pSetVal));
            break;
        case VariantType::TpUint2:
		    bResult = VariantSetValueComplie(reinterpret_cast<std::set<uint16_t> *>(data_.data.m_pSetVal), reinterpret_cast<std::set<uint16_t> *>(value.data.m_pSetVal));
            break;
        case VariantType::TpInt4:
            bResult = VariantSetValueComplie(reinterpret_cast<std::set<int32_t> *>(data_.data.m_pSetVal), reinterpret_cast<std::set<int32_t> *>(value.data.m_pSetVal));
            break;
        case VariantType::TpUint4:
            bResult = VariantSetValueComplie(reinterpret_cast<std::set<uint32_t> *>(data_.data.m_pSetVal), reinterpret_cast<std::set<uint32_t> *>(value.data.m_pSetVal));
            break;
        case VariantType::tpInt8:
            bResult = VariantSetValueComplie(reinterpret_cast<std::set<int64_t> *>(data_.data.m_pSetVal), reinterpret_cast<std::set<int64_t> *>(value.data.m_pSetVal));
            break;
        case VariantType::TpUint8:
            bResult = VariantSetValueComplie(reinterpret_cast<std::set<uint64_t> *>(data_.data.m_pSetVal), reinterpret_cast<std::set<uint64_t> *>(value.data.m_pSetVal));
            break;
        case VariantType::TpReal4:
            bResult = VariantSetValueComplie(reinterpret_cast<std::set<float> *>(data_.data.m_pSetVal), reinterpret_cast<std::set<float> *>(value.data.m_pSetVal));
            break;
        case VariantType::TpReal8:
            bResult = VariantSetValueComplie(reinterpret_cast<std::set<double> *>(data_.data.m_pSetVal), reinterpret_cast<std::set<double> *>(value.data.m_pSetVal));
            break;
        case VariantType::TpBstr:
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
        if (data_.m_vt != (uint16_t)VariantType::TpBstr)
        {
            return 0 == memcmp(this, &value, sizeof(value));
        }
		else if (data_.m_vt == static_cast<uint16_t>(VariantType::tpCustom) && value.m_vt == static_cast<uint16_t>(VariantType::tpCustom))
		{
			return false;
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


// 添加向量指针访问方法
const std::vector<TpVariant>* TpVariant::toVectorPtr() const {
    if (isVector()) {
        return data_.data.m_vectorVal;
    }
    return nullptr;
}


uint16_t TpVariant::getVariantType() const {
    return data_.m_vt;
}

void TpVariant::clear()
{
	VariantValueClear(data_);
}

const char* TpVariant::variantTypeName() const 
{
    switch (static_cast<VariantType>(data_.m_vt)) {
        case VariantType::tpBool: return "bool";
        case VariantType::TpInt4: return "int32";
        case VariantType::TpUint4: return "uint32";
        case VariantType::tpInt8: return "int64";
        case VariantType::TpUint8: return "uint64";
        case VariantType::TpReal4: return "float";
        case VariantType::TpReal8: return "double";
        case VariantType::TpBstr: return "string";
        case VariantType::TpVector: return "vector";
        default: return "unknown";
	}
}