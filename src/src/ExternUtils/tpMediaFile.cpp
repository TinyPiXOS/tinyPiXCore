
#include "media/media/media.h"
#include "tpMediaFile.h"

struct tpMediaFileData{
	tpString name;
	MediaFormatContext *format_ctx;
	uint8_t is_net_file;
	tpMediaFileData(){
		is_net_file=0;
		format_ctx=nullptr;
	};
};

tpMediaFile::tpMediaFile(const tpString &name)
{
	data_ = new tpMediaFileData();
	tpMediaFileData *data = static_cast<tpMediaFileData *>(data_);
	if(!data)
		return ;

	data->name=name;

	if (media_is_network_file(data->name.c_str()))
		data->is_net_file=1;
	media_init(data->is_net_file);

	if(media_get_file_info(data->name.c_str(),&data->format_ctx)<0)
	{
		return ;
	}
		

	return;
}

tpMediaFile::~tpMediaFile()
{
	tpMediaFileData *data = static_cast<tpMediaFileData *>(data_);
	if(!data)
		return ;
	media_deinit(data->is_net_file);
}


tpInt64 tpMediaFile::getDuration()
{
	tpMediaFileData *data = static_cast<tpMediaFileData *>(data_);
	double duration;
	if (data->format_ctx->duration != AV_NOPTS_VALUE) {
		duration = (double)data->format_ctx->duration / AV_TIME_BASE;
	}
	else 
		return -1;
	return ((tpUInt64)duration );
}

/// @brief 获取比特率(音频)
/// @return kbps
tpInt64 tpMediaFile::getBitRate()
{
	tpMediaFileData *data = static_cast<tpMediaFileData *>(data_);
	tpInt64 bit_rate=data->format_ctx->bit_rate;
	return bit_rate;
}

/// @brief 获取文件是音频还是视频
/// @return
tpMediaFile::tpMediaFileType tpMediaFile::getType()
{
	return TP_MEDIA_FILE_NONE;
}

/// @brief 获取文件格式
/// @return (MP3,WAV,MP4....)
int tpMediaFile::getFormat()
{

	return 0;
}

int tpMediaFile::getResolution(tpUInt32 *width, tpUInt32 *height)
{
	tpMediaFileData *data = static_cast<tpMediaFileData *>(data_);
	if(!data)
		return -1;

	for (int i = 0; i < data->format_ctx->nb_streams; i++) {
        MediaStream *stream = data->format_ctx->streams[i];
        if (stream->codecpar->codec_type == AVMEDIA_TYPE_VIDEO) {
            *width = stream->codecpar->width;
            *height = stream->codecpar->height;
            break;
        }
    }
	return 0;
}

/// @brief 获取视频帧宽度
/// @return
tpUInt32 tpMediaFile::getWidth()
{
	tpMediaFileData *data = static_cast<tpMediaFileData *>(data_);
	tpUInt32 w,h;
	getResolution(&w,&h);
	return w;
}

/// @brief 获取视频帧高度
/// @return
tpUInt32 tpMediaFile::getHeight()
{
	tpMediaFileData *data = static_cast<tpMediaFileData *>(data_);
	tpUInt32 w,h;
	getResolution(&w,&h);
	return h;
}

/// @brief 获取视频码率
/// @return
tpUInt32 tpMediaFile::getFrameRate()
{
	return 0;
}
