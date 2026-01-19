//音频测试程序
#include <iostream>
#include <stdio.h>
#include "Bluetooth/TpBluetoothLocal.h"
#include "Bluetooth/TpBluetoothDevice.h"
#include "Bluetooth/TpBluetoothAudioManager.h"
#include "Bluetooth/TpBluetoothSocket.h"
#include "Bluetooth/TpBluetoothAddress.h"
#include "Media/TpAudioOutput.h"
#include "Media/TpMediaPlayer.h"
#include "Bluetooth/TpBluetoothDiscovery.h"
#include "Bluetooth/TpBluetoothAudioDevice.h"

//主要用于启动bluealsa守护进程
int example_audio_service()
{
	int err=0;
	TpBluetoothAudioManager audio_service;
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

	TpBluetoothAddress tws_addr(TpString("41:42:AE:49:83:B9"));
//	TpBluetoothDiscovery scan("hci0");
//	TpList<TpBluetoothDevice *>dev_list=scan.getDeviceList();


	TpBluetoothAudioDevice audio_dev("hci0",tws_addr);
	audio_dev.connectToDevice();	
	TpString dev_name=audio_dev.getDevice();
	sleep(3);
	
	TpAudioOutput audio(dev_name);
	TpMediaPlayer player;
	player.setAudioOutput(&audio);
	audio.setVolume(100);

	player.addFile("/home/pix/Media/MeiNanBian.mp3");										//添加本地文件
	player.addFile("/home/pix/Media/phone.wav");	
	player.addFile("/home/pix/Media/test.mp3");
	if(player.openDevice()<0)
	{
		printf("open device error\n");
		return -1;
	}
	player.playStart();
	sleep(10);
	player.closeDevice();
	sleep(3);
	printf("断开连接\n");
	audio_dev.disconnectDevice();
}






int main()
{
	example_play_audio();
}