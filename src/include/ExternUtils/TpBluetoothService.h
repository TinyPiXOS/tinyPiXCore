#ifndef _TP_BLUETOOTH_SERVICE_H_
#define _TP_BLUETOOTH_SERVICE_H_

#include <map>
#include "TpUtils.h"
#include "TpVariant.h"
#include "TpVector.h"
#include "TpBluetoothDevice.h"
#include "TpBluetoothAddress.h"
#include "TpBluetoothUuid.h"

TP_DEF_VOID_TYPE_VAR(ItpBluetoothServiceData);

class TpBluetoothService{
public:
    class Sequence
    {
    public:
        Sequence();
        Sequence(const Sequence& other);
        Sequence(Sequence&& other) noexcept;
        ~Sequence();
        
        Sequence& operator=(const Sequence& other);
        Sequence& operator=(Sequence&& other) noexcept;
        
        // 添加各种类型的值
        Sequence& append(bool value);
		Sequence& append(int8_t value);
        Sequence& append(uint8_t value);
		Sequence& append(int16_t value);
        Sequence& append(uint16_t value);
        Sequence& append(int32_t value);
        Sequence& append(uint32_t value);
        Sequence& append(int64_t value);
        Sequence& append(uint64_t value);
        Sequence& append(float value);
        Sequence& append(double value);
        Sequence& append(const char* value);
        Sequence& append(const std::string& value);
		Sequence& append(const TpBluetoothUuid& value);
        Sequence& append(const Sequence& value); // 嵌套序列支持
        
        // 流式操作符
        Sequence& operator<<(bool value);
		Sequence& operator<<(int8_t value);
        Sequence& operator<<(uint8_t value);
		Sequence& operator<<(int16_t value);
        Sequence& operator<<(uint16_t value);
        Sequence& operator<<(int32_t value);
        Sequence& operator<<(uint32_t value);
        Sequence& operator<<(int64_t value);
        Sequence& operator<<(uint64_t value);
        Sequence& operator<<(float value);
        Sequence& operator<<(double value);
        Sequence& operator<<(const char* value);
        Sequence& operator<<(const std::string& value);
		Sequence& operator<<(const TpBluetoothUuid& value);
        Sequence& operator<<(const Sequence& value);
        
        // 访问元素
        const TpVariant& at(size_t index) const;
        TpVariant& operator[](size_t index);
        
        // 容器信息
        size_t size() const;
        bool isEmpty() const;
        void clear();
        
        // 类型安全的取值方法，index:在序列中的位置
        bool boolValueAt(size_t index) const;
		int8_t int8ValueAt(size_t index) const;
        uint8_t uint8ValueAt(size_t index) const;
		int16_t int16ValueAt(size_t index) const;
        uint16_t uint16ValueAt(size_t index) const;
        int32_t int32ValueAt(size_t index) const;
        uint32_t uint32ValueAt(size_t index) const;
        int64_t int64ValueAt(size_t index) const;
        uint64_t uint64ValueAt(size_t index) const;
        float floatValueAt(size_t index) const;
        double doubleValueAt(size_t index) const;
        std::string stringValueAt(size_t index) const;
		TpBluetoothUuid uuidValueAt(size_t index) const;
        Sequence sequenceValueAt(size_t index) const; // 嵌套序列取值

		const std::vector<TpVariant>& getValues() const;
        void setValues(const std::vector<TpVariant>& values);
    private:
        // 添加通用 append 方法
        Sequence& appendVariant(const TpVariant& value);
        
        std::vector<TpVariant> m_values;
    };

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
	TpBluetoothService();
	//赋值
	TpBluetoothService& operator=(const TpBluetoothService &other);
	//移动赋值
	TpBluetoothService& operator=(TpBluetoothService&& other) noexcept;
	//移动构造
	TpBluetoothService(TpBluetoothService&& other) noexcept;
	//拷贝构造
	TpBluetoothService(const TpBluetoothService &other);

	~TpBluetoothService();

public:
	/// @brief 获取服务的UUID
	/// @return 
	TpBluetoothUuid getServiceUuid() const;

	/// @brief 设置服务的UUID
	/// @param uuid 
	/// @return 
	int setServiceUuid(const TpBluetoothUuid& uuid);

	/// @brief 获取服务的名字
	/// @return 
	TpString getServiceName() const;

	/// @brief 设置服务的名字
	/// @param name 
	/// @return 
	int setServiceName(const TpString& name);

	/// @brief 获取产生服务的设备地址
	/// @return 
	TpBluetoothAddress getDeviceAddress() const;

	/// @brief 获取服务的描述
	/// @return 
	TpString getServiceDescription() const;

	/// @brief 设置服务的描述
	/// @param desc 
	/// @return 
	int setServiceDescription(const TpString& desc);

	/// @brief 设置服务的通道号
	/// @param channel 
	/// @return 
	int setServiceChannel(tpUInt16 channel);

	/// @brief 获取通道号
	/// @return 
	tpUInt16 getServerChannel();

	/// @brief 设置服务提供者
	/// @param provider 
	/// @return 
	int setServiceProvider(const TpString& provider);

	/// @brief 获取服务提供者
	/// @return 
	TpString getServiceProvider() const;

	/// @brief 注册服务
	/// @param address 
	/// @return 
	tpBool registerService(const TpBluetoothAddress& address= TpBluetoothAddress());

	/// @brief 是否注册
	/// @return 
	tpBool isRegisted();

	/// @brief 注销服务
	/// @return 
	tpBool unregisterService();

	/// @brief 设置服务的所有UUID（会覆盖原来的设置），如果没有设置主UUID，会默认以第一个为主UUID
	/// @param uuids 
	/// @return 
	int setServiceClassUuids(TpList<TpBluetoothUuid>& uuids);

	/// @brief 新增uuid到服务的UUID列表
	/// @param uuid 
	/// @return 
	int addServiceClassUuid(TpBluetoothUuid& uuid);

	/// @brief 获取服务的所有UUID
	/// @return 
	TpList<TpBluetoothUuid> getServiceClassUuids()const ;

	/// @brief 
	/// @param handle 
	/// @return 
	int setServiceRecHandle(tpUInt32 handle);

	/// @brief 
	/// @return 
	tpUInt32 getServiceRecHandle() const;

	/// @brief 
	/// @param attributeId 
	/// @param value 
	void setAttribute(uint16_t attributeId, const TpVariant& value);

    /// @brief 
    /// @param attributeId 
    /// @param value 
    void setAttribute(uint16_t attributeId, const Sequence& value);

	/// @brief 
	/// @param attributeId 
	/// @return 
	const TpVariant& getAttribute(uint16_t attributeId) const;

	/// @brief 
	/// @return 
	const std::map<uint16_t, TpVariant>& getAttributes() const;

	/// @brief 
	/// @return 
	Sequence getProtocolDescriptorList() const;

	/// @brief 
	/// @return 
	Sequence getProfileDescriptorList() const;

	/// @brief 
	/// @param list 
	/// @return 
	int setProtocolDescriptorList(const Sequence& list);

	/// @brief 
	/// @param list 
	/// @return 
	int setProfileDescriptorList(const Sequence& list);

private:
	ItpBluetoothServiceData *data_;
};




#endif