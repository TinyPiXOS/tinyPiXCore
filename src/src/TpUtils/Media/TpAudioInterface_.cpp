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
#include "TpAudioInterface_.h"
#include "TpAudioDevice.h"
#include "TpSound.h"

struct TpAudioInfData
{
    TpString name;
    PIAudioConf *audio;
    struct AudioStreamParams *hard_params;
    struct MediaAudioInfo *audio_params;
    TpAudioInfData()
    {
        audio = nullptr;
        hard_params = nullptr;
    };
};


TpAudioInterface_::TpAudioInterface_(const TpString &name):TpMediaInterface()
{
  
}

TpAudioInterface_::~TpAudioInterface_()
{

}


static void callback_get_audio_list(AudioCardDevice *device, void *user_data)
{
    if (!device)
        return;
    TpList<TpString> *list = static_cast<TpList<TpString> *>(user_data);
    TpString card = TpString(device->hw) + TpString(" ") + TpString(device->name);
    list->push_back(card);
}

TpList<TpString> TpAudioInterface_::getDevices()
{
    TpList<TpString> list;
    Audio_Get_Device_List(callback_get_audio_list, &list);
    return list;
}



int TpAudioInterface_::setVolume(tpUInt8 volume)
{
    TpAudioInfData *audData = static_cast<TpAudioInfData *>(data_);
    if (!audData->audio_params)
        return -1;
    return Audio_Set_Volume(audData->audio_params, volume);
}

int TpAudioInterface_::getVolume()
{
    TpAudioInfData *audData = static_cast<TpAudioInfData *>(data_);
    if (!audData->audio_params)
        return -1;
    return Audio_Get_Volume(audData->audio_params);
}



