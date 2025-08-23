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
#include "tpBluetoothService.h"
#include "tpDbusConnectManage.h"
#include "bluetooth/include/blt_device.h"
#include "bluetooth/include/blt_service_reg.h"
#include "bluetooth/include/blt_sdp.h"

struct tpBluetoothServiceData{
	tpBluetoothAddress addr;	//服务产生或注册的设备
	tpBluetoothUuid uuid;		//服务的uuid
	tpList<tpBluetoothUuid> class_uuids;
	tpString name;				//服务的名字
	tpUInt32 rec_handle;
	tpString desc;				//服务的描述
	tpString provider;
	tpUInt16 port;				//通道
	tpBool is_registed;			//是否注册
	int servicefd;
	std::map<uint16_t, tpVariant> m_attributes;
	tpBluetoothServiceData(){
		port=0;
		is_registed=TP_FALSE;
		rec_handle=0;
	};
};


tpBluetoothService::tpBluetoothService()
{
	data_ = new tpBluetoothServiceData();
	tpBluetoothServiceData *data = static_cast<tpBluetoothServiceData *>(data_);
	if(tpDbusConnectManage::instance().connection()!=TP_TRUE)
	{
		fprintf(stderr,"[Error]: connect to dbus error\n");
		return ;
	}
	service_registry_init();
}

// 拷贝赋值
tpBluetoothService& tpBluetoothService::operator=(const tpBluetoothService& other) {
	if (this != &other) {
        // 删除现有数据
        delete static_cast<tpBluetoothServiceData*>(data_);
        
        // 深拷贝数据
        data_ = new tpBluetoothServiceData();
        const tpBluetoothServiceData* otherData = static_cast<const tpBluetoothServiceData*>(other.data_);
        *static_cast<tpBluetoothServiceData*>(data_) = *otherData;
    }
    return *this;
}

// 移动构造函数
tpBluetoothService::tpBluetoothService(tpBluetoothService&& other) noexcept
    : data_(other.data_)
{
    other.data_ = nullptr;
}

// 移动赋值运算符
tpBluetoothService& tpBluetoothService::operator=(tpBluetoothService&& other) noexcept {
    if (this != &other) {
        delete static_cast<tpBluetoothServiceData*>(data_);
        data_ = other.data_;
        other.data_ = nullptr;
    }
    return *this;
}


tpBluetoothService::~tpBluetoothService()
{
	tpBluetoothServiceData *data = static_cast<tpBluetoothServiceData *>(data_);
	if(!data)
		return ;
//	unregisterService();
	delete(data);
}



/// @brief 获取服务的UUID
/// @return 
tpBluetoothUuid tpBluetoothService::getServiceUuid() const
{
	tpBluetoothServiceData *data = static_cast<tpBluetoothServiceData *>(data_);
	if(!data->uuid.isNull())
		return data->uuid;
	
	tpBluetoothUuid uuid;
	if(data->class_uuids.size()==0)
		return uuid;
	uuid=data->class_uuids.front();
	return uuid;
}

/// @brief 设置服务的UUID
/// @param uuid 
/// @return 
int tpBluetoothService::setServiceUuid(const tpBluetoothUuid& uuid)
{
	tpBluetoothServiceData *data = static_cast<tpBluetoothServiceData *>(data_);
	data->uuid=uuid;
	return 0;
}

/// @brief 获取服务的名字
/// @return 
tpString tpBluetoothService::getServiceName() const
{
	tpBluetoothServiceData *data = static_cast<tpBluetoothServiceData *>(data_);
	return data->name;
}

/// @brief 设置服务的名字
/// @param name 
/// @return 
int tpBluetoothService::setServiceName(const tpString& name)
{
	tpBluetoothServiceData *data = static_cast<tpBluetoothServiceData *>(data_);
	data->name=name;
	return 0;
}

/// @brief 获取产生服务的设备地址
/// @return 
tpBluetoothAddress tpBluetoothService::getDeviceAddress() const
{
	tpBluetoothServiceData *data = static_cast<tpBluetoothServiceData *>(data_);
	return data->addr;
}


/// @brief 获取服务的描述
/// @return 
tpString tpBluetoothService::getServiceDescription() const
{
	tpBluetoothServiceData *data = static_cast<tpBluetoothServiceData *>(data_);

	return data->desc;
}

/// @brief 设置服务的描述
/// @param desc 
/// @return 
int tpBluetoothService::setServiceDescription(const tpString& desc)
{
	tpBluetoothServiceData *data = static_cast<tpBluetoothServiceData *>(data_);
	data->desc=desc;
	return 0;
}


int tpBluetoothService::setServiceChannel(tpUInt16 channel)
{
	tpBluetoothServiceData *data = static_cast<tpBluetoothServiceData *>(data_);
	data->port=channel;
	return 0;
}

/// @brief 获取通道号
/// @return 
tpUInt16 tpBluetoothService::getServerChannel()
{
	tpBluetoothServiceData *data = static_cast<tpBluetoothServiceData *>(data_);

	return data->port;
}

int tpBluetoothService::setServiceProvider(const tpString& provider)
{
	tpBluetoothServiceData *data = static_cast<tpBluetoothServiceData *>(data_);
	data->provider=provider;
	return 0;
}

tpString tpBluetoothService::getServiceProvider() const
{
	tpBluetoothServiceData *data = static_cast<tpBluetoothServiceData *>(data_);
	return data->provider;
}

tpBool tpBluetoothService::registerService(const tpBluetoothAddress& address)
{
	tpBluetoothServiceData *data = static_cast<tpBluetoothServiceData *>(data_);
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
		data->name=tpString(name);
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

tpBool tpBluetoothService::isRegisted()
{
	tpBluetoothServiceData *data = static_cast<tpBluetoothServiceData *>(data_);
	return data->is_registed;
}

tpBool tpBluetoothService::unregisterService()
{
	tpBluetoothServiceData *data = static_cast<tpBluetoothServiceData *>(data_);
	unregister_bluetooth_service(data->servicefd);
	data->is_registed=TP_FALSE;
	return TP_TRUE;
}


int tpBluetoothService::setServiceClassUuids(tpList<tpBluetoothUuid>& uuids)
{
	tpBluetoothServiceData *data = static_cast<tpBluetoothServiceData *>(data_);
	data->class_uuids.empty();
	for(auto &it : uuids)
	{
		data->class_uuids.emplace_back(it);
	}
	return 0;
}

int tpBluetoothService::addServiceClassUuid(tpBluetoothUuid& uuid)
{
	tpBluetoothServiceData *data = static_cast<tpBluetoothServiceData *>(data_);

	auto it = std::find(data->class_uuids.begin(), data->class_uuids.end(), uuid);
	if(it!=data->class_uuids.end())
	{
		return 0;
	}
	data->class_uuids.emplace_back(uuid);
	return 0;
}


tpList<tpBluetoothUuid> tpBluetoothService::getServiceClassUuids()
{
	tpBluetoothServiceData *data = static_cast<tpBluetoothServiceData *>(data_);
	return data->class_uuids;
}


int tpBluetoothService::setServiceRecHandle(tpUInt32 handle)
{
	tpBluetoothServiceData *data = static_cast<tpBluetoothServiceData *>(data_);
	data->rec_handle=handle;
	return 0;
}

tpUInt32 tpBluetoothService::getServiceRecHandle()
{
	tpBluetoothServiceData *data = static_cast<tpBluetoothServiceData *>(data_);
	return data->rec_handle;
}

void tpBluetoothService::setAttribute(uint16_t attributeId, const tpVariant& value) {
	tpBluetoothServiceData *data = static_cast<tpBluetoothServiceData *>(data_);
	data->m_attributes[attributeId] = value;
}

// 修改 setAttribute 方法
void tpBluetoothService::setAttribute(uint16_t attributeId, const Sequence& value) {
	tpBluetoothServiceData *data = static_cast<tpBluetoothServiceData *>(data_);
    // 使用 getValues() 访问私有成员
    std::vector<tpVariant>* vec = new std::vector<tpVariant>(value.getValues());
    data->m_attributes[attributeId] = tpVariant(vec);
}

const tpVariant& tpBluetoothService::attribute(uint16_t attributeId) const {
	tpBluetoothServiceData *data = static_cast<tpBluetoothServiceData *>(data_);
    auto it = data->m_attributes.find(attributeId);
    if (it == data->m_attributes.end()) {
        throw std::out_of_range("Attribute not found");
    }
    return it->second;
}

const std::map<uint16_t, tpVariant>& tpBluetoothService::attributes() const {
	tpBluetoothServiceData *data = static_cast<tpBluetoothServiceData *>(data_);
    return data->m_attributes;
}


// 设置协议描述符列表
int tpBluetoothService::setProtocolDescriptorList(const Sequence& list) {
    setAttribute(ProtocolDescriptorList, list);
	return 0;
}

// 设置蓝牙配置文件描述符列表
int tpBluetoothService::setProfileDescriptorList(const Sequence& list) {
    setAttribute(BluetoothProfileDescriptorList, list);
	return 0;
}

// 获取协议描述符列表
tpBluetoothService::Sequence tpBluetoothService::getProtocolDescriptorList() const {
    try {
        const tpVariant& attr = attribute(ProtocolDescriptorList);
        if (!attr.isVector()) {
            throw std::runtime_error("ProtocolDescriptorList is not a vector");
        }
        
        const std::vector<tpVariant>* vec = attr.toVectorPtr();
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
tpBluetoothService::Sequence tpBluetoothService::getProfileDescriptorList() const {
    try {
        const tpVariant& attr = attribute(BluetoothProfileDescriptorList);
        if (!attr.isVector()) {
            throw std::runtime_error("BluetoothProfileDescriptorList is not a vector");
        }
        
        const std::vector<tpVariant>* vec = attr.toVectorPtr();
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


























tpBluetoothService::Sequence::Sequence() = default;

tpBluetoothService::Sequence::Sequence(const Sequence& other) 
    : m_values(other.m_values) {}

tpBluetoothService::Sequence::Sequence(Sequence&& other) noexcept 
    : m_values(std::move(other.m_values)) {}

tpBluetoothService::Sequence::~Sequence() = default;

tpBluetoothService::Sequence& tpBluetoothService::Sequence::operator=(const Sequence& other) {
    if (this != &other) {
        m_values = other.m_values;
    }
    return *this;
}

tpBluetoothService::Sequence& tpBluetoothService::Sequence::operator=(Sequence&& other) noexcept {
    if (this != &other) {
        m_values = std::move(other.m_values);
    }
    return *this;
}

// 添加各种类型的值
tpBluetoothService::Sequence& tpBluetoothService::Sequence::append(bool value) {
    m_values.push_back(tpVariant(value));
    return *this;
}

tpBluetoothService::Sequence& tpBluetoothService::Sequence::append(int8_t value) {
    m_values.push_back(tpVariant(value));
    return *this;
}

tpBluetoothService::Sequence& tpBluetoothService::Sequence::append(uint8_t value) {
    m_values.push_back(tpVariant(value));
    return *this;
}

tpBluetoothService::Sequence& tpBluetoothService::Sequence::append(int16_t value) {
    m_values.push_back(tpVariant(value));
    return *this;
}

tpBluetoothService::Sequence& tpBluetoothService::Sequence::append(uint16_t value) {
    m_values.push_back(tpVariant(value));
    return *this;
}

tpBluetoothService::Sequence& tpBluetoothService::Sequence::append(int32_t value) {
    m_values.push_back(tpVariant(value));
    return *this;
}

tpBluetoothService::Sequence& tpBluetoothService::Sequence::append(uint32_t value) {
    m_values.push_back(tpVariant(value));
    return *this;
}

tpBluetoothService::Sequence& tpBluetoothService::Sequence::append(int64_t value) {
    m_values.push_back(tpVariant(value));
    return *this;
}

tpBluetoothService::Sequence& tpBluetoothService::Sequence::append(uint64_t value) {
    m_values.push_back(tpVariant(value));
    return *this;
}

tpBluetoothService::Sequence& tpBluetoothService::Sequence::append(float value) {
    m_values.push_back(tpVariant(value));
    return *this;
}

tpBluetoothService::Sequence& tpBluetoothService::Sequence::append(double value) {
    m_values.push_back(tpVariant(value));
    return *this;
}

tpBluetoothService::Sequence& tpBluetoothService::Sequence::append(const char* value) {
    m_values.push_back(tpVariant(value));
    return *this;
}

tpBluetoothService::Sequence& tpBluetoothService::Sequence::append(const std::string& value) {
    m_values.push_back(tpVariant(value));
    return *this;
}

tpBluetoothService::Sequence& tpBluetoothService::Sequence::append(const Sequence& value) {
	std::vector<tpVariant>* newVec = new std::vector<tpVariant>(value.m_values);
    m_values.push_back(tpVariant(newVec));
    return *this;
}

// 流式操作符实现
tpBluetoothService::Sequence& tpBluetoothService::Sequence::operator<<(bool value) {
    return append(value);
}

tpBluetoothService::Sequence& tpBluetoothService::Sequence::operator<<(int8_t value) {
    return append(value);
}

tpBluetoothService::Sequence& tpBluetoothService::Sequence::operator<<(uint8_t value) {
    return append(value);
}

tpBluetoothService::Sequence& tpBluetoothService::Sequence::operator<<(int16_t value) {
    return append(value);
}

tpBluetoothService::Sequence& tpBluetoothService::Sequence::operator<<(uint16_t value) {
    return append(value);
}

tpBluetoothService::Sequence& tpBluetoothService::Sequence::operator<<(int32_t value) {
    return append(value);
}

tpBluetoothService::Sequence& tpBluetoothService::Sequence::operator<<(uint32_t value) {
    return append(value);
}

tpBluetoothService::Sequence& tpBluetoothService::Sequence::operator<<(int64_t value) {
    return append(value);
}

tpBluetoothService::Sequence& tpBluetoothService::Sequence::operator<<(uint64_t value) {
    return append(value);
}

tpBluetoothService::Sequence& tpBluetoothService::Sequence::operator<<(float value) {
    return append(value);
}

tpBluetoothService::Sequence& tpBluetoothService::Sequence::operator<<(double value) {
    return append(value);
}

tpBluetoothService::Sequence& tpBluetoothService::Sequence::operator<<(const char* value) {
    return append(value);
}

tpBluetoothService::Sequence& tpBluetoothService::Sequence::operator<<(const std::string& value) {
    return append(value);
}

tpBluetoothService::Sequence& tpBluetoothService::Sequence::operator<<(const Sequence& value) {
    return append(value);
}

// 访问元素
const tpVariant& tpBluetoothService::Sequence::at(size_t index) const {
    if (index >= m_values.size()) {
        throw std::out_of_range("Index out of range in Sequence::at");
    }
    return m_values[index];
}

tpVariant& tpBluetoothService::Sequence::operator[](size_t index) {
    if (index >= m_values.size()) {
        throw std::out_of_range("Index out of range in Sequence::operator[]");
    }
    return m_values[index];
}

size_t tpBluetoothService::Sequence::size() const {
    return m_values.size();
}

bool tpBluetoothService::Sequence::isEmpty() const {
    return m_values.empty();
}

void tpBluetoothService::Sequence::clear() {
    m_values.clear();
}

// 类型安全的取值方法
bool tpBluetoothService::Sequence::boolValueAt(size_t index) const {
    const tpVariant& var = at(index);
    if (!var.isBool()) {
        throw std::runtime_error("Value is not a bool");
    }
    return var.toBool();
}

int8_t tpBluetoothService::Sequence::int8ValueAt(size_t index) const {
    const tpVariant& var = at(index);
    if (!var.isInt8()) {
        throw std::runtime_error("Value is not an int8");
    }
    return var.toInt8();
}

uint8_t tpBluetoothService::Sequence::uint8ValueAt(size_t index) const {
    const tpVariant& var = at(index);
    if (!var.isUint8()) {
        throw std::runtime_error("Value is not a uint8");
    }
    return var.toUInt8();
}

int16_t tpBluetoothService::Sequence::int16ValueAt(size_t index) const {
    const tpVariant& var = at(index);
    if (!var.isInt16()) {
        throw std::runtime_error("Value is not an int16");
    }
    return var.toInt16();
}

uint16_t tpBluetoothService::Sequence::uint16ValueAt(size_t index) const {
    const tpVariant& var = at(index);
    if (!var.isUint16()) {
        throw std::runtime_error("Value is not a uint16");
    }
    return var.toUInt16();
}

int32_t tpBluetoothService::Sequence::int32ValueAt(size_t index) const {
    const tpVariant& var = at(index);
    if (!var.isInt32()) {
        throw std::runtime_error("Value is not an int32");
    }
    return var.toInt32();
}

uint32_t tpBluetoothService::Sequence::uint32ValueAt(size_t index) const {
    const tpVariant& var = at(index);
    if (!var.isUint32()) {
        throw std::runtime_error("Value is not a uint32");
    }
    return var.toUInt32();
}

int64_t tpBluetoothService::Sequence::int64ValueAt(size_t index) const {
    const tpVariant& var = at(index);
    if (!var.isInt64()) {
        throw std::runtime_error("Value is not an int64");
    }
    return var.toInt64();
}

uint64_t tpBluetoothService::Sequence::uint64ValueAt(size_t index) const {
    const tpVariant& var = at(index);
    if (!var.isUint64()) {
        throw std::runtime_error("Value is not a uint64");
    }
    return var.toUint64();
}

float tpBluetoothService::Sequence::floatValueAt(size_t index) const {
    const tpVariant& var = at(index);
    if (!var.isFloat()) {
        throw std::runtime_error("Value is not a float");
    }
    return var.toFloat();
}

double tpBluetoothService::Sequence::doubleValueAt(size_t index) const {
    const tpVariant& var = at(index);
    if (!var.isDouble()) {
        throw std::runtime_error("Value is not a double");
    }
    return var.toDouble();
}

std::string tpBluetoothService::Sequence::stringValueAt(size_t index) const {
    const tpVariant& var = at(index);
    if (!var.isString()) {
        throw std::runtime_error("Value is not a string");
    }
    return var.toString();
}

tpBluetoothService::Sequence tpBluetoothService::Sequence::sequenceValueAt(size_t index) const {
    const tpVariant& var = at(index);
    if (!var.isVector()) {
        throw std::runtime_error("Value is not a vector");
    }
    
    const std::vector<tpVariant>* vec = var.toVectorPtr();
    if (!vec) {
        throw std::runtime_error("Vector pointer is null");
    }
    
    Sequence result;
    // 使用 setValues() 设置私有成员
    result.setValues(*vec);
    return result;
}

// 通用 append 方法
tpBluetoothService::Sequence& tpBluetoothService::Sequence::appendVariant(const tpVariant& value) {
    m_values.push_back(value);
    return *this;
}

const std::vector<tpVariant>& tpBluetoothService::Sequence::getValues() const {
    return m_values;
}

void tpBluetoothService::Sequence::setValues(const std::vector<tpVariant>& values) {
    m_values = values;
}
