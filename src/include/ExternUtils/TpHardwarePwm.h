#ifndef __TP_HARDWARE_I2C_H
#define __TP_HARDWARE_I2C_H

#include "TpUtils.h"
#include "TpHardwareDevice.h"

TP_DEF_VOID_TYPE_VAR(ITpHardwarePwm);

class TpHardwarePwm : public TpHardwareDevice
{
public:
	/// @brief 
	/// @param name 设备名
	TpHardwarePwm(const TpString& name, tpUInt8 channel=0);
	/// @brief 
	/// @param num pwm控制器号
	TpHardwarePwm(tpUInt8 num, tpUInt8 channel=0);	
	~TpHardwarePwm();

public:
	/// @brief 获取本机上当前内核已开启的PWM控制器编号的列表，但是不一定全部可以使用，可能会被其他功能占用或硬件未引出，需要详细判断
	/// @return 控制器号列表
	static TpList<tpUInt8> getPwmNumbers();

public:
	/// @brief 打开设备
	/// @return 
	tpBool open() override;
	/// @brief 关闭设备
	void close() override;

	int setDutyCycle(float duty);

	int setPeriod(tpUInt32 ns);

private:
	ssize_t read(uint8_t* buffer, size_t length) override ;		//禁止调用
	ssize_t write(const uint8_t* data, size_t length) override ;	//禁止使用
	bool exportPwm();
	bool unexportPwm();
	bool isExportPwm();

private:
	ITpHardwarePwm *data_;
};



#endif