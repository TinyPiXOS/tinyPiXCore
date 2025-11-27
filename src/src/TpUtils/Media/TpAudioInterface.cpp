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
#include "TpAudioInterface.h"
#include "TpAudioDevice.h"
#include "TpSound.h"

struct TpAudioInfData
{
    TpString name;
    PIAudioConf *audio;
    struct MediaParams *user;
    struct AudioStreamParams *hard_params;
    std::atomic<bool> running;
    std::thread thread_t;

    TpAudioInfData()
    {
        running = false;
        user = nullptr;
        audio = nullptr;
        hard_params = nullptr;
    };
};

TpAudioInterface::TpAudioInterface(const TpString &name)
{
    data_ = new TpAudioInfData();
    TpAudioInfData *audData = static_cast<TpAudioInfData *>(data_);
    MediaParams *user = media_user_config_creat();
    if (user == NULL)
    {
        fprintf(stderr, "[Error]:Failed to creat TpAudioInterface\n");
    }
    audData->user = user;
    TpString usedDev;
    if (name == TpString("default"))
        usedDev = TpSound::getUsedDevice();
    else
        usedDev = name;
    size_t pos = usedDev.find(' '); // 查找第一个空格位置
    if (pos == std::string::npos)   // 无空格时返回整个字符串
        audData->name = usedDev;
    else
        audData->name = usedDev.substr(0, pos); // 截取开头到空格前的部分
    printf("device:%s\n", audData->name.c_str());
}

TpAudioInterface::~TpAudioInterface()
{
    TpAudioInfData *audData = static_cast<TpAudioInfData *>(data_);
    if (!audData)
        return;
    Audio_Set_Close(audData->user);
    if (audData->running == true && audData->thread_t.joinable())
    {
        audData->thread_t.join();
    }
    audData->running = false;
    if (!Audio_State_Is_Exit(audData->user))
        Audio_Device_Close(audData->audio);
    media_user_config_free(audData->user);
}

int TpAudioInterface::threadAudio()
{
    TpAudioInfData *audData = static_cast<TpAudioInfData *>(data_);
    audData->running = true;
    Audio_Play_Main(audData->audio, audData->user);
    return 0;
}

int TpAudioInterface::openDevice()
{
    TpAudioInfData *audData = static_cast<TpAudioInfData *>(data_);
    if (!audData->user)
        return -1;
    if (audData->running)
        return -1;
    PIAudioConf *audio = Audio_Play_Open(audData->name.c_str());
    if (audio == nullptr)
        return -1;
    audData->audio = audio;
    printf("pcm_play %p\n", audData->audio);
    audData->thread_t = std::thread(&TpAudioInterface::threadAudio, this);
    return 0;
}

tpBool TpAudioInterface::isOpen()
{
    TpAudioInfData *audData = static_cast<TpAudioInfData *>(data_);
    return (audData->running == true ? TP_TRUE : TP_FALSE);
}

int TpAudioInterface::closeDevice()
{
    TpAudioInfData *audData = static_cast<TpAudioInfData *>(data_);
    Audio_Set_Close(audData->user);
    while (!Audio_State_Is_Exit(audData->user)) //
        usleep(100);
    return Audio_Device_Close(audData->audio);
}

int TpAudioInterface::setVolume(tpUInt8 volume)
{
    TpAudioInfData *audData = static_cast<TpAudioInfData *>(data_);
    if (!audData->user)
        return -1;
    return Audio_Set_Volume(audData->user->audio_params, volume);
}

int TpAudioInterface::getVolume()
{
    TpAudioInfData *audData = static_cast<TpAudioInfData *>(data_);
    if (!audData->user)
        return -1;
    return Audio_Get_Volume(audData->user->audio_params);
}

int TpAudioInterface::setSpeed(float speed)
{
    TpAudioInfData *audData = static_cast<TpAudioInfData *>(data_);
    if (!audData->user)
        return -1;
    return Audio_Set_Speed(audData->user, speed);
}

int TpAudioInterface::getSpeed()
{
    TpAudioInfData *audData = static_cast<TpAudioInfData *>(data_);
    if (!audData->user)
        return -1;
    return Audio_Get_Speed(audData->user);
}

int TpAudioInterface::setPosition(tpUInt32 position)
{
    TpAudioInfData *audData = static_cast<TpAudioInfData *>(data_);
    if (!audData->user)
        return -1;
    return Audio_Set_Position(audData->user, position);
}

int TpAudioInterface::getPosition()
{
    TpAudioInfData *audData = static_cast<TpAudioInfData *>(data_);
    if (!audData->user)
        return -1;
    if (!audData->audio)
        return -1;
    return Audio_Get_Position(audData->user, audData->audio);
}

tpUInt32 TpAudioInterface::getDuration()
{
    TpAudioInfData *audData = static_cast<TpAudioInfData *>(data_);
    if (!audData->user)
        return 0;
    double val = Audio_Get_Length(audData->user);
    if (val < 0)
        return 0;
    tpUInt32 duration = (tpUInt32)(val + 0.5);
    return duration;
}

int TpAudioInterface::addFile(const TpString &file)
{
    return addFile(file.c_str());
}
int TpAudioInterface::addFile(const char *file)
{
    TpAudioInfData *audData = static_cast<TpAudioInfData *>(data_);
    if (!audData->user)
        return -1;
    return Audio_Add_File(audData->user, file);
}

int TpAudioInterface::deleteFile(const TpString &file)
{
    return deleteFile(file.c_str());
}
int TpAudioInterface::deleteFile(const char *file)
{
    TpAudioInfData *audData = static_cast<TpAudioInfData *>(data_);
    if (!audData->user)
        return -1;
    return Audio_Del_File(audData->user, file);
}

int TpAudioInterface::setFile(const TpString &file)
{
    return setFile(file.c_str());
}
int TpAudioInterface::setFile(const char *file)
{
    TpAudioInfData *audData = static_cast<TpAudioInfData *>(data_);
    if (!audData->user)
        return -1;
    return Audio_Set_Play(audData->user, file);
}

int TpAudioInterface::playStart()
{
    TpAudioInfData *audData = static_cast<TpAudioInfData *>(data_);
    if (!audData->user || !audData->audio)
        return -1;
    return Audio_Set_Start(audData->user, NULL);
}

int TpAudioInterface::playContinue()
{
    TpAudioInfData *audData = static_cast<TpAudioInfData *>(data_);
    if (!audData->user || !audData->audio)
        return -1;
    return Audio_Set_Continue(audData->user);
}

int TpAudioInterface::playPause()
{
    TpAudioInfData *audData = static_cast<TpAudioInfData *>(data_);
    if (!audData->user || !audData->audio)
        return -1;
    return Audio_Set_Suspend(audData->user);
}

int TpAudioInterface::playStop()
{
    TpAudioInfData *audData = static_cast<TpAudioInfData *>(data_);
    if (!audData->user || !audData->audio)
        return -1;
    return Audio_Set_Stop(audData->user);
}

int TpAudioInterface::playNext()
{
    TpAudioInfData *audData = static_cast<TpAudioInfData *>(data_);
    if (!audData->user || !audData->audio)
        return -1;
    return Audio_Play_Next(audData->user);
}

int TpAudioInterface::playLast()
{
    TpAudioInfData *audData = static_cast<TpAudioInfData *>(data_);
    if (!audData->user || !audData->audio)
        return -1;
    return Audio_Play_Last(audData->user);
}

int TpAudioInterface::setSampleParame(SampleRate rate, SampleChannel channel, SampleBits bits)
{
    TpAudioInfData *audData = static_cast<TpAudioInfData *>(data_);
    if (!audData->user)
        return -1;
    if (Audio_Set_Hard_Params(audData->audio, audData->user, (uint32_t)rate, (uint16_t)channel, (uint16_t)bits) < 0)
        ;
    return -1;

    return 0;
}

int TpAudioInterface::playStream(tpUInt8 *data, tpUInt32 frames, tpInt64 offset, tpInt32 delay)
{
    TpAudioInfData *audData = static_cast<TpAudioInfData *>(data_);
    if (!audData->user || !audData->audio)
        return -1;
    if (!audData->hard_params)
    {
        std::cerr << "Uninitialized hardware\n";
        return -1;
    }
    return Audio_Write_Stream(audData->audio, audData->user, audData->hard_params, (uint8_t *)data, (uint32_t)frames, (int)offset, (int)delay);
}

int TpAudioInterface::setNonblock(tpBool nonblock)
{
    TpAudioInfData *audData = static_cast<TpAudioInfData *>(data_);
    if (!audData->user)
        return -1;
    uint8_t nonblock_u8 = (nonblock == true ? 1 : 0);
    return Audio_Set_Nonblock(audData->audio, audData->user, nonblock_u8);
}

int TpAudioInterface::getMaxVolume()
{
    return USER_CONF_VOLUME_MAX;
}

int TpAudioInterface::getMinVolume()
{
    return USER_CONF_VOLUME_MIN;
}

float TpAudioInterface::getMaxSpeed()
{
    return USER_CONF_SPEED_MAX;
}

float TpAudioInterface::getMinSpeed()
{
    return USER_CONF_SPEED_MIN;
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

tpBool TpAudioInterface::isPlayEnd()
{
    TpAudioInfData *audData = static_cast<TpAudioInfData *>(data_);
    if (!audData)
    {
        return TP_TRUE;
    }
    if (Audio_Get_Is_Playing(audData->user) == false)
        return TP_TRUE;
    return TP_FALSE;
}
