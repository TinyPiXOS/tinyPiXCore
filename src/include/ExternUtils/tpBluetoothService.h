#ifndef _TP_BLUETOOTH_SERVICE_H_
#define _TP_BLUETOOTH_SERVICE_H_

#include "tpUtils.h"
#include "tpBluetoothDevice.h"
#include "tpBluetoothAddress.h"
#include "tpBluetoothUuid.h"

TP_DEF_VOID_TYPE_VAR(ItpBluetoothServiceData);


class tpBluetoothService{
public:
	enum Protocol{
		TP_BLUET_UNKNOWN_PROTOCOL,
		TP_BLUET_L2CAP_PROTOCOL,
		TP_BLUET_RFCOMM_PROTOCOL,
	};

	enum Profile{
		TP_BLUET_UNKNOWN_PROFILE,
		TP_BLUET_SPP_PROFILE,
	};
	enum AttributeType{
		ServiceRecordHandle	= 0X0000,	//指定可从中检索属性的服务记录。
		ServiceClassIds	= 0X0001,	//服务遵循的服务类的 UUID。最常见的服务类定义在（QBluetoothUuid::ServiceClassUuid)
		ServiceRecordState	= 0X0002,	//当添加、删除或修改任何其他服务属性时，属性也会发生变化。
		ServiceId	= 0X0003,	//唯一标识服务的 UUID。
		ProtocolDescriptorList	= 0X0004,	//服务使用的协议列表。最常用的协议 UUID 定义在QBluetoothUuid::ProtocolUuid
		BrowseGroupList	= 0X0005,	//该服务所在浏览组的列表。
		LanguageBaseAttributeIdList	= 0X0006,	//支持人类可读属性的语言基础属性 ID 列表。
		ServiceInfoTimeToLive	= 0X0007,	//服务记录预计保持有效且不变的秒数。
		ServiceAvailability	= 0X0008,	//表示服务可用性的值。
		BluetoothProfileDescriptorList	= 0X0009,	//该服务符合的配置文件列表。
		DocumentationUrl	= 0X000A,	//指向服务文档的 URL。
		ClientExecutableUrl	= 0X000B,	//指向可用于使用该服务的应用程序位置的 URL。
		IconUrl	= 0X000C,	//代表服务的图标位置的 URL。
		AdditionalProtocolDescriptorList	= 0X000D,	//服务使用的附加协议。此属性扩展了ProtocolDescriptorList。
		PrimaryLanguageBase	= 0X0100,	//主要语言文本描述符的基本索引。
		ServiceName	= PrimaryLanguageBase + 0X0000,	//以主要语言表示的蓝牙服务名称。
		ServiceDescription	= PrimaryLanguageBase + 0X0001,	//以主要语言描述蓝牙服务。
		ServiceProvider	= PrimaryLanguageBase + 0X0002,	//提供蓝牙服务主要语言的公司/实体的名称。
	};
	
public:
	tpBluetoothService();
	tpBluetoothService& operator=(const tpBluetoothService &other);
	tpBluetoothService(const tpBluetoothService &other);

	~tpBluetoothService();

public:
	/// @brief 获取服务的UUID
	/// @return 
	tpBluetoothUuid getServiceUuid() const;

	/// @brief 设置服务的UUID
	/// @param uuid 
	/// @return 
	int setServiceUuid(const tpBluetoothUuid& uuid);

	/// @brief 获取服务的名字
	/// @return 
	tpString getServiceName() const;

	/// @brief 设置服务的名字
	/// @param name 
	/// @return 
	int setServiceName(const tpString& name);

	/// @brief 获取产生服务的设备地址
	/// @return 
	tpBluetoothAddress getDeviceAddress() const;

	/// @brief 获取服务的描述
	/// @return 
	tpString getServiceDescription() const;

	/// @brief 设置服务的描述
	/// @param desc 
	/// @return 
	int setServiceDescription(const tpString& desc);

	/// @brief 设置服务的通道号
	/// @param channel 
	/// @return 
	int setServiceChannel(tpUInt16 channel);

	/// @brief 获取通道号
	/// @return 
	tpUInt16 getServerChannel();

	/// @brief 
	/// @param address 
	/// @return 
	tpBool registerService(const tpBluetoothAddress& address= tpBluetoothAddress());

	/// @brief 
	/// @return 
	tpBool isRegisted();

	/// @brief 
	/// @return 
	tpBool unregisterService();


private:
	ItpBluetoothServiceData *data_;
};




#endif