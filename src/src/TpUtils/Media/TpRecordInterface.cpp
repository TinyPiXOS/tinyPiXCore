/*///------------------------------------------------------------------------------------------------------------------------//
        录音接口
说 明 : 当录音功能以接口调用形式使用的时候调用此接口
日 期 : 2024.1.25

/*/
//------------------------------------------------------------------------------------------------------------------------//

#include <iostream>
#include <thread>
#include <stdint.h>
#include "TpRecordInterface.h"
#include "TpMediaDevice.h"
#include "TpRecordDevice.h"
#include "TpSound.h"
struct TpRecordInfData
{
    struct MediaAudioHandle *record;
    struct MediaUserParams *user;
    TpString name;
    std::atomic<bool> running;
    std::thread thread_t;
    TpRecordInfData()
    {
        running = false;
        user = nullptr;
        record = nullptr;
    };
};


TpRecordInterface::TpRecordInterface(const TpString &device)
{
    data_ = new TpRecordInfData();
    TpRecordInfData *recData = static_cast<TpRecordInfData *>(data_);
    struct MediaUserParams *user = media_user_config_creat();
    if (user == NULL)
    {
        std::cerr << "Failed to creat TpAudioInterface" << std::endl;
        delete(recData);
        return ;
    }

    recData->name = TpMediaDevice::getFormatName(device);
    
    user->audio_params=media_audio_info_creat(recData->name.c_str());
    if(!user->audio_params)
    {
        perror("audio_params creat error\n");
        media_user_config_delete(user);
        delete(recData);
        return ;
    }
    recData->user = user;
}

TpRecordInterface::~TpRecordInterface()
{
    TpRecordInfData *recData = static_cast<TpRecordInfData *>(data_);
    if (recData->thread_t.joinable())
    {
        recData->thread_t.join();
    }
    if (!Media_State_Is_Exit(recData->user))
        Audio_Device_Close(recData->record);
    media_audio_info_delete(recData->user->audio_params);
    media_user_config_delete(recData->user);
}

int TpRecordInterface::threadRecord()
{
    TpRecordInfData *recData = static_cast<TpRecordInfData *>(data_);
    Audio_Record_Main(recData->record, recData->user);
    return 0;
}

int TpRecordInterface::openDevice()
{
    TpRecordInfData *recData = static_cast<TpRecordInfData *>(data_);
    if (!recData->user)
        return -1;
    if (recData->running)
        return -1;
    struct MediaAudioHandle *hard = Audio_Record_Open(recData->name.c_str()); // recData->name.c_str()
    if (hard == NULL)
        return -1;
    recData->record = hard;
    recData->running = true;
    recData->thread_t = std::thread(&TpRecordInterface::threadRecord, this);
    printf("device open ok\n");
    return 0;
}

tpBool TpRecordInterface::isOpen()
{
    TpRecordInfData *recData = static_cast<TpRecordInfData *>(data_);
    return (recData->running == true ? TP_TRUE : TP_FALSE);
}

int TpRecordInterface::closeDevice()
{
    TpRecordInfData *recData = static_cast<TpRecordInfData *>(data_);
    Audio_Set_Close(recData->user);
    while (!Media_State_Is_Exit(recData->user)) //
        usleep(10);
    return Audio_Device_Close(recData->record);
}

int TpRecordInterface::recordStart(TpString &file, AudioType type, AudioBitRate rate)
{
    return recordStart(file.c_str(), type, rate);
}

int TpRecordInterface::recordStart(const char *file, AudioType type, AudioBitRate rate)
{
    TpRecordInfData *recData = static_cast<TpRecordInfData *>(data_);
    if (!recData->user)
        return -1;
    if (Record_Set_Start(recData->user, file) < 0)
    {
        std::cerr << "录音开始错误\n";
        return -1;
    }
    return 0;
}

int TpRecordInterface::recordContinue()
{
    TpRecordInfData *recData = static_cast<TpRecordInfData *>(data_);
    if (!recData->user)
        return -1;
    return Media_Set_Continue(recData->user);
}

int TpRecordInterface::recordPause()
{
    TpRecordInfData *recData = static_cast<TpRecordInfData *>(data_);
    if (!recData->user)
        return -1;
    return Media_Set_Suspend(recData->user);
}


int TpRecordInterface::recordStop()
{
    TpRecordInfData *recData = static_cast<TpRecordInfData *>(data_);
    if (!recData->user)
        return -1;
    return Media_Set_Stop(recData->user);
}
