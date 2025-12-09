#ifndef __TP_AUDIO_FORMAT_H
#define __TP_AUDIO_FORMAT_H

#include <TpCore.h>

TP_DEF_VOID_TYPE_VAR(ItpAudioFmtData);

class TpAudioFormat
{
public:
    enum SampleRate{
		TP_AUDIO_RATE_11025 = 11025,
		TP_AUDIO_RATE_22050 = 22050,
		TP_AUDIO_RATE_44100 = 44100,
		TP_AUDIO_RATE_47250 = 47250,

		TP_AUDIO_RATE_8000 = 8000,
		TP_AUDIO_RATE_24000 = 24000,
		TP_AUDIO_RATE_32000 = 32000,
		TP_AUDIO_RATE_48000 = 48000,
		TP_AUDIO_RATE_96000 = 96000,
		TP_AUDIO_RATE_192000 = 192000,
	};

	enum SampleChannel{
		TP_AUDIO_CHANNEL_1	= 8,
		TP_AUDIO_CHANNEL_2  = 16,
		TP_AUDIO_CHANNEL_2_1 = 24,
		TP_AUDIO_CHANNEL_5_1 = 48,
		TP_AUDIO_CHANNEL_7_1 = 64,
	};

	enum SampleBits{
		TP_AUDIO_BITS_8	= 8,
		TP_AUDIO_BITS_16 = 16,
		TP_AUDIO_BITS_24 = 24,
		TP_AUDIO_BITS_32 = 32,
	};
public:
    TpAudioFormat();
    ~TpAudioFormat();
public:
    /// @brief 设置采样率
    /// @param rate 采样率
    void setSampleRate(SampleRate rate);
    /// @brief 获取采样率
    /// @return 
    SampleRate getSampleRate();
    /// @brief 设置通道数
    /// @param channel 通道数
    void setChannelCount(SampleChannel channel);
    /// @brief 获取通道数
    /// @return 
    SampleChannel getChannelCount();
    /// @brief 设置样本大小
    /// @param size 样本大小
    void setSampleSize(SampleBits size);
    /// @brief 获取编码格式
    /// @return 
    SampleBits getSampleSize();
    /// @brief 设置编码格式
    /// @param codec 编码格式
    void setCodec(TpString codec);
    /// @brief 获取编码格式
    /// @return 
    TpString getCodec();




private:
    ItpAudioFmtData *data_;
};


#endif