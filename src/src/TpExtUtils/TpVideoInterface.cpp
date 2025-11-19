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
#include <libavutil/imgutils.h>
#include "TpVideoInterface.h"
#include "TpAudioDevice.h"
#include "TpVideoDevice.h"
#include "TpSound.h"

struct TpVideoInfData
{
	TpString v_name;
	TpString a_name;
	PIAudioConf *audio;
	struct MediaParams *user;
	std::atomic<bool> running;
	std::thread thread_t;

	void *context_; //
	TpVideoInfData()
	{
		running = false;
		user = nullptr;
		audio = nullptr;
		context_ = nullptr;
	};
};


TpVideoInterface::TpVideoInterface(const TpString& audio_name,const TpString& video_name )
{
	data_ = new TpVideoInfData();
	TpVideoInfData *vidData = static_cast<TpVideoInfData *>(data_);
	MediaParams *user=media_user_config_creat();
	if(user==NULL)
	{
		std::cerr << "Failed to creat TpAudioInterface" << std::endl;
	}

	TpString usedAudioDev;
	if(audio_name == TpString("default"))
		usedAudioDev=TpSound::getUsedDevice();
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

TpVideoInterface::~TpVideoInterface()
{
	TpVideoInfData *vidData = static_cast<TpVideoInfData *>(data_);
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

int TpVideoInterface::threadVideo()
{
	TpVideoInfData *vidData = static_cast<TpVideoInfData *>(data_);
	
	Video_Play_Main(vidData->user,vidData->a_name.c_str());
	//	printf("play main exit\n");
	return 0;
}

int TpVideoInterface::openDevice()
{
	TpVideoInfData *vidData = static_cast<TpVideoInfData *>(data_);
	if (!vidData->user)
		return -1;
	if (vidData->running)
		return -1;
	///	printf("device open ok\n");
	vidData->running = true;
	vidData->thread_t = std::thread(&TpVideoInterface::threadVideo, this);
	//	printf("device open ok\n");
	return 0;
}

tpBool TpVideoInterface::isOpen()
{
	TpVideoInfData *vidData = static_cast<TpVideoInfData *>(data_);
	return (vidData->running == true ? TP_TRUE : TP_FALSE);
}

int TpVideoInterface::closeDevice()
{
	TpVideoInfData *vidData = static_cast<TpVideoInfData *>(data_);
	Audio_Set_Close(vidData->user);

	while (!Audio_State_Is_Exit(vidData->user))
		usleep(10);
	return 0;
}

int TpVideoInterface::setVolume(tpUInt16 volume)
{
	TpVideoInfData *vidData = static_cast<TpVideoInfData *>(data_);
	if (!vidData->user)
		return -1;
	return Audio_Set_Volume(vidData->user, volume);
}

int TpVideoInterface::getVolume()
{
	TpVideoInfData *vidData = static_cast<TpVideoInfData *>(data_);
	if (!vidData->user)
		return -1;
	return Audio_Get_Volume(vidData->user);
}

int TpVideoInterface::setSpeed(float speed)
{
	TpVideoInfData *vidData = static_cast<TpVideoInfData *>(data_);
	if (!vidData->user)
		return -1;
	return Audio_Set_Speed(vidData->user, speed);
}

float TpVideoInterface::getSpeed()
{
	TpVideoInfData *vidData = static_cast<TpVideoInfData *>(data_);
	if (!vidData->user)
		return -1;
	return Audio_Get_Speed(vidData->user);
}

int TpVideoInterface::setPosition(tpUInt32 position)
{
	TpVideoInfData *vidData = static_cast<TpVideoInfData *>(data_);
	if (!vidData->user)
		return -1;
	return Audio_Set_Position(vidData->user, position);
}

int TpVideoInterface::getPosition()
{
	TpVideoInfData *vidData = static_cast<TpVideoInfData *>(data_);
	if (!vidData->user)
		return -1;
	return Video_Get_Position(vidData->user, vidData->audio);
}

tpUInt32 TpVideoInterface::getDuration()
{
	TpVideoInfData *vidData = static_cast<TpVideoInfData *>(data_);
	if (!vidData->user)
		return 0;
	double val = Audio_Get_Length(vidData->user);
	if (val < 0)
		return 0;
	tpUInt32 duration = (tpUInt32)(val + 0.5);
	return duration;
}

int TpVideoInterface::addFile(const TpString &file)
{
	return addFile(file.c_str());
}
int TpVideoInterface::addFile(const char *file)
{
	TpVideoInfData *vidData = static_cast<TpVideoInfData *>(data_);
	if (!vidData->user)
		return -1;
	return Audio_Add_File(vidData->user, file);
}

int TpVideoInterface::deleteFile(const TpString &file)
{
	return deleteFile(file.c_str());
}
int TpVideoInterface::deleteFile(const char *file)
{
	TpVideoInfData *vidData = static_cast<TpVideoInfData *>(data_);
	if (!vidData->user)
		return -1;
	return Audio_Del_File(vidData->user, file);
}

int TpVideoInterface::setFile(const TpString &file)
{
	return setFile(file.c_str());
}
int TpVideoInterface::setFile(const char *file)
{
	TpVideoInfData *vidData = static_cast<TpVideoInfData *>(data_);
	if (!vidData->user)
		return -1;
	return Audio_Set_Play(vidData->user, file);
}

int TpVideoInterface::playStart()
{
	TpVideoInfData *vidData = static_cast<TpVideoInfData *>(data_);
	if (!vidData->user)
		return -1;
	return Audio_Set_Start(vidData->user, NULL);
}

int TpVideoInterface::playContinue()
{
	TpVideoInfData *vidData = static_cast<TpVideoInfData *>(data_);
	if (!vidData->user)
		return -1;
	return Audio_Set_Continue(vidData->user);
}

int TpVideoInterface::playPause()
{
	TpVideoInfData *vidData = static_cast<TpVideoInfData *>(data_);
	if (!vidData->user)
		return -1;
	return Audio_Set_Suspend(vidData->user);
}

int TpVideoInterface::playStop()
{
	TpVideoInfData *vidData = static_cast<TpVideoInfData *>(data_);
	if (!vidData->user)
		return -1;
	return Audio_Set_Stop(vidData->user);
}

int TpVideoInterface::playNext()
{
	TpVideoInfData *vidData = static_cast<TpVideoInfData *>(data_);
	if (!vidData->user)
		return -1;
	return Audio_Play_Next(vidData->user);
}

int TpVideoInterface::playLast()
{
	TpVideoInfData *vidData = static_cast<TpVideoInfData *>(data_);
	if (!vidData->user)
		return -1;
	return Audio_Play_Last(vidData->user);
}

int TpVideoInterface::setScalingMode(TpVideoScalingType mode)
{
	TpVideoInfData *vidData = static_cast<TpVideoInfData *>(data_);
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

int TpVideoInterface::setWindowCoordinates(tpInt16 x, tpInt16 y)
{
	TpVideoInfData *vidData = static_cast<TpVideoInfData *>(data_);
	if (!vidData->user)
		return -1;
	return Video_Set_Coordinates(vidData->user, (int16_t)x, (int16_t)y);
}

int TpVideoInterface::setWindowSize(tpUInt16 width, tpUInt16 height)
{
	TpVideoInfData *vidData = static_cast<TpVideoInfData *>(data_);
	if (!vidData->user)
		return -1;
	return Video_Set_Width_Height(vidData->user, (uint16_t)width, (uint16_t)height);
}

tpBool TpVideoInterface::isPlayEnd()
{
	TpVideoInfData *vidData = static_cast<TpVideoInfData *>(data_);
	if (!vidData)
	{
		return TP_TRUE;
	}
	if (Audio_Get_Is_Playing(vidData->user) == false)
		return TP_TRUE;
	return TP_FALSE;
}

int TpVideoInterface::staticBridge(uint8_t **data, int *linesize, uint32_t format, void *rawCtx)
{
	// 安全类型转换
	auto *ctx = static_cast<CallbackContext *>(rawCtx);
	// 通过指针调用用户回调（传递原始userdata）
	return ctx->callback ? ctx->callback(data, linesize, format, ctx->userdata) : -1;
}

int TpVideoInterface::setDisplayFunction(UserCallback callback, void *userdata, TpVideoDecodeType format)
{
	TpVideoInfData *vidData = static_cast<TpVideoInfData *>(data_);
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

int TpVideoInterface::setDecode(TpVideoDecodeType format)
{
	TpVideoInfData *vidData = static_cast<TpVideoInfData *>(data_);
	if (!vidData->user)
		return -1;

	uint32_t format_video;
	switch (format)
	{
	case TP_VIDEO_DECODE_RGB24:
		format_video = AV_PIX_FMT_RGB24;
		break;
	case TP_VIDEO_DECODE_BGR24:
		format_video = AV_PIX_FMT_BGR24;
		break;
	case TP_VIDEO_DECODE_RGBA8888:
		format_video = AV_PIX_FMT_RGBA;
		break;
	case TP_VIDEO_DECODE_IYUV:
		format_video = AV_PIX_FMT_YUV420P;
		break;
	case TP_VIDEO_DECODE_YUV2:
		format_video = AV_PIX_FMT_YUYV422;
		break;
	case TP_VIDEO_DECODE_UYVY:
		format_video = AV_PIX_FMT_UYVY422;
		break;
	default:
		format_video = AV_PIX_FMT_RGB24;
		break;
	}

	return Audio_Set_Video_Decode_Format(vidData->user, format_video);
}