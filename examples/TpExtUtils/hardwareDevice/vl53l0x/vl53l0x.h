#ifndef __VL53L0X_H_
#define __VL53L0X_H_

#include "TpHardwareI2c.h"

class TpVl53l0x : public TpHardwareI2c
{
public:
	TpVl53l0x(const TpString& name);
	TpVl53l0x(tpUInt8 bus);
	~TpVl53l0x();

public:
	tpBool open() override;
	void close() override;

private:
	enum vcselPeriodType { VcselPeriodPreRange, VcselPeriodFinalRange };
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
	bool init(bool io_2v8);
	uint32_t getMeasurementTimingBudget();
	bool setMeasurementTimingBudget(uint32_t budget_us);
	void getSequenceStepEnables(SequenceStepEnables *enables);
	uint8_t getVcselPulsePeriod(vcselPeriodType type);
	void getSequenceStepTimeouts(SequenceStepEnables const * enables, SequenceStepTimeouts *timeouts);
	uint32_t timeoutMclksToMicroseconds(uint16_t timeout_period_mclks, uint8_t vcsel_period_pclks);
	uint32_t timeoutMicrosecondsToMclks(uint32_t timeout_period_us, uint8_t vcsel_period_pclks);
	bool performSingleRefCalibration(uint8_t vhv_init_byte);
	bool setSignalRateLimit(float limit_Mcps);
	float getSignalRateLimit();
};


#endif
