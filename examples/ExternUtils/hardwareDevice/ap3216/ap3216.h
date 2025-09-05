#ifndef __TP_AP3216_H_
#define __TP_AP3216_H_

#include "TpHardwareI2c.h"



TP_DEF_VOID_TYPE_VAR(ITpAp3216);
TP_DEF_VOID_TYPE_VAR(ITpAp3216Manager);

class TpAp3216{
public:
	TpAp3216& operator=(const TpAp3216& other);
	// 移动赋值运算符声明
	TpAp3216& operator=(TpAp3216&& other) noexcept;
	~TpAp3216();
public:
	tpBool isNull();
	/// @brief 光照强度
	/// @return 
	float getLux();
	/// @brief 距离
	/// @return 
	tpInt16 getPs();
	/// @brief 是否是靠近
	/// @return 
	tpBool isCloser();
	/// @brief 红外强度
	/// @return 
	tpInt16 getIr();
private:
	friend class TpAp3216Manager;
	TpAp3216(tpUInt16 ir, float lux, tpUInt16 ps, tpBool closer, tpBool ir_ps_of);
	TpAp3216();
	ITpAp3216 *data_;
};



class TpAp3216Manager : public TpHardwareI2c
{
public:
	/// @brief 工作模式
	enum SystemModeType{
		POWER_DOWN = 0X00,
		FUNCTION_ALS = 0X01,
		FUNCTION_PS_IR = 0X02,
		FUNCTION_ALS_PS_IR = 0X03,
		FUNCTION_RESET = 0x04,
		FUNCTION_ALS_ONCE = 0X05,
		FUNCTION_PS_IR_ONCE = 0X06,
		FUNCTION_ALS_PS_IR_ONCE = 0X07,
	};
	/// @brief 光照强度允许采样范围(量程)
	enum AlsDynamicRange{
		ALS_DYNAMIC_RANGE_0=0X00,	//最高，0～20661lux
		ALS_DYNAMIC_RANGE_1=0X01,	//相对高，0～5162lux
		ALS_DYNAMIC_RANGE_2=0X02,	//相对小，0～1291lux
		ALS_DYNAMIC_RANGE_3=0X03,	//最小，0～323lux
	};
	/// @brief 探测距离设置
	enum PsDetectionRange{
		PS_DETECTION_RANGE_0 = 0X03,	//最远
		PS_DETECTION_RANGE_1 = 0X02,
		PS_DETECTION_RANGE_2 = 0X01,
		PS_DETECTION_RANGE_3 = 0X00,
	};
	/// @brief 探测距离响应速度
	enum PsResponseSpeed{
		PS_RESPONSE_SPEED_0 = 0X00,		//响应最快
		PS_RESPONSE_SPEED_1 = 0X01,
		PS_RESPONSE_SPEED_2 = 0X02,
		PS_RESPONSE_SPEED_3 = 0X03,
		PS_RESPONSE_SPEED_4 = 0X04,
		PS_RESPONSE_SPEED_5 = 0X05,
		PS_RESPONSE_SPEED_6 = 0X06,
		PS_RESPONSE_SPEED_7 = 0X07,
	};
	/// @brief 探测距离的探测时间，影响探测精度
	enum PsResponseTime{
		PS_RESPONSE_TIME_0 = 0X00,		//探测最快，精度最低
		PS_RESPONSE_TIME_1 = 0X01,
		PS_RESPONSE_TIME_2 = 0X02,
		PS_RESPONSE_TIME_3 = 0X03,
	};
	

public:
	TpAp3216Manager(const TpString& name);
	TpAp3216Manager(tpUInt8 bus);
	~TpAp3216Manager();

public:
	tpBool open() override;
	void close() override;
	/// @brief 设置工作模式
	/// @param type 工作模式
	/// @return 
	int setMode(TpAp3216Manager::SystemModeType type);

	/// @brief 获取工作模式
	/// @return 
	TpAp3216Manager::SystemModeType getMode();
	
	/// @brief 设置光照强度的采样量程
	/// @param range 量程
	/// @return 
	int setAlsRange(TpAp3216Manager::AlsDynamicRange range);

	/// @brief 获取光照强度的采样量程(无I2C通信)
	/// @return 
	TpAp3216Manager::AlsDynamicRange getAlsRange();

	/// @brief 获取所有采样数据
	/// @return 
	TpAp3216 getSampleData();

private:
	/// @brief 重启
	/// @return 
	int reset();

	/// @brief 写单个寄存器
	/// @param reg 
	/// @param buf 
	/// @param timeout_ms 
	/// @return 
	tpInt64 writeOneReg(tpUInt8 reg, const tpUInt8 buf, uint32_t timeout_ms=1000);

	/// @brief 读单个寄存器
	/// @param reg 
	/// @param buf 
	/// @param timeout_ms 
	/// @return 
	tpInt64 readOneReg(tpUInt8 reg, tpUInt8 *buf, uint32_t timeout_ms=1000);

	/// @brief 从寄存器读取光照强度测量范围
	/// @return 
	tpInt8 getAlsRange_();

	/// @brief 设置是否使用迟滞模式来判断是否靠近，若需要超高灵敏度可以设置为禁止，否则都应设置为TRUE
	/// @param hysteresis 
	/// @return 
	int setCloserHysteresis(tpBool hysteresis);

	/// @brief 是否使用迟滞模式来判断是否靠近
	/// @return 
	tpBool isCloserHysteresis();

	int setPsDistance();

	int getPsDistance();//我有不理解的地方，als中断是干什么的，als_calib校准因子是干什么的，ps_mean_time和ps_wait感觉没区别啊

	int setPsSampleTime();

	/// @brief 获取具体探测的时间，探测时间越久，结果越精准，但是会降低响应速度
	/// @return 
	int getPsSampleTime();

	int setPsResponseSpeed();

	/// @brief 获取距离探测的响应速度
	/// @return 
	int getPsResponseSpeed();

private:
	ITpAp3216Manager *data_;
};


#endif
