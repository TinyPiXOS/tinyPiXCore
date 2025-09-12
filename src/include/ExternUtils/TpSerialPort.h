#ifndef __TP_SERIAL_PORT_H
#define __TP_SERIAL_PORT_H

#include "TpUtils.h"
#include "TpHardwareDevice.h"
#include "TpSignalSlot.h"

TP_DEF_VOID_TYPE_VAR(ItpSerialPortData);

class TpSerialPort :  public TpHardwareDevice
{	
public:
	enum DataBits{
		TP_DATA_BITS_5=5,
		TP_DATA_BITS_6=6,
		TP_DATA_BITS_7=7,
		TP_DATA_BITS_8=8,
	};
	enum Parity{
		TP_PARITY_NONE,
		TP_PARITY_ODD,
		TP_PARITY_EVEN,
	};
	enum StopBits{
		TP_STOP_BITS_1,
		TP_STOP_BITS_1_5,
		TP_STOP_BITS_2
	};
	enum FlowControl{
		TP_FLOW_CONTROL_NONE,
		TP_FLOW_CONTROL_HARDWARE,
		TP_FLOW_CONTROL_SOFTWARE,
	};
	enum BaudRate{
		TP_BAUD_RATE_1200=1200,
		TP_BAUD_RATE_2400=2400,
		TP_BAUD_RATE_4800=4800,
		TP_BAUD_RATE_9600=9600,
		TP_BAUD_RATE_19200=19200,
		TP_BAUD_RATE_38400=38400,
		TP_BAUD_RATE_57600=57600,
		TP_BAUD_RATE_76800=76800,
		TP_BAUD_RATE_115200=115200,
	};
public:
	TpSerialPort(const TpString& name);
	~TpSerialPort();
public:
	/// @brief 打开设备
	/// @return 
	tpBool open();

	/// @brief 关闭设备
	void close();

	/// @brief 
	/// @param buffer 
	/// @param length 
	/// @return 
	ssize_t read(uint8_t* buffer, size_t length);

	/// @brief 
	/// @param data 
	/// @param length 
	/// @return 
	ssize_t write(const uint8_t* data, size_t length);

	int	sendBreak(int duration = 0);
	/// @brief 设置波特率
	/// @param baudRate 
	/// @return 
	int	setBaudRate(tpUInt32 baudRate);

	tpUInt32 getBaudRate();
	/// @brief 
	/// @param set 
	/// @return 
	int	setBreakEnabled(tpBool set = TP_TRUE);

	/// @brief 设置数据位数
	/// @param dataBits 
	/// @return 
	int	setDataBits(TpSerialPort::DataBits dataBits);
	/// @brief 获取数据位
	/// @return 
	TpSerialPort::DataBits getDataBits();
	/// @brief 设置DTR
	/// @param set 
	/// @return 
	int	setDataTerminalReady(tpBool set);

	/// @brief 获取DTR
	/// @return 
	tpBool getDataTerminalReady();

	/// @brief 设置RTS
	/// @param set 
	/// @return 
	int	setRequestToSend(tpBool set);

	/// @brief 获取RTS
	/// @return 
	tpBool getRequestToSend();

	/// @brief 设置流控
	/// @param flowControl 
	/// @return 
	int	setFlowControl(TpSerialPort::FlowControl flowControl);

	/// @brief 获取流控
	/// @return 
	TpSerialPort::FlowControl getFlowControl();
	/// @brief 设置停止位
	/// @param stopBits 
	/// @return 
	int	setStopBits(TpSerialPort::StopBits stopBits);

	/// @brief 获取停止位
	/// @return 
	TpSerialPort::StopBits getStopBits();

	/// @brief 设置校验位
	/// @param parity 
	/// @return 
	int	setParity(TpSerialPort::Parity parity);

	/// @brief 获取校验位
	/// @return 
	TpSerialPort::Parity getParity();

public
signals:
    declare_signal(readyRead);

private:
	void handleRead();
	void handleHangup();
private:
	ItpSerialPortData *data_;
};




#endif