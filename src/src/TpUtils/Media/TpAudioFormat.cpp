#include <iostream>
#include <thread>
#include <stdint.h>
#include <sys/types.h>
#include "TpMediaDevice.h"
#include "TpAudioFormat.h"

struct TpAudioFmtData{
    TpAudioFormat::SampleRate rate;
    TpAudioFormat::SampleChannel channel;
    TpAudioFormat::SampleBits bits;
    TpString codec_fmt;
    TpAudioFmtData()
    {
    }
};

TpAudioFormat::TpAudioFormat()
{

}

TpAudioFormat::~TpAudioFormat()
{

}

/// @brief 设置采样率
/// @param rate 采样率
void TpAudioFormat::setSampleRate(TpAudioFormat::SampleRate rate)
{
    TpAudioFmtData *rData = static_cast<TpAudioFmtData *>(data_);
    rData->rate=rate;
}
/// @brief 获取采样率
/// @return 
TpAudioFormat::SampleRate TpAudioFormat::getSampleRate()
{
    TpAudioFmtData *rData = static_cast<TpAudioFmtData *>(data_);
    return rData->rate;
}
/// @brief 设置通道数
/// @param channel 通道数
void TpAudioFormat::setChannelCount(TpAudioFormat::SampleChannel channel)
{
    TpAudioFmtData *rData = static_cast<TpAudioFmtData *>(data_);
    rData->channel=channel;
}
/// @brief 获取通道数
/// @return 
TpAudioFormat::SampleChannel TpAudioFormat::getChannelCount()
{
    TpAudioFmtData *rData = static_cast<TpAudioFmtData *>(data_);
    return rData->channel;
}
/// @brief 设置样本大小
/// @param size 样本大小
void TpAudioFormat::setSampleSize(TpAudioFormat::SampleBits size)
{
    TpAudioFmtData *rData = static_cast<TpAudioFmtData *>(data_);
    rData->bits=size;
}
/// @brief 获取编码格式
/// @return 
TpAudioFormat::SampleBits TpAudioFormat::getSampleSize()
{
    TpAudioFmtData *rData = static_cast<TpAudioFmtData *>(data_);
    return rData->bits;
}
/// @brief 设置编码格式
/// @param codec 编码格式
void TpAudioFormat::setCodec(TpString codec)
{
    TpAudioFmtData *rData = static_cast<TpAudioFmtData *>(data_);
    rData->codec_fmt=codec;
}
/// @brief 获取编码格式
/// @return 
TpString TpAudioFormat::getCodec()
{
    TpAudioFmtData *rData = static_cast<TpAudioFmtData *>(data_);
    return rData->codec_fmt;
}