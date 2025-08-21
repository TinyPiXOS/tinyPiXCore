#ifndef _TP_BLUETOOTH_UUID_H_
#define _TP_BLUETOOTH_UUID_H_

#include "tpUtils.h"

TP_DEF_VOID_TYPE_VAR(ItpBluetoothUuidData);


class tpBluetoothUuid{
public:
	enum Format{
		Format16Bit = 0,
		Format32Bit,
		Format128Bit,
		FormatUnknown
	};
	enum ProtocolUuid{

	};
	enum ProfileUuid{

	};
public:
	tpBluetoothUuid();
	tpBluetoothUuid(tpUInt16 uuid);
	tpBluetoothUuid(tpUInt32 uuid);
	tpBluetoothUuid(const tpUInt8 uuid[16]);
	tpBluetoothUuid(const tpString &uuid);
	tpBluetoothUuid(const tpBluetoothUuid::ProtocolUuid uuid);
	tpBluetoothUuid(const tpBluetoothUuid::ProfileUuid uuid);
	tpBluetoothUuid(const tpBluetoothUuid& uuid);
	~tpBluetoothUuid();

	tpBluetoothUuid& operator=(const tpBluetoothUuid &other);
	tpBool operator!=(const tpBluetoothUuid &other) const;
	tpBool operator==(const tpBluetoothUuid &other) const;
public:
	tpBluetoothUuid::Format getFormat() const;
	tpUInt16 toUInt16(tpBool *ok = nullptr)const;
	tpUInt32 toUInt32(tpBool *ok = nullptr)const;
	//tpUInt128 toUInt128()const;
	tpString toString()const;
	tpBool isNull() const;

	static tpString protocolToString(tpBluetoothUuid::ProtocolUuid uuid);
	static tpString ProfileToString(tpBluetoothUuid::ProfileUuid uuid);
private:
	ItpBluetoothUuidData *data_;
};




#endif
