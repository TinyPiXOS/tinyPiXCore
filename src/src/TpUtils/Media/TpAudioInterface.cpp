/*///------------------------------------------------------------------------------------------------------------------------//
        音频接口
说 明 : 当音频功能以接口调用形式使用的时候调用此接口
日 期 : 2024.1.20

/*/
//------------------------------------------------------------------------------------------------------------------------//

#include <iostream>
#include <thread>
#include <stdint.h>
#include <sys/types.h>
#include "TpMediaDevice.h"
#include "TpAudioInterface.h"
#include "TpSound.h"

struct TpAudioInfData
{
    TpString name;
    //struct MediaAudioHandle *audio;
    //struct AudioSamplesParams *hard_params;
    struct MediaAudioInfo *audio_params;
    TpAudioInfData()
    {
        //audio = nullptr;
        //hard_params = nullptr;
        audio_params=nullptr;
    };
};


TpAudioInterface::TpAudioInterface(const TpString &name)
{   	
    aData_ = new TpAudioInfData();
    TpAudioInfData *audData = static_cast<TpAudioInfData *>(aData_);
    if(!audData)
        return ;

   	audData->name = TpMediaDevice::getFormatName(name);
    
    MediaAudioInfo *audio=media_audio_info_creat(audData->name.c_str());
    if(!audio)
    {
        std::cerr << "Failed to creat TpVideoInterface" << std::endl;
    }

    struct MediaAudioHandle *pcm_play=Audio_Play_Open(audData->name.c_str());
	if(pcm_play==NULL){
		fprintf(stderr, "[Error]: Audio pcm open error\n");
        media_audio_info_delete(audio);
        return ;
	}
    
    audData->audio_params=audio;
    audData->audio_params->handle=pcm_play;

    printf("[Debug]: TpAudioInterface ok\n");
}


TpAudioInterface::~TpAudioInterface()
{
	TpAudioInfData *audData = static_cast<TpAudioInfData *>(aData_);
	if (!audData)
		return;
    Audio_Device_Close(audData->audio_params->handle);

	media_audio_info_delete(audData->audio_params);
    audData->audio_params=NULL;
	delete (audData);
}


static void callback_get_audio_list(AudioCardDevice *device, void *user_data)
{
    if (!device)
        return;
    TpList<TpString> *list = static_cast<TpList<TpString> *>(user_data);
    TpString card = TpString(device->hw) + TpString(" ") + TpString(device->name);
    list->push_back(card);
}

TpList<TpString> TpAudioInterface::getDevices()
{
    TpList<TpString> list;
    Audio_Get_Device_List(callback_get_audio_list, &list);
    return list;
}



int TpAudioInterface::setVolume(tpUInt8 volume)
{
    TpAudioInfData *audData = static_cast<TpAudioInfData *>(aData_);
    if (!audData->audio_params)
        return -1;
    return Audio_Set_Volume(audData->audio_params, volume);
}

int TpAudioInterface::getVolume()
{
    TpAudioInfData *audData = static_cast<TpAudioInfData *>(aData_);
    if (!audData->audio_params)
        return -1;
    return Audio_Get_Volume(audData->audio_params);
}


/*int TpAudioInterface::setSampleParame(TpAudioFormat::SampleRate rate, TpAudioFormat::SampleChannel channel, TpAudioFormat::SampleBits bits)
{
    TpAudioInfData *audData = static_cast<TpAudioInfData *>(aData_);
    if (!audData->audio_params)
        return -1;
    if (Audio_Set_Hard_Params(audData->audio_params->, audData->user, (uint32_t)rate, (uint16_t)channel, (uint16_t)bits) < 0)
        ;
    return -1;

    return 0;
}

int TpAudioInterface::playStream(tpUInt8 *data, tpUInt32 frames, tpInt64 offset, tpInt32 delay)
{
    TpAudioInfData *audData = static_cast<TpAudioInfData *>(aData_);
    if (!audData->audio_params || !audData->audio)
        return -1;
    if (!audData->hard_params)
    {
        std::cerr << "Uninitialized hardware\n";
        return -1;
    }
    return Audio_Write_Stream(audData->audio, audData->audio_params, audData->hard_params, (uint8_t *)data, (uint32_t)frames, (int)offset, (int)delay);
}

int TpAudioInterface::setNonblock(tpBool nonblock)
{
    TpAudioInfData *audData = static_cast<TpAudioInfData *>(aData_);
    if (!audData->audio_params)
        return -1;
    uint8_t nonblock_u8 = (nonblock == true ? 1 : 0);
    return Audio_Set_Nonblock(audData->audio, audData->audio_params, nonblock_u8);
}*/

int TpAudioInterface::getMaxVolume()
{
    return USER_CONF_VOLUME_MAX;
}

int TpAudioInterface::getMinVolume()
{
    return USER_CONF_VOLUME_MIN;
}

void *TpAudioInterface::getAudioInfo()
{
    TpAudioInfData *audData = static_cast<TpAudioInfData *>(aData_);
    if(!audData)   
        return NULL;
    return audData->audio_params;
}