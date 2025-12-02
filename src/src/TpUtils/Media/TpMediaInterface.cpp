/*///------------------------------------------------------------------------------------------------------------------------//
        媒体接口
说 明 : 媒体通用接口
日 期 : 2025.8.6

/*/
//------------------------------------------------------------------------------------------------------------------------//

#include <iostream>
#include <thread>
#include <stdint.h>
#include <sys/types.h>
#include <libavutil/imgutils.h>
#include "TpAudioDevice.h"
#include "TpMediaDevice.h"
#include "TpSound.h"
#include "TpMediaInterface.h"
#include "Media/Video/video_play.h"

struct TpMediaInfData
{
    struct MediaParams *user;
    std::atomic<bool> running;
    std::thread thread_t;

    void *context_; //
    TpMediaInfData()
    {
        running = false;
        user = nullptr;
        context_ = nullptr;
    };
};

TpMediaInterface::TpMediaInterface()
{
    data_ = new TpMediaInfData();
    TpMediaInfData *medData = static_cast<TpMediaInfData *>(data_);
	if(!medData)
	{
		return ;
	}

    MediaParams *user = media_user_config_creat();
    if (user == NULL)
    {
        std::cerr << "Failed to creat TpAudioInterface" << std::endl;
		delete(medData);
        return;
    }

    medData->user = user;
}

TpMediaInterface::~TpMediaInterface()
{
    TpMediaInfData *medData = static_cast<TpMediaInfData *>(data_);
    if (!medData)
        return;
    Audio_Set_Close(medData->user);
    if (medData->thread_t.joinable())
    {
        medData->thread_t.join();
    }
    medData->running = false;
    while (!Audio_State_Is_Exit(medData->user))
        usleep(10);

    Media_Set_Video_Callback(medData->user->video_params, nullptr, nullptr);    
    media_user_config_delete(medData->user);
    delete (medData);
}

int TpMediaInterface::threadMedia()
{
    TpMediaInfData *medData = static_cast<TpMediaInfData *>(data_);

    Media_Play_Main(medData->user);
    //	printf("play main exit\n");
    return 0;
}

int TpMediaInterface::openDevice()
{
    TpMediaInfData *medData = static_cast<TpMediaInfData *>(data_);
    if (!medData->user)
        return -1;
    if (medData->running)
        return -1;
	printf("device open ok\n");
    medData->running = true;
    medData->thread_t = std::thread(&TpMediaInterface::threadMedia, this);
    //	printf("device open ok\n");
    return 0;
}

tpBool TpMediaInterface::isOpen()
{
    TpMediaInfData *medData = static_cast<TpMediaInfData *>(data_);
    return (medData->running == true ? TP_TRUE : TP_FALSE);
}

int TpMediaInterface::closeDevice()
{
    TpMediaInfData *medData = static_cast<TpMediaInfData *>(data_);
    Audio_Set_Close(medData->user);

    while (!Audio_State_Is_Exit(medData->user))
        usleep(10);
    return 0;
}

int TpMediaInterface::setSpeed(float speed)
{
    TpMediaInfData *medData = static_cast<TpMediaInfData *>(data_);
    if (!medData->user)
        return -1;
    return Audio_Set_Speed(medData->user, speed);
}

float TpMediaInterface::getSpeed()
{
    TpMediaInfData *medData = static_cast<TpMediaInfData *>(data_);
    if (!medData->user)
        return -1;
    return Audio_Get_Speed(medData->user);
}

int TpMediaInterface::setPosition(tpUInt32 position)
{
    TpMediaInfData *medData = static_cast<TpMediaInfData *>(data_);
    if (!medData->user)
        return -1;
    return Audio_Set_Position(medData->user, position);
}

int TpMediaInterface::getPosition()
{
    TpMediaInfData *medData = static_cast<TpMediaInfData *>(data_);
    if (!medData->user)
        return -1;
	if(medData->user->video_params)
    	return Video_Get_Position(medData->user);
	else	
		return Audio_Get_Position(medData->user);
}

tpUInt32 TpMediaInterface::getDuration()
{
    TpMediaInfData *medData = static_cast<TpMediaInfData *>(data_);
    if (!medData->user)
        return 0;
    double val = Audio_Get_Length(medData->user);
    if (val < 0)
        return 0;
    tpUInt32 duration = (tpUInt32)(val + 0.5);
    return duration;
}

int TpMediaInterface::addFile(const TpString &file)
{
    return addFile(file.c_str());
}
int TpMediaInterface::addFile(const char *file)
{
    TpMediaInfData *medData = static_cast<TpMediaInfData *>(data_);
    if (!medData->user)
        return -1;
    return Audio_Add_File(medData->user, file);
}

int TpMediaInterface::deleteFile(const TpString &file)
{
    return deleteFile(file.c_str());
}
int TpMediaInterface::deleteFile(const char *file)
{
    TpMediaInfData *medData = static_cast<TpMediaInfData *>(data_);
    if (!medData->user)
        return -1;
    return Audio_Del_File(medData->user, file);
}

int TpMediaInterface::setFile(const TpString &file)
{
    return setFile(file.c_str());
}
int TpMediaInterface::setFile(const char *file)
{
    TpMediaInfData *medData = static_cast<TpMediaInfData *>(data_);
    if (!medData->user)
        return -1;
    return Audio_Set_Play(medData->user, file);
}

int TpMediaInterface::playStart()
{
    TpMediaInfData *medData = static_cast<TpMediaInfData *>(data_);
    if (!medData->user)
        return -1;
    return Audio_Set_Start(medData->user, NULL);
}

int TpMediaInterface::playContinue()
{
    TpMediaInfData *medData = static_cast<TpMediaInfData *>(data_);
    if (!medData->user)
        return -1;
    return Audio_Set_Continue(medData->user);
}

int TpMediaInterface::playPause()
{
    TpMediaInfData *medData = static_cast<TpMediaInfData *>(data_);
    if (!medData->user)
        return -1;
    return Audio_Set_Suspend(medData->user);
}

int TpMediaInterface::playStop()
{
    TpMediaInfData *medData = static_cast<TpMediaInfData *>(data_);
    if (!medData->user)
        return -1;
    return Audio_Set_Stop(medData->user);
}

int TpMediaInterface::playNext()
{
    TpMediaInfData *medData = static_cast<TpMediaInfData *>(data_);
    if (!medData->user)
        return -1;
    return Audio_Play_Next(medData->user);
}

int TpMediaInterface::playLast()
{
    TpMediaInfData *medData = static_cast<TpMediaInfData *>(data_);
    if (!medData->user)
        return -1;
    return Audio_Play_Last(medData->user);
}

tpBool TpMediaInterface::isPlayEnd()
{
    TpMediaInfData *medData = static_cast<TpMediaInfData *>(data_);
    if (!medData)
    {
        return TP_TRUE;
    }
    if (Audio_Get_Is_Playing(medData->user) == false)
        return TP_TRUE;
    return TP_FALSE;
}

float TpMediaInterface::getMaxSpeed()
{
    return USER_CONF_SPEED_MAX;
}

float TpMediaInterface::getMinSpeed()
{
    return USER_CONF_SPEED_MIN;
}

int TpMediaInterface::setAudioInterface(void *aud)
{
	TpMediaInfData *medData = static_cast<TpMediaInfData *>(data_);
    if (!medData->user)
        return -1;
	struct MediaAudioInfo *audio_params=(struct MediaAudioInfo *)aud;

	medData->user->audio_params=audio_params;
	return 0;
}

int TpMediaInterface::setVideoInterface(void *vid)
{
	TpMediaInfData *medData = static_cast<TpMediaInfData *>(data_);
    if (!medData->user)
        return -1;
	struct MediaVideoInfo *video_params=(struct MediaVideoInfo *)vid;

	medData->user->video_params=video_params;
	return 0;
}