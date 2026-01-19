
#include <iostream>
#include "vl53l0x.h"

#define VL53L0X_SLAVE_ADDR	0X52


#define SYSRANGE_START                              0x00

#define SYSTEM_THRESH_HIGH                          0x0C
#define SYSTEM_THRESH_LOW                           0x0E

#define SYSTEM_SEQUENCE_CONFIG                      0x01
#define SYSTEM_RANGE_CONFIG                         0x09
#define SYSTEM_INTERMEASUREMENT_PERIOD              0x04

#define SYSTEM_INTERRUPT_CONFIG_GPIO                0x0A

#define GPIO_HV_MUX_ACTIVE_HIGH                     0x84

#define SYSTEM_INTERRUPT_CLEAR                      0x0B

#define RESULT_INTERRUPT_STATUS                     0x13
#define RESULT_RANGE_STATUS                         0x14

#define RESULT_CORE_AMBIENT_WINDOW_EVENTS_RTN       0xBC
#define RESULT_CORE_RANGING_TOTAL_EVENTS_RTN        0xC0
#define RESULT_CORE_AMBIENT_WINDOW_EVENTS_REF       0xD0
#define RESULT_CORE_RANGING_TOTAL_EVENTS_REF        0xD4
#define RESULT_PEAK_SIGNAL_RATE_REF                 0xB6

#define ALGO_PART_TO_PART_RANGE_OFFSET_MM           0x28

#define I2C_SLAVE_DEVICE_ADDRESS                    0x8A

#define MSRC_CONFIG_CONTROL                         0x60

#define PRE_RANGE_CONFIG_MIN_SNR                    0x27
#define PRE_RANGE_CONFIG_VALID_PHASE_LOW            0x56
#define PRE_RANGE_CONFIG_VALID_PHASE_HIGH           0x57
#define PRE_RANGE_MIN_COUNT_RATE_RTN_LIMIT          0x64

#define FINAL_RANGE_CONFIG_MIN_SNR                  0x67
#define FINAL_RANGE_CONFIG_VALID_PHASE_LOW          0x47
#define FINAL_RANGE_CONFIG_VALID_PHASE_HIGH         0x48
#define FINAL_RANGE_CONFIG_MIN_COUNT_RATE_RTN_LIMIT 0x44

#define PRE_RANGE_CONFIG_SIGMA_THRESH_HI            0x61
#define PRE_RANGE_CONFIG_SIGMA_THRESH_LO            0x62

#define PRE_RANGE_CONFIG_VCSEL_PERIOD               0x50
#define PRE_RANGE_CONFIG_TIMEOUT_MACROP_HI          0x51
#define PRE_RANGE_CONFIG_TIMEOUT_MACROP_LO          0x52

#define SYSTEM_HISTOGRAM_BIN                        0x81
#define HISTOGRAM_CONFIG_INITIAL_PHASE_SELECT       0x33
#define HISTOGRAM_CONFIG_READOUT_CTRL               0x55

#define FINAL_RANGE_CONFIG_VCSEL_PERIOD             0x70
#define FINAL_RANGE_CONFIG_TIMEOUT_MACROP_HI        0x71
#define FINAL_RANGE_CONFIG_TIMEOUT_MACROP_LO        0x72
#define CROSSTALK_COMPENSATION_PEAK_RATE_MCPS       0x20

#define MSRC_CONFIG_TIMEOUT_MACROP                  0x46

#define SOFT_RESET_GO2_SOFT_RESET_N                 0xBF
#define IDENTIFICATION_MODEL_ID                     0xC0
#define IDENTIFICATION_REVISION_ID                  0xC2

#define OSC_CALIBRATE_VAL                           0xF8

#define GLOBAL_CONFIG_VCSEL_WIDTH                   0x32
#define GLOBAL_CONFIG_SPAD_ENABLES_REF_0            0xB0
#define GLOBAL_CONFIG_SPAD_ENABLES_REF_1            0xB1
#define GLOBAL_CONFIG_SPAD_ENABLES_REF_2            0xB2
#define GLOBAL_CONFIG_SPAD_ENABLES_REF_3            0xB3
#define GLOBAL_ONFIG_SPAD_ENABLES_REF_4            0xB4
#define GLOBAL_CONFIG_SPAD_ENABLES_REF_5            0xB5

#define GLOBAL_CONFIG_REF_EN_START_SELECT           0xB6
#define DYNAMIC_SPAD_NUM_REQUESTED_REF_SPAD         0x4E
#define DYNAMIC_SPAD_REF_EN_START_OFFSET            0x4F
#define POWER_MANAGEMENT_GO1_POWER_FORCE            0x80

#define VHV_CONFIG_PAD_SCL_SDA__EXTSUP_HV           0x89

#define ALGO_PHASECAL_LIM                           0x30
#define ALGO_PHASECAL_CONFIG_TIMEOUT                0x30

#define startTimeout() (timeout_start_ms = millis())

#define calcMacroPeriod(vcsel_period_pclks) ((((uint32_t)2304 * (vcsel_period_pclks) * 1655) + 500) / 1000)
#define decodeVcselPeriod(reg_val)      (((reg_val) + 1) << 1)
#define checkTimeoutExpired() (io_timeout > 0 && ((uint16_t)(millis() - timeout_start_ms) > io_timeout))

struct SequenceStepEnables
{
	boolean tcc, msrc, dss, pre_range, final_range;
};

struct SequenceStepTimeouts
{
	uint16_t pre_range_vcsel_period_pclks, final_range_vcsel_period_pclks;

	uint16_t msrc_dss_tcc_mclks, pre_range_mclks, final_range_mclks;
	uint32_t msrc_dss_tcc_us,    pre_range_us,    final_range_us;
};


TpVl53l0x::TpVl53l0x(const TpString& name):TpHardwareI2c(name,VL53L0X_SLAVE_ADDR)
{
	
}

TpVl53l0x::TpVl53l0x(tpUInt8 bus):TpHardwareI2c(bus,VL53L0X_SLAVE_ADDR)
{

}

TpVl53l0x::~TpVl53l0x()
{

}


tpBool TpVl53l0x::open()
{
	if(!TpHardwareI2c::open())
		return TP_FALSE;


	return TP_TRUE;
}
void TpVl53l0x::close()
{
	TpHardwareI2c::close();
}

tpInt64 TpVl53l0x::writeOneReg(tpUInt8 reg, const tpUInt8 buf, uint32_t timeout_ms)
{
	return writeReg(reg,&buf,1,timeout_ms);
}

tpInt64 TpVl53l0x::readOneReg(tpUInt8 reg, tpUInt8 *buf, uint32_t timeout_ms)
{
	return readReg(reg,buf,1,timeout_ms);
}


void TpVl53l0x::getSequenceStepEnables(SequenceStepEnables * enables)
{
	uint8_t sequence_config ;
	if(readOneReg(SYSTEM_SEQUENCE_CONFIG,&sequence_config)<0)
		return ;

	enables->tcc          = (sequence_config >> 4) & 0x1;
	enables->dss          = (sequence_config >> 3) & 0x1;
	enables->msrc         = (sequence_config >> 2) & 0x1;
	enables->pre_range    = (sequence_config >> 6) & 0x1;
	enables->final_range  = (sequence_config >> 7) & 0x1;
}


// Get the VCSEL pulse period in PCLKs for the given period type.
// based on VL53L0X_get_vcsel_pulse_period()
uint8_t TpVl53l0x::getVcselPulsePeriod(vcselPeriodType type)
{
	if (type == VcselPeriodPreRange)
	{
		uint8_t value;
		readOneReg(PRE_RANGE_CONFIG_VCSEL_PERIOD,&value);
		return decodeVcselPeriod(value);
	}
	else if (type == VcselPeriodFinalRange)
	{
		uint8_t value;
		readOneReg(FINAL_RANGE_CONFIG_VCSEL_PERIOD,&value);
		return decodeVcselPeriod(value);
	}
	else { return 255; }
}

// Get sequence step timeouts
// based on get_sequence_step_timeout(),
// but gets all timeouts instead of just the requested one, and also stores
// intermediate values
void TpVl53l0x::getSequenceStepTimeouts(SequenceStepEnables const * enables, SequenceStepTimeouts * timeouts)
{
  timeouts->pre_range_vcsel_period_pclks = getVcselPulsePeriod(VcselPeriodPreRange);

	timeouts->msrc_dss_tcc_mclks = readReg(MSRC_CONFIG_TIMEOUT_MACROP) + 1;
	timeouts->msrc_dss_tcc_us = timeoutMclksToMicroseconds(timeouts->msrc_dss_tcc_mclks,timeouts->pre_range_vcsel_period_pclks);

	timeouts->pre_range_mclks = decodeTimeout(readReg16Bit(PRE_RANGE_CONFIG_TIMEOUT_MACROP_HI));
	timeouts->pre_range_us = timeoutMclksToMicroseconds(timeouts->pre_range_mclks,
								timeouts->pre_range_vcsel_period_pclks);

	timeouts->final_range_vcsel_period_pclks = getVcselPulsePeriod(VcselPeriodFinalRange);

	timeouts->final_range_mclks = decodeTimeout(readReg16Bit(FINAL_RANGE_CONFIG_TIMEOUT_MACROP_HI));

	if (enables->pre_range)
	{
	timeouts->final_range_mclks -= timeouts->pre_range_mclks;
	}

	timeouts->final_range_us = timeoutMclksToMicroseconds(timeouts->final_range_mclks,
								timeouts->final_range_vcsel_period_pclks);
}

uint32_t TpVl53l0x::getMeasurementTimingBudget()
{
	SequenceStepEnables enables;
	SequenceStepTimeouts timeouts;

	uint16_t const StartOverhead     = 1910;
	uint16_t const EndOverhead        = 960;
	uint16_t const MsrcOverhead       = 660;
	uint16_t const TccOverhead        = 590;
	uint16_t const DssOverhead        = 690;
	uint16_t const PreRangeOverhead   = 660;
	uint16_t const FinalRangeOverhead = 550;

	// "Start and end overhead times always present"
	uint32_t budget_us = StartOverhead + EndOverhead;

	getSequenceStepEnables(&enables);
	getSequenceStepTimeouts(&enables, &timeouts);

	if (enables.tcc)
	{
	budget_us += (timeouts.msrc_dss_tcc_us + TccOverhead);
	}

	if (enables.dss)
	{
	budget_us += 2 * (timeouts.msrc_dss_tcc_us + DssOverhead);
	}
	else if (enables.msrc)
	{
	budget_us += (timeouts.msrc_dss_tcc_us + MsrcOverhead);
	}

	if (enables.pre_range)
	{
	budget_us += (timeouts.pre_range_us + PreRangeOverhead);
	}

	if (enables.final_range)
	{
	budget_us += (timeouts.final_range_us + FinalRangeOverhead);
	}

	measurement_timing_budget_us = budget_us; // store for internal reuse
	return budget_us;
}


//以微秒为单位设置测量定时预算，即一次测量所允许的时间；ST API和该库负责在测距序列中的子步骤之间划分定时预算。更长的时间预算允许更精确的测量。将预算增加N倍，距离测量标准偏差将减少sqrt（N）倍。默认为约33毫秒；根据VL53L0X_set_measure_timeg_budget_micro_seconds（），最小值为20ms
bool TpVl53l0x::setMeasurementTimingBudget(uint32_t budget_us)
{
	SequenceStepEnables enables;
	SequenceStepTimeouts timeouts;

	uint16_t const StartOverhead     = 1910;
	uint16_t const EndOverhead        = 960;
	uint16_t const MsrcOverhead       = 660;
	uint16_t const TccOverhead        = 590;
	uint16_t const DssOverhead        = 690;
	uint16_t const PreRangeOverhead   = 660;
	uint16_t const FinalRangeOverhead = 550;

	uint32_t used_budget_us = StartOverhead + EndOverhead;

	getSequenceStepEnables(&enables);
	getSequenceStepTimeouts(&enables, &timeouts);

	if (enables.tcc)
	{
	used_budget_us += (timeouts.msrc_dss_tcc_us + TccOverhead);
	}

	if (enables.dss)
	{
	used_budget_us += 2 * (timeouts.msrc_dss_tcc_us + DssOverhead);
	}
	else if (enables.msrc)
	{
	used_budget_us += (timeouts.msrc_dss_tcc_us + MsrcOverhead);
	}

	if (enables.pre_range)
	{
	used_budget_us += (timeouts.pre_range_us + PreRangeOverhead);
	}

	if (enables.final_range)
	{
	used_budget_us += FinalRangeOverhead;

	// "Note that the final range timeout is determined by the timing
	// budget and the sum of all other timeouts within the sequence.
	// If there is no room for the final range timeout, then an error
	// will be set. Otherwise the remaining time will be applied to
	// the final range."

	if (used_budget_us > budget_us)
	{
		// "Requested timeout too big."
		return false;
	}

	uint32_t final_range_timeout_us = budget_us - used_budget_us;

	// set_sequence_step_timeout() begin
	// (SequenceStepId == VL53L0X_SEQUENCESTEP_FINAL_RANGE)

	// "For the final range timeout, the pre-range timeout
	//  must be added. To do this both final and pre-range
	//  timeouts must be expressed in macro periods MClks
	//  because they have different vcsel periods."

	uint32_t final_range_timeout_mclks =
		timeoutMicrosecondsToMclks(final_range_timeout_us,
									timeouts.final_range_vcsel_period_pclks);

	if (enables.pre_range)
	{
		final_range_timeout_mclks += timeouts.pre_range_mclks;
	}

	writeReg16Bit(FINAL_RANGE_CONFIG_TIMEOUT_MACROP_HI,encodeTimeout(final_range_timeout_mclks));

	// set_sequence_step_timeout() end

	measurement_timing_budget_us = budget_us; // store for internal reuse
	}
	return true;
}

bool TpVl53l0x::init(bool io_2v8)
{
	// check model ID register (value specified in datasheet)
	tpUInt8 value;

	if (readOneReg(IDENTIFICATION_MODEL_ID,&value)<0 || value!= 0xEE) 
	{ 
		return false; 
	}

	// VL53L0X_DataInit() begin

	// sensor uses 1V8 mode for I/O by default; switch to 2V8 mode if necessary
	if (io_2v8)
	{
	writeOneReg(VHV_CONFIG_PAD_SCL_SDA__EXTSUP_HV,
		readOneReg(VHV_CONFIG_PAD_SCL_SDA__EXTSUP_HV) | 0x01); // set bit 0
	}

	// "Set I2C standard mode"
	writeOneReg(0x88, 0x00);

	writeOneReg(0x80, 0x01);
	writeOneReg(0xFF, 0x01);
	writeOneReg(0x00, 0x00);

	readOneReg(0x91,&stop_variable);

	writeOneReg(0x00, 0x01);
	writeOneReg(0xFF, 0x00);
	writeOneReg(0x80, 0x00);

	// disable SIGNAL_RATE_MSRC (bit 1) and SIGNAL_RATE_PRE_RANGE (bit 4) limit checks
	writeOneReg(MSRC_CONFIG_CONTROL, readOneReg(MSRC_CONFIG_CONTROL) | 0x12);

	// set final range signal rate limit to 0.25 MCPS (million counts per second)
	setSignalRateLimit(0.25);

	writeOneReg(SYSTEM_SEQUENCE_CONFIG, 0xFF);

	// VL53L0X_DataInit() end

	// VL53L0X_StaticInit() begin

	uint8_t spad_count;
	bool spad_type_is_aperture;
	if (!getSpadInfo(&spad_count, &spad_type_is_aperture)) { return false; }

	// The SPAD map (RefGoodSpadMap) is read by VL53L0X_get_info_from_device() in
	// the API, but the same data seems to be more easily readable from
	// GLOBAL_CONFIG_SPAD_ENABLES_REF_0 through _6, so read it from there
	uint8_t ref_spad_map[6];
	readReg(GLOBAL_CONFIG_SPAD_ENABLES_REF_0, ref_spad_map, 6);

	// -- VL53L0X_set_reference_spads() begin (assume NVM values are valid)

	writeOneReg(0xFF, 0x01);
	writeOneReg(DYNAMIC_SPAD_REF_EN_START_OFFSET, 0x00);
	writeOneReg(DYNAMIC_SPAD_NUM_REQUESTED_REF_SPAD, 0x2C);
	writeOneReg(0xFF, 0x00);
	writeOneReg(GLOBAL_CONFIG_REF_EN_START_SELECT, 0xB4);

	uint8_t first_spad_to_enable = spad_type_is_aperture ? 12 : 0; // 12 is the first aperture spad
	uint8_t spads_enabled = 0;

	for (uint8_t i = 0; i < 48; i++)
	{
	if (i < first_spad_to_enable || spads_enabled == spad_count)
	{
		// This bit is lower than the first one that should be enabled, or
		// (reference_spad_count) bits have already been enabled, so zero this bit
		ref_spad_map[i / 8] &= ~(1 << (i % 8));
	}
	else if ((ref_spad_map[i / 8] >> (i % 8)) & 0x1)
	{
		spads_enabled++;
	}
	}

	writeReg(GLOBAL_CONFIG_SPAD_ENABLES_REF_0, ref_spad_map, 6);

	// -- VL53L0X_set_reference_spads() end

	// -- VL53L0X_load_tuning_settings() begin
	// DefaultTuningSettings from vl53l0x_tuning.h

	writeOneReg(0xFF, 0x01);
	writeOneReg(0x00, 0x00);

	writeOneReg(0xFF, 0x00);
	writeOneReg(0x09, 0x00);
	writeOneReg(0x10, 0x00);
	writeOneReg(0x11, 0x00);

	writeOneReg(0x24, 0x01);
	writeOneReg(0x25, 0xFF);
	writeOneReg(0x75, 0x00);

	writeOneReg(0xFF, 0x01);
	writeOneReg(0x4E, 0x2C);
	writeOneReg(0x48, 0x00);
	writeOneReg(0x30, 0x20);

	writeOneReg(0xFF, 0x00);
	writeOneReg(0x30, 0x09);
	writeOneReg(0x54, 0x00);
	writeOneReg(0x31, 0x04);
	writeOneReg(0x32, 0x03);
	writeOneReg(0x40, 0x83);
	writeOneReg(0x46, 0x25);
	writeOneReg(0x60, 0x00);
	writeOneReg(0x27, 0x00);
	writeOneReg(0x50, 0x06);
	writeOneReg(0x51, 0x00);
	writeOneReg(0x52, 0x96);
	writeOneReg(0x56, 0x08);
	writeOneReg(0x57, 0x30);
	writeOneReg(0x61, 0x00);
	writeOneReg(0x62, 0x00);
	writeOneReg(0x64, 0x00);
	writeOneReg(0x65, 0x00);
	writeOneReg(0x66, 0xA0);

	writeOneReg(0xFF, 0x01);
	writeOneReg(0x22, 0x32);
	writeOneReg(0x47, 0x14);
	writeOneReg(0x49, 0xFF);
	writeOneReg(0x4A, 0x00);

	writeOneReg(0xFF, 0x00);
	writeOneReg(0x7A, 0x0A);
	writeOneReg(0x7B, 0x00);
	writeOneReg(0x78, 0x21);

	writeOneReg(0xFF, 0x01);
	writeOneReg(0x23, 0x34);
	writeOneReg(0x42, 0x00);
	writeOneReg(0x44, 0xFF);
	writeOneReg(0x45, 0x26);
	writeOneReg(0x46, 0x05);
	writeOneReg(0x40, 0x40);
	writeOneReg(0x0E, 0x06);
	writeOneReg(0x20, 0x1A);
	writeOneReg(0x43, 0x40);

	writeOneReg(0xFF, 0x00);
	writeOneReg(0x34, 0x03);
	writeOneReg(0x35, 0x44);

	writeOneReg(0xFF, 0x01);
	writeOneReg(0x31, 0x04);
	writeOneReg(0x4B, 0x09);
	writeOneReg(0x4C, 0x05);
	writeOneReg(0x4D, 0x04);

	writeOneReg(0xFF, 0x00);
	writeOneReg(0x44, 0x00);
	writeOneReg(0x45, 0x20);
	writeOneReg(0x47, 0x08);
	writeOneReg(0x48, 0x28);
	writeOneReg(0x67, 0x00);
	writeOneReg(0x70, 0x04);
	writeOneReg(0x71, 0x01);
	writeOneReg(0x72, 0xFE);
	writeOneReg(0x76, 0x00);
	writeOneReg(0x77, 0x00);

	writeOneReg(0xFF, 0x01);
	writeOneReg(0x0D, 0x01);

	writeOneReg(0xFF, 0x00);
	writeOneReg(0x80, 0x01);
	writeOneReg(0x01, 0xF8);

	writeOneReg(0xFF, 0x01);
	writeOneReg(0x8E, 0x01);
	writeOneReg(0x00, 0x01);
	writeOneReg(0xFF, 0x00);
	writeOneReg(0x80, 0x00);

	// -- VL53L0X_load_tuning_settings() end

	// "Set interrupt config to new sample ready"
	// -- VL53L0X_SetGpioConfig() begin

	writeOneReg(SYSTEM_INTERRUPT_CONFIG_GPIO, 0x04);
	readOneReg(GPIO_HV_MUX_ACTIVE_HIGH,&value);
	writeOneReg(GPIO_HV_MUX_ACTIVE_HIGH, value & ~0x10); // active low
	writeOneReg(SYSTEM_INTERRUPT_CLEAR, 0x01);

	// -- VL53L0X_SetGpioConfig() end

	measurement_timing_budget_us = getMeasurementTimingBudget();

	// "Disable MSRC and TCC by default"
	// MSRC = Minimum Signal Rate Check
	// TCC = Target CentreCheck
	// -- VL53L0X_SetSequenceStepEnable() begin

	writeOneReg(SYSTEM_SEQUENCE_CONFIG, 0xE8);

	// -- VL53L0X_SetSequenceStepEnable() end

	// "Recalculate timing budget"
	setMeasurementTimingBudget(measurement_timing_budget_us);

	// VL53L0X_StaticInit() end

	// VL53L0X_PerformRefCalibration() begin (VL53L0X_perform_ref_calibration())

	// -- VL53L0X_perform_vhv_calibration() begin

	writeOneReg(SYSTEM_SEQUENCE_CONFIG, 0x01);
	if (!performSingleRefCalibration(0x40)) { return false; }

	// -- VL53L0X_perform_vhv_calibration() end

	// -- VL53L0X_perform_phase_calibration() begin

	writeOneReg(SYSTEM_SEQUENCE_CONFIG, 0x02);
	if (!performSingleRefCalibration(0x00)) { return false; }

	// -- VL53L0X_perform_phase_calibration() end

	// "restore the previous Sequence Config"
	writeOneReg(SYSTEM_SEQUENCE_CONFIG, 0xE8);

	// VL53L0X_PerformRefCalibration() end

	return true;
}

// Convert sequence step timeout from MCLKs to microseconds with given VCSEL period in PCLKs
// based on VL53L0X_calc_timeout_us()
uint32_t TpVl53l0x::timeoutMclksToMicroseconds(uint16_t timeout_period_mclks, uint8_t vcsel_period_pclks)
{
  uint32_t macro_period_ns = calcMacroPeriod(vcsel_period_pclks);

  return ((timeout_period_mclks * macro_period_ns) + 500) / 1000;
}

// Convert sequence step timeout from microseconds to MCLKs with given VCSEL period in PCLKs
// based on VL53L0X_calc_timeout_mclks()
uint32_t TpVl53l0x::timeoutMicrosecondsToMclks(uint32_t timeout_period_us, uint8_t vcsel_period_pclks)
{
  uint32_t macro_period_ns = calcMacroPeriod(vcsel_period_pclks);

  return (((timeout_period_us * 1000) + (macro_period_ns / 2)) / macro_period_ns);
}


bool TpVl53l0x::setSignalRateLimit(float limit_Mcps)
{
  if (limit_Mcps < 0 || limit_Mcps > 511.99) { return false; }

  // Q9.7 fixed point format (9 integer bits, 7 fractional bits)
  writeReg16Bit(FINAL_RANGE_CONFIG_MIN_COUNT_RATE_RTN_LIMIT, limit_Mcps * (1 << 7));
  return true;
}

// Get the return signal rate limit check value in MCPS
float TpVl53l0x::getSignalRateLimit()
{
  return (float)readReg16Bit(FINAL_RANGE_CONFIG_MIN_COUNT_RATE_RTN_LIMIT) / (1 << 7);
}



// based on VL53L0X_perform_single_ref_calibration()
bool TpVl53l0x::performSingleRefCalibration(uint8_t vhv_init_byte)
{
	writeOneReg(SYSRANGE_START, 0x01 | vhv_init_byte); // VL53L0X_REG_SYSRANGE_MODE_START_STOP

	startTimeout();

	while ((readOneReg(RESULT_INTERRUPT_STATUS) & 0x07) == 0)
	{
		if (checkTimeoutExpired()) { return false; }
	}

	writeOneReg(SYSTEM_INTERRUPT_CLEAR, 0x01);

	writeOneReg(SYSRANGE_START, 0x00);

	return true;
}