#include <iostream>
#include "TpSerialPort.h"
#include "TpApp.h"
#include "TpFixScreen.h"

int main(int32_t argc, char *argv[])
{
	TpApp app(argc, argv);
	TpFixScreen *vScreen = new TpFixScreen();
	vScreen->setBackGroundColor(_RGBA(128, 128, 128, 255));
	vScreen->setVisible(true); // vScreen setvisible will be update display weekly
	app.bindVScreen(vScreen);

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
	int ret=tty.write(testData,9);
		printf("发送数据测试,%d\n",ret);

	connect(&tty, readyRead, [&]()
        {
			uint8_t buf[10240];
			memset(buf,0,10240);
			int len=tty.read((uint8_t *)buf,10240);
			printf("recv[%d]byte: %s\n",len, buf);
			
		});

	
	/*while(1)
	{

		int ret=tty.write(testData,9);
		printf("发送数据测试,%d\n",ret);
		sleep(1);
	}*/

	app.run();
}