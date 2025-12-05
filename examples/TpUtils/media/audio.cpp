#include <iostream>
#include "Media/TpAudioInterface.h"
#include "Media/TpMediaInterface.h"



int example_play_all()
{
	TpList<TpString> cards=TpAudioInterface::getDevices();
	for(auto &it : cards)
	{
		printf("device:%s\n",it.c_str());
	}

	TpString device("hw:0,1 USB Audio");
	TpAudioInterface audio(device);
	TpMediaInterface media;
	media.setAudioOutput(&audio);
	audio.setVolume(100);
//	audio.addFile("https://gstreamer.freedesktop.org/data/media/medium/shoutout.mp3");		//添加网络的文件地址
//	audio.addFile("/home/pix/Media/lvse_48000_L.wav");	
//	audio.addFile("/home/pix/Media/test_danshengdao.mp3");	
//	audio.setFile("/home/pix/Media/phone.wav");										//添加本地文件
	media.setFile("/home/pix/Media/MeiNanBian.mp3");	
	media.addFile("/home/pix/Media/test.mp3");
	media.addFile("/home/pix/Media/test.mp3");
	media.addFile("/home/pix/Media/test.mp3");
	if(media.openDevice()<0)
	{
		printf("open device error\n");
		return -1;
	}
	printf("开始播放\n");
	media.playStart();
	//audio.playNext();
	sleep(1);
	printf("已经开始播放\n");
	for(int i=0;i<5;i++)
	{
		printf("播放进度:%d s\n",media.getPosition());
		sleep(1);
	}
	printf("set postion\n");
	media.setPosition(10);
	for(int i=0;i<5;i++)
	{
		printf("播放进度:%d s\n",media.getPosition());
		sleep(1);
	}
	media.playNext();
	sleep(10);
	media.playNext();
	sleep(10);
	media.playNext();
	sleep(10);
	media.closeDevice();
	return 0;

}



int main()
{
//	example_general();
	example_play_all();
}