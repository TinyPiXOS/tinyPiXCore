#ifndef __SHT20_H_
#define __SHT20_H_

#include "TpHardwareI2c.h"

class TpSht20 : public TpHardwareI2c
{
public:
	enum PrecisionType{
		PRECISION_NONE,			// 未知，禁止设置
		PRECISION_RH12_T14 , 	// 湿度12bit, 温度14bit
		PRECISION_RH8_T12,      // 湿度8bit,  温度12bit
		PRECISION_RH10_T13,     // 湿度10bit, 温度13bit
		PRECISION_RH11_T11      // 湿度11bit, 温度11bit
	};

public:
	TpSht20(const TpString& name);
	~TpSht20();

public:
	tpBool open() override;
	void close() override;
	float getTemperature(tpBool *is_ok);
	float getHumidity(tpBool *is_ok);
	int setPrecision(TpSht20::PrecisionType type);
	TpSht20::PrecisionType getPrecision();


private:


};


#endif
