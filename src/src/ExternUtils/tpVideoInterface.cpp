/*///------------------------------------------------------------------------------------------------------------------------//
		视频播放接口
说 明 : 此程序是以接口的形式运行所用接口，如需视频以服务形式独立运行需要使用tpVideoServer
日 期 : 2024.1.23

/*/
//------------------------------------------------------------------------------------------------------------------------//

#include <iostream>
#include <thread>
#include <stdint.h>
#include <sys/types.h>
#include <SDL2/SDL.h>
#include "tpVideoInterface.h"
#include "tpAudioDevice.h"
#include "tpVideoDevice.h"
#include "tpSound.h"

struct tpVideoInfData
{
	tpString v_name;
	tpString a_name;
	PIAudioConf *audio;
	struct MediaParams *user;
	std::atomic<bool> running;
	std::thread thread_t;

	void *context_; //
	tpVideoInfData()
	{
		running = false;
		user = nullptr;
		audio = nullptr;
		context_ = nullptr;
	};
};


tpVideoInterface::tpVideoInterface(const tpString& audio_name,const tpString& video_name )
{
	data_ = new tpVideoInfData();
	tpVideoInfData *vidData = static_cast<tpVideoInfData *>(data_);
	MediaParams *user=media_user_config_creat();
	if(user==NULL)
	{
		std::cerr << "Failed to creat tpAudioInterface" << std::endl;
	}

	tpString usedAudioDev;
	if(audio_name == tpString("default"))
		usedAudioDev=tpSound::getUsedDevice();
	else
		usedAudioDev=audio_name;
    size_t pos = usedAudioDev.find(' ');      			// 查找第一个空格位置
	if (pos == std::string::npos) // 无空格时返回整个字符串
         vidData->a_name = audio_name;
	else
   		vidData->a_name = audio_name.substr(0, pos);      // 截取开头到空格前的部分
	vidData->v_name=video_name;
	vidData->user=user;
}

tpVideoInterface::~tpVideoInterface()
{
	tpVideoInfData *vidData = static_cast<tpVideoInfData *>(data_);
	if (!vidData)
		return;
	Audio_Set_Close(vidData->user);
	if (vidData->thread_t.joinable())
	{
		vidData->thread_t.join();
	}
	vidData->running=false;
	while (!Audio_State_Is_Exit(vidData->user))
		usleep(10);

	Audio_Set_Video_Callback(vidData->user, nullptr, nullptr);

	CallbackContext *context_ = (CallbackContext *)vidData->context_;
	delete context_;

	media_user_config_free(vidData->user);
	delete (vidData);
}

int tpVideoInterface::threadVideo()
{
	tpVideoInfData *vidData = static_cast<tpVideoInfData *>(data_);
	
	Video_Play_Main(vidData->user,vidData->a_name.c_str());
	//	printf("play main exit\n");
	return 0;
}

int tpVideoInterface::openDevice()
{
	tpVideoInfData *vidData = static_cast<tpVideoInfData *>(data_);
	if (!vidData->user)
		return -1;
	if (vidData->running)
		return -1;
	///	printf("device open ok\n");
	vidData->running = true;
	vidData->thread_t = std::thread(&tpVideoInterface::threadVideo, this);
	//	printf("device open ok\n");
	return 0;
}

tpBool tpVideoInterface::isOpen()
{
	tpVideoInfData *vidData = static_cast<tpVideoInfData *>(data_);
	return (vidData->running == true ? TP_TRUE : TP_FALSE);
}

int tpVideoInterface::closeDevice()
{
	tpVideoInfData *vidData = static_cast<tpVideoInfData *>(data_);
	Audio_Set_Close(vidData->user);

	while (!Audio_State_Is_Exit(vidData->user))
		usleep(10);
	return 0;
}

int tpVideoInterface::setVolume(tpUInt16 volume)
{
	tpVideoInfData *vidData = static_cast<tpVideoInfData *>(data_);
	if (!vidData->user)
		return -1;
	return Audio_Set_Volume(vidData->user, volume);
}

int tpVideoInterface::getVolume()
{
	tpVideoInfData *vidData = static_cast<tpVideoInfData *>(data_);
	if (!vidData->user)
		return -1;
	return Audio_Get_Volume(vidData->user);
}

int tpVideoInterface::setSpeed(float speed)
{
	tpVideoInfData *vidData = static_cast<tpVideoInfData *>(data_);
	if (!vidData->user)
		return -1;
	return Audio_Set_Speed(vidData->user, speed);
}

float tpVideoInterface::getSpeed()
{
	tpVideoInfData *vidData = static_cast<tpVideoInfData *>(data_);
	if (!vidData->user)
		return -1;
	return Audio_Get_Speed(vidData->user);
}

int tpVideoInterface::setPosition(tpUInt32 position)
{
	tpVideoInfData *vidData = static_cast<tpVideoInfData *>(data_);
	if (!vidData->user)
		return -1;
	return Audio_Set_Position(vidData->user, position);
}

int tpVideoInterface::getPosition()
{
	tpVideoInfData *vidData = static_cast<tpVideoInfData *>(data_);
	if (!vidData->user)
		return -1;
	return Video_Get_Position(vidData->user, vidData->audio);
}

tpUInt32 tpVideoInterface::getDuration()
{
	tpVideoInfData *vidData = static_cast<tpVideoInfData *>(data_);
	if (!vidData->user)
		return 0;
	double val = Audio_Get_Length(vidData->user);
	if (val < 0)
		return 0;
	tpUInt32 duration = (tpUInt32)(val + 0.5);
	return duration;
}

int tpVideoInterface::addFile(const tpString &file)
{
	return addFile(file.c_str());
}
int tpVideoInterface::addFile(const char *file)
{
	tpVideoInfData *vidData = static_cast<tpVideoInfData *>(data_);
	if (!vidData->user)
		return -1;
	return Audio_Add_File(vidData->user, file);
}

int tpVideoInterface::deleteFile(const tpString &file)
{
	return deleteFile(file.c_str());
}
int tpVideoInterface::deleteFile(const char *file)
{
	tpVideoInfData *vidData = static_cast<tpVideoInfData *>(data_);
	if (!vidData->user)
		return -1;
	return Audio_Del_File(vidData->user, file);
}

int tpVideoInterface::setFile(const tpString &file)
{
	return setFile(file.c_str());
}
int tpVideoInterface::setFile(const char *file)
{
	tpVideoInfData *vidData = static_cast<tpVideoInfData *>(data_);
	if (!vidData->user)
		return -1;
	return Audio_Set_Play(vidData->user, file);
}

int tpVideoInterface::playStart()
{
	tpVideoInfData *vidData = static_cast<tpVideoInfData *>(data_);
	if (!vidData->user)
		return -1;
	return Audio_Set_Start(vidData->user, NULL);
}

int tpVideoInterface::playContinue()
{
	tpVideoInfData *vidData = static_cast<tpVideoInfData *>(data_);
	if (!vidData->user)
		return -1;
	return Audio_Set_Continue(vidData->user);
}

int tpVideoInterface::playPause()
{
	tpVideoInfData *vidData = static_cast<tpVideoInfData *>(data_);
	if (!vidData->user)
		return -1;
	return Audio_Set_Suspend(vidData->user);
}

int tpVideoInterface::playStop()
{
	tpVideoInfData *vidData = static_cast<tpVideoInfData *>(data_);
	if (!vidData->user)
		return -1;
	return Audio_Set_Stop(vidData->user);
}

int tpVideoInterface::playNext()
{
	tpVideoInfData *vidData = static_cast<tpVideoInfData *>(data_);
	if (!vidData->user)
		return -1;
	return Audio_Play_Next(vidData->user);
}

int tpVideoInterface::playLast()
{
	tpVideoInfData *vidData = static_cast<tpVideoInfData *>(data_);
	if (!vidData->user)
		return -1;
	return Audio_Play_Last(vidData->user);
}

int tpVideoInterface::setScalingMode(tpVideoScalingType mode)
{
	tpVideoInfData *vidData = static_cast<tpVideoInfData *>(data_);
	if (!vidData->user)
		return -1;
	VideoScalingType type;
	switch (mode)
	{
	case TP_VIDEO_SCALING_STRETCH:
		type = MEDIA_VIDEO_SCALING_STRETCH;
		break;
	case TP_VIDEO_SCALING_FILL:
		type = MEDIA_VIDEO_SCALING_FILL;
		break;
	case TP_VIDEO_SCALING_FIT:
		type = MEDIA_VIDEO_SCALING_FIT;
		break;
	case TP_VIDEO_SCALING_ZOOM:
		type = MEDIA_VIDEO_SCALING_ZOOM;
		break;
	case TP_VIDEO_SCALING_CROP:
		type = MEDIA_VIDEO_SCALING_CROP;
		break;
	case TP_VIDEO_SCALING_LETTERBOX:
		type = MEDIA_VIDEO_SCALING_LETTERBOX;
		break;
	}
	return Video_Set_Fill_Mode(vidData->user, type);
}

int tpVideoInterface::setWindowCoordinates(tpInt16 x, tpInt16 y)
{
	tpVideoInfData *vidData = static_cast<tpVideoInfData *>(data_);
	if (!vidData->user)
		return -1;
	return Video_Set_Coordinates(vidData->user, (int16_t)x, (int16_t)y);
}

int tpVideoInterface::setWindowSize(tpUInt16 width, tpUInt16 height)
{
	tpVideoInfData *vidData = static_cast<tpVideoInfData *>(data_);
	if (!vidData->user)
		return -1;
	return Video_Set_Width_Height(vidData->user, (uint16_t)width, (uint16_t)height);
}

tpBool tpVideoInterface::isPlayEnd()
{
	tpVideoInfData *vidData = static_cast<tpVideoInfData *>(data_);
	if (!vidData)
	{
		return TP_TRUE;
	}
	if (Audio_Get_Is_Playing(vidData->user) == false)
		return TP_TRUE;
	return TP_FALSE;
}

int tpVideoInterface::staticBridge(uint8_t **data, int *linesize, uint32_t format, void *rawCtx)
{
	// 安全类型转换
	auto *ctx = static_cast<CallbackContext *>(rawCtx);
	// 通过指针调用用户回调（传递原始userdata）
	return ctx->callback ? ctx->callback(data, linesize, format, ctx->userdata) : -1;
}

int tpVideoInterface::setDisplayFunction(UserCallback callback, void *userdata, tpVideoDecodeType format)
{
	tpVideoInfData *vidData = static_cast<tpVideoInfData *>(data_);
	if (!vidData->user)
		return -1;

	CallbackContext *context_ = (CallbackContext *)vidData->context_;
	delete context_;
	context_ = nullptr;

	// 创建新上下文（存储回调指针）
	vidData->context_ = new CallbackContext{callback, userdata};

	using CCallback = int (*)(uint8_t **, int *, uint32_t, void *);
	CCallback bridge = &staticBridge; // 获取静态函数地址

	if (format != TP_VIDEO_DECODE_RGB24)
		setDecode(format);

	Audio_Set_Video_Callback(
		vidData->user,
		bridge,								 // 传递函数指针的地址（符合int(**)(...)类型）
		(CallbackContext *)vidData->context_ // 用户数据
	);
	return 0;
}

int tpVideoInterface::setDecode(tpVideoDecodeType format)
{
	tpVideoInfData *vidData = static_cast<tpVideoInfData *>(data_);
	if (!vidData->user)
		return -1;

	uint32_t format_video;
	switch (format)
	{
	case TP_VIDEO_DECODE_RGB24:
		format_video = SDL_PIXELFORMAT_RGB24;
		break;
	case TP_VIDEO_DECODE_BGR24:
		format_video = SDL_PIXELFORMAT_BGR24;
		break;
	case TP_VIDEO_DECODE_RGBA8888:
		format_video = SDL_PIXELFORMAT_RGBA8888;
		break;
	case TP_VIDEO_DECODE_IYUV:
		format_video = SDL_PIXELFORMAT_IYUV;
		break;
	case TP_VIDEO_DECODE_YUV2:
		format_video = SDL_PIXELFORMAT_YUY2;
		break;
	case TP_VIDEO_DECODE_UYVY:
		format_video = SDL_PIXELFORMAT_UYVY;
		break;
	default:
		format_video = SDL_PIXELFORMAT_RGB24;
		break;
	}

	return Audio_Set_Video_Decode_Format(vidData->user, format_video);
}