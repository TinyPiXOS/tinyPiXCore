#ifndef __TP_HARDWARE_I2C_H
#define __TP_HARDWARE_I2C_H

#include "TpUtils.h"
#include "TpHardwareDevice.h"

TP_DEF_VOID_TYPE_VAR(ITpHardwarePwm);

/// @brief 硬件PWM类，一个SOC会有多个PWM控制器，每个控制器会有多个通道
class TpHardwarePwm : public TpHardwareDevice
{
public:
	/// @brief 
	/// @param num pwm控制器号
	TpHardwarePwm(tpUInt8 num, tpUInt8 channel=0);	
	~TpHardwarePwm();

public:
	/// @brief 获取本机上当前内核已开启的PWM控制器编号的列表，但是不一定全部可以使用，可能会被其他功能占用或硬件未引出，需要详细判断
	/// @return 控制器号列表
	static TpList<tpUInt8> getPwmNumbers();
	/// @brief 获取编号为num的PWM的可用通道数量
	/// @param num 编号
	/// @return 可用通道数量，例如可用通道数为4,那么可以打开的通道就是0～3
	static int getAvailableChannels(tpUInt8 num);
public:
	/// @brief 打开设备
	/// @return 
	tpBool open() override;
	/// @brief 关闭设备
	void close() override;

	/// @brief 设置PWM输出的占空碧
	/// @param duty 0～100%
	/// @return 
	int setDutyCycle(float duty);

	/// @brief 设置PWM输出的周期
	/// @param ns 纳秒
	/// @return 
	int setPeriod(tpUInt32 ns);

	/// @brief 使能PWM输出，当调用open的时候会默认打开输出，可以手动关闭PWM输出
	/// @param enable 
	/// @return 
	int setEnable(tpBool enable);

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