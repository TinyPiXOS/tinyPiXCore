
#include <string>
#include <iomanip>
#include <sstream>
#include <stdexcept>
#include <cstdint>
#include "TpBluetoothUuid.h"
#include "include/blt_sdp.h"

enum TpBluetoothUuidType
{
    TP_UUID_TYPE_NONE,
    TP_UUID_TYPE_PROTOCOL,
    TP_UUID_TYPE_PROFILE,
    TP_UUID_TYPE_OTHER,
};

struct TpBluetoothUuidData
{
    TpBluetoothUuid::Format format;
    TpBluetoothUuidType type;
    tpUInt8 uuid128[16]; // 全部扩展成128位uuid
    TpBluetoothUuidData()
    {
        format = TpBluetoothUuid::FormatUnknown;
        type = TP_UUID_TYPE_NONE;
        memset(uuid128, 0, sizeof(uuid128));
    }
    TpBluetoothUuidData(const TpBluetoothUuidData &other) : format(other.format), type(other.type)
    {
        memcpy(uuid128, other.uuid128, sizeof(uuid128));
        format = other.format;
        type = other.type;
    }
};

// 辅助函数：深拷贝创建私有数据对象
static TpBluetoothUuidData *clonePrivate(const TpBluetoothUuidData *other)
{
    if (!other)
        return new TpBluetoothUuidData();
    return new TpBluetoothUuidData(*other);
}

TpBluetoothUuid::TpBluetoothUuid()
    : data_(new TpBluetoothUuidData())
{
}

TpBluetoothUuid::TpBluetoothUuid(tpUInt16 uuid)
    : data_(new TpBluetoothUuidData())
{
    TpBluetoothUuidData *data = static_cast<TpBluetoothUuidData *>(data_);
    bluet_uuid16_to_uuid128(uuid, data->uuid128);
    data->format = Format16Bit;
}

TpBluetoothUuid::TpBluetoothUuid(tpUInt32 uuid)
    : data_(new TpBluetoothUuidData())
{
    TpBluetoothUuidData *data = static_cast<TpBluetoothUuidData *>(data_);
    bluet_uuid32_to_uuid128(uuid, data->uuid128);
    data->format = Format32Bit;
}

TpBluetoothUuid::TpBluetoothUuid(const tpUInt8 uuid[16])
    : data_(new TpBluetoothUuidData())
{
    TpBluetoothUuidData *data = static_cast<TpBluetoothUuidData *>(data_);
    memcpy(data->uuid128, uuid, 16);
    data->format = Format128Bit;
}

TpBluetoothUuid::TpBluetoothUuid(const TpString &uuid)
    : data_(new TpBluetoothUuidData())
{
    TpBluetoothUuidData *data = static_cast<TpBluetoothUuidData *>(data_);
    bluet_uuidstr_to_uuid128(uuid.c_str(), data->uuid128);
    data->format = Format128Bit;
}

TpBluetoothUuid::TpBluetoothUuid(const TpBluetoothUuid &other)
    : data_(new TpBluetoothUuidData(*static_cast<TpBluetoothUuidData *>(other.data_)))
{
}

TpBluetoothUuid::TpBluetoothUuid(const TpBluetoothUuid::ProtocolUuid uuid_p)
{
    tpUInt16 uuid = static_cast<tpUInt16>(uuid_p);
    data_ = new TpBluetoothUuidData();
    TpBluetoothUuidData *data = static_cast<TpBluetoothUuidData *>(data_);
    bluet_uuid16_to_uuid128(uuid, data->uuid128);
    data->format = TpBluetoothUuid::Format16Bit;
    data->type = TP_UUID_TYPE_PROTOCOL;
}

TpBluetoothUuid::TpBluetoothUuid(const TpBluetoothUuid::ProfileUuid uuid_p)
{
    tpUInt16 uuid = static_cast<tpUInt16>(uuid_p);
    data_ = new TpBluetoothUuidData();
    TpBluetoothUuidData *data = static_cast<TpBluetoothUuidData *>(data_);
    bluet_uuid16_to_uuid128(uuid, data->uuid128);
    data->format = TpBluetoothUuid::Format16Bit;
    data->type = TP_UUID_TYPE_PROFILE;
}

TpBluetoothUuid::~TpBluetoothUuid()
{
    TpBluetoothUuidData *data = static_cast<TpBluetoothUuidData *>(data_);
    data->format = TpBluetoothUuid::FormatUnknown;
    data->type = TP_UUID_TYPE_NONE;
}

// 赋值运算符
TpBluetoothUuid &TpBluetoothUuid::operator=(const TpBluetoothUuid &other)
{
    TpBluetoothUuidData *data = static_cast<TpBluetoothUuidData *>(data_);
    TpBluetoothUuidData *data_other = static_cast<TpBluetoothUuidData *>(other.data_);
    if (this != &other)
    {
        // 深度复制数据
        memcpy(data->uuid128, data_other->uuid128, 16);
        data->format = data_other->format;
        data->type = data_other->type;
    }
    return *this;
}

tpBool TpBluetoothUuid::operator==(const TpBluetoothUuid &other) const
{
    TpBluetoothUuidData *data = static_cast<TpBluetoothUuidData *>(data_);
    TpBluetoothUuidData *data_other = static_cast<TpBluetoothUuidData *>(other.data_);

    // 不同格式不可能相等

    for (int i = 0; i < 16; i++)
    {
        if (data->uuid128[i] != data_other->uuid128[i])
        {
            return TP_FALSE;
        }
    }
    return TP_TRUE;
}

tpBool TpBluetoothUuid::operator!=(const TpBluetoothUuid &other) const
{
    return !(*this == other) ? TP_TRUE : TP_FALSE;
}

TpBluetoothUuid::Format TpBluetoothUuid::getFormat() const
{
    return static_cast<TpBluetoothUuidData *>(data_)->format;
}

// 转换为16位UUID
tpUInt16 TpBluetoothUuid::toUInt16(tpBool *ok) const
{
    TpBluetoothUuidData *data = static_cast<TpBluetoothUuidData *>(data_);
    tpUInt16 uuid;
    if (bluet_uuid128_to_uuid16(data->uuid128, &uuid) < 0)
    {
        if (ok)
            *ok = TP_FALSE;
    }
    if (ok)
        *ok = TP_TRUE;
    return uuid;
}

// 转换为32位UUID
tpUInt32 TpBluetoothUuid::toUInt32(tpBool *ok) const
{
    TpBluetoothUuidData *data = static_cast<TpBluetoothUuidData *>(data_);
    tpUInt32 uuid;
    if (bluet_uuid128_to_uuid32(data->uuid128, &uuid) < 0)
    {
        if (ok)
            *ok = TP_FALSE;
    }

    if (ok)
        *ok = TP_TRUE;
    return uuid;
}

TpString TpBluetoothUuid::toString() const
{
    TpBluetoothUuidData *data = static_cast<TpBluetoothUuidData *>(data_);

    char *uuidstr = bluet_uuid128_to_uuidstr(data->uuid128);
    if (!uuidstr)
        return "UnKnown";
    TpString uuid(uuidstr);
    free(uuidstr);
    return uuid;
}

// 检查是否为无效UUID
tpBool TpBluetoothUuid::isNull() const
{
    return (getFormat() == TpBluetoothUuid::FormatUnknown) ? TP_TRUE : TP_FALSE;
}

TpString TpBluetoothUuid::toName() const
{
    TpBluetoothUuidData *data = static_cast<TpBluetoothUuidData *>(data_);
    TpString name("Unknown");
    uint16_t uuid16;
    if (bluet_uuid128_to_uuid16(data->uuid128, &uuid16) < 0)
        return "Unknown";

    switch (data->type)
    {
    case TP_UUID_TYPE_PROTOCOL:
        return "Unknown";

    case TP_UUID_TYPE_PROFILE:
    default:
    {
        const char *uuid_name = bluet_uuid_to_name(uuid16);
        if (!uuid_name)
        {
            break;
        }
        name = TpString(uuid_name);
        break;
    }
    }
    return name;
}
