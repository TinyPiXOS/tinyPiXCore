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
#include "TpVideoInterface_.h"
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


TpVideoInterface_::TpVideoInterface_(const TpString& audio_name,const TpString& video_name )
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

TpVideoInterface_::~TpVideoInterface_()
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

int TpVideoInterface_::threadVideo()
{
	TpVideoInfData *vidData = static_cast<TpVideoInfData *>(data_);
	
	Video_Play_Main(vidData->user,vidData->a_name.c_str());
	//	printf("play main exit\n");
	return 0;
}

int TpVideoInterface_::openDevice()
{
	TpVideoInfData *vidData = static_cast<TpVideoInfData *>(data_);
	if (!vidData->user)
		return -1;
	if (vidData->running)
		return -1;
	///	printf("device open ok\n");
	vidData->running = true;
	vidData->thread_t = std::thread(&TpVideoInterface_::threadVideo, this);
	//	printf("device open ok\n");
	return 0;
}

tpBool TpVideoInterface_::isOpen()
{
	TpVideoInfData *vidData = static_cast<TpVideoInfData *>(data_);
	return (vidData->running == true ? TP_TRUE : TP_FALSE);
}

int TpVideoInterface_::closeDevice()
{
	TpVideoInfData *vidData = static_cast<TpVideoInfData *>(data_);
	Audio_Set_Close(vidData->user);

	while (!Audio_State_Is_Exit(vidData->user))
		usleep(10);
	return 0;
}



int TpVideoInterface_::setScalingMode(TpVideoScalingType mode)
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

int TpVideoInterface_::setWindowCoordinates(tpInt16 x, tpInt16 y)
{
	TpVideoInfData *vidData = static_cast<TpVideoInfData *>(data_);
	if (!vidData->user)
		return -1;
	return Video_Set_Coordinates(vidData->user, (int16_t)x, (int16_t)y);
}

int TpVideoInterface_::setWindowSize(tpUInt16 width, tpUInt16 height)
{
	TpVideoInfData *vidData = static_cast<TpVideoInfData *>(data_);
	if (!vidData->user)
		return -1;
	return Video_Set_Width_Height(vidData->user, (uint16_t)width, (uint16_t)height);
}


int TpVideoInterface_::staticBridge(uint8_t **data, int *linesize, uint32_t format, void *rawCtx)
{
	// 安全类型转换
	auto *ctx = static_cast<CallbackContext *>(rawCtx);
	// 通过指针调用用户回调（传递原始userdata）
	return ctx->callback ? ctx->callback(data, linesize, format, ctx->userdata) : -1;
}

int TpVideoInterface_::setDisplayFunction(UserCallback callback, void *userdata, TpVideoDecodeType format)
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

int TpVideoInterface_::setDecode(TpVideoDecodeType format)
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