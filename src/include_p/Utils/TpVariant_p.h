/***
 * @Author: hywang
 * @Date: 2024-05-22 11:00:41
 * @LastEditors: hywang
 * @LastEditTime: 2024-05-25 14:25:07
 * @FilePath: /pix-singlegui/PixSingleGUI/include_p/Utils/TpVariant_p.h
 * @Description:
 * @
 * @PiXOS
 */

#ifndef __TP_VARIANT_P_H
#define __TP_VARIANT_P_H

#include <type_traits>
#include <iostream>
#include <string>
#include <set>

#include "TpVariant.h"

template <typename T>
void VariantSetValueDelete(std::set<T> *pSetValue)
{
    if (pSetValue)
    {
        delete pSetValue;
        pSetValue = nullptr;
    }
}

template <typename T>
void VariantSetValueCopy(TpVariant::VariantValue &toValue, const std::set<T> *pFromValue)
{
    if (nullptr == pFromValue)
        return;

    toValue.data.m_pSetVal = new std::set<T>(*pFromValue);
}

template <typename T>
uint32_t VariantSetValueSize(std::set<T> *pSetValue)
{
    return pSetValue ? static_cast<uint32_t>(pSetValue->size()) : 0;
}

template <typename T>
bool VariantSetValueAt(std::set<T> *pSetValue, uint32_t uIndex, T &subValue)
{
    if (nullptr == pSetValue)
        return false;

    auto it = pSetValue->begin();

    advance(it, uIndex);
    if (it == pSetValue->end())
        return false;

    subValue = *it;
    return true;
}

template <typename T>
bool VariantSetValueAdd(std::set<T> *pSetValue, T value)
{
    if (nullptr == pSetValue)
        return false;

    auto it = pSetValue->find(value);

    if (it == pSetValue->end())
        return false;

    pSetValue->insert(value);
    return true;
}

template <typename T>
bool VariantSetValueRemove(std::set<T> *pSetValue, T value)
{
    if (nullptr == pSetValue)
        return false;

    auto it = pSetValue->find(value);

    if (it == pSetValue->end())
        return false;

    pSetValue->erase(it);
    return true;
}

void VariantValueInit(TpVariant::VariantValue &value)
{
    memset(&value, 0, sizeof(value));
    value.m_vt = (uint16_t)TpVariant::VariantType::TpEmpty;
	value.data.m_strVal = nullptr;
    value.data.m_pSetVal = nullptr;
	value.data.m_vectorVal = nullptr; // 确保初始化为 nullptr
}

void VariantValueClear(TpVariant::VariantValue &value)
{
    if (value.m_vt & (uint16_t)TpVariant::VariantType::TpSet)
    {
        uint16_t uType = (uint16_t)value.m_vt & 0xff;

        switch ((TpVariant::VariantType)uType)
        {
        case TpVariant::VariantType::tpBool:
            VariantSetValueDelete(reinterpret_cast<std::set<bool> *>(value.data.m_pSetVal));
            break;
		case TpVariant::VariantType::TpInt1:
            VariantSetValueDelete(reinterpret_cast<std::set<int8_t> *>(value.data.m_pSetVal));
            break;
        case TpVariant::VariantType::TpUint1:
            VariantSetValueDelete(reinterpret_cast<std::set<uint8_t> *>(value.data.m_pSetVal));
            break;
        case TpVariant::VariantType::TpInt2:
            VariantSetValueDelete(reinterpret_cast<std::set<int16_t> *>(value.data.m_pSetVal));
            break;
        case TpVariant::VariantType::TpUint2:
            VariantSetValueDelete(reinterpret_cast<std::set<uint16_t> *>(value.data.m_pSetVal));
            break;
        case TpVariant::VariantType::TpInt4:
            VariantSetValueDelete(reinterpret_cast<std::set<int32_t> *>(value.data.m_pSetVal));
            break;
        case TpVariant::VariantType::TpUint4:
            VariantSetValueDelete(reinterpret_cast<std::set<uint32_t> *>(value.data.m_pSetVal));
            break;
        case TpVariant::VariantType::tpInt8:
            VariantSetValueDelete(reinterpret_cast<std::set<int64_t> *>(value.data.m_pSetVal));
            break;
        case TpVariant::VariantType::TpUint8:
            VariantSetValueDelete(reinterpret_cast<std::set<uint64_t> *>(value.data.m_pSetVal));
            break;
        case TpVariant::VariantType::TpReal4:
            VariantSetValueDelete(reinterpret_cast<std::set<float> *>(value.data.m_pSetVal));
            break;
        case TpVariant::VariantType::TpReal8:
            VariantSetValueDelete(reinterpret_cast<std::set<double> *>(value.data.m_pSetVal));
            break;
        case TpVariant::VariantType::TpBstr:
            VariantSetValueDelete(reinterpret_cast<std::set<std::string> *>(value.data.m_pSetVal));
            break;
        default:
            break;
        }
    }
    else
    {
        if (((uint16_t)TpVariant::VariantType::TpBstr == value.m_vt) && value.data.m_strVal)
            delete[] value.data.m_strVal;
		// 向量处理
		else if (value.m_vt == static_cast<uint16_t>(TpVariant::VariantType::TpVector) && value.data.m_vectorVal) 
		{
        	if (value.data.m_vectorVal) {
                delete value.data.m_vectorVal;
                value.data.m_vectorVal = nullptr;
            }
    	}
    }

    memset(&value, 0, sizeof(value));
    value.m_vt = (uint16_t)TpVariant::VariantType::TpEmpty;
}

void VariantValueCopy(TpVariant::VariantValue &toValue, const TpVariant::VariantValue &from)
{
    VariantValueClear(toValue);

    if (from.m_vt & (uint16_t)TpVariant::VariantType::TpSet)
    {
        uint16_t uType = (uint16_t)from.m_vt & 0xff;

        switch ((TpVariant::VariantType)uType)
        {
        case TpVariant::VariantType::tpBool:
            toValue.m_vt = from.m_vt;
            VariantSetValueCopy(toValue, reinterpret_cast<std::set<bool> *>(from.data.m_pSetVal));
            break;
		case TpVariant::VariantType::TpInt1:
            toValue.m_vt = from.m_vt;
            VariantSetValueCopy(toValue, reinterpret_cast<std::set<int8_t> *>(from.data.m_pSetVal));
            break;
        case TpVariant::VariantType::TpUint1:
            toValue.m_vt = from.m_vt;
            VariantSetValueCopy(toValue, reinterpret_cast<std::set<uint8_t> *>(from.data.m_pSetVal));
            break;
        case TpVariant::VariantType::TpInt2:
            toValue.m_vt = from.m_vt;
            VariantSetValueCopy(toValue, reinterpret_cast<std::set<int32_t> *>(from.data.m_pSetVal));
            break;
        case TpVariant::VariantType::TpUint2:
            toValue.m_vt = from.m_vt;
            VariantSetValueCopy(toValue, reinterpret_cast<std::set<uint32_t> *>(from.data.m_pSetVal));
            break;
        case TpVariant::VariantType::TpInt4:
            toValue.m_vt = from.m_vt;
            VariantSetValueCopy(toValue, reinterpret_cast<std::set<int32_t> *>(from.data.m_pSetVal));
            break;
        case TpVariant::VariantType::TpUint4:
            toValue.m_vt = from.m_vt;
            VariantSetValueCopy(toValue, reinterpret_cast<std::set<uint32_t> *>(from.data.m_pSetVal));
            break;
        case TpVariant::VariantType::tpInt8:
            toValue.m_vt = from.m_vt;
            VariantSetValueCopy(toValue, reinterpret_cast<std::set<int64_t> *>(from.data.m_pSetVal));
            break;
        case TpVariant::VariantType::TpUint8:
            toValue.m_vt = from.m_vt;
            VariantSetValueCopy(toValue, reinterpret_cast<std::set<uint64_t> *>(from.data.m_pSetVal));
            break;
        case TpVariant::VariantType::TpReal4:
            toValue.m_vt = from.m_vt;
            VariantSetValueCopy(toValue, reinterpret_cast<std::set<float> *>(from.data.m_pSetVal));
            break;
        case TpVariant::VariantType::TpReal8:
            toValue.m_vt = from.m_vt;
            VariantSetValueCopy(toValue, reinterpret_cast<std::set<double> *>(from.data.m_pSetVal));
            break;
        case TpVariant::VariantType::TpBstr:
            toValue.m_vt = from.m_vt;
            VariantSetValueCopy(toValue, reinterpret_cast<std::set<std::string> *>(from.data.m_pSetVal));
            break;
        default:
            break;
        }
    }
    else
    {
        if ((uint16_t)TpVariant::VariantType::TpBstr == from.m_vt)
        {
            toValue.m_vt = from.m_vt;
            if (nullptr != from.data.m_strVal)
            {
                uint32_t uLen = strlen(from.data.m_strVal);

                toValue.data.m_strVal = new char[uLen + 1];
                memcpy(toValue.data.m_strVal, from.data.m_strVal, uLen);
                toValue.data.m_strVal[uLen] = '\0';
            }
            else
            {
                toValue.data.m_strVal = nullptr;
            }
        }
		 // 向量处理
		else if (from.m_vt == static_cast<uint16_t>(TpVariant::VariantType::TpVector)) {
            toValue.m_vt = from.m_vt; // 设置类型
            if (from.data.m_vectorVal) {
                toValue.data.m_vectorVal = new std::vector<TpVariant>(*from.data.m_vectorVal);
            } else {
                toValue.data.m_vectorVal = nullptr;
            }
        }
        else
        {
            memcpy(&toValue, &from, sizeof(from));
        }
    }
}

uint32_t VariantValueSetSize(const TpVariant::VariantValue &valueSet)
{
    uint32_t uCount = 0;

    if (valueSet.m_vt & (uint16_t)TpVariant::VariantType::TpSet)
    {
        uint16_t uType = (uint16_t)valueSet.m_vt & 0xff;

        switch ((TpVariant::VariantType)uType)
        {
        case TpVariant::VariantType::tpBool:
            uCount = VariantSetValueSize(reinterpret_cast<std::set<bool> *>(valueSet.data.m_pSetVal));
            break;
		case TpVariant::VariantType::TpInt1:
            uCount = VariantSetValueSize(reinterpret_cast<std::set<int8_t> *>(valueSet.data.m_pSetVal));
            break;
        case TpVariant::VariantType::TpUint1:
            uCount = VariantSetValueSize(reinterpret_cast<std::set<uint8_t> *>(valueSet.data.m_pSetVal));
            break;
        case TpVariant::VariantType::TpInt2:
            uCount = VariantSetValueSize(reinterpret_cast<std::set<int16_t> *>(valueSet.data.m_pSetVal));
            break;
        case TpVariant::VariantType::TpUint2:
            uCount = VariantSetValueSize(reinterpret_cast<std::set<uint16_t> *>(valueSet.data.m_pSetVal));
            break;
        case TpVariant::VariantType::TpInt4:
            uCount = VariantSetValueSize(reinterpret_cast<std::set<int32_t> *>(valueSet.data.m_pSetVal));
            break;
        case TpVariant::VariantType::TpUint4:
            uCount = VariantSetValueSize(reinterpret_cast<std::set<uint32_t> *>(valueSet.data.m_pSetVal));
            break;
        case TpVariant::VariantType::tpInt8:
            uCount = VariantSetValueSize(reinterpret_cast<std::set<int64_t> *>(valueSet.data.m_pSetVal));
            break;
        case TpVariant::VariantType::TpUint8:
            uCount = VariantSetValueSize(reinterpret_cast<std::set<uint64_t> *>(valueSet.data.m_pSetVal));
            break;
        case TpVariant::VariantType::TpReal4:
            uCount = VariantSetValueSize(reinterpret_cast<std::set<float> *>(valueSet.data.m_pSetVal));
            break;
        case TpVariant::VariantType::TpReal8:
            uCount = VariantSetValueSize(reinterpret_cast<std::set<double> *>(valueSet.data.m_pSetVal));
            break;
        case TpVariant::VariantType::TpBstr:
            uCount = VariantSetValueSize(reinterpret_cast<std::set<std::string> *>(valueSet.data.m_pSetVal));
            break;
        default:
            break;
        }
    }
    else
    {
        uCount = 1;
    }

    return uCount;
}

bool VariantValueSetAt(const TpVariant::VariantValue &valueSet, uint32_t uIndex, TpVariant::VariantValue &value)
{
    bool bResult = false;

    VariantValueClear(value);
    if (valueSet.m_vt & (uint16_t)TpVariant::VariantType::TpSet)
    {
        TpVariant::VariantValue value;

        value.m_vt = (uint16_t)valueSet.m_vt & 0xff;
        switch ((TpVariant::VariantType)value.m_vt)
        {
        case TpVariant::VariantType::tpBool:
            bResult = VariantSetValueAt(reinterpret_cast<std::set<bool> *>(valueSet.data.m_pSetVal), uIndex, value.data.m_bVal);
            break;
		case TpVariant::VariantType::TpInt1:
            bResult = VariantSetValueAt(reinterpret_cast<std::set<int8_t> *>(valueSet.data.m_pSetVal), uIndex, value.data.m_i1Val);
            break;
        case TpVariant::VariantType::TpUint1:
            bResult = VariantSetValueAt(reinterpret_cast<std::set<uint8_t> *>(valueSet.data.m_pSetVal), uIndex, value.data.m_ui1Val);
            break;
        case TpVariant::VariantType::TpInt2:
            bResult = VariantSetValueAt(reinterpret_cast<std::set<int16_t> *>(valueSet.data.m_pSetVal), uIndex, value.data.m_i2Val);
            break;
        case TpVariant::VariantType::TpUint2:
            bResult = VariantSetValueAt(reinterpret_cast<std::set<uint16_t> *>(valueSet.data.m_pSetVal), uIndex, value.data.m_ui2Val);
            break;
        case TpVariant::VariantType::TpInt4:
            bResult = VariantSetValueAt(reinterpret_cast<std::set<int32_t> *>(valueSet.data.m_pSetVal), uIndex, value.data.m_i4Val);
            break;
        case TpVariant::VariantType::TpUint4:
            bResult = VariantSetValueAt(reinterpret_cast<std::set<uint32_t> *>(valueSet.data.m_pSetVal), uIndex, value.data.m_ui4Val);
            break;
        case TpVariant::VariantType::tpInt8:
            bResult = VariantSetValueAt(reinterpret_cast<std::set<int64_t> *>(valueSet.data.m_pSetVal), uIndex, value.data.m_i8Val);
            break;
        case TpVariant::VariantType::TpUint8:
            bResult = VariantSetValueAt(reinterpret_cast<std::set<uint64_t> *>(valueSet.data.m_pSetVal), uIndex, value.data.m_ui8Val);
            break;
        case TpVariant::VariantType::TpReal4:
            bResult = VariantSetValueAt(reinterpret_cast<std::set<float> *>(valueSet.data.m_pSetVal), uIndex, value.data.m_r4Val);
            break;
        case TpVariant::VariantType::TpReal8:
            bResult = VariantSetValueAt(reinterpret_cast<std::set<double> *>(valueSet.data.m_pSetVal), uIndex, value.data.m_r8Val);
            break;
        case TpVariant::VariantType::TpBstr:
        {
            std::string strValue;

            bResult = VariantSetValueAt(reinterpret_cast<std::set<std::string> *>(valueSet.data.m_pSetVal), uIndex, strValue);
            if (bResult)
            {
                uint32_t uLen = (uint32_t)strValue.length();

                value.data.m_strVal = new char[uLen + 1];
                memcpy(value.data.m_strVal, strValue.c_str(), uLen);
                value.data.m_strVal[uLen] = '\0';
            }
        }
        break;
        default:
            bResult = false;
            break;
        }
    }
    else
    {
        VariantValueCopy(value, valueSet);
    }

    return bResult;
}

bool VariantValueSetAdd(TpVariant::VariantValue &valueSet, const TpVariant::VariantValue &value)
{
    bool bResult = false;

    if (valueSet.m_vt == (uint16_t)TpVariant::VariantType::TpEmpty)
    {
        if (value.m_vt == (uint16_t)TpVariant::VariantType::TpEmpty)
            return false;

        valueSet.m_vt = (uint16_t)TpVariant::VariantType::TpSet | (uint16_t)value.m_vt;
    }

    if (valueSet.m_vt & (uint16_t)TpVariant::VariantType::TpSet)
    {
        uint16_t uType = (uint16_t)valueSet.m_vt & 0xff;

        if (uType != (uint16_t)value.m_vt)
            return false;

        switch ((TpVariant::VariantType)uType)
        {
        case TpVariant::VariantType::tpBool:
            if (nullptr == valueSet.data.m_pSetVal)
                valueSet.data.m_pSetVal = new std::set<bool>();
            bResult = VariantSetValueAdd(reinterpret_cast<std::set<bool> *>(valueSet.data.m_pSetVal), value.data.m_bVal);
            break;
		case TpVariant::VariantType::TpInt1:
            if (nullptr == valueSet.data.m_pSetVal)
                valueSet.data.m_pSetVal = new std::set<int8_t>();
            bResult = VariantSetValueAdd(reinterpret_cast<std::set<int8_t> *>(valueSet.data.m_pSetVal), value.data.m_i1Val);
            break;
        case TpVariant::VariantType::TpUint1:
            if (nullptr == valueSet.data.m_pSetVal)
                valueSet.data.m_pSetVal = new std::set<uint8_t>();
            bResult = VariantSetValueAdd(reinterpret_cast<std::set<uint8_t> *>(valueSet.data.m_pSetVal), value.data.m_ui1Val);
            break;
        case TpVariant::VariantType::TpInt2:
            if (nullptr == valueSet.data.m_pSetVal)
                valueSet.data.m_pSetVal = new std::set<int16_t>();
            bResult = VariantSetValueAdd(reinterpret_cast<std::set<int16_t> *>(valueSet.data.m_pSetVal), value.data.m_i2Val);
            break;
        case TpVariant::VariantType::TpUint2:
            if (nullptr == valueSet.data.m_pSetVal)
                valueSet.data.m_pSetVal = new std::set<uint16_t>();
            bResult = VariantSetValueAdd(reinterpret_cast<std::set<uint16_t> *>(valueSet.data.m_pSetVal), value.data.m_ui2Val);
            break;
        case TpVariant::VariantType::TpInt4:
            if (nullptr == valueSet.data.m_pSetVal)
                valueSet.data.m_pSetVal = new std::set<int32_t>();
            bResult = VariantSetValueAdd(reinterpret_cast<std::set<int32_t> *>(valueSet.data.m_pSetVal), value.data.m_i4Val);
            break;
        case TpVariant::VariantType::TpUint4:
            if (nullptr == valueSet.data.m_pSetVal)
                valueSet.data.m_pSetVal = new std::set<uint32_t>();
            bResult = VariantSetValueAdd(reinterpret_cast<std::set<uint32_t> *>(valueSet.data.m_pSetVal), value.data.m_ui4Val);
            break;
        case TpVariant::VariantType::tpInt8:
            if (nullptr == valueSet.data.m_pSetVal)
                valueSet.data.m_pSetVal = new std::set<int64_t>();
            bResult = VariantSetValueAdd(reinterpret_cast<std::set<int64_t> *>(valueSet.data.m_pSetVal), value.data.m_i8Val);
            break;
        case TpVariant::VariantType::TpUint8:
            if (nullptr == valueSet.data.m_pSetVal)
                valueSet.data.m_pSetVal = new std::set<uint64_t>();
            bResult = VariantSetValueAdd(reinterpret_cast<std::set<uint64_t> *>(valueSet.data.m_pSetVal), value.data.m_ui8Val);
            break;
        case TpVariant::VariantType::TpReal4:
            if (nullptr == valueSet.data.m_pSetVal)
                valueSet.data.m_pSetVal = new std::set<float>();
            bResult = VariantSetValueAdd(reinterpret_cast<std::set<float> *>(valueSet.data.m_pSetVal), value.data.m_r4Val);
            break;
        case TpVariant::VariantType::TpReal8:
            if (nullptr == valueSet.data.m_pSetVal)
                valueSet.data.m_pSetVal = new std::set<double>();
            bResult = VariantSetValueAdd(reinterpret_cast<std::set<double> *>(valueSet.data.m_pSetVal), value.data.m_r8Val);
            break;
        case TpVariant::VariantType::TpBstr:
            if (value.data.m_strVal)
            {
                if (nullptr == valueSet.data.m_pSetVal)
                    valueSet.data.m_pSetVal = new std::set<std::string>();
                bResult = VariantSetValueAdd(reinterpret_cast<std::set<std::string> *>(valueSet.data.m_pSetVal), std::string(value.data.m_strVal));
            }
            else
            {
                bResult = false;
            }
            break;
        default:
            bResult = false;
            break;
        }
    }
    else
    {
        bResult = false;
    }

    return bResult;
}

#endif // __TP_VARIANT_P_H