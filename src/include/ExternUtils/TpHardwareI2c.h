#ifndef __TP_HARDWARE_I2C_H
#define __TP_HARDWARE_I2C_H

#include "TpUtils.h"
#include "TpHardwareDevice.h"

TP_DEF_VOID_TYPE_VAR(ITpHardwareI2c);

class TpHardwareI2c : public TpHardwareDevice
{
public:
	TpHardwareI2c(const TpString& name);
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

	/// @brief 从指定寄存器开始读取
	/// @param reg 起始寄存器
	/// @param data 数据
	/// @param length 数据长度
	/// @param timeout_ms 
	/// @return 
	tpInt64 readReg(tpUInt8 reg, tpUInt8* data, size_t length, uint32_t timeout_ms = 1000);

	/// @brief 从指定寄存器开始写入
	/// @param reg 
	/// @param data 
	/// @param length 
	/// @param timeout_ms 
	/// @return 
	tpInt64 writeReg(tpUInt8 reg, const tpUInt8* data, size_t length, uint32_t timeout_ms = 1000);

	/// @brief 写命令
	/// @param cmd 命令
	/// @param data 数据
	/// @param timeout_ms 超时时间
	/// @return 
	tpInt64 writeCmd(tpUInt8 cmd, uint32_t timeout_ms = 1000);

	/// @brief 设置IIC从机地址
	/// @param address 
	/// @return 
	int setSlaveAddress(tpUInt8 address);
	/// @brief 获取IIC从机地址
	/// @return 
	tpUInt8 getSlaveAddress();
	/// @brief 探测设备是否存在
	/// @return 
	tpBool probeDevice();



private:
	ITpHardwareI2c *data_;
};



#endif