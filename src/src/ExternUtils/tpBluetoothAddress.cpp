/*///------------------------------------------------------------------------------------------------------------------------//
		蓝牙MAC地址
说 明 : 
日 期 : 2025.4.23

/*///------------------------------------------------------------------------------------------------------------------------//

#include <sstream>
#include <algorithm>
#include <iomanip>
#include <cstring>
#include <string.h>
#include "tpBluetoothAddress.h"

struct tpBluetoothAddressData{
	uint8_t address[6];
	tpBluetoothAddressData(){};
};


static char* mac_to_string(const uint8_t addr[6]) {
    const char hex_table[] = "0123456789ABCDEF";
    char* str = (char *)malloc(18);  // 6字节转12字符 + 5冒号 + 终止符 = 18字节[5](@ref)
    
    for (int i = 0; i < 6; ++i) {
        str[i*3]   = hex_table[(addr[i] >> 4) & 0x0F];  // 高四位[5](@ref)
        str[i*3+1] = hex_table[addr[i] & 0x0F];         // 低四位[5](@ref)
        str[i*3+2] = (i < 5) ? ':' : '\0';               // 冒号分隔[4](@ref)
    }
    return str;
}


tpBluetoothAddress::tpBluetoothAddress(const tpString &address)
{
	data_ = new tpBluetoothAddressData();
	parseString(address);
}

tpBluetoothAddress::tpBluetoothAddress(const tpBluetoothAddress& other) 
{
	data_ = new tpBluetoothAddressData();
	tpBluetoothAddressData *data = static_cast<tpBluetoothAddressData *>(data_);
	tpBluetoothAddressData *data_other = static_cast<tpBluetoothAddressData *>(other.data_);
	memcpy(data->address, data_other->address, sizeof(data->address));
}

tpBluetoothAddress::tpBluetoothAddress()
{
	data_ = new tpBluetoothAddressData();
	tpBluetoothAddressData *data = static_cast<tpBluetoothAddressData *>(data_);
	memset(data->address, 0, 6);
}

tpBluetoothAddress::~tpBluetoothAddress()
{
	tpBluetoothAddressData *data = static_cast<tpBluetoothAddressData *>(data_);
	if(!data)
		return ;
	delete (data);
	data=NULL;
}

// 拷贝赋值
tpBluetoothAddress& tpBluetoothAddress::operator=(const tpBluetoothAddress& other) {
	tpBluetoothAddressData *data = static_cast<tpBluetoothAddressData *>(data_);
    if (this != &other) {
        delete[] data;
        data_ = new tpBluetoothAddressData();
		memcpy(data_,other.data_,sizeof(tpBluetoothAddressData));
    }
    return *this;
}

// 移动赋值运算符实现
tpBluetoothAddress& tpBluetoothAddress::operator=(tpBluetoothAddress&& other) noexcept {
	tpBluetoothAddressData *data = static_cast<tpBluetoothAddressData *>(data_);
    if (this != &other) {
        delete[] data;
        data_ = other.data_;
        other.data_ = nullptr; // 确保原对象析构时不会释放内存
    }
    return *this;
}



// 直接接受字符串的赋值运算符实现（可选优化）
/*tpBluetoothAddress& tpBluetoothAddress::operator=(const char* address) {
    delete[] address_data;
    address_data = new char[strlen(address) + 1];
    strcpy(address_data, address);
    return *this;
}*/


bool tpBluetoothAddress::operator==(const tpBluetoothAddress &other)
{
	tpBluetoothAddressData *data = static_cast<tpBluetoothAddressData *>(data_);
	tpBluetoothAddressData *data_other = static_cast<tpBluetoothAddressData *>(other.data_);
    return memcmp(data->address, data_other->address, 6) == 0;
}

bool tpBluetoothAddress::operator!=(const tpBluetoothAddress &other)
{
	tpBluetoothAddressData *data = static_cast<tpBluetoothAddressData *>(data_);
    return !(*this == other);
}

tpBool tpBluetoothAddress::isNull() 
{
	tpBluetoothAddressData *data = static_cast<tpBluetoothAddressData *>(data_);
    for(auto b : data->address) {
        if(b != 0) return TP_FALSE;
    }
    return TP_TRUE;
}

tpString tpBluetoothAddress::toString() 
{
	tpBluetoothAddressData *data = static_cast<tpBluetoothAddressData *>(data_);
    std::ostringstream oss;
    oss << std::hex << std::setfill('0');
    for(int i=0; i<6; ++i) {
        oss << std::setw(2) << static_cast<int>(data->address[i]);
        if(i < 5) oss << ":";
    }
    return oss.str();
}

tpUInt64 tpBluetoothAddress::toUInt64()
{
	tpBluetoothAddressData *data = static_cast<tpBluetoothAddressData *>(data_);
	tpUInt64 result = 0;
	for(int i=0; i<6; ++i) {
		result |= static_cast<tpUInt64>(data->address[i]) << (8*(5-i));
	}
	return result;
}


void tpBluetoothAddress::parseString(const tpString &str) 
{
	tpBluetoothAddressData *data = static_cast<tpBluetoothAddressData *>(data_);
    std::istringstream iss(str);
    tpString token;
    int idx = 0;
    char* endptr = nullptr;
    
    std::fill_n(data->address, 6, 0);  // C++11替代memset的现代写法[6](@ref)

    while(std::getline(iss, token, ':')) {
        if(idx >= 6) {
            throw std::invalid_argument("Too many address segments");
        }
        
        // 校验有效性（C++11正则表达式方案需要<regex>头文件，此处采用手动校验）
        if(token.empty() || token.size() > 2 || 
           !std::all_of(token.begin(), token.end(), ::isxdigit)) {  // C++11算法[1](@ref)
            throw std::invalid_argument("Invalid hex characters");
        }

        const long val = std::strtol(token.c_str(), &endptr, 16);  // C++11兼容方案
        if(*endptr != '\0' || val > 0xFF) {
            throw std::invalid_argument("Invalid byte value");
        }
        
        data->address[idx++] = static_cast<uint8_t>(val);
    }

    if(idx != 6) {
        throw std::invalid_argument("Requires exactly 6 bytes");
    }
}