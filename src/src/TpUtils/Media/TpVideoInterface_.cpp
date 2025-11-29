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
    struct MediaVideoInfo *video_params;

	void *context_; //
	TpVideoInfData()
	{
        video_params=nullptr;
		context_ = nullptr;
	};
};


TpVideoInterface_::TpVideoInterface_(const TpString &name):TpMediaInterface()
{
	data_ = new TpVideoInfData();
	TpVideoInfData *vidData = static_cast<TpVideoInfData *>(data_);
	MediaVideoInfo *video=media_video_info_creat();
    if(!video)
    {
        std::cerr << "Failed to creat TpVideoInterface" << std::endl;
    }
	vidData->v_name=name;
	setVideoInterface(video);
    vidData->video_params=video;
}

TpVideoInterface_::~TpVideoInterface_()
{
	TpVideoInfData *vidData = static_cast<TpVideoInfData *>(data_);
	if (!vidData)
		return;
	
	Audio_Set_Video_Callback(vidData->video_params, nullptr, nullptr);

	CallbackContext *context_ = (CallbackContext *)vidData->context_;
	delete context_;

	media_video_info_delete(vidData->video_params);
	delete (vidData);
}


int TpVideoInterface_::setScalingMode(TpVideoScalingType mode)
{
	TpVideoInfData *vidData = static_cast<TpVideoInfData *>(data_);
	if (!vidData->video_params)
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
	return Video_Set_Fill_Mode(vidData->video_params, type);
}

int TpVideoInterface_::setWindowCoordinates(tpInt16 x, tpInt16 y)
{
	TpVideoInfData *vidData = static_cast<TpVideoInfData *>(data_);
	if (!vidData->video_params)
		return -1;
	return Video_Set_Coordinates(vidData->video_params, (int16_t)x, (int16_t)y);
}

int TpVideoInterface_::setWindowSize(tpUInt16 width, tpUInt16 height)
{
	TpVideoInfData *vidData = static_cast<TpVideoInfData *>(data_);
	if (!vidData->video_params)
		return -1;
	return Video_Set_Width_Height(vidData->video_params, (uint16_t)width, (uint16_t)height);
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
	if (!vidData->video_params)
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
		vidData->video_params,
		bridge,								 // 传递函数指针的地址（符合int(**)(...)类型）
		(CallbackContext *)vidData->context_ // 用户数据
	);
	return 0;
}

int TpVideoInterface_::setDecode(TpVideoDecodeType format)
{
	TpVideoInfData *vidData = static_cast<TpVideoInfData *>(data_);
	if (!vidData->video_params)
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

	return Audio_Set_Video_Decode_Format(vidData->video_params, format_video);
}