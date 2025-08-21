
#include <string>
#include <iomanip>
#include <sstream>
#include <stdexcept>
#include <cstdint>
#include "tpBluetoothUuid.h"
#include "bluetooth/include/blt_sdp.h"


enum tpBluetoothUuidType{
	TP_UUID_TYPE_NONE,
	TP_UUID_TYPE_PROTOCOL,
	TP_UUID_TYPE_PROFILE,
	TP_UUID_TYPE_OTHER,
};

struct tpBluetoothUuidData{
	tpBluetoothUuid::Format format;
	tpBluetoothUuidType type;
	tpUInt8 uuid128[16];		//全部扩展成128位uuid
	tpBluetoothUuidData(){
		format=tpBluetoothUuid::FormatUnknown;
		type=TP_UUID_TYPE_NONE;
		memset(uuid128, 0, sizeof(uuid128));
	}
	tpBluetoothUuidData(const tpBluetoothUuidData &other) : format(other.format),type(other.type)
    {
        memcpy(uuid128, other.uuid128, sizeof(uuid128));
        format = other.format;
		type = other.type;
    }
};


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
    : data_(new tpBluetoothUuidData()) 
{
	tpBluetoothUuidData *data = static_cast<tpBluetoothUuidData *>(data_);
	bluet_uuid16_to_uuid128(uuid,data->uuid128);
    data->format = Format16Bit;
}

tpBluetoothUuid::tpBluetoothUuid(tpUInt32 uuid)
    : data_(new tpBluetoothUuidData()) 
{
	tpBluetoothUuidData *data = static_cast<tpBluetoothUuidData *>(data_);
	bluet_uuid32_to_uuid128(uuid,data->uuid128);
    data->format = Format32Bit;
}

tpBluetoothUuid::tpBluetoothUuid(const tpUInt8 uuid[16])
    : data_(new tpBluetoothUuidData()) 
{
	tpBluetoothUuidData *data = static_cast<tpBluetoothUuidData *>(data_);
	memcpy(data->uuid128, uuid, 16);
	data->format = Format128Bit;
}

tpBluetoothUuid::tpBluetoothUuid(const tpString &uuid)
    : data_(new tpBluetoothUuidData()) 
{
	tpBluetoothUuidData *data = static_cast<tpBluetoothUuidData *>(data_);
	bluet_uuidstr_to_uuid128(uuid.c_str(),data->uuid128);
	data->format = Format128Bit;
}

tpBluetoothUuid::tpBluetoothUuid(const tpBluetoothUuid& other)
	: data_(new tpBluetoothUuidData(*static_cast<tpBluetoothUuidData*> (other.data_))) 
{	
}


tpBluetoothUuid::tpBluetoothUuid(const tpBluetoothUuid::ProtocolUuid uuid_p)
{
	tpUInt16 uuid=static_cast<tpUInt16>(uuid_p);
	data_ = new tpBluetoothUuidData();
	tpBluetoothUuidData *data = static_cast<tpBluetoothUuidData *>(data_);
	bluet_uuid16_to_uuid128(uuid,data->uuid128);
	data->format=tpBluetoothUuid::Format16Bit;
	data->type=TP_UUID_TYPE_PROTOCOL;
}

tpBluetoothUuid::tpBluetoothUuid(const tpBluetoothUuid::ProfileUuid uuid_p)
{
	tpUInt16 uuid=static_cast<tpUInt16>(uuid_p);
	data_ = new tpBluetoothUuidData();
	tpBluetoothUuidData *data = static_cast<tpBluetoothUuidData *>(data_);
	bluet_uuid16_to_uuid128(uuid,data->uuid128);
	data->format=tpBluetoothUuid::Format16Bit;
	data->type=TP_UUID_TYPE_PROFILE;
}


tpBluetoothUuid::~tpBluetoothUuid()
{
	tpBluetoothUuidData *data = static_cast<tpBluetoothUuidData *>(data_);
	data->format=tpBluetoothUuid::FormatUnknown;
	data->type=TP_UUID_TYPE_NONE;
}


// 赋值运算符
tpBluetoothUuid& tpBluetoothUuid::operator=(const tpBluetoothUuid &other) 
{
	tpBluetoothUuidData *data = static_cast<tpBluetoothUuidData *>(data_);
	tpBluetoothUuidData *data_other = static_cast<tpBluetoothUuidData*>(other.data_);
	if (this != &other) {
        // 深度复制数据
        memcpy(data->uuid128, data_other->uuid128, 16);
        data->format = data_other->format;
		data->type=data_other->type;
    }
    return *this;
}

tpBool tpBluetoothUuid::operator==(const tpBluetoothUuid &other) const 
{
	tpBluetoothUuidData *data = static_cast<tpBluetoothUuidData *>(data_);
	tpBluetoothUuidData *data_other = static_cast<tpBluetoothUuidData*>(other.data_);

	// 不同格式不可能相等

	for (int i = 0; i < 16; i++) {
		if (data->uuid128[i] != data_other->uuid128[i]) {
			return TP_FALSE;
		}
	}
    return TP_TRUE;
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
    tpUInt16 uuid;
	if(bluet_uuid128_to_uuid16(data->uuid128,&uuid)<0)
	{
		if(ok) *ok=TP_FALSE;
	}
	if(ok) *ok=TP_TRUE;
	return uuid;
}

// 转换为32位UUID
tpUInt32 tpBluetoothUuid::toUInt32(tpBool *ok) const 
{
    tpBluetoothUuidData *data = static_cast<tpBluetoothUuidData*>(data_);
    tpUInt32 uuid;
	if(bluet_uuid128_to_uuid32(data->uuid128,&uuid)<0)
	{
		if(ok) *ok=TP_FALSE;
	}
		
	if(ok) *ok=TP_TRUE;
	return uuid;
}

tpString tpBluetoothUuid::toString() const 
{
    tpBluetoothUuidData *data = static_cast<tpBluetoothUuidData*>(data_);

	char *uuidstr=bluet_uuid128_to_uuidstr(data->uuid128);
	if(!uuidstr)
		return "UnKnown";
	tpString uuid(uuidstr);
	free(uuidstr);
	return uuid;
}

// 检查是否为无效UUID
tpBool tpBluetoothUuid::isNull() const 
{
    return (getFormat() == tpBluetoothUuid::FormatUnknown) ? TP_TRUE : TP_FALSE;
}



tpString tpBluetoothUuid::toName()const
{
	tpBluetoothUuidData *data = static_cast<tpBluetoothUuidData*>(data_);
	switch(data->type)
	{
		case TP_UUID_TYPE_PROTOCOL:
			return "Unknown";
		case TP_UUID_TYPE_PROFILE:
		{
			// 使用字符串流直接构建完整UUID
			char *uuidstr=bluet_uuid128_to_uuidstr(data->uuid128);
			if(!uuidstr)
				return "Unknown";
			const char* name = bluet_uuid_to_name(uuidstr);
			if (!name) {
				return "Unknown";
			}
			free(uuidstr);
			return tpString(name);
		}
		default:
			return "Unknown";
	}
}

