#ifndef __TP_GPIO_H
#define __TP_GPIO_H

#include "TpUtils.h"
#include "TpHardwareDevice.h"

TP_DEF_VOID_TYPE_VAR(ITpGpioData);

class TpGpio : public TpHardwareDevice
{
public:
	/// @brief GPIO输入输出模式
	enum GpioDirectionType{
		OUTPUT,
		INPUT,
	};

	enum GpioInterruptType{
		EDGE_NONE,		//非中断引脚
		EDGE_RISING,		//上升沿触发
		EDGE_FALLING,	//下降沿触发
		EDGE_BOTH,		//边沿触发
	};

public:
	/// @brief 
	/// @param number gpio编号
	TpGpio(tpUInt16 number);
	
	//每种芯片计算方式不一样暂时不支持
	//TpGpio(tpUInt16 port,tpUInt16 pin);
	~TpGpio();
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

	/// @brief 设置GPIO输入输出模式
	/// @param type 模式
	/// @return 
	int setDirection(GpioDirectionType type);

	/// @brief 获取GPIO输入输出模式
	/// @return 
	GpioDirectionType getDirection();

	/// @brief 设置输出高电平
	/// @return 
	int setHeight();

	/// @brief 设置输出低电平
	/// @return 
	int setLow();

	/// @brief 获取当前GPIO的输入电平
	/// @return TP_TRUE为高电平，TP_FALSE为低电平
	tpBool getLevel();


private:	
	bool exportGpio();
	bool unexportGpio();
	bool writeToFile(const TpString& path, const TpString& value);
	TpString readFromFile(const TpString& path);
private:
	ITpGpioData *data_;
};



#endif