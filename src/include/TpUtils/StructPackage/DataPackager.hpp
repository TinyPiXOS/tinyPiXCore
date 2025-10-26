#ifndef __DATA_PACKAGER_HPP
#define __DATA_PACKAGER_HPP

#include <cstdint>
#include <string.h>
#include <type_traits>
#include <string>
#include <vector>
#include "DataPackagerMacro.h"

// bool i8 ui8 i16 ui16 i32 ui32 i64 ui64 float double enum struct array std::sting std::vector

// pod? enum? struct? array? string? vector?
template <bool, bool, bool, bool, bool, bool, typename _Ty>
struct _PStructPackerCase
{
};

class PStructPackager
{
    PStructPackager(const PStructPackager &);
    PStructPackager(PStructPackager &&);
    void operator=(const PStructPackager &);
    bool operator==(const PStructPackager &);

public:
    StructXBuffer buffer;

    PStructPackager() {}
    template <typename _Ty>
    PStructPackager &operator<<(_Ty &_value)
    {
        _PStructPackerCase<std::is_pod<_Ty>::value,
                           std::is_enum<_Ty>::value,
                           std::is_class<_Ty>::value,
                           std::is_array<_Ty>::value,
                           std::is_same<_Ty, std::string>::value,
                           false, _Ty>::run(*this, _value);
        return *this;
    }

    const void *data() { return buffer.data(); }
    std::uint32_t size() { return (uint32_t)buffer.size(); }

    template <typename _Ty>
    PStructPackager &convert(const char *_name, _Ty &_value)
    {
        _PStructPackerCase<std::is_pod<_Ty>::value,
                           std::is_enum<_Ty>::value,
                           std::is_class<_Ty>::value,
                           std::is_array<_Ty>::value,
                           std::is_same<_Ty, std::string>::value,
                           false, _Ty>::run(*this, _value);
        return *this;
    }

    template <typename _Ty>
    PStructPackager &convert(const char *_name, std::vector<_Ty> &_value)
    {
        _PStructPackerCase<false, false, true, false, false, true, std::vector<_Ty>>::run(*this, _value);
        return *this;
    }

    PStructPackager &operator<<(std::string &_value)
    {
        uint32_t size = (uint32_t)_value.size();
        buffer.append(&size, sizeof(size));
        buffer.append(_value.data(), (std::uint32_t)size);
        return *this;
    }

    template <typename _Ty>
    PStructPackager &operator<<(std::vector<_Ty> &_value)
    {
        uint32_t size = (uint32_t)_value.size();
        buffer.append(&size, sizeof(size));
        for (uint32_t i = 0; i < size; i++)
        {
            *this << _value[i];
        }
        return *this;
    }
};

// base pod
template <typename _Ty>
struct _PStructPackerCase<true, false, false, false, false, false, _Ty>
{
    static void run(PStructPackager &_tool, _Ty &_value)
    {
        _tool.buffer.append(&_value, sizeof(_value));
    }
};
// enum
template <typename _Ty>
struct _PStructPackerCase<true, true, false, false, false, false, _Ty>
{
    static void run(PStructPackager &_tool, _Ty &_value)
    {
        _tool.buffer.append(&_value, sizeof(_value));
    }
};
// pod struct
template <typename _Ty>
struct _PStructPackerCase<true, false, true, false, false, false, _Ty>
{
    static void run(PStructPackager &_tool, _Ty &_value)
    {
        _tool.buffer.append(&_value, sizeof(_value));
    }
};
// not pod struct
template <typename _Ty>
struct _PStructPackerCase<false, false, true, false, false, false, _Ty>
{
    static void run(PStructPackager &_tool, _Ty &_value)
    {
        PStructTool<_Ty>::serlize(_tool, _value);
    }
};

// pod array
template <typename _Ty, uint32_t _Nx>
struct _PStructPackerCase<true, false, false, true, false, false, _Ty[_Nx]>
{
    static void run(PStructPackager &_tool, _Ty _value[])
    {
        _tool.buffer.append(_value, sizeof(_Ty) * _Nx);
    }
};
// not pod array
template <typename _Ty, uint32_t _Nx>
struct _PStructPackerCase<false, false, false, true, false, false, _Ty[_Nx]>
{
    static void run(PStructPackager &_tool, _Ty _value[])
    {
        for (uint32_t i = 0; i < _Nx; i++)
        {
            _tool << _value[i];
        }
    }
};
// string
template <typename _Ty>
struct _PStructPackerCase<false, false, true, false, true, false, _Ty>
{
    static void run(PStructPackager &_tool, std::string &_value)
    {
        _tool << _value;
    }
};

// vector
template <typename _Ty>
struct _PStructPackerCase<false, false, true, false, false, true, std::vector<_Ty>>
{
    static void run(PStructPackager &_tool, std::vector<_Ty> &_value)
    {
        _tool << _value;
    }
};

//     pod? enum? struct? array? string? vector?
template <bool, bool, bool, bool, bool, bool, typename _Ty>
struct _PStructUnpackerCase
{
};

class PStructUnpackager
{
    PStructUnpackager(const PStructUnpackager &);
    PStructUnpackager(PStructUnpackager &&);
    void operator=(const PStructUnpackager &);
    bool operator==(const PStructUnpackager &);

public:
    const char *m_data;
    std::uint32_t m_size;
    std::uint32_t m_pos;

    // 不会对数据进行拷贝， 使用中请勿删除原数据
    PStructUnpackager(const void *_data, std::uint32_t _size)
    {
        m_data = (const char *)_data;
        m_size = _size;
        m_pos = 0;
    }
    template <typename _Ty>
    PStructUnpackager &operator>>(_Ty &_value)
    {
        _PStructUnpackerCase<std::is_pod<_Ty>::value,
                             std::is_enum<_Ty>::value,
                             std::is_class<_Ty>::value,
                             std::is_array<_Ty>::value,
                             std::is_same<_Ty, std::string>::value,
                             false, _Ty>::run(*this, _value);
        return *this;
    }

    template <typename _Ty>
    PStructUnpackager &convert(const char *_name, _Ty &_value)
    {
        _PStructUnpackerCase<std::is_pod<_Ty>::value,
                             std::is_enum<_Ty>::value,
                             std::is_class<_Ty>::value,
                             std::is_array<_Ty>::value,
                             std::is_same<_Ty, std::string>::value,
                             false, _Ty>::run(*this, _value);
        return *this;
    }

    template <typename _Ty>
    PStructUnpackager &convert(const char *_name, std::vector<_Ty> &_value)
    {
        _PStructUnpackerCase<false, false, true, false, false, true, std::vector<_Ty>>::run(*this, _value);
        return *this;
    }

    PStructUnpackager &operator>>(std::string &_value)
    {
        uint32_t value_size = 0;

        memcpy(&value_size, m_data, sizeof(uint32_t));
        m_data += sizeof(uint32_t);
        m_pos += sizeof(uint32_t);
        _value.clear();
        _value.append(m_data, value_size);
        m_data += value_size;
        m_pos += value_size;
        return *this;
    }

    template <typename _Ty>
    PStructUnpackager &operator>>(std::vector<_Ty> &_value)
    {
        uint32_t value_size = 0;
        memcpy(&value_size, m_data, sizeof(uint32_t));
        m_data += sizeof(uint32_t);
        m_pos += sizeof(uint32_t);
        _value.clear();
        _value.resize(value_size);
        for (uint32_t i = 0; i < value_size; i++)
        {
            *this >> _value[i];
        }
        return *this;
    }
};

// base pod
template <typename _Ty>
struct _PStructUnpackerCase<true, false, false, false, false, false, _Ty>
{
    static void run(PStructUnpackager &_tool, _Ty &_value)
    {
        if (_tool.m_pos >= _tool.m_size)
            return;
        memcpy(&_value, _tool.m_data, sizeof(_value));
        _tool.m_data += sizeof(_value);
        _tool.m_pos += sizeof(_value);
    }
};
// enum
template <typename _Ty>
struct _PStructUnpackerCase<true, true, false, false, false, false, _Ty>
{
    static void run(PStructUnpackager &_tool, _Ty &_value)
    {
        if (_tool.m_pos >= _tool.m_size)
            return;
        memcpy(&_value, _tool.m_data, sizeof(_value));
        _tool.m_data += sizeof(_value);
        _tool.m_pos += sizeof(_value);
    }
};
// pod struct
template <typename _Ty>
struct _PStructUnpackerCase<true, false, true, false, false, false, _Ty>
{
    static void run(PStructUnpackager &_tool, _Ty &_value)
    {
        if (_tool.m_pos >= _tool.m_size)
            return;
        memcpy(&_value, _tool.m_data, sizeof(_value));
        _tool.m_data += sizeof(_value);
        _tool.m_pos += sizeof(_value);
    }
};
// not pod struct
template <typename _Ty>
struct _PStructUnpackerCase<false, false, true, false, false, false, _Ty>
{
    static void run(PStructUnpackager &_tool, _Ty &_value)
    {
        PStructTool<_Ty>::serlize(_tool, _value);
    }
};

// pod array
template <typename _Ty, uint32_t _Nx>
struct _PStructUnpackerCase<true, false, false, true, false, false, _Ty[_Nx]>
{
    static void run(PStructUnpackager &_tool, _Ty _value[])
    {
        if (_tool.m_pos >= _tool.m_size)
            return;
        memcpy(_value, _tool.m_data, sizeof(_Ty) * _Nx);
        _tool.m_data += sizeof(_Ty) * _Nx;
        _tool.m_pos += sizeof(_value);
    }
};
// not pod array
template <typename _Ty, uint32_t _Nx>
struct _PStructUnpackerCase<false, false, false, true, false, false, _Ty[_Nx]>
{
    static void run(PStructUnpackager &_tool, _Ty _value[])
    {
        if (_tool.m_pos >= _tool.m_size)
            return;
        for (uint32_t i = 0; i < _Nx; i++)
        {
            _tool >> _value[i];
        }
    }
};
// string
template <typename _Ty>
struct _PStructUnpackerCase<false, false, true, false, true, false, _Ty>
{
    static void run(PStructUnpackager &_tool, std::string &_value)
    {
        if (_tool.m_pos >= _tool.m_size)
            return;
        _tool >> _value;
    }
};

// vector
template <typename _Ty>
struct _PStructUnpackerCase<false, false, true, false, false, true, std::vector<_Ty>>
{
    static void run(PStructUnpackager &_tool, std::vector<_Ty> &_value)
    {
        if (_tool.m_pos >= _tool.m_size)
            return;
        _tool >> _value;
    }
};

#endif