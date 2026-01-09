#include <iostream>
#include "Media/TpVideoOutput.h"
#include "Media/TpAudioOutput.h"
#include "Media/TpMediaPlayer.h"
#include "Media/TpVideoFrame.h"




auto callback_display = [](uint8_t **data, int *linesize, uint32_t format, void *userdata) -> int
{
    char *user = (char *)userdata;

    uint8_t *r = data[0];
    uint8_t *g = data[1];
    uint8_t *b = data[2];
    printf("userdata:%s\n", user);
    for (int i = 0; i < 20; i++)
    {
        printf("%02x ", data[0][i]);
    }
    printf("\n");

    return 0;
};

auto callback_display_frame = [](const TpVideoFrame &frame) -> int
{
    char *user = (char *)frame.userdata();
    void *userdata = frame.userdata();

    // 注意：如果 data() 方法返回的是 const uint8_t**，可能需要相应的调整
    const uint8_t *r = frame.data()[0];
    const uint8_t *g = frame.data()[1];
    const uint8_t *b = frame.data()[2];
    
    printf("userdata:%s\n", user);
    for (int i = 0; i < 20; i++)
    {
        printf("%02x ", frame.data()[0][i]);
    }
    printf("\n");
    return 0;
};
/*
FrameStats stats;
TpVideoOutput::UserCallback processFrame = [](uint8_t** data, int* linesize, uint32_t fmt, void* ctx)
{
        auto* stats = static_cast<FrameStats*>(ctx);
        stats->frameCount++;
        // 处理视频帧...
        return 0;
    };
*/


int example_video_play()
{
    TpString device("hw:0,1 USB Audio");
    TpVideoOutput video;
    
    TpMediaPlayer media;
    media.setVideoOutput(&video);
    TpAudioOutput *audio=media.audioOutput();
    audio->setVolume(100);
    //media.addFile("/System/data/Videos/demo.mp4");
    media.addFile("/home/pix/Media/demo.mp4");
    // media.addFile("/home/pix/Media/gravity.mpg");
    media.addFile("/home/pix/Media/sintel_trailer-480p.mkv");
    media.addFile("https://gstreamer.freedesktop.org/data/media/large/gravity.mpg");
    media.addFile("https://gstreamer.freedesktop.org/data/media/sintel_trailer-480p.mkv");

    const char *data = "Test User Data";
    std::function<int(uint8_t **, int *, uint32_t, void *)> func = callback_display;
    std::function<int(const TpVideoFrame&)> func_ = callback_display_frame;
    video.setDisplayFunction(func_, (void *)data);	//不需要可以直接使用video.setDisplayFunction(func_);
    media.openDevice();
    /*	video.setWindowSize(1080,720);
        video.setWindowCoordinates(200,200);
        video.setScalingMode(TpVideoOutput::TP_VIDEO_SCALING_FIT);		//推荐格式
    //	video.setScalingMode(TpVideoOutput::TP_VIDEO_SCALING_STRETCH);
        video.playStart();
        video.setSpeed(1.0);
        sleep(2);
        video.setWindowSize(400,600);
        sleep(2);
    //	video.setScalingMode(TpVideoOutput::TP_VIDEO_SCALING_CROP);
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
    //	video.setScalingMode(TpVideoOutput::TP_VIDEO_SCALING_FIT);
    video.setWindowCoordinates(0, 0);
    video.setWindowSize(1024, 768);
    video.setScalingMode(TpVideoOutput::TP_VIDEO_SCALING_FIT); // 推荐格式
    media.playStart();
    media.setSpeed(1.0);
    printf("文件时长%d\n", media.getDuration());
    /*for (int i = 0; i < 10; i++)
    {
        printf("文件时长%d\n", video.getDuration());
        printf("position%d\n", video.getPosition());
        sleep(1);
    }
    printf("文件时长%d\n", video.getDuration());*/
    media.getPosition();
    media.setPosition(20);
    printf("后退成功\n");
	for (int i = 0; i < 50; i++)
    {
        printf("文件时长%d\n", media.getDuration());
        printf("position%d\n", media.getPosition());
        usleep(100000);
    }
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
    media.closeDevice();
    return 0;
}





int main()
{
    //example_timer();
    example_video_play();
    return 0;
}