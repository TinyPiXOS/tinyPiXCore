/***
 * @Author: hywang
 * @Date: 2024-05-22 11:00:41
 * @LastEditors: hywang
 * @LastEditTime: 2024-05-25 14:25:07
 * @FilePath: /pix-singlegui/PixSingleGUI/include_p/Utils/tpVariant_p.h
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

#include "tpVariant.h"

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
void VariantSetValueCopy(tpVariant::VariantValue &toValue, const std::set<T> *pFromValue)
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

void VariantValueInit(tpVariant::VariantValue &value)
{
    memset(&value, 0, sizeof(value));
    value.m_vt = (uint16_t)tpVariant::VariantType::tpEmpty;
    value.data.m_pSetVal = nullptr;
}

void VariantValueClear(tpVariant::VariantValue &value)
{
    if (value.m_vt & (uint16_t)tpVariant::VariantType::tpSet)
    {
        uint16_t uType = (uint16_t)value.m_vt & 0xff;

        switch ((tpVariant::VariantType)uType)
        {
        case tpVariant::VariantType::tpBool:
            VariantSetValueDelete(reinterpret_cast<std::set<bool> *>(value.data.m_pSetVal));
            break;
        case tpVariant::VariantType::tpInt4:
            VariantSetValueDelete(reinterpret_cast<std::set<int32_t> *>(value.data.m_pSetVal));
            break;
        case tpVariant::VariantType::tpUint4:
            VariantSetValueDelete(reinterpret_cast<std::set<uint32_t> *>(value.data.m_pSetVal));
            break;
        case tpVariant::VariantType::tpInt8:
            VariantSetValueDelete(reinterpret_cast<std::set<int64_t> *>(value.data.m_pSetVal));
            break;
        case tpVariant::VariantType::tpUint8:
            VariantSetValueDelete(reinterpret_cast<std::set<uint64_t> *>(value.data.m_pSetVal));
            break;
        case tpVariant::VariantType::tpReal4:
            VariantSetValueDelete(reinterpret_cast<std::set<float> *>(value.data.m_pSetVal));
            break;
        case tpVariant::VariantType::tpReal8:
            VariantSetValueDelete(reinterpret_cast<std::set<double> *>(value.data.m_pSetVal));
            break;
        case tpVariant::VariantType::tpBstr:
            VariantSetValueDelete(reinterpret_cast<std::set<std::string> *>(value.data.m_pSetVal));
            break;
        default:
            break;
        }
    }
    else
    {
        if (((uint16_t)tpVariant::VariantType::tpBstr == value.m_vt) && value.data.m_strVal)
            delete[] value.data.m_strVal;
    }

    memset(&value, 0, sizeof(value));
    value.m_vt = (uint16_t)tpVariant::VariantType::tpEmpty;
}

void VariantValueCopy(tpVariant::VariantValue &toValue, const tpVariant::VariantValue &from)
{
    VariantValueClear(toValue);

    if (from.m_vt & (uint16_t)tpVariant::VariantType::tpSet)
    {
        uint16_t uType = (uint16_t)from.m_vt & 0xff;

        switch ((tpVariant::VariantType)uType)
        {
        case tpVariant::VariantType::tpBool:
            toValue.m_vt = from.m_vt;
            VariantSetValueCopy(toValue, reinterpret_cast<std::set<bool> *>(from.data.m_pSetVal));
            break;
        case tpVariant::VariantType::tpInt4:
            toValue.m_vt = from.m_vt;
            VariantSetValueCopy(toValue, reinterpret_cast<std::set<int32_t> *>(from.data.m_pSetVal));
            break;
        case tpVariant::VariantType::tpUint4:
            toValue.m_vt = from.m_vt;
            VariantSetValueCopy(toValue, reinterpret_cast<std::set<uint32_t> *>(from.data.m_pSetVal));
            break;
        case tpVariant::VariantType::tpInt8:
            toValue.m_vt = from.m_vt;
            VariantSetValueCopy(toValue, reinterpret_cast<std::set<int64_t> *>(from.data.m_pSetVal));
            break;
        case tpVariant::VariantType::tpUint8:
            toValue.m_vt = from.m_vt;
            VariantSetValueCopy(toValue, reinterpret_cast<std::set<uint64_t> *>(from.data.m_pSetVal));
            break;
        case tpVariant::VariantType::tpReal4:
            toValue.m_vt = from.m_vt;
            VariantSetValueCopy(toValue, reinterpret_cast<std::set<float> *>(from.data.m_pSetVal));
            break;
        case tpVariant::VariantType::tpReal8:
            toValue.m_vt = from.m_vt;
            VariantSetValueCopy(toValue, reinterpret_cast<std::set<double> *>(from.data.m_pSetVal));
            break;
        case tpVariant::VariantType::tpBstr:
            toValue.m_vt = from.m_vt;
            VariantSetValueCopy(toValue, reinterpret_cast<std::set<std::string> *>(from.data.m_pSetVal));
            break;
        default:
            break;
        }
    }
    else
    {
        if ((uint16_t)tpVariant::VariantType::tpBstr == from.m_vt)
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
        else
        {
            memcpy(&toValue, &from, sizeof(from));
        }
    }
}

uint32_t VariantValueSetSize(const tpVariant::VariantValue &valueSet)
{
    uint32_t uCount = 0;

    if (valueSet.m_vt & (uint16_t)tpVariant::VariantType::tpSet)
    {
        uint16_t uType = (uint16_t)valueSet.m_vt & 0xff;

        switch ((tpVariant::VariantType)uType)
        {
        case tpVariant::VariantType::tpBool:
            uCount = VariantSetValueSize(reinterpret_cast<std::set<bool> *>(valueSet.data.m_pSetVal));
            break;
        case tpVariant::VariantType::tpInt4:
            uCount = VariantSetValueSize(reinterpret_cast<std::set<int32_t> *>(valueSet.data.m_pSetVal));
            break;
        case tpVariant::VariantType::tpUint4:
            uCount = VariantSetValueSize(reinterpret_cast<std::set<uint32_t> *>(valueSet.data.m_pSetVal));
            break;
        case tpVariant::VariantType::tpInt8:
            uCount = VariantSetValueSize(reinterpret_cast<std::set<int64_t> *>(valueSet.data.m_pSetVal));
            break;
        case tpVariant::VariantType::tpUint8:
            uCount = VariantSetValueSize(reinterpret_cast<std::set<uint64_t> *>(valueSet.data.m_pSetVal));
            break;
        case tpVariant::VariantType::tpReal4:
            uCount = VariantSetValueSize(reinterpret_cast<std::set<float> *>(valueSet.data.m_pSetVal));
            break;
        case tpVariant::VariantType::tpReal8:
            uCount = VariantSetValueSize(reinterpret_cast<std::set<double> *>(valueSet.data.m_pSetVal));
            break;
        case tpVariant::VariantType::tpBstr:
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

bool VariantValueSetAt(const tpVariant::VariantValue &valueSet, uint32_t uIndex, tpVariant::VariantValue &value)
{
    bool bResult = false;

    VariantValueClear(value);
    if (valueSet.m_vt & (uint16_t)tpVariant::VariantType::tpSet)
    {
        tpVariant::VariantValue value;

        value.m_vt = (uint16_t)valueSet.m_vt & 0xff;
        switch ((tpVariant::VariantType)value.m_vt)
        {
        case tpVariant::VariantType::tpBool:
            bResult = VariantSetValueAt(reinterpret_cast<std::set<bool> *>(valueSet.data.m_pSetVal), uIndex, value.data.m_bVal);
            break;
        case tpVariant::VariantType::tpInt4:
            bResult = VariantSetValueAt(reinterpret_cast<std::set<int32_t> *>(valueSet.data.m_pSetVal), uIndex, value.data.m_i4Val);
            break;
        case tpVariant::VariantType::tpUint4:
            bResult = VariantSetValueAt(reinterpret_cast<std::set<uint32_t> *>(valueSet.data.m_pSetVal), uIndex, value.data.m_ui4Val);
            break;
        case tpVariant::VariantType::tpInt8:
            bResult = VariantSetValueAt(reinterpret_cast<std::set<int64_t> *>(valueSet.data.m_pSetVal), uIndex, value.data.m_i8Val);
            break;
        case tpVariant::VariantType::tpUint8:
            bResult = VariantSetValueAt(reinterpret_cast<std::set<uint64_t> *>(valueSet.data.m_pSetVal), uIndex, value.data.m_ui8Val);
            break;
        case tpVariant::VariantType::tpReal4:
            bResult = VariantSetValueAt(reinterpret_cast<std::set<float> *>(valueSet.data.m_pSetVal), uIndex, value.data.m_r4Val);
            break;
        case tpVariant::VariantType::tpReal8:
            bResult = VariantSetValueAt(reinterpret_cast<std::set<double> *>(valueSet.data.m_pSetVal), uIndex, value.data.m_r8Val);
            break;
        case tpVariant::VariantType::tpBstr:
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

bool VariantValueSetAdd(tpVariant::VariantValue &valueSet, const tpVariant::VariantValue &value)
{
    bool bResult = false;

    if (valueSet.m_vt == (uint16_t)tpVariant::VariantType::tpEmpty)
    {
        if (value.m_vt == (uint16_t)tpVariant::VariantType::tpEmpty)
            return false;

        valueSet.m_vt = (uint16_t)tpVariant::VariantType::tpSet | (uint16_t)value.m_vt;
    }

    if (valueSet.m_vt & (uint16_t)tpVariant::VariantType::tpSet)
    {
        uint16_t uType = (uint16_t)valueSet.m_vt & 0xff;

        if (uType != (uint16_t)value.m_vt)
            return false;

        switch ((tpVariant::VariantType)uType)
        {
        case tpVariant::VariantType::tpBool:
            if (nullptr == valueSet.data.m_pSetVal)
                valueSet.data.m_pSetVal = new std::set<bool>();
            bResult = VariantSetValueAdd(reinterpret_cast<std::set<bool> *>(valueSet.data.m_pSetVal), value.data.m_bVal);
            break;
        case tpVariant::VariantType::tpInt4:
            if (nullptr == valueSet.data.m_pSetVal)
                valueSet.data.m_pSetVal = new std::set<int32_t>();
            bResult = VariantSetValueAdd(reinterpret_cast<std::set<int32_t> *>(valueSet.data.m_pSetVal), value.data.m_i4Val);
            break;
        case tpVariant::VariantType::tpUint4:
            if (nullptr == valueSet.data.m_pSetVal)
                valueSet.data.m_pSetVal = new std::set<uint32_t>();
            bResult = VariantSetValueAdd(reinterpret_cast<std::set<uint32_t> *>(valueSet.data.m_pSetVal), value.data.m_ui4Val);
            break;
        case tpVariant::VariantType::tpInt8:
            if (nullptr == valueSet.data.m_pSetVal)
                valueSet.data.m_pSetVal = new std::set<int64_t>();
            bResult = VariantSetValueAdd(reinterpret_cast<std::set<int64_t> *>(valueSet.data.m_pSetVal), value.data.m_i8Val);
            break;
        case tpVariant::VariantType::tpUint8:
            if (nullptr == valueSet.data.m_pSetVal)
                valueSet.data.m_pSetVal = new std::set<uint64_t>();
            bResult = VariantSetValueAdd(reinterpret_cast<std::set<uint64_t> *>(valueSet.data.m_pSetVal), value.data.m_ui8Val);
            break;
        case tpVariant::VariantType::tpReal4:
            if (nullptr == valueSet.data.m_pSetVal)
                valueSet.data.m_pSetVal = new std::set<float>();
            bResult = VariantSetValueAdd(reinterpret_cast<std::set<float> *>(valueSet.data.m_pSetVal), value.data.m_r4Val);
            break;
        case tpVariant::VariantType::tpReal8:
            if (nullptr == valueSet.data.m_pSetVal)
                valueSet.data.m_pSetVal = new std::set<double>();
            bResult = VariantSetValueAdd(reinterpret_cast<std::set<double> *>(valueSet.data.m_pSetVal), value.data.m_r8Val);
            break;
        case tpVariant::VariantType::tpBstr:
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