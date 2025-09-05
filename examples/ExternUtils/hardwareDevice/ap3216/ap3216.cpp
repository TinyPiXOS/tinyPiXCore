#include <iostream>
#include "ap3216.h"

//数字环境光传感器 [ALS]、接近传感器 [PS] 和红外LED [IR]

#define AP3216C_SLAVE_ADDR  0x1E  // 默认7位从机地址

//寄存器定义
#define REG_SYS_CONF    0x00		//系统配置
#define REG_IR_DATA_L   0x0A		//IR
#define REG_IR_DATA_H   0x0B		//IR
#define REG_ALS_DATA_L  0x0C		//ALS
#define REG_ALS_DATA_H  0x0D		//ALS
#define REG_PS_DATA_L   0x0E		//PS
#define REG_PS_DATA_H   0x0F		//PS

#define REG_ALS_CONF    0x10 // B5:B4 = range, B3..B0=persist
#define REG_ALS_CALIB   0x19 // factor = val/64

#define REG_PS_LEDCURR  0x21 // B6:B4 current
#define REG_PS_INTMODE  0x22 // not used here
#define REG_PS_MEANTIME 0x23 // B1:B0 = 0..3 => 1..4 times
#define REG_PS_WAIT     0x24 // B3:B0 waiting




struct TpAp3216Data{
	tpUInt16 ir;	//红外强度
	tpUInt16 ps;	//接近距离
	float lux;		//光照强度
	tpBool is_closer;	//是否接近
	tpBool ir_overflow;		//ir溢出标志，溢出时ir和ps数据无效
	tpBool is_null;
	TpAp3216Data(tpUInt16 ir_, tpUInt16 ps_, float lux_, tpBool is_closer_, tpBool ir_overflow_):
	ir(ir_),ps(ps_),lux(lux_),is_closer(is_closer_),ir_overflow(ir_overflow_){
		is_null=TP_FALSE;
	}
};

TpAp3216::TpAp3216(tpUInt16 ir, float lux, tpUInt16 ps, tpBool closer, tpBool ir_ps_of)
{
	data_ = new TpAp3216Data(ir,ps,lux,closer,ir_ps_of);

}
TpAp3216::TpAp3216()
{
	data_ = new TpAp3216Data(0,0,0,TP_FALSE,TP_FALSE);
	TpAp3216Data *data = static_cast<TpAp3216Data *>(data_);
	data->is_null=TP_TRUE;
}
TpAp3216::~TpAp3216()
{
	TpAp3216Data *data = static_cast<TpAp3216Data *>(data_);
	delete(data);
}

// 拷贝赋值
TpAp3216& TpAp3216::operator=(const TpAp3216& other) {
	TpAp3216Data *data = static_cast<TpAp3216Data *>(data_);
	TpAp3216Data *other_data = static_cast<TpAp3216Data *>(other.data_);
    if (this != &other) {
        delete[] data;
        data_ = new TpAp3216Data(other_data->ir,other_data->lux,other_data->ps,other_data->is_closer,other_data->ir_overflow);
    }
    return *this;
}

// 移动赋值运算符实现
TpAp3216& TpAp3216::operator=(TpAp3216&& other) noexcept {
	TpAp3216Data *data = static_cast<TpAp3216Data *>(data_);
    if (this != &other) {
        delete[] data;
        data_ = other.data_;
        other.data_ = nullptr; // 确保原对象析构时不会释放内存
    }
    return *this;
}

tpBool TpAp3216::isNull()
{
	TpAp3216Data *data = static_cast<TpAp3216Data *>(data_);
	return data->is_null;
}

float TpAp3216::getLux()
{
	TpAp3216Data *data = static_cast<TpAp3216Data *>(data_);
	if(data->is_null)
		return -1;
	return data->lux;
}

tpInt16 TpAp3216::getPs()
{
	TpAp3216Data *data = static_cast<TpAp3216Data *>(data_);
	if(data->ir_overflow || data->is_null)
		return -1;
	return data->ps;
}

tpBool TpAp3216::isCloser()
{
	TpAp3216Data *data = static_cast<TpAp3216Data *>(data_);
	return data->is_closer;
}

tpInt16 TpAp3216::getIr()
{
	TpAp3216Data *data = static_cast<TpAp3216Data *>(data_);
	if(data->ir_overflow || data->is_null)
		return -1;
	return data->ir;
}



struct TpAp3216ManagerData{
	uint8_t als_range;	
	tpBool is_open;
	TpAp3216ManagerData(){
		is_open=TP_FALSE;
		als_range=0;
	}
};


static float als_resolution_by_range(uint8_t range) {
    switch (range & 0x3) {
        case 0: return 0.35f;   // 0~20661
        case 1: return 0.0788f;  // 0~5162
        case 2: return 0.0197;  // 0~1291
        case 3: return 0.0049f; // 0~323
        default: return 0.35f;
    }
}



TpAp3216Manager::TpAp3216Manager(const TpString& name):TpHardwareI2c(name,AP3216C_SLAVE_ADDR)
{
	data_ = new TpAp3216ManagerData();
	TpAp3216ManagerData *data = static_cast<TpAp3216ManagerData *>(data_);
}
TpAp3216Manager::TpAp3216Manager(tpUInt8 bus):TpHardwareI2c(bus,AP3216C_SLAVE_ADDR)
{
	TpAp3216ManagerData *data = static_cast<TpAp3216ManagerData *>(data_);
	delete(data);
}
TpAp3216Manager::~TpAp3216Manager()
{

}
tpBool TpAp3216Manager::open()
{
	TpAp3216ManagerData *data = static_cast<TpAp3216ManagerData *>(data_);

	if(!TpHardwareI2c::open())
		return TP_FALSE;

	int ret=getAlsRange_();
	if(ret<0)
		return TP_FALSE;
	data->als_range=static_cast<uint8_t>(ret);

	data->is_open=TP_TRUE;
	return TP_TRUE;
}

void TpAp3216Manager::close()
{
	TpHardwareI2c::close();
}


int TpAp3216Manager::setMode(TpAp3216Manager::SystemModeType type)
{
	return writeOneReg(REG_SYS_CONF,type);
}	


TpAp3216Manager::SystemModeType TpAp3216Manager::getMode()
{
	tpUInt8 type;
	readOneReg(REG_SYS_CONF,&type);
	return static_cast<TpAp3216Manager::SystemModeType>(type&0x07);
}


int TpAp3216Manager::setAlsRange(TpAp3216Manager::AlsDynamicRange range)
{
	TpAp3216ManagerData *data = static_cast<TpAp3216ManagerData *>(data_);
	tpUInt8 val;
	tpUInt8 range_val=(tpUInt8)range;
	readOneReg(REG_ALS_CONF,&val);
	val&=0x3F;
	val|=(range_val<<4);
	if(writeOneReg(REG_ALS_CONF,val)<0)
		return -1;
	data->als_range=static_cast<tpUInt8>(range);
	return 0;
}

TpAp3216Manager::AlsDynamicRange TpAp3216Manager::getAlsRange()
{
	TpAp3216ManagerData *data = static_cast<TpAp3216ManagerData *>(data_);
	
	return static_cast<TpAp3216Manager::AlsDynamicRange>(data->als_range);
}

tpInt8 TpAp3216Manager::getAlsRange_()
{
	tpUInt8 val;
	if(readOneReg(REG_ALS_CONF,&val)<0)
		return -1;
	return ((val>>4)&0x03);
}

int TpAp3216Manager::reset()
{
	if (writeOneReg(REG_SYS_CONF, 0x04) < 0) 
		return -1;
    usleep(10000); // 10ms per datasheet typical
    return 0;
}

tpInt64 TpAp3216Manager::writeOneReg(tpUInt8 reg, const tpUInt8 buf, uint32_t timeout_ms)
{
	return writeReg(reg,&buf,1,timeout_ms);
}

tpInt64 TpAp3216Manager::readOneReg(tpUInt8 reg, tpUInt8 *buf, uint32_t timeout_ms)
{
	return readReg(reg,buf,1,timeout_ms);
}


TpAp3216 TpAp3216Manager::getSampleData()
{
	TpAp3216ManagerData *data = static_cast<TpAp3216ManagerData *>(data_);
	if(!data || !data->is_open)
		return TpAp3216();

	tpUInt8 buf[6];
	if(readReg(0X0A,buf,6)<0)
		return TpAp3216();

	tpUInt16 ir_value = ((tpUInt16)buf[1] << 2) | (buf[0] & 0x03);	//红外强度

	tpUInt16 als_value= (tpUInt16)buf[2] | ((tpUInt16)buf[3] << 8);		//
	float lux=als_value*als_resolution_by_range(data->als_range);

	tpBool is_closer=((buf[5]&0x80)==0 ? TP_FALSE : TP_TRUE);	//是否靠近

	tpBool ir_ps_of = (buf[0] & 0x80)?TP_TRUE:TP_FALSE;	//ir&ps数据是否无效(0有效，1无效)，为1表示溢出，此时红外强度过高，ps无效

	tpUInt16 ps_value = (((tpUInt16)(buf[5] & 0x3F)) << 4) | (buf[4] & 0x0F);	//接近距离

	return TpAp3216(ir_value,lux,ps_value,is_closer,ir_ps_of);
}















