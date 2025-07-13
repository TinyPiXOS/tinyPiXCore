#include <iostream>
#include "tpVideoInterface.h"




auto callback_display = [](uint8_t** data, int* linesize, uint32_t format, void* userdata) -> int
{
	char *user=(char *)userdata;

	uint8_t *r=data[0];
	uint8_t *g=data[1];
	uint8_t *b=data[2];
	printf("userdata:%s\n",user);
	for(int i=0;i<20;i++)
	{
		printf("%02x ",data[0][i]);
	}
	printf("\n");
};
/*
FrameStats stats;
tpVideoInterface::UserCallback processFrame = [](uint8_t** data, int* linesize, uint32_t fmt, void* ctx) 
{
        auto* stats = static_cast<FrameStats*>(ctx);
        stats->frameCount++;
        // 处理视频帧...
        return 0; 
    };
*/

int main()
{
	tpString device("hw:0,1 USB Audio");
	tpVideoInterface video(device);
	video.setVolume(100);
	video.addFile("/home/pix/Media/sintel_trailer-480p.mkv");
	video.addFile("/home/pix/Media/hahaha.mp4");
	video.addFile("/home/pix/Media/gravity.mpg");
	video.addFile("/home/pix/Media/sintel_trailer-480p.mkv");
	video.addFile("https://gstreamer.freedesktop.org/data/media/large/gravity.mpg");
	video.addFile("https://gstreamer.freedesktop.org/data/media/sintel_trailer-480p.mkv");

	char *data="Test User Data";
	std::function<int(uint8_t**, int*, uint32_t, void*)> func = callback_display;
//	video.setDisplayFunction(&func,data);
	video.openDevice();
/*	video.setWindowSize(1080,720);
	video.setWindowCoordinates(200,200);
	video.setScalingMode(tpVideoInterface::TP_VIDEO_SCALING_FIT);		//推荐格式
//	video.setScalingMode(tpVideoInterface::TP_VIDEO_SCALING_STRETCH);
	video.playStart();
	video.setSpeed(1.0);
	sleep(2);
	video.setWindowSize(400,600);
	sleep(2);
//	video.setScalingMode(tpVideoInterface::TP_VIDEO_SCALING_CROP);
	video.setWindowCoordinates(600,600);
	video.setWindowSize(500,500);
	sleep(2);
	video.setWindowCoordinates(200,200);
	video.setWindowSize(800,800);*/
//	sleep(2);
	/*video.setPosition(0);
	sleep(10);
	video.setVolume(50);
	sleep(10);
	printf("暂停播放\n");
	video.playPause();
	sleep(5);
	printf("继续播放\n");
	video.playContinue();
	sleep(5);*/
//	video.setScalingMode(tpVideoInterface::TP_VIDEO_SCALING_FIT);
	video.setWindowCoordinates(0,0);
	video.setWindowSize(1024,768);
	video.setScalingMode(tpVideoInterface::TP_VIDEO_SCALING_FIT);		//推荐格式
	video.playStart();
	video.setSpeed(1.0);
	printf("文件时长%d\n",video.getDuration());
	for(int i=0;i<10;i++)
	{
		printf("文件时长%d\n",video.getDuration());
		printf("position%d\n",video.getPosition());
		sleep(1);
	}
	printf("文件时长%d\n",video.getDuration());
	video.getPosition();
//	printf("播放下一个\n");
//	video.playNext();
	sleep(10);
	/*video.playLast();
	sleep(10);
	video.playNext();
	printf("播放上一个\n");
	sleep(10);
	video.playLast();
	sleep(10);*/
	video.closeDevice();
	return 0;
}