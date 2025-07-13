#include <iostream>
#include "tpAudioInterface.h"


int my_sleep(uint16_t s,tpAudioInterface& audio)
{
	for(int i=0;i<s;i++)
	{
		printf("播放进度:%d s\n",audio.getPosition());
		sleep(1);
	}
}


int example_play_all()
{
	tpList<tpString> cards=tpAudioInterface::getDevices();
	for(auto &it : cards)
	{
		printf("device:%s\n",it.c_str());
	}

	tpString device("hw:0,0 USB Audio");
	tpAudioInterface audio(device);
	audio.setVolume(100);
//	audio.addFile("https://gstreamer.freedesktop.org/data/media/medium/shoutout.mp3");		//添加网络的文件地址
//	audio.addFile("/home/pix/Media/lvse_48000_L.wav");	
//	audio.addFile("/home/pix/Media/test_danshengdao.mp3");	
//	audio.setFile("/home/pix/Media/phone.wav");										//添加本地文件
	audio.setFile("/home/pix/Media/MeiNanBian.mp3");	
	audio.addFile("/home/pix/Media/test.mp3");
	audio.addFile("/home/pix/Media/test.mp3");
	audio.addFile("/home/pix/Media/test.mp3");
	if(audio.openDevice()<0)
	{
		printf("open device error\n");
		return -1;
	}
	audio.playStart();
//audio.playNext();
	for(int i=0;i<5;i++)
	{
		printf("播放进度:%d s\n",audio.getPosition());
		sleep(1);
	}
	printf("set postion\n");
	audio.setPosition(10);
	for(int i=0;i<5;i++)
	{
		printf("播放进度:%d s\n",audio.getPosition());
		sleep(1);
	}
	audio.playNext();
	sleep(10);
	audio.playNext();
	sleep(10);
	audio.playNext();
	sleep(10);
	audio.closeDevice();
	return 0;

}


int example_general()
{
	tpAudioInterface audio;
	audio.setVolume(100);
//	audio.addFile("https://gstreamer.freedesktop.org/data/media/medium/shoutout.mp3");		//添加网络的文件地址
	audio.addFile("/home/pix/Media/lvse_48000_L.wav");	
	audio.addFile("/home/pix/Media/test_danshengdao.mp3");	
	audio.addFile("/home/pix/Media/MeiNanBian.mp3");										//添加本地文件
	audio.addFile("/home/pix/Media/phone.wav");	
	audio.addFile("/home/pix/Media/test.mp3");
	if(audio.openDevice()<0)
		printf("open device error\n");
	audio.playStart();
	sleep(10);
	printf("当前进度:%d/%lf\n",audio.getPosition(),audio.getDuration());
	audio.setPosition(0);
	audio.setSpeed(2.0);
	sleep(10);
	audio.setVolume(50);
	audio.setSpeed(1.2);
	sleep(10);
	printf("暂停\n");
	printf("当前进度:%d/%lf\n",audio.getPosition(),audio.getDuration());
	audio.playPause();
	sleep(5);
	printf("继续\n");
	audio.setSpeed(2.0);
	audio.playContinue();
	sleep(5);
	printf("播放下一首测试\n");
	audio.playNext();
	sleep(10);
	printf("播放上一首测试\n");
	audio.playLast();
	sleep(10);
	printf("关闭\n");
	audio.closeDevice();
	return 0;
}

int main()
{
//	example_general();
	example_play_all();
}