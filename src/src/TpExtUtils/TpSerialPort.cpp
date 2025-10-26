


#include <fcntl.h>
#include <dirent.h>
#include <sys/ioctl.h>
#include <linux/serial.h>
#include <asm/termbits.h> // 包含 termios2 定义
#include "TpSerialPort.h"
#include "TpSocketNotifier.h"

struct SerialParams {
	int baudRate = 115200;      // 波特率
	int dataBits = 8;          // 数据位 (5,6,7,8)
	int stopBits = 1;          // 停止位 (1,2)
	char parity = 'N';         // 校验位 (N:无校验, O:奇校验, E:偶校验)
	bool flowControl = false;  // 流控 (true:启用, false:禁用)
	int vmin = 1;              // 最小读取字符数
	int vtime = 10;            // 读取超时 (0.1秒单位)
};

struct TpSerialPortData{
	int devfd;
	TpString name;
	tpBool is_open;
	struct termios2 tty;	//串口配置
	TpSerialPort::Parity parity;	//奇偶校验，用于设置1.5停止位的时候，保存设置之前的正常的奇偶校验(因为设置1.5停止会强制修改启用校验)
	TpSerialPort::StopBits stop_bits;	//停止位
	TpSocketNotifier *notifier_read;
	TpSerialPortData(const TpString &name_):name(name_){
		memset(&tty,0,sizeof(struct termios2));
		is_open=TP_FALSE;
		notifier_read=nullptr;
	}
};


TpList<TpString> TpSerialPort::getUsbSerialPorts()
{
	const std::string tty_dir = "/dev/";
	TpList<TpString> list;

	// 打开 PWM 目录
	DIR* dir = opendir(tty_dir.c_str());
	if (!dir) {
		fprintf(stderr,"[Error]: get i2c buss error\n");
		return list;
	}

	// 遍历目录项
	struct dirent* entry;
	while ((entry = readdir(dir)) != nullptr) {
		std::string name(entry->d_name);
		
		// 检查是否是 i2c 设备
		if (name.find("ttyUSB") == 0) {			
			list.push_back(name);
		}
	}

	return list;
}

TpSerialPort::TpSerialPort(const TpString& name)
{
	data_ = new TpSerialPortData(name);

}

TpSerialPort::~TpSerialPort()
{
	TpSerialPortData *data = static_cast<TpSerialPortData *>(data_);
	if(!data)
		return;
	close();
	delete(data);
}


tpBool TpSerialPort::open()
{
	TpSerialPortData *data = static_cast<TpSerialPortData *>(data_);
	data->devfd = ::open(data->name.c_str(), O_RDWR | O_NOCTTY | O_SYNC);

	if (data->devfd < 0) {
		perror("Error opening the serial port");
		return TP_FALSE;
	}
	// 获取串口信息
	if (ioctl(data->devfd, TCGETS2, &data->tty) != 0) {
		throw std::runtime_error("Failed to get serial attributes");
		return TP_FALSE;
	}
	// 1. 输入模式：原始数据，无预处理
	data->tty.c_iflag &= ~(IGNBRK | BRKINT | PARMRK | ISTRIP | INLCR | IGNCR | ICRNL | IXON | IXOFF | IXANY);
    
    // 2. 输出模式：原始输出，无处理
    data->tty.c_oflag &= ~OPOST;
    
    // 3. 本地模式：禁用回显和信号，非规范模式
    data->tty.c_lflag &= ~(ECHO | ECHONL | ICANON | ISIG | IEXTEN);

	setBaudRate(TP_BAUD_RATE_115200);

	data->notifier_read=new TpSocketNotifier(data->devfd, TpSocketNotifier::Read, 
		[this]() { handleRead(); },
		[this]() { handleHangup(); }
	);
	data->is_open=TP_TRUE;
	return TP_TRUE;
}

void TpSerialPort::close()
{
	TpSerialPortData *data = static_cast<TpSerialPortData *>(data_);
	if(!data || !data->is_open)
		return ;
	if (data->notifier_read) {
        delete data->notifier_read; 
		data->notifier_read = nullptr;
    }
	::close(data->devfd);
}

ssize_t TpSerialPort::read(uint8_t* buffer, size_t length)
{
	TpSerialPortData *data = static_cast<TpSerialPortData *>(data_);
	if(!data || !data->is_open)
		return -1;
	ssize_t result = ::read(data->devfd, buffer, length);
    
    if (result < 0) {
		fprintf(stderr,"[Error]: I2C read error:%s\n",strerror(errno));
    }
	return result;
}

ssize_t TpSerialPort::write(const uint8_t* buffer, size_t length)
{
	TpSerialPortData *data = static_cast<TpSerialPortData *>(data_);
	if(!data || !data->is_open)
	{
		return -1;
	}
	ssize_t result = ::write(data->devfd, buffer, length);
    
    if (result < 0) {
		fprintf(stderr,"[Error]: I2C read error:%s\n",strerror(errno));
    }
	return result;
}

int	TpSerialPort::sendBreak(int duration)
{
	return 0;
}
/// @brief 设置波特率
/// @param baudRate 
/// @return 
int	TpSerialPort::setBaudRate(tpUInt32 baudRate)
{
	TpSerialPortData *data = static_cast<TpSerialPortData *>(data_);
	if(!data || !data->is_open)
		return -1;
	struct termios2 *tty=&data->tty;

	tty->c_cflag &= ~CBAUD;
	tty->c_cflag |= BOTHER;
	tty->c_ispeed = (speed_t)baudRate;
	tty->c_ospeed = (speed_t)baudRate;
	if (ioctl(data->devfd, TCSETS2, &tty) != 0) {
		throw std::runtime_error("Failed to set custom baud rate");
		return -1;
	}
	return 0;
}

tpUInt32 TpSerialPort::getBaudRate()
{
	TpSerialPortData *data = static_cast<TpSerialPortData *>(data_);
	return data->tty.c_ispeed;
}
/// @brief 
/// @param set 
/// @return 
int	TpSerialPort::setBreakEnabled(tpBool set)
{
	TpSerialPortData *data = static_cast<TpSerialPortData *>(data_);
	return 0;
}

/// @brief 设置数据位数
/// @param dataBits 
/// @return 
int	TpSerialPort::setDataBits(TpSerialPort::DataBits dataBits)
{
	TpSerialPortData *data = static_cast<TpSerialPortData *>(data_);
	if(!data || !data->is_open)
		return -1;
	data->tty.c_cflag &= ~CSIZE;
	switch (dataBits) 
	{
		case TP_DATA_BITS_5: data->tty.c_cflag |= CS5; break;
		case TP_DATA_BITS_6: data->tty.c_cflag |= CS6; break;
		case TP_DATA_BITS_7: data->tty.c_cflag |= CS7; break;
		case TP_DATA_BITS_8: data->tty.c_cflag |= CS8; break;
		default: throw std::invalid_argument("Invalid data bits value"); return -1;
	}
	if (ioctl(data->devfd, TCSETS2, &data->tty) != 0) {
		throw std::runtime_error("Failed to set custom baud rate");
		return -1;
	}
	return 0;
}
/// @brief 获取数据位
/// @return 
TpSerialPort::DataBits TpSerialPort::getDataBits()
{
	TpSerialPortData *data = static_cast<TpSerialPortData *>(data_);
	switch (data->tty.c_cflag & CSIZE)
	{
		case CS5: return TP_DATA_BITS_5;
		case CS6: return TP_DATA_BITS_6;
		case CS7: return TP_DATA_BITS_7;
		case CS8: return TP_DATA_BITS_8;
	}
	return TP_DATA_BITS_8;
}
/// @brief 设置DTR
/// @param set 
/// @return 
int	TpSerialPort::setDataTerminalReady(tpBool set)
{
	TpSerialPortData *data = static_cast<TpSerialPortData *>(data_);
	if(!data || !data->is_open)
		return -1;
	int status;
	// 获取当前调制解调器控制线的状态
	if (ioctl(data->devfd, TIOCMGET, &status) < 0) {
		throw std::runtime_error("Failed to get modem control lines");
	}
	
	// 设置DTR状态
	if (set) {
		status |= TIOCM_DTR;   // 置位DTR
	} else {
		status &= ~TIOCM_DTR;  // 清除DTR
	}
	
	// 应用新设置
	if (ioctl(data->devfd, TIOCMSET, &status) < 0) {
		throw std::runtime_error("Failed to set DTR");
		return -1;
	}	
	return 0;
}

/// @brief 获取DTR
/// @return 
tpBool TpSerialPort::getDataTerminalReady()
{
	TpSerialPortData *data = static_cast<TpSerialPortData *>(data_);
	int status;
	if (ioctl(data->devfd, TIOCMGET, &status) < 0) {
		throw std::runtime_error("Failed to get modem control lines");
	}
	return (status & TIOCM_DTR) ? TP_TRUE :TP_FALSE;
}

/// @brief 设置RTS
/// @param set 
/// @return 
int	TpSerialPort::setRequestToSend(tpBool set)
{
	TpSerialPortData *data = static_cast<TpSerialPortData *>(data_);
	if(!data || !data->is_open)
		return -1;
	int status;
	if (ioctl(data->devfd, TIOCMGET, &status) < 0) {
		throw std::runtime_error("Failed to get modem control lines");
	}
	
	// 设置RTS状态
	if (set) {
		status |= TIOCM_RTS;   // 置位RTS
	} else {
		status &= ~TIOCM_RTS;  // 清除RTS
	}
	
	if (ioctl(data->devfd, TIOCMSET, &status) < 0) {
		throw std::runtime_error("Failed to set RTS");
		return -1;
	}
	return 0;
}

/// @brief 获取RTS
/// @return 
tpBool TpSerialPort::getRequestToSend()
{
	TpSerialPortData *data = static_cast<TpSerialPortData *>(data_);
	int status;
	if (ioctl(data->devfd, TIOCMGET, &status) < 0) {
		throw std::runtime_error("Failed to get modem control lines");
	}
	return (status & TIOCM_RTS) ? TP_TRUE :TP_FALSE;
}

/// @brief 设置流控
/// @param flowControl 
/// @return 
int	TpSerialPort::setFlowControl(TpSerialPort::FlowControl flowControl)
{
	TpSerialPortData *data = static_cast<TpSerialPortData *>(data_);
	if(!data || !data->is_open)
		return -1;
	switch (flowControl) 
	{
		case TP_FLOW_CONTROL_HARDWARE:
			// 启用硬件流控 (RTS/CTS)
			data->tty.c_cflag |= CRTSCTS;
			// 禁用软件流控
			data->tty.c_iflag &= ~(IXON | IXOFF | IXANY);
			break;
			
		case TP_FLOW_CONTROL_SOFTWARE:
			// 禁用硬件流控
			data->tty.c_cflag &= ~CRTSCTS;
			// 启用软件流控 (XON/XOFF)
			data->tty.c_iflag |= (IXON | IXOFF);
			// 设置XON/XOFF字符 (可选)
			data->tty.c_cc[VSTART] = 0x11; // XON (Ctrl+Q)
			data->tty.c_cc[VSTOP] = 0x13;  // XOFF (Ctrl+S)
			break;
			
		case TP_FLOW_CONTROL_NONE:
		default:
			// 禁用所有流控
			data->tty.c_cflag &= ~CRTSCTS;
			data->tty.c_iflag &= ~(IXON | IXOFF | IXANY);
			break;
	}
	if (ioctl(data->devfd, TCSETS2, &data->tty) != 0) {
		throw std::runtime_error("Failed to set custom baud rate");
		return -1;
	}
	return 0;
}

/// @brief 获取流控
/// @return
TpSerialPort::FlowControl TpSerialPort::getFlowControl()
{
	TpSerialPortData *data = static_cast<TpSerialPortData *>(data_);
	if( (data->tty.c_cflag & CRTSCTS) != 0)
		return TP_FLOW_CONTROL_HARDWARE;
	else if( (data->tty.c_iflag & (IXON | IXOFF)) != 0)
		return TP_FLOW_CONTROL_SOFTWARE;
	else	
		return TP_FLOW_CONTROL_NONE;
}

/// @brief 设置停止位
/// @param stopBits 
/// @return 
int	TpSerialPort::setStopBits(TpSerialPort::StopBits stopBits)
{
	TpSerialPortData *data = static_cast<TpSerialPortData *>(data_);
	if(!data || !data->is_open)
		return -1;
	switch(stopBits)
	{
		case TP_STOP_BITS_1:
			data->tty.c_cflag &= ~CSTOPB;
			setParity(data->parity);
			break;
		case TP_STOP_BITS_1_5:
			data->tty.c_cflag |= CSTOPB;
			data->tty.c_cflag |= CMSPAR;  // 启用 stick parity 模式
			data->tty.c_cflag |= PARENB;  // 必须启用奇偶校验位
			break;
		case TP_STOP_BITS_2:
			data->tty.c_cflag |= CSTOPB;
			setParity(data->parity);
			break;
	}
	if (ioctl(data->devfd, TCSETS2, &data->tty) != 0) {
		throw std::runtime_error("Failed to set custom baud rate");
		return -1;
	}
	data->stop_bits=stopBits;
	return 0;
}

/// @brief 获取停止位
/// @return 
TpSerialPort::StopBits TpSerialPort::getStopBits()
{
	TpSerialPortData *data = static_cast<TpSerialPortData *>(data_);
	return data->stop_bits;
}

/// @brief 设置校验位
/// @param parity 
/// @return 
int	TpSerialPort::setParity(TpSerialPort::Parity parity)
{
	TpSerialPortData *data = static_cast<TpSerialPortData *>(data_);
	if(!data || !data->is_open)
		return -1;
	switch (parity) 
	{
		case TP_PARITY_NONE: // 无校验
			data->tty.c_cflag &= ~PARENB;
			break;
		case TP_PARITY_ODD: // 奇校验
			data->tty.c_cflag |= PARENB;
			data->tty.c_cflag |= PARODD;
			break;
		case TP_PARITY_EVEN: // 偶校验
			data->tty.c_cflag |= PARENB;
			data->tty.c_cflag &= ~PARODD;
			break;
		default:
			throw std::invalid_argument("Invalid parity value");
			return -1;
	}
	if (ioctl(data->devfd, TCSETS2, &data->tty) != 0) {
		throw std::runtime_error("Failed to set custom baud rate");
		return -1;
	}
	data->parity=parity;
	return 0;
}

/// @brief 获取校验位
/// @return 
TpSerialPort::Parity TpSerialPort::getParity()
{
	TpSerialPortData *data = static_cast<TpSerialPortData *>(data_);
	return data->parity;
}

void TpSerialPort::handleRead()
{
	printf("有数据可读\n");
	readyRead.emit();
}

//异常处理
void TpSerialPort::handleHangup() 
{
	std::cerr << "Serial port connection issue detected" << std::endl;
}
