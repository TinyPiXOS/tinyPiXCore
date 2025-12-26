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
#include "TpMediaDevice.h"
#include "TpSound.h"
#include "TpMediaPlayer.h"
#include "TpAudioOutput.h"
#include "TpVideoOutput.h"
#include "Log/elog.h"

struct TpMediaInfData
{
    struct MediaUserParams *user;
    std::atomic<bool> running;
    std::thread thread_t;

    TpAudioOutput *audio;
    TpVideoOutput *video;

    void *context_; //
    TpMediaInfData()
    {
        audio=nullptr;
        video=nullptr;
        running = false;
        user = nullptr;
        context_ = nullptr;
    };
};

TpMediaPlayer::TpMediaPlayer()
{
    data_ = new TpMediaInfData();

    TpMediaInfData *medData = static_cast<TpMediaInfData *>(data_);
	if(!medData)
	{
		return ;
	}

    struct MediaUserParams *user = media_user_config_creat();
    if (user == NULL)
    {
        std::cerr << "Failed to creat TpAudioOutput" << std::endl;
		delete(medData);
        return;
    }
    
    media_init(1);
    
    TpLog_Init();

    medData->user = user;
    medData->audio=new TpAudioOutput();
    medData->user->audio_params=(struct MediaAudioInfo*)(medData->audio->getAudioInfo());
    medData->video=new TpVideoOutput();
    medData->user->video_params=(struct MediaVideoInfo*)(medData->video->getVideoInfo());
}

TpMediaPlayer::~TpMediaPlayer()
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
    while (!Media_State_Is_Exit(medData->user))
        usleep(10);

    Media_Set_Video_Callback(medData->user->video_params, nullptr, nullptr);    
    media_user_config_delete(medData->user);
    
    media_deinit(1);
    if(medData->audio)
        delete(medData->audio);
    if(medData->video)  
        delete(medData->video);

    TpLog_Deinit();
    delete (medData);
}

int TpMediaPlayer::threadMedia()
{
    TpMediaInfData *medData = static_cast<TpMediaInfData *>(data_);

    Media_Play_Main(medData->user);
    //	printf("play main exit\n");
    return 0;
}

int TpMediaPlayer::openDevice()
{
    TpMediaInfData *medData = static_cast<TpMediaInfData *>(data_);
    if (!medData->user)
        return -1;
    if (medData->running)
        return -1;
	printf("device open ok\n");
    medData->running = true;
    medData->thread_t = std::thread(&TpMediaPlayer::threadMedia, this);
    
    return 0;
}

tpBool TpMediaPlayer::isOpen()
{
    TpMediaInfData *medData = static_cast<TpMediaInfData *>(data_);
    return (medData->running == true ? TP_TRUE : TP_FALSE);
}

int TpMediaPlayer::closeDevice()
{
    TpMediaInfData *medData = static_cast<TpMediaInfData *>(data_);
    Audio_Set_Close(medData->user);

    while (!Media_State_Is_Exit(medData->user))
        usleep(10);
    return 0;
}

TpAudioOutput *TpMediaPlayer::audioOutput()
{
    TpMediaInfData *medData = static_cast<TpMediaInfData *>(data_);
    return medData->audio;
}


int TpMediaPlayer::setAudioOutput(TpAudioOutput *audio)
{
    TpMediaInfData *medData = static_cast<TpMediaInfData *>(data_);
    if(medData->audio)
        delete(medData->audio);
    medData->audio=nullptr;
    medData->user->audio_params=(struct MediaAudioInfo*)(audio->getAudioInfo());
    return 0;
}

TpVideoOutput *TpMediaPlayer::videoOutput()
{
    TpMediaInfData *medData = static_cast<TpMediaInfData *>(data_);
    return medData->video;
}


int TpMediaPlayer::setVideoOutput(TpVideoOutput *video)
{
    TpMediaInfData *medData = static_cast<TpMediaInfData *>(data_);
    if(medData->video)
        delete(medData->video);
    medData->video=nullptr;
    medData->user->video_params=(struct MediaVideoInfo*)(video->getVideoInfo());
    return 0;
}

int TpMediaPlayer::setSpeed(float speed)
{
    TpMediaInfData *medData = static_cast<TpMediaInfData *>(data_);
    if (!medData->user)
        return -1;
    return Media_Set_Speed(medData->user, speed);
}

float TpMediaPlayer::getSpeed()
{
    TpMediaInfData *medData = static_cast<TpMediaInfData *>(data_);
    if (!medData->user)
        return -1;
    return Media_Get_Speed(medData->user);
}

int TpMediaPlayer::setPosition(tpUInt32 position)
{
    TpMediaInfData *medData = static_cast<TpMediaInfData *>(data_);
    if (!medData->user)
        return -1;
    return Media_Set_Position(medData->user, position);
}

int TpMediaPlayer::getPosition()
{
    TpMediaInfData *medData = static_cast<TpMediaInfData *>(data_);
    if (!medData->user)
        return -1;
	if(medData->user->video_params)
    	return Video_Get_Position(medData->user);
	else	
		return Media_Get_Position(medData->user);
}

tpUInt32 TpMediaPlayer::getDuration()
{
    TpMediaInfData *medData = static_cast<TpMediaInfData *>(data_);
    if (!medData->user)
        return 0;
    double val = Media_Get_Length(medData->user);
    if (val < 0)
        return 0;
    tpUInt32 duration = (tpUInt32)(val + 0.5);
    return duration;
}

int TpMediaPlayer::addFile(const TpString &file)
{
    return addFile(file.c_str());
}
int TpMediaPlayer::addFile(const char *file)
{
    TpMediaInfData *medData = static_cast<TpMediaInfData *>(data_);
    if (!medData->user)
        return -1;
    return Media_Add_File(medData->user, file);
}

int TpMediaPlayer::deleteFile(const TpString &file)
{
    return deleteFile(file.c_str());
}
int TpMediaPlayer::deleteFile(const char *file)
{
    TpMediaInfData *medData = static_cast<TpMediaInfData *>(data_);
    if (!medData->user)
        return -1;
    return Media_Del_File(medData->user, file);
}

int TpMediaPlayer::setFile(const TpString &file)
{
    return setFile(file.c_str());
}
int TpMediaPlayer::setFile(const char *file)
{
    TpMediaInfData *medData = static_cast<TpMediaInfData *>(data_);
    if (!medData->user)
        return -1;
    return Media_Set_Play(medData->user, file);
}

int TpMediaPlayer::playStart()
{
    TpMediaInfData *medData = static_cast<TpMediaInfData *>(data_);
    if (!medData->user)
        return -1;
    return Media_Set_Start(medData->user, NULL);
}

int TpMediaPlayer::playContinue()
{
    TpMediaInfData *medData = static_cast<TpMediaInfData *>(data_);
    if (!medData->user)
        return -1;
    return Media_Set_Continue(medData->user);
}

int TpMediaPlayer::playPause()
{
    TpMediaInfData *medData = static_cast<TpMediaInfData *>(data_);
    if (!medData->user)
        return -1;
    return Media_Set_Suspend(medData->user);
}

int TpMediaPlayer::playStop()
{
    TpMediaInfData *medData = static_cast<TpMediaInfData *>(data_);
    if (!medData->user)
        return -1;
    return Media_Set_Stop(medData->user);
}

int TpMediaPlayer::playNext()
{
    TpMediaInfData *medData = static_cast<TpMediaInfData *>(data_);
    if (!medData->user)
        return -1;
    return Media_Play_Next(medData->user);
}

int TpMediaPlayer::playLast()
{
    TpMediaInfData *medData = static_cast<TpMediaInfData *>(data_);
    if (!medData->user)
        return -1;
    return Media_Play_Last(medData->user);
}

tpBool TpMediaPlayer::isPlayEnd()
{
    TpMediaInfData *medData = static_cast<TpMediaInfData *>(data_);
    if (!medData)
    {
        return TP_TRUE;
    }
    if (Media_Get_Is_Playing(medData->user) == false)
        return TP_TRUE;
    return TP_FALSE;
}

float TpMediaPlayer::getMaxSpeed()
{
    return USER_CONF_SPEED_MAX;
}

float TpMediaPlayer::getMinSpeed()
{
    return USER_CONF_SPEED_MIN;
}

