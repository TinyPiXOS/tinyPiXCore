#include <iostream>
#include "TpRecordInterface.h"


int main()
{
	TpString card("hw:0,0");	//hw:0,0
	TpRecordInterface record(card);

	record.openDevice();

	sleep(1);
	while(record.recordStart("/home/pix/record.wav")<0)		
	{
		usleep(1000);
	}
	printf("开始录音\n");
	sleep(10);
//	printf("录音暂停\n");
//	record.recordPause();
//	sleep(3);
//	printf("录音继续\n");
//	record.recordContinue();
//	sleep(3);
	printf("录音结束\n");
	record.recordStop();
	record.closeDevice();
	return 0;
}