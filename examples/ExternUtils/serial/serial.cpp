#include <iostream>
#include "TpSerialPort.h"

int main()
{
	TpSerialPort tty("/dev/ttyUSB0");
	if(!tty.open())
	{
		printf("打开失败\n");
	}
	tty.setBaudRate(115200);
	tty.setParity(TpSerialPort::TP_PARITY_NONE);
	tty.setDataBits(TpSerialPort::TP_DATA_BITS_8);
	tty.setStopBits(TpSerialPort::TP_STOP_BITS_1);
	tpUInt8 testData[20]="test data\n";
	while(1)
	{
		int ret=tty.write(testData,9);
		printf("发送数据测试,%d\n",ret);
		sleep(1);
	}
}