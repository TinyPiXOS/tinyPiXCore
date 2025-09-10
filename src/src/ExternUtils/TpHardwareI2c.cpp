/*///------------------------------------------------------------------------------------------------------------------------//
		硬件I2C
说 明 : 使用此功能需要/dev中有I2C设备，如果没有需要修改内核设备树以及驱动(如有需要)以支持硬件I2C。如不想修改内核可以使用软件I2C接口，但会牺牲性能
日 期 : 2025.08.28

/*///------------------------------------------------------------------------------------------------------------------------//

#include <fcntl.h>
#include <unistd.h>
#include <signal.h>
#include <sys/ioctl.h>
#include <linux/i2c.h>
#include <linux/i2c-dev.h>
#include <cerrno>
#include <dirent.h>
#include <algorithm>
#include "TpHardwareI2c.h"

#define PATH_I2C_DEVICE	"/dev/i2c-"


enum I2cTransferError{
	I2C_TRANSFER_ERROR_NONE=0,
	I2C_TRANSFER_ERROR_TIMEOUT=-1,	//超时
	I2C_TRANSFER_ERROR_IO=-2,		//io读写错误
};

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



static volatile sig_atomic_t i2c_timeout_flag = 0;
static int default_timeout_ms = 1000; // 默认1秒

void i2c_alarm_handler(int sig) {
    i2c_timeout_flag = 1;
}

void i2c_set_default_timeout(int timeout_ms) {
    default_timeout_ms = timeout_ms;
}

// 底层调用，信号 + ioctl 实现超时控制
static int i2c_rdwr_with_timeout(int fd, struct i2c_rdwr_ioctl_data *packets, int timeout_ms)
{
	if(timeout_ms<=0)
	{
		return ioctl(fd, I2C_RDWR, packets);
	}

    struct sigaction sa_old, sa_new;
    sa_new.sa_handler = i2c_alarm_handler;
    sigemptyset(&sa_new.sa_mask);
    sa_new.sa_flags = 0;

    sigaction(SIGALRM, &sa_new, &sa_old);

    i2c_timeout_flag = 0;
    alarm((timeout_ms + 999) / 1000); // 转换为秒，向上取整

    int ret = ioctl(fd, I2C_RDWR, packets);

    alarm(0); // 取消定时器
    sigaction(SIGALRM, &sa_old, NULL);

    if (i2c_timeout_flag) {
        fprintf(stderr, "I2C transaction timeout\n");
        return I2C_TRANSFER_ERROR_TIMEOUT;
    }
    if (ret < 0) {
        perror("I2C_RDWR failed");
        return I2C_TRANSFER_ERROR_IO;
    }
    return 0;
}


TpHardwareI2c::TpHardwareI2c(const TpString& name): TpHardwareI2c(
        name,0X00)
{
}

TpHardwareI2c::TpHardwareI2c(const TpString& name, tpUInt8 address)
{
	data_ = new TpHardwareI2cData();
	TpHardwareI2cData *data = static_cast<TpHardwareI2cData *>(data_);
	data->address=address;
	data->path=name;
}

TpHardwareI2c::TpHardwareI2c(tpUInt8 bus, tpUInt8 address): TpHardwareI2c(
        TpString(PATH_I2C_DEVICE) + std::to_string(static_cast<int>(bus)),
        address)
{	
}

TpHardwareI2c::~TpHardwareI2c()
{
	TpHardwareI2cData *data = static_cast<TpHardwareI2cData *>(data_);
	if(!data)
		return ;
	if(data->is_open)
		close();
	delete(data);
}

TpList<tpUInt8> TpHardwareI2c::getI2cBuss()
{
	const std::string pwm_dir = "/dev/";
	TpList<tpUInt8> controllers;

	// 打开 PWM 目录
	DIR* dir = opendir(pwm_dir.c_str());
	if (!dir) {
		fprintf(stderr,"[Error]: get i2c buss error\n");
		return controllers;
	}

	// 遍历目录项
	struct dirent* entry;
	while ((entry = readdir(dir)) != nullptr) {
		std::string name(entry->d_name);
		
		// 检查是否是 i2c 设备
		if (name.find("i2c-") == 0) {
			// 提取编号部分
			std::string num_str = name.substr(4); // 
			
			// 验证是否为纯数字
			if (!num_str.empty() && 
				std::all_of(num_str.begin(), num_str.end(), ::isdigit)) {
				try {
					int controller_num = std::stoi(num_str);
					controllers.push_back((tpUInt8)controller_num);
				} catch (const std::exception& e) {
					// 忽略转换失败的项目
					std::cerr << "Warning: Invalid controller number in " 
								<< name << ": " << e.what() << std::endl;
				}
			}
		}
	}

	// 关闭目录
	closedir(dir);

	return controllers;

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
		fprintf(stderr,"[Error]: Failed to set I2C slave address 0x%02X, %s\n",data->address,strerror(errno));
		close();
		return TP_FALSE;
	}

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
		fprintf(stderr,"[Error]: I2C read error:%s\n",strerror(errno));
    }
    
    return result;
}

ssize_t TpHardwareI2c::write(const uint8_t* buffer, size_t size) 
{
	TpHardwareI2cData *data = static_cast<TpHardwareI2cData *>(data_);
	if(!data || !data->is_open)
		return -1;

	ssize_t result = ::write(data->devfd, buffer, size);
    if (result < 0) {
		fprintf(stderr,"[Error]: I2C write error:%s\n",strerror(errno));
    }
	return result;
}

tpInt64 TpHardwareI2c::readReg(tpUInt8 reg, tpUInt8* buf, size_t length, uint32_t timeout_ms)
{
	TpHardwareI2cData *data = static_cast<TpHardwareI2cData *>(data_);
	if(!data || !data->is_open)
		return -1;
	struct i2c_rdwr_ioctl_data packets;
	struct i2c_msg messages[2];

	messages[0].addr  = data->address;
	messages[0].flags = 0;     // 写
	messages[0].len   = 1;
	messages[0].buf   = &reg;

	messages[1].addr  = data->address;
	messages[1].flags = I2C_M_RD;  // 读
	messages[1].len   = length;
	messages[1].buf   = (uint8_t *)buf;

	packets.msgs  = messages;
	packets.nmsgs = 2;

	tpInt64 ret = i2c_rdwr_with_timeout(data->devfd, &packets, timeout_ms);

	if(ret==0)
		ret=length;
	return ret;
}

tpInt64 TpHardwareI2c::writeReg(tpUInt8 reg, const tpUInt8* buf, size_t length, uint32_t timeout_ms)
{
	TpHardwareI2cData *data = static_cast<TpHardwareI2cData *>(data_);
	if(!data || !data->is_open)
		return -1;
	struct i2c_rdwr_ioctl_data packets;
	struct i2c_msg messages[2];

	messages[0].addr  = data->address;       // 从设备地址
	messages[0].flags = 0;          // 写
	messages[0].len   = 1;
	messages[0].buf   = &reg;

	messages[1].addr  = data->address;
	messages[1].flags = 0;     // 写
	messages[1].len   = length;
	messages[1].buf   = (uint8_t *)buf;

	packets.msgs  = messages;
	packets.nmsgs = 2;

	tpInt64 ret = i2c_rdwr_with_timeout(data->devfd, &packets, timeout_ms);
	if(ret==0)
		ret=length;
	return ret;
}

tpInt64 TpHardwareI2c::writeCmd(tpUInt8 cmd, uint32_t timeout_ms)
{
	TpHardwareI2cData *data = static_cast<TpHardwareI2cData *>(data_);
	if(!data || !data->is_open)
		return -1;
	struct i2c_rdwr_ioctl_data packets;
	struct i2c_msg messages;

	messages.addr  = data->address;       // 从设备地址
	messages.flags = 0;          // 写
	messages.len   = 1;
	messages.buf   = &cmd;

	packets.msgs  = &messages;
	packets.nmsgs = 1;

	int ret = i2c_rdwr_with_timeout(data->devfd, &packets, timeout_ms);
	if(ret==0)
		ret=1;
	return ret;
}

tpInt64 TpHardwareI2c::readData(tpUInt8* buf, size_t length, uint32_t timeout_ms)
{
    TpHardwareI2cData *data = static_cast<TpHardwareI2cData *>(data_);
    if (!data || !data->is_open || !buf || length == 0)
        return -1;

    struct i2c_rdwr_ioctl_data packets;
    struct i2c_msg message;

    message.addr  = data->address;
    message.flags = I2C_M_RD;   // 只读
    message.len   = static_cast<__u16>(length);
    message.buf   = buf;

    packets.msgs  = &message;
    packets.nmsgs = 1;

    tpInt64 ret = i2c_rdwr_with_timeout(data->devfd, &packets, (int)timeout_ms);
	if(ret==0)
		ret=length;
	return ret;
}


//设置从机地址
int TpHardwareI2c::setSlaveAddress(tpUInt8 address)
{
	TpHardwareI2cData *data = static_cast<TpHardwareI2cData *>(data_);
	if(!data || !data->is_open)
		return -1;
	if (ioctl(data->devfd, I2C_SLAVE, address) < 0) {
		fprintf(stderr,"[Error]: Failed to set I2C slave address 0x%02X, %s\n",address,strerror(errno));
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

TpList<tpUInt8> TpHardwareI2c::getSlaveDevices(tpUInt8 bus)
{
	TpList<tpUInt8> list;
	TpString device=TpString(PATH_I2C_DEVICE) + std::to_string(static_cast<int>(bus));
	int file = ::open(device.c_str(), O_RDWR);
	if (file < 0) {
		fprintf(stderr,"[Error]: 总线不存在\n");
	}
	for (tpUInt8 addr = 0x03; addr <= 0x77; addr++)
	{
		if (ioctl(file, I2C_SLAVE, addr) < 0) {
            continue;
        }
		uint8_t dummy;
		if (::read(file, &dummy, 1) < 0) 
		{
			if (errno == EIO) {
				// EIO表示设备无响应，但地址有效
				list.push_back(addr);
			} else if (errno == ENXIO) {
				// ENXIO表示地址无效或设备不存在
				continue;
			}
		}
	}
	return list;
}
    