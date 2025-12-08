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
#include "TpMediaInterface.h"
#include "TpAudioInterface.h"
#include "TpVideoInterface.h"
#include "Log/elog.h"

struct TpMediaInfData
{
    struct MediaUserParams *user;
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

    struct MediaUserParams *user = media_user_config_creat();
    if (user == NULL)
    {
        std::cerr << "Failed to creat TpAudioInterface" << std::endl;
		delete(medData);
        return;
    }
    
    media_init(1);
    elog_init();
    elog_set_fmt(ELOG_LVL_ASSERT, ELOG_FMT_ALL);
    elog_set_fmt(ELOG_LVL_ERROR, ELOG_FMT_LVL | ELOG_FMT_TAG | ELOG_FMT_TIME);
    elog_set_fmt(ELOG_LVL_WARN, ELOG_FMT_LVL | ELOG_FMT_TAG | ELOG_FMT_TIME);
    elog_set_fmt(ELOG_LVL_INFO, ELOG_FMT_LVL | ELOG_FMT_TAG | ELOG_FMT_TIME);
    elog_set_fmt(ELOG_LVL_DEBUG,  ELOG_FMT_LVL | ELOG_FMT_TAG | ELOG_FMT_TIME);
    elog_set_fmt(ELOG_LVL_VERBOSE, ELOG_FMT_ALL & ~ELOG_FMT_FUNC);

    elog_set_text_color_enabled(true);
    elog_start();
    elog_set_filter_lvl(ELOG_LVL_VERBOSE);
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
    while (!Media_State_Is_Exit(medData->user))
        usleep(10);

    Media_Set_Video_Callback(medData->user->video_params, nullptr, nullptr);    
    media_user_config_delete(medData->user);
    
    media_deinit(1);
    elog_deinit();
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

    while (!Media_State_Is_Exit(medData->user))
        usleep(10);
    return 0;
}

int TpMediaInterface::setAudioOutput(TpAudioInterface *audio)
{
    TpMediaInfData *medData = static_cast<TpMediaInfData *>(data_);

    medData->user->audio_params=(struct MediaAudioInfo*)(audio->getAudioInfo());
    return 0;
}

int TpMediaInterface::setVideoOutput(TpVideoInterface *video)
{
    TpMediaInfData *medData = static_cast<TpMediaInfData *>(data_);

    medData->user->video_params=(struct MediaVideoInfo*)(video->getVideoInfo());
    return 0;
}

int TpMediaInterface::setSpeed(float speed)
{
    TpMediaInfData *medData = static_cast<TpMediaInfData *>(data_);
    if (!medData->user)
        return -1;
    return Media_Set_Speed(medData->user, speed);
}

float TpMediaInterface::getSpeed()
{
    TpMediaInfData *medData = static_cast<TpMediaInfData *>(data_);
    if (!medData->user)
        return -1;
    return Media_Get_Speed(medData->user);
}

int TpMediaInterface::setPosition(tpUInt32 position)
{
    TpMediaInfData *medData = static_cast<TpMediaInfData *>(data_);
    if (!medData->user)
        return -1;
    return Media_Set_Position(medData->user, position);
}

int TpMediaInterface::getPosition()
{
    TpMediaInfData *medData = static_cast<TpMediaInfData *>(data_);
    if (!medData->user)
        return -1;
	if(medData->user->video_params)
    	return Video_Get_Position(medData->user);
	else	
		return Media_Get_Position(medData->user);
}

tpUInt32 TpMediaInterface::getDuration()
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

int TpMediaInterface::addFile(const TpString &file)
{
    return addFile(file.c_str());
}
int TpMediaInterface::addFile(const char *file)
{
    TpMediaInfData *medData = static_cast<TpMediaInfData *>(data_);
    if (!medData->user)
        return -1;
    return Media_Add_File(medData->user, file);
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
    return Media_Del_File(medData->user, file);
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
    return Media_Set_Play(medData->user, file);
}

int TpMediaInterface::playStart()
{
    TpMediaInfData *medData = static_cast<TpMediaInfData *>(data_);
    if (!medData->user)
        return -1;
    return Media_Set_Start(medData->user, NULL);
}

int TpMediaInterface::playContinue()
{
    TpMediaInfData *medData = static_cast<TpMediaInfData *>(data_);
    if (!medData->user)
        return -1;
    return Media_Set_Continue(medData->user);
}

int TpMediaInterface::playPause()
{
    TpMediaInfData *medData = static_cast<TpMediaInfData *>(data_);
    if (!medData->user)
        return -1;
    return Media_Set_Suspend(medData->user);
}

int TpMediaInterface::playStop()
{
    TpMediaInfData *medData = static_cast<TpMediaInfData *>(data_);
    if (!medData->user)
        return -1;
    return Media_Set_Stop(medData->user);
}

int TpMediaInterface::playNext()
{
    TpMediaInfData *medData = static_cast<TpMediaInfData *>(data_);
    if (!medData->user)
        return -1;
    return Media_Play_Next(medData->user);
}

int TpMediaInterface::playLast()
{
    TpMediaInfData *medData = static_cast<TpMediaInfData *>(data_);
    if (!medData->user)
        return -1;
    return Media_Play_Last(medData->user);
}

tpBool TpMediaInterface::isPlayEnd()
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

float TpMediaInterface::getMaxSpeed()
{
    return USER_CONF_SPEED_MAX;
}

float TpMediaInterface::getMinSpeed()
{
    return USER_CONF_SPEED_MIN;
}

