//音频测试程序
#include <iostream>
#include <stdio.h>
#include "tpBluetoothLocal.h"
#include "tpBluetoothDevice.h"
#include "tpBluetoothAudioManager.h"
#include "tpBluetoothSocket.h"
#include "tpBluetoothAddress.h"
#include "tpAudioInterface.h"
#include "tpBluetoothDiscovery.h"
#include "tpBluetoothAudioDevice.h"

//主要用于启动bluealsa守护进程
int example_audio_service()
{
	int err=0;
	tpBluetoothAudioManager audio_service;
	while(!audio_service.isRuning())
	{
		usleep(50000);
		err++;
		if(err>20)
		{
			std::cout << "蓝牙音频服务启动失败\n";
			return 0;
		}
	}
}


int example_play_audio()
{
	example_audio_service();

	tpBluetoothAddress tws_addr(tpString("41:42:AE:49:83:B9"));
//	tpBluetoothDiscovery scan("hci0");
//	tpList<tpBluetoothDevice *>dev_list=scan.getDeviceList();


	tpBluetoothAudioDevice audio_dev("hci0",tws_addr);
	audio_dev.connectToDevice();	
	tpString dev_name=audio_dev.getDevice();
	sleep(3);
	
	tpAudioInterface audio(dev_name);

	audio.setVolume(100);
	audio.addFile("/home/pix/Media/MeiNanBian.mp3");										//添加本地文件
	audio.addFile("/home/pix/Media/phone.wav");	
	audio.addFile("/home/pix/Media/test.mp3");
	if(audio.openDevice()<0)
	{
		printf("open device error\n");
		return -1;
	}
	audio.playStart();
	sleep(10);
	audio.closeDevice();
	sleep(3);
	printf("断开连接\n");
	audio_dev.disconnectDevice();
}






int main()
{
	example_play_audio();
}