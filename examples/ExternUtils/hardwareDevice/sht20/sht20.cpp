
#include <iostream>
#include "sht20.h"



//从设备地址
#define SHT20_SLAVE_ADDR	0X40
//寄存器地址定义
//#define	REG_WRITE		0X80
//#define	REG_READ		0X81

//命令
#define TRIG_TEMP_HOST		0XE3	//触发温度测量(主机模式，阻塞)
#define TRIG_HUMI_HOST		0XE5	//触发湿度测量(主机模式，阻塞)
#define TRIG_TEMP_UNHOST	0XF3	//触发温度测量(非主机模式，非阻塞)
#define TRIG_HUMI_UNHOST	0XF5	//触发湿度测量(非主机模式，非阻塞)
#define SHT20_WRITE_REG     0xE6    // 写用户寄存器
#define SHT20_READ_REG      0xE7    // 读用户寄存器
#define SHT20_SOFT_RESET    0xFE    // 软复位




static tpUInt8 sht20_crc_check(tpUInt8 *data, tpUInt8 len, tpUInt8 checksum)
{
	tpUInt8 crc = 0x00;
	tpUInt8 i, j;

	for (i = 0; i < len; i++)
	{
		crc ^= data[i];
		for (j = 8; j > 0; j--)
		{
			if (crc & 0x80)
				crc = (crc << 1) ^ 0x131;  // 多项式0x131
			else
				crc <<= 1;
		}
	}

	return (crc == checksum) ? 0 : 1;
}

TpSht20::TpSht20(const TpString& name):TpHardwareI2c(name,SHT20_SLAVE_ADDR)
{
	
}

TpSht20::TpSht20(tpUInt8 bus):TpHardwareI2c(bus,SHT20_SLAVE_ADDR)
{

}

TpSht20::~TpSht20()
{

}


tpBool TpSht20::open()
{
	if(!TpHardwareI2c::open())
		return TP_FALSE;
	

	uint8_t data=SHT20_SOFT_RESET;
	writeCmd(SHT20_SOFT_RESET,1000);
	usleep(20000);

	return TP_TRUE;
}
void TpSht20::close()
{
	TpHardwareI2c::close();
}



float TpSht20::getTemperature(tpBool *is_ok)
{
	tpUInt8 data[3];
	readReg(TRIG_TEMP_UNHOST,(tpUInt8 *)&data,3);
	if(sht20_crc_check(data,2,data[2])!=1)
	{
		if(is_ok) *is_ok=TP_FALSE;
		return 0;
	}
		
	uint16_t raw_data = (data[0] << 8) | (data[1] & 0xFC);
	if(is_ok) *is_ok=TP_TRUE;
    return  (-46.85f + 175.72f * raw_data / 65536.0f);
}

float TpSht20::getHumidity(tpBool *is_ok)
{
	tpUInt8 data[3];
	readReg(TRIG_HUMI_UNHOST,(tpUInt8 *)&data,3);
	if(sht20_crc_check(data,2,data[2])!=1)
	{
		if(is_ok) *is_ok=TP_FALSE;
		return 0;
	}
		
	uint16_t raw_data = (data[0] << 8) | (data[1] & 0xFC);
	if(is_ok) *is_ok=TP_TRUE;
    return  (-6.0f + 125.0f * raw_data / 65536.0f);
}


int TpSht20::setPrecision(PrecisionType type)
{
	tpUInt8 value;
	if (readReg(SHT20_READ_REG, &value,1) < 0)
		return -1;

	// 清除 bit7 和 bit0
	value &= ~((1 << 7) | (1 << 0));

	switch (type) 
	{
		case PRECISION_RH12_T14:
			// 00
			break;
		case PRECISION_RH8_T12:
			value |= (1 << 0); // 01
			break;
		case PRECISION_RH10_T13:
			value |= (1 << 7); // 10
			break;
		case PRECISION_RH11_T11:
			value |= (1 << 7) | (1 << 0); // 11
			break;
		default:
			return -1;
	}

	return writeReg(SHT20_WRITE_REG,&value,1);
}

TpSht20::PrecisionType TpSht20::getPrecision()
{
	uint8_t value;
    if (readReg(SHT20_READ_REG, &value,1) < 0)
        return PRECISION_NONE;

    int bits = ((value >> 7) & 0x01) << 1 | (value & 0x01);

    switch (bits) {
		case 0: return PRECISION_RH12_T14; 
		case 1: return PRECISION_RH8_T12;  
		case 2: return PRECISION_RH10_T13; 
		case 3: return PRECISION_RH11_T11; 
		default: return PRECISION_NONE;
    }
    return PRECISION_NONE;
}
