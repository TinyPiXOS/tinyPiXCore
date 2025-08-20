#include "tpBluetoothUuid.h"




struct tpBluetoothUuidData{
	tpBluetoothUuid::Format format;
	union{
		tpUInt16 uuid16;
		tpUInt32 uuid32;
		tpUInt8 uuid128[16];
	};
	tpBluetoothUuidData(){
		memset(uuid128, 0, sizeof(uuid128));
	}
	tpBluetoothUuidData(const tpBluetoothUuidData &other) : format(other.format)
    {
        // 安全地复制union数据
		switch(format)
		{
			case tpBluetoothUuid::Format128Bit:
				memcpy(uuid128, other.uuid128, sizeof(uuid128));
				break;
			case tpBluetoothUuid::Format32Bit:
				uuid32 = other.uuid32;
				break;
			case tpBluetoothUuid::Format16Bit:
				uuid16 = other.uuid16;
				break;
			default:
				memset(uuid128, 0, sizeof(uuid128));
				break;
		}
    }
	tpBluetoothUuidData(tpUInt16 uuid): format(tpBluetoothUuid::Format16Bit),uuid16(uuid) {}
	tpBluetoothUuidData(tpUInt32 uuid): format(tpBluetoothUuid::Format32Bit),uuid32(uuid) {}
	tpBluetoothUuidData(const uint8_t uuid[16]): format(tpBluetoothUuid::Format128Bit) {
		memcpy(uuid128, uuid, sizeof(uuid128));
	}
	tpBluetoothUuidData(const tpString& uuid);
};

tpBluetoothUuidData::tpBluetoothUuidData(const tpString& uuid)
    : format(tpBluetoothUuid::FormatUnknown)
{
    tpString clean = uuid.toUpper().removeChar('-').removeChar('{').removeChar('}');
    if (clean.length() == 4) {
        bool ok = false;
        uuid16 = clean.toUShort(&ok, 16);
        format = ok ? tpBluetoothUuid::Format16Bit : tpBluetoothUuid::FormatUnknown;
    } 
    else if (clean.length() == 8) {
        bool ok = false;
        uuid32 = clean.toUInt(&ok, 16);
        format = ok ? tpBluetoothUuid::Format32Bit : tpBluetoothUuid::FormatUnknown;
    } 
    else if (clean.length() == 32) {
        for (int i = 0; i < 16; i++) {
            bool ok = false;
            uuid128[i] = clean.mid(i * 2, 2).toUShort(&ok, 16);
            if (!ok) {
                format = tpBluetoothUuid::FormatUnknown;
                return;
            }
        }
        format = tpBluetoothUuid::Format128Bit;
    }
}

// 辅助函数：深拷贝创建私有数据对象
static tpBluetoothUuidData* clonePrivate(const tpBluetoothUuidData* other)
{
    if (!other) return new tpBluetoothUuidData();
    return new tpBluetoothUuidData(*other);
}



tpBluetoothUuid::tpBluetoothUuid()
	: data_(new tpBluetoothUuidData()) 
{}

tpBluetoothUuid::tpBluetoothUuid(tpUInt16 uuid)
    : data_(new tpBluetoothUuidData(uuid)) 
{}

tpBluetoothUuid::tpBluetoothUuid(tpUInt32 uuid)
    : data_(new tpBluetoothUuidData(uuid)) 
{}

tpBluetoothUuid::tpBluetoothUuid(const tpUInt8 uuid[16])
    : data_(new tpBluetoothUuidData(uuid)) 
{}

tpBluetoothUuid::tpBluetoothUuid(const tpString &uuid)
    : data_(new tpBluetoothUuidData(uuid)) 
{}

tpBluetoothUuid::tpBluetoothUuid(const tpBluetoothUuid& other)
{
	tpBluetoothUuidData *data_other = static_cast<tpBluetoothUuidData *>(other.data_);
	data_=clonePrivate(data_other);
}


tpBluetoothUuid::tpBluetoothUuid(const tpBluetoothUuid::ProtocolUuid uuid)
{}

tpBluetoothUuid::tpBluetoothUuid(const tpBluetoothUuid::ProfileUuid uuid)
{}


tpBluetoothUuid::~tpBluetoothUuid()
{
	tpBluetoothUuidData *data = static_cast<tpBluetoothUuidData *>(data_);
}


// 赋值运算符
tpBluetoothUuid& tpBluetoothUuid::operator=(const tpBluetoothUuid &other) 
{
    if (this != &other) {
        // 删除旧数据
        delete static_cast<tpBluetoothUuidData*>(data_);
        
        // 创建新数据副本
        data_ = clonePrivate(static_cast<const tpBluetoothUuidData*>(other.data_));
    }
    return *this;
}

tpBool tpBluetoothUuid::operator==(const tpBluetoothUuid &other) const 
{
	tpBluetoothUuidData *data = static_cast<tpBluetoothUuidData *>(data_);
	tpBluetoothUuidData *data_other = static_cast<tpBluetoothUuidData*>(other.data_);

	// 不同格式不可能相等
	if (data->format != data_other->format) return TP_FALSE;

	// 根据格式比较
	switch (data->format) {
		case tpBluetoothUuid::Format16Bit: 
			return (data->uuid16 == data_other->uuid16) ? TP_TRUE : TP_FALSE;
		case tpBluetoothUuid::Format32Bit: 
			return (data->uuid32 == data_other->uuid32) ? TP_TRUE : TP_FALSE;
		case tpBluetoothUuid::Format128Bit: 
			return (memcmp(data->uuid128, data_other->uuid128, sizeof(data->uuid128)) == 0) ? TP_TRUE : TP_FALSE;
		case tpBluetoothUuid::FormatUnknown: 
			return TP_TRUE; // 两个未知的UUID视为相等
		default:
			return TP_FALSE;
	}
}

tpBool tpBluetoothUuid::operator!=(const tpBluetoothUuid &other) const 
{
    return !(*this == other) ? TP_TRUE : TP_FALSE;
}

tpBluetoothUuid::Format tpBluetoothUuid::getFormat() const 
{
    return static_cast<tpBluetoothUuidData*>(data_)->format;
}

// 转换为16位UUID
tpUInt16 tpBluetoothUuid::toUInt16(tpBool *ok) const 
{
    tpBluetoothUuidData *data = static_cast<tpBluetoothUuidData*>(data_);
    
    tpBool valid = ((data->format == Format16Bit) ? TP_TRUE : TP_FALSE);
    if (ok) *ok = valid;
    
    return valid ? data->uuid16 : 0;
}

// 转换为32位UUID
tpUInt32 tpBluetoothUuid::toUInt32(tpBool *ok) const 
{
    tpBluetoothUuidData *data = static_cast<tpBluetoothUuidData*>(data_);
    
    tpBool valid = ((data->format == Format32Bit || data->format == Format16Bit)? TP_TRUE : TP_FALSE);
    if (ok) *ok = valid;
    
    if (valid) {
        if (data->format == Format32Bit) {
            return data->uuid32;
        }
        return data->uuid16; // Format16Bit转换为uint32_t
    }
    return 0;
}

tpString tpBluetoothUuid::toString() const 
{
    tpBluetoothUuidData *data = static_cast<tpBluetoothUuidData*>(data_);
    
    switch (data->format) {
    case Format16Bit:
        return tpString::number(data->uuid16, 16).rightJustified(4, '0').toUpper();
        
    case Format32Bit:
        return tpString::number(data->uuid32, 16).rightJustified(8, '0').toUpper();
        
    case Format128Bit: {
        tpString result;
        for (int i = 0; i < 16; i++) {
            if (i == 4 || i == 6 || i == 8 || i == 10) {
                result += '-';
            }
            
            // 直接使用数字转换和填充
            char hex[3];
            snprintf(hex, sizeof(hex), "%02X", data->uuid128[i]);
            result += hex;
        }
        return result;
    }
    
    default:
        return "Unknown";
    }
}

// 检查是否为无效UUID
tpBool tpBluetoothUuid::isNull() const 
{
    return (getFormat() == tpBluetoothUuid::FormatUnknown) ? TP_TRUE : TP_FALSE;
}
