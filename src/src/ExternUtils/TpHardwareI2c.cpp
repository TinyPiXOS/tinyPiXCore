/*///------------------------------------------------------------------------------------------------------------------------//
		硬件I2C
说 明 : 使用此功能需要/dev中有I2C设备，如果没有需要修改内核设备树以及驱动(如有需要)以支持硬件I2C。如不想修改内核可以使用软件I2C接口，但会牺牲性能
日 期 : 2025.08.28

/*///------------------------------------------------------------------------------------------------------------------------//

#include <fcntl.h>
#include <unistd.h>
#include <sys/ioctl.h>
#include <linux/i2c.h>
#include <linux/i2c-dev.h>
#include <cerrno>
#include "TpHardwareI2c.h"

#define PATH_I2C_DEVICE	"/dev/i2c-"

struct TpHardwareI2cData{
	TpString path;
	tpUInt8 address;		//从机地址
	tpBool is_open;
	int devfd;
	TpHardwareI2cData(){
		is_open=TP_FALSE;
		address=0X00;
		devfd=-1;
	}
};


TpHardwareI2c::TpHardwareI2c(const TpString& name, tpUInt8 address)
{
	data_ = new TpHardwareI2cData();
	TpHardwareI2cData *data = static_cast<TpHardwareI2cData *>(data_);
	data->address=address;
	data->path=name;
}

TpHardwareI2c::TpHardwareI2c(tpUInt8 bus, tpUInt8 address)
{	
	TpString name=PATH_I2C_DEVICE+std::to_string(static_cast<int>(bus));
	TpHardwareI2c(name,address);
}

TpHardwareI2c::~TpHardwareI2c()
{
	TpHardwareI2cData *data = static_cast<TpHardwareI2cData *>(data_);
	if(!data)
		return ;

	delete(data);
}

tpBool TpHardwareI2c::open()
{
	TpHardwareI2cData *data = static_cast<TpHardwareI2cData *>(data_);
	data->devfd=::open(data->path.c_str(),O_RDWR);
	if(data->devfd<0)
	{
		fprintf(stderr,"[Error]: 打开设备失败\n");
		return TP_FALSE;
	}

	if (ioctl(data->devfd, I2C_SLAVE, data->address) < 0) {
		fprintf(stderr,"[Error]: Failed to set I2C slave address 0x%02X, %s\n",data->address,errno);
		close();
		return TP_FALSE;
	}
	setTimeout(1000);

	data->is_open=TP_TRUE;
	return TP_TRUE;
}

void TpHardwareI2c::close()
{
	TpHardwareI2cData *data = static_cast<TpHardwareI2cData *>(data_);
	::close(data->devfd);
	data->devfd=-1;
}

ssize_t TpHardwareI2c::read(uint8_t* buffer, size_t size) 
{
	TpHardwareI2cData *data = static_cast<TpHardwareI2cData *>(data_);
	if(!data || !data->is_open)
		return -1;
	ssize_t result = ::read(data->devfd, buffer, size);
    
    if (result < 0) {
		fprintf(stderr,"[Error]: I2C read error:%s\n",errno);
    }
    
    return result;
}

ssize_t TpHardwareI2c::write(const uint8_t* buffer, size_t size) 
{
	TpHardwareI2cData *data = static_cast<TpHardwareI2cData *>(data_);
	if(!data || !data->is_open)
		return -1;

	ssize_t result = ::write(data->devfd, data, size);
    if (result < 0) {
		fprintf(stderr,"[Error]: I2C write error:%s\n",errno);
    }
	return result;
}

//设置从机地址
int TpHardwareI2c::setSlaveAddress(tpUInt8 address)
{
	TpHardwareI2cData *data = static_cast<TpHardwareI2cData *>(data_);
	if(!data || !data->is_open)
		return -1;
	if (ioctl(data->devfd, I2C_SLAVE, data->address) < 0) {
		fprintf(stderr,"[Error]: Failed to set I2C slave address 0x%02X, %s\n",data->address,errno);
		return -1;
	}
	data->address=address;
	return 0;
}

/// @brief 获取从机地址
/// @return 
tpUInt8 TpHardwareI2c::getSlaveAddress()
{
	TpHardwareI2cData *data = static_cast<TpHardwareI2cData *>(data_);
	return data->address;
}


tpBool TpHardwareI2c::probeDevice()
{
	TpHardwareI2cData *data = static_cast<TpHardwareI2cData *>(data_);
	if(!data || !data->is_open)
		return TP_FALSE;
	uint8_t dummy;
	if (::read(data->devfd, &dummy, 1) < 0) {
		if (errno == EIO) {
			// EIO表示设备无响应，但地址有效
			return TP_TRUE;
		} else if (errno == ENXIO) {
			// ENXIO表示地址无效或设备不存在
			return TP_FALSE;
		}
	}

	// 其他情况也认为设备存在
	return TP_TRUE;
}

int TpHardwareI2c::setTimeout(int timeout_ms)
{
	TpHardwareI2cData *data = static_cast<TpHardwareI2cData *>(data_);
	if(!data || !data->is_open)
		return -1;
	if (ioctl(data->devfd, I2C_TIMEOUT, timeout_ms) < 0) {
		fprintf(stderr,"[Error]: Failed to set I2C timeout\n");
		return -1;
	}
	return 0;
}



    