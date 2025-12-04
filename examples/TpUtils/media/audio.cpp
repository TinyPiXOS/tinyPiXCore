#include <iostream>
#include "Media/TpAudioInterface.h"
#include "Media/TpAudioInterface.h"

int my_sleep(uint16_t s,TpAudioInterface& audio)
{
	for(int i=0;i<s;i++)
	{
		printf("播放进度:%d s\n",audio.getPosition());
		sleep(1);
	}
}


int example_play_all()
{
	TpList<TpString> cards=TpAudioInterface::getDevices();
	for(auto &it : cards)
	{
		printf("device:%s\n",it.c_str());
	}

	TpString device("hw:0,1 USB Audio");
	TpAudioInterface audio(device);
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
	printf("开始播放\n");
	audio.playStart();
//audio.playNext();
sleep(1);
	printf("已经开始播放\n");
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

int example_play_all_new()
{
	TpList<TpString> cards=TpAudioInterface::getDevices();
	for(auto &it : cards)
	{
		printf("device:%s\n",it.c_str());
	}

	TpString device("hw:0,1 USB Audio");

	TpAudioInterface audio(device);
	printf("Set Volume to 100\n");
	audio.setVolume(100);
	printf("Set file\n");
//	audio.addFile("https://gstreamer.freedesktop.org/data/media/medium/shoutout.mp3");		//添加网络的文件地址
//	audio.addFile("/home/pix/Media/lvse_48000_L.wav");	
//	audio.addFile("/home/pix/Media/test_danshengdao.mp3");	
//	audio.setFile("/home/pix/Media/phone.wav");										//添加本地文件
	audio.setFile("/home/pix/Media/MeiNanBian.mp3");
	printf("Add file\n");	
	audio.addFile("/home/pix/Media/test.mp3");
	audio.addFile("/home/pix/Media/test.mp3");
	audio.addFile("/home/pix/Media/test.mp3");
	printf("openDevice\n");
	if(audio.openDevice()<0)
	{
		printf("open device error\n");
		return -1;
	}
	printf("开始播放\n");
	audio.playStart();
	//audio.playNext();
	sleep(1);
	printf("已经开始播放\n");
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



int main()
{
//	example_general();
	example_play_all_new();
}