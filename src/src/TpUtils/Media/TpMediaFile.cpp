
#include "Media/media.h"
#include "TpMediaFile.h"

struct TpMediaFileData{
	TpString name;
	MediaFormatContext *format_ctx;
	uint8_t is_net_file;
	TpMediaFileData(){
		is_net_file=0;
		format_ctx=nullptr;
	};
};

TpMediaFile::TpMediaFile(const TpString &name)
{
	data_ = new TpMediaFileData();
	TpMediaFileData *data = static_cast<TpMediaFileData *>(data_);
	if(!data)
		return ;

	data->name=name;

	if (media_is_network_file(data->name.c_str()))
		data->is_net_file=1;
	media_init(data->is_net_file);

	if(media_get_file_info(data->name.c_str(),&data->format_ctx)<0)
	{
		throw std::runtime_error("Error: 文件不存在或找不到媒体流");
		return ;
	}
	return;
}

TpMediaFile::TpMediaFile(TpMediaFile&& other) noexcept
    : data_(other.data_)
{
    other.data_ = nullptr;
}

TpMediaFile::~TpMediaFile()
{
	TpMediaFileData *data = static_cast<TpMediaFileData *>(data_);
	if(!data)
		return ;
	media_delete_file_info(data->format_ctx);
	media_deinit(data->is_net_file);
}

// 移动赋值运算符声明
TpMediaFile& TpMediaFile::operator=(TpMediaFile&& other) noexcept
{
	TpMediaFileData *data = static_cast<TpMediaFileData *>(data_);
    if (this != &other) {
        delete[] data;
        data_ = other.data_;
        other.data_ = nullptr; // 确保原对象析构时不会释放内存
    }
    return *this;
}
tpBool TpMediaFile::isMediaFile(const TpString &file)
{
	try {
        TpMediaFile mf(file);
    } catch (const std::runtime_error& e) {
		return TP_FALSE;
    } 
	return TP_TRUE;
}


tpInt64 TpMediaFile::getDuration()
{
	TpMediaFileData *data = static_cast<TpMediaFileData *>(data_);
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
tpInt64 TpMediaFile::getBitRate()
{
	TpMediaFileData *data = static_cast<TpMediaFileData *>(data_);
	tpInt64 bit_rate=data->format_ctx->bit_rate;
	return bit_rate;
}

/// @brief 获取文件是音频还是视频
/// @return
TpMediaFile::TpMediaFileType TpMediaFile::getType()
{
	return TP_MEDIA_FILE_NONE;
}

/// @brief 获取文件格式
/// @return (MP3,WAV,MP4....)
int TpMediaFile::getFormat()
{

	return 0;
}

int TpMediaFile::getResolution(tpUInt32 *width, tpUInt32 *height)
{
	TpMediaFileData *data = static_cast<TpMediaFileData *>(data_);
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
tpUInt32 TpMediaFile::getWidth()
{
	TpMediaFileData *data = static_cast<TpMediaFileData *>(data_);
	tpUInt32 w,h;
	getResolution(&w,&h);
	return w;
}

/// @brief 获取视频帧高度
/// @return
tpUInt32 TpMediaFile::getHeight()
{
	TpMediaFileData *data = static_cast<TpMediaFileData *>(data_);
	tpUInt32 w,h;
	getResolution(&w,&h);
	return h;
}

/// @brief 获取视频码率
/// @return
tpUInt32 TpMediaFile::getFrameRate()
{
	return 0;
}
