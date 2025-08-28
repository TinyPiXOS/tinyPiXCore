#ifndef __TP_HARDWARE_I2C_H
#define __TP_HARDWARE_I2C_H

#include "TpUtils.h"
#include "TpHardwareDevice.h"

TP_DEF_VOID_TYPE_VAR(ITpHardwareI2c);

class TpHardwareI2c : public TpHardwareDevice
{
public:
	/// @brief 
	/// @param name 设备名
	/// @param address 从设备地址
	TpHardwareI2c(const TpString& name, tpUInt8 address);
	/// @brief 
	/// @param bus I2C总线编号
	/// @param address 从设备地址
	TpHardwareI2c(tpUInt8 bus, tpUInt8 address);	
	~TpHardwareI2c();
public:
	/// @brief 打开设备
	/// @return 
	tpBool open();
	/// @brief 关闭设备
	void close();
	/// @brief 
	/// @param buffer 
	/// @param length 
	/// @return 
	ssize_t read(uint8_t* buffer, size_t length);
	/// @brief 
	/// @param data 
	/// @param length 
	/// @return 
	ssize_t write(const uint8_t* data, size_t length);
	/// @brief 设置IIC从及地址
	/// @param address 
	/// @return 
	int setSlaveAddress(tpUInt8 address);
	/// @brief 获取IIC从机地址
	/// @return 
	tpUInt8 getSlaveAddress();
	/// @brief 探测设备是否存在
	/// @return 
	tpBool probeDevice();
	int setTimeout(int timeout_ms);

private:
	ITpHardwareI2c *data_;
};



#endif