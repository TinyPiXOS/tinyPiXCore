/*///------------------------------------------------------------------------------------------------------------------------//
		蓝牙协议相关
说 明 : 
日 期 : 2025.8.7

/*///------------------------------------------------------------------------------------------------------------------------//

#include <iomanip>
#include <sstream>
#include <stdexcept>
#include <cstdint>
#include <list>
#include <algorithm>
#include "TpBluetoothService.h"
#include "TpDbusConnectManage.h"
#include "bluetooth/include/blt_device.h"
#include "bluetooth/include/blt_service_reg.h"
#include "bluetooth/include/blt_sdp.h"

struct TpBluetoothServiceData{
	TpBluetoothAddress addr;	//服务产生或注册的设备
	TpBluetoothUuid uuid;		//服务的uuid
	TpList<TpBluetoothUuid> class_uuids;
	TpString name;				//服务的名字
	tpUInt32 rec_handle;
	TpString desc;				//服务的描述
	TpString provider;
	tpUInt16 port;				//通道
	tpBool is_registed;			//是否注册
	int servicefd;
	std::map<uint16_t, TpVariant> m_attributes;
	TpBluetoothServiceData(){
		port=0;
		is_registed=TP_FALSE;
		rec_handle=0;
	};
};


TpBluetoothService::TpBluetoothService()
{
	data_ = new TpBluetoothServiceData();
	TpBluetoothServiceData *data = static_cast<TpBluetoothServiceData *>(data_);
	if(TpDbusConnectManage::instance().connection()!=TP_TRUE)
	{
		fprintf(stderr,"[Error]: connect to dbus error\n");
		return ;
	}
	service_registry_init();
}

// 拷贝赋值
TpBluetoothService& TpBluetoothService::operator=(const TpBluetoothService& other) {
	if (this != &other) {
        // 删除现有数据
        delete static_cast<TpBluetoothServiceData*>(data_);
        
        // 深拷贝数据
        data_ = new TpBluetoothServiceData();
        const TpBluetoothServiceData* otherData = static_cast<const TpBluetoothServiceData*>(other.data_);
        *static_cast<TpBluetoothServiceData*>(data_) = *otherData;
    }
    return *this;
}

// 移动构造函数
TpBluetoothService::TpBluetoothService(TpBluetoothService&& other) noexcept
    : data_(other.data_)
{
    other.data_ = nullptr;
}

// 移动赋值运算符
TpBluetoothService& TpBluetoothService::operator=(TpBluetoothService&& other) noexcept {
    if (this != &other) {
        delete static_cast<TpBluetoothServiceData*>(data_);
        data_ = other.data_;
        other.data_ = nullptr;
    }
    return *this;
}


TpBluetoothService::~TpBluetoothService()
{
	TpBluetoothServiceData *data = static_cast<TpBluetoothServiceData *>(data_);
	if(!data)
		return ;
//	unregisterService();
	delete(data);
}



/// @brief 获取服务的UUID
/// @return 
TpBluetoothUuid TpBluetoothService::getServiceUuid() const
{
	TpBluetoothServiceData *data = static_cast<TpBluetoothServiceData *>(data_);
	if(!data->uuid.isNull())
		return data->uuid;
	
	TpBluetoothUuid uuid;
	if(data->class_uuids.size()==0)
		return uuid;
	uuid=data->class_uuids.front();
	return uuid;
}

/// @brief 设置服务的UUID
/// @param uuid 
/// @return 
int TpBluetoothService::setServiceUuid(const TpBluetoothUuid& uuid)
{
	TpBluetoothServiceData *data = static_cast<TpBluetoothServiceData *>(data_);
	data->uuid=uuid;
	return 0;
}

/// @brief 获取服务的名字
/// @return 
TpString TpBluetoothService::getServiceName() const
{
	TpBluetoothServiceData *data = static_cast<TpBluetoothServiceData *>(data_);
	return data->name;
}

/// @brief 设置服务的名字
/// @param name 
/// @return 
int TpBluetoothService::setServiceName(const TpString& name)
{
	TpBluetoothServiceData *data = static_cast<TpBluetoothServiceData *>(data_);
	data->name=name;
	return 0;
}

/// @brief 获取产生服务的设备地址
/// @return 
TpBluetoothAddress TpBluetoothService::getDeviceAddress() const
{
	TpBluetoothServiceData *data = static_cast<TpBluetoothServiceData *>(data_);
	return data->addr;
}


/// @brief 获取服务的描述
/// @return 
TpString TpBluetoothService::getServiceDescription() const
{
	TpBluetoothServiceData *data = static_cast<TpBluetoothServiceData *>(data_);

	return data->desc;
}

/// @brief 设置服务的描述
/// @param desc 
/// @return 
int TpBluetoothService::setServiceDescription(const TpString& desc)
{
	TpBluetoothServiceData *data = static_cast<TpBluetoothServiceData *>(data_);
	data->desc=desc;
	return 0;
}


int TpBluetoothService::setServiceChannel(tpUInt16 channel)
{
	TpBluetoothServiceData *data = static_cast<TpBluetoothServiceData *>(data_);
	data->port=channel;
	return 0;
}

/// @brief 获取通道号
/// @return 
tpUInt16 TpBluetoothService::getServerChannel()
{
	TpBluetoothServiceData *data = static_cast<TpBluetoothServiceData *>(data_);

	return data->port;
}

int TpBluetoothService::setServiceProvider(const TpString& provider)
{
	TpBluetoothServiceData *data = static_cast<TpBluetoothServiceData *>(data_);
	data->provider=provider;
	return 0;
}

TpString TpBluetoothService::getServiceProvider() const
{
	TpBluetoothServiceData *data = static_cast<TpBluetoothServiceData *>(data_);
	return data->provider;
}

tpBool TpBluetoothService::registerService(const TpBluetoothAddress& address)
{
	TpBluetoothServiceData *data = static_cast<TpBluetoothServiceData *>(data_);
	if(data->uuid.isNull())
	{
		fprintf(stderr,"[Error]: 未设置uuid\n");
		return TP_FALSE;
	}

	const char* name = bluet_uuid_to_name(data->uuid.toUInt16());
	if (!name) {
		fprintf(stderr,"[Error]: uuid不合法\n");
		return TP_FALSE;
	}
	
	if(data->name.empty())
	{
		data->name=TpString(name);
	}

	data->servicefd=register_bluetooth_service( data->uuid.toString().c_str(),
                                  data->name.c_str(),
                                  data->port, 
                                  "server");		//暂时只支持注册为服务端(客户端暂无必要场景)
	if(data->servicefd<0)
		return TP_FALSE;
	data->is_registed=TP_TRUE;
	return data->is_registed;
}

tpBool TpBluetoothService::isRegisted()
{
	TpBluetoothServiceData *data = static_cast<TpBluetoothServiceData *>(data_);
	return data->is_registed;
}

tpBool TpBluetoothService::unregisterService()
{
	TpBluetoothServiceData *data = static_cast<TpBluetoothServiceData *>(data_);
	unregister_bluetooth_service(data->servicefd);
	data->is_registed=TP_FALSE;
	return TP_TRUE;
}


int TpBluetoothService::setServiceClassUuids(TpList<TpBluetoothUuid>& uuids)
{
	TpBluetoothServiceData *data = static_cast<TpBluetoothServiceData *>(data_);
	data->class_uuids.empty();
	for(auto &it : uuids)
	{
		data->class_uuids.emplace_back(it);
	}
	return 0;
}

int TpBluetoothService::addServiceClassUuid(TpBluetoothUuid& uuid)
{
	TpBluetoothServiceData *data = static_cast<TpBluetoothServiceData *>(data_);

	auto it = std::find(data->class_uuids.begin(), data->class_uuids.end(), uuid);
	if(it!=data->class_uuids.end())
	{
		return 0;
	}
	data->class_uuids.emplace_back(uuid);
	return 0;
}


TpList<TpBluetoothUuid> TpBluetoothService::getServiceClassUuids()
{
	TpBluetoothServiceData *data = static_cast<TpBluetoothServiceData *>(data_);
	return data->class_uuids;
}


int TpBluetoothService::setServiceRecHandle(tpUInt32 handle)
{
	TpBluetoothServiceData *data = static_cast<TpBluetoothServiceData *>(data_);
	data->rec_handle=handle;
	return 0;
}

tpUInt32 TpBluetoothService::getServiceRecHandle()
{
	TpBluetoothServiceData *data = static_cast<TpBluetoothServiceData *>(data_);
	return data->rec_handle;
}

void TpBluetoothService::setAttribute(uint16_t attributeId, const TpVariant& value) {
	TpBluetoothServiceData *data = static_cast<TpBluetoothServiceData *>(data_);
	data->m_attributes[attributeId] = value;
}

// 修改 setAttribute 方法
void TpBluetoothService::setAttribute(uint16_t attributeId, const Sequence& value) {
	TpBluetoothServiceData *data = static_cast<TpBluetoothServiceData *>(data_);
    // 使用 getValues() 访问私有成员
    std::vector<TpVariant>* vec = new std::vector<TpVariant>(value.getValues());
    data->m_attributes[attributeId] = TpVariant(vec);
}

const TpVariant& TpBluetoothService::attribute(uint16_t attributeId) const {
	TpBluetoothServiceData *data = static_cast<TpBluetoothServiceData *>(data_);
    auto it = data->m_attributes.find(attributeId);
    if (it == data->m_attributes.end()) {
        throw std::out_of_range("Attribute not found");
    }
    return it->second;
}

const std::map<uint16_t, TpVariant>& TpBluetoothService::attributes() const {
	TpBluetoothServiceData *data = static_cast<TpBluetoothServiceData *>(data_);
    return data->m_attributes;
}


// 设置协议描述符列表
int TpBluetoothService::setProtocolDescriptorList(const Sequence& list) {
    setAttribute(ProtocolDescriptorList, list);
	return 0;
}

// 设置蓝牙配置文件描述符列表
int TpBluetoothService::setProfileDescriptorList(const Sequence& list) {
    setAttribute(BluetoothProfileDescriptorList, list);
	return 0;
}

// 获取协议描述符列表
TpBluetoothService::Sequence TpBluetoothService::getProtocolDescriptorList() const {
    try {
        const TpVariant& attr = attribute(ProtocolDescriptorList);
        if (!attr.isVector()) {
            throw std::runtime_error("ProtocolDescriptorList is not a vector");
        }
        
        const std::vector<TpVariant>* vec = attr.toVectorPtr();
        if (!vec) {
            throw std::runtime_error("Vector pointer is null");
        }
        
        Sequence result;
        result.setValues(*vec);
        return result;
    } catch (const std::exception& e) {
        // 返回空序列
		fprintf(stderr, "Error getting protocol descriptor list: %s\n", e.what());
        return Sequence();
    }
}

// 获取蓝牙配置文件描述符列表
TpBluetoothService::Sequence TpBluetoothService::getProfileDescriptorList() const {
    try {
        const TpVariant& attr = attribute(BluetoothProfileDescriptorList);
        if (!attr.isVector()) {
            throw std::runtime_error("BluetoothProfileDescriptorList is not a vector");
        }
        
        const std::vector<TpVariant>* vec = attr.toVectorPtr();
        if (!vec) {
            throw std::runtime_error("Vector pointer is null");
        }
        
        Sequence result;
        result.setValues(*vec);
        return result;
    } catch (const std::exception& e) {
        // 返回空序列
        return Sequence();
    }
}


























TpBluetoothService::Sequence::Sequence() = default;

TpBluetoothService::Sequence::Sequence(const Sequence& other) 
    : m_values(other.m_values) {}

TpBluetoothService::Sequence::Sequence(Sequence&& other) noexcept 
    : m_values(std::move(other.m_values)) {}

TpBluetoothService::Sequence::~Sequence() = default;

TpBluetoothService::Sequence& TpBluetoothService::Sequence::operator=(const Sequence& other) {
    if (this != &other) {
        m_values = other.m_values;
    }
    return *this;
}

TpBluetoothService::Sequence& TpBluetoothService::Sequence::operator=(Sequence&& other) noexcept {
    if (this != &other) {
        m_values = std::move(other.m_values);
    }
    return *this;
}

// 添加各种类型的值
TpBluetoothService::Sequence& TpBluetoothService::Sequence::append(bool value) {
    m_values.push_back(TpVariant(value));
    return *this;
}

TpBluetoothService::Sequence& TpBluetoothService::Sequence::append(int8_t value) {
    m_values.push_back(TpVariant(value));
    return *this;
}

TpBluetoothService::Sequence& TpBluetoothService::Sequence::append(uint8_t value) {
    m_values.push_back(TpVariant(value));
    return *this;
}

TpBluetoothService::Sequence& TpBluetoothService::Sequence::append(int16_t value) {
    m_values.push_back(TpVariant(value));
    return *this;
}

TpBluetoothService::Sequence& TpBluetoothService::Sequence::append(uint16_t value) {
    m_values.push_back(TpVariant(value));
    return *this;
}

TpBluetoothService::Sequence& TpBluetoothService::Sequence::append(int32_t value) {
    m_values.push_back(TpVariant(value));
    return *this;
}

TpBluetoothService::Sequence& TpBluetoothService::Sequence::append(uint32_t value) {
    m_values.push_back(TpVariant(value));
    return *this;
}

TpBluetoothService::Sequence& TpBluetoothService::Sequence::append(int64_t value) {
    m_values.push_back(TpVariant(value));
    return *this;
}

TpBluetoothService::Sequence& TpBluetoothService::Sequence::append(uint64_t value) {
    m_values.push_back(TpVariant(value));
    return *this;
}

TpBluetoothService::Sequence& TpBluetoothService::Sequence::append(float value) {
    m_values.push_back(TpVariant(value));
    return *this;
}

TpBluetoothService::Sequence& TpBluetoothService::Sequence::append(double value) {
    m_values.push_back(TpVariant(value));
    return *this;
}

TpBluetoothService::Sequence& TpBluetoothService::Sequence::append(const char* value) {
    m_values.push_back(TpVariant(value));
    return *this;
}

TpBluetoothService::Sequence& TpBluetoothService::Sequence::append(const std::string& value) {
    m_values.push_back(TpVariant(value));
    return *this;
}

TpBluetoothService::Sequence& TpBluetoothService::Sequence::append(const Sequence& value) {
	std::vector<TpVariant>* newVec = new std::vector<TpVariant>(value.getValues());
    m_values.push_back(TpVariant(newVec));
    return *this;
}

// 流式操作符实现
TpBluetoothService::Sequence& TpBluetoothService::Sequence::operator<<(bool value) {
    return append(value);
}

TpBluetoothService::Sequence& TpBluetoothService::Sequence::operator<<(int8_t value) {
    return append(value);
}

TpBluetoothService::Sequence& TpBluetoothService::Sequence::operator<<(uint8_t value) {
    return append(value);
}

TpBluetoothService::Sequence& TpBluetoothService::Sequence::operator<<(int16_t value) {
    return append(value);
}

TpBluetoothService::Sequence& TpBluetoothService::Sequence::operator<<(uint16_t value) {
    return append(value);
}

TpBluetoothService::Sequence& TpBluetoothService::Sequence::operator<<(int32_t value) {
    return append(value);
}

TpBluetoothService::Sequence& TpBluetoothService::Sequence::operator<<(uint32_t value) {
    return append(value);
}

TpBluetoothService::Sequence& TpBluetoothService::Sequence::operator<<(int64_t value) {
    return append(value);
}

TpBluetoothService::Sequence& TpBluetoothService::Sequence::operator<<(uint64_t value) {
    return append(value);
}

TpBluetoothService::Sequence& TpBluetoothService::Sequence::operator<<(float value) {
    return append(value);
}

TpBluetoothService::Sequence& TpBluetoothService::Sequence::operator<<(double value) {
    return append(value);
}

TpBluetoothService::Sequence& TpBluetoothService::Sequence::operator<<(const char* value) {
    return append(value);
}

TpBluetoothService::Sequence& TpBluetoothService::Sequence::operator<<(const std::string& value) {
    return append(value);
}

TpBluetoothService::Sequence& TpBluetoothService::Sequence::operator<<(const Sequence& value) {
    return append(value);
}

// 访问元素
const TpVariant& TpBluetoothService::Sequence::at(size_t index) const {
    if (index >= m_values.size()) {
        throw std::out_of_range("Index out of range in Sequence::at");
    }
    return m_values[index];
}

TpVariant& TpBluetoothService::Sequence::operator[](size_t index) {
    if (index >= m_values.size()) {
        throw std::out_of_range("Index out of range in Sequence::operator[]");
    }
    return m_values[index];
}

size_t TpBluetoothService::Sequence::size() const {
    return m_values.size();
}

bool TpBluetoothService::Sequence::isEmpty() const {
    return m_values.empty();
}

void TpBluetoothService::Sequence::clear() {
    m_values.clear();
}

// 类型安全的取值方法
bool TpBluetoothService::Sequence::boolValueAt(size_t index) const {
    const TpVariant& var = at(index);
    if (!var.isBool()) {
        throw std::runtime_error("Value is not a bool");
    }
    return var.toBool();
}

int8_t TpBluetoothService::Sequence::int8ValueAt(size_t index) const {
    const TpVariant& var = at(index);
    if (!var.isInt8()) {
        throw std::runtime_error("Value is not an int8");
    }
    return var.toInt8();
}

uint8_t TpBluetoothService::Sequence::uint8ValueAt(size_t index) const {
    const TpVariant& var = at(index);
    if (!var.isUint8()) {
        throw std::runtime_error("Value is not a uint8");
    }
    return var.toUInt8();
}

int16_t TpBluetoothService::Sequence::int16ValueAt(size_t index) const {
    const TpVariant& var = at(index);
    if (!var.isInt16()) {
        throw std::runtime_error("Value is not an int16");
    }
    return var.toInt16();
}

uint16_t TpBluetoothService::Sequence::uint16ValueAt(size_t index) const {
    const TpVariant& var = at(index);
    if (!var.isUint16()) {
        throw std::runtime_error("Value is not a uint16");
    }
    return var.toUInt16();
}

int32_t TpBluetoothService::Sequence::int32ValueAt(size_t index) const {
    const TpVariant& var = at(index);
    if (!var.isInt32()) {
        throw std::runtime_error("Value is not an int32");
    }
    return var.toInt32();
}

uint32_t TpBluetoothService::Sequence::uint32ValueAt(size_t index) const {
    const TpVariant& var = at(index);
    if (!var.isUint32()) {
        throw std::runtime_error("Value is not a uint32");
    }
    return var.toUInt32();
}

int64_t TpBluetoothService::Sequence::int64ValueAt(size_t index) const {
    const TpVariant& var = at(index);
    if (!var.isInt64()) {
        throw std::runtime_error("Value is not an int64");
    }
    return var.toInt64();
}

uint64_t TpBluetoothService::Sequence::uint64ValueAt(size_t index) const {
    const TpVariant& var = at(index);
    if (!var.isUint64()) {
        throw std::runtime_error("Value is not a uint64");
    }
    return var.toUint64();
}

float TpBluetoothService::Sequence::floatValueAt(size_t index) const {
    const TpVariant& var = at(index);
    if (!var.isFloat()) {
        throw std::runtime_error("Value is not a float");
    }
    return var.toFloat();
}

double TpBluetoothService::Sequence::doubleValueAt(size_t index) const {
    const TpVariant& var = at(index);
    if (!var.isDouble()) {
        throw std::runtime_error("Value is not a double");
    }
    return var.toDouble();
}

std::string TpBluetoothService::Sequence::stringValueAt(size_t index) const {
    const TpVariant& var = at(index);
    if (!var.isString()) {
        throw std::runtime_error("Value is not a string");
    }
    return var.toString();
}

TpBluetoothService::Sequence TpBluetoothService::Sequence::sequenceValueAt(size_t index) const {
    const TpVariant& var = at(index);
    if (!var.isVector()) {
        throw std::runtime_error("Value is not a vector");
    }
    
    const std::vector<TpVariant>* vec = var.toVectorPtr();
    if (!vec) {
        throw std::runtime_error("Vector pointer is null");
    }
    
    Sequence result;
    // 使用 setValues() 设置私有成员
    result.setValues(*vec);
    return result;
}

// 通用 append 方法
TpBluetoothService::Sequence& TpBluetoothService::Sequence::appendVariant(const TpVariant& value) {
    m_values.push_back(value);
    return *this;
}

const std::vector<TpVariant>& TpBluetoothService::Sequence::getValues() const {
    return m_values;
}

void TpBluetoothService::Sequence::setValues(const std::vector<TpVariant>& values) {
    m_values = values;
}
