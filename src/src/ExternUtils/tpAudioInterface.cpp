/*///------------------------------------------------------------------------------------------------------------------------//
		音频接口
说 明 : 当音频功能以接口调用形式使用的时候调用此接口
日 期 : 2024.1.20

/*///------------------------------------------------------------------------------------------------------------------------//

#include <iostream>
#include <thread>
#include <stdint.h>
#include <sys/types.h>
#include "tpAudioInterface.h"
#include "tpAudioDevice.h"
#include "tpSound.h"


struct tpAudioInfData{
	tpString name;
	PIAudioConf *audio;
	struct MediaParams *user;
	struct AudioStreamParams *hard_params;
	std::atomic<bool> running;
	std::thread thread_t;
	
	tpAudioInfData(){
		running=false;
		user=nullptr;
		audio=nullptr;
		hard_params=nullptr;

	};
};

tpAudioInterface::tpAudioInterface(const tpString &name)
{
	data_ = new tpAudioInfData();
	tpAudioInfData *audData = static_cast<tpAudioInfData *>(data_);
	MediaParams *user=media_user_config_creat();
	if(user==NULL)
	{
		fprintf(stderr,"[Error]:Failed to creat tpAudioInterface\n");
	}
	audData->user=user;
	tpString usedDev;
	if(name == tpString("default"))
		usedDev=tpSound::getUsedDevice();
	else
		usedDev=name;
    size_t pos = usedDev.find(' ');      			// 查找第一个空格位置
	if (pos == std::string::npos) // 无空格时返回整个字符串
        audData->name = usedDev;
	else
   		audData->name = usedDev.substr(0, pos);      // 截取开头到空格前的部分
	printf("device:%s\n",audData->name.c_str());
}

tpAudioInterface::~tpAudioInterface()
{
	tpAudioInfData *audData = static_cast<tpAudioInfData *>(data_);
	if(!audData)
		return ;
	Audio_Set_Close(audData->user);
	if(audData->running==true && audData->thread_t.joinable())
	{
		audData->thread_t.join();
	}
	audData->running=false;
	if(!Audio_State_Is_Exit(audData->user))
		Audio_Device_Close(audData->audio);
	media_user_config_free(audData->user);
}

int tpAudioInterface::threadAudio()
{
	tpAudioInfData *audData = static_cast<tpAudioInfData *>(data_);
	audData->running = true;
	Audio_Play_Main(audData->audio,audData->user);
	return 0;
}

int tpAudioInterface::openDevice()
{
	tpAudioInfData *audData = static_cast<tpAudioInfData *>(data_);
	if(!audData->user)
		return -1;
	if(audData->running)
		return -1;
	PIAudioConf *audio=Audio_Play_Open(audData->name.c_str());
	if(audio==nullptr)
		return -1;
	audData->audio=audio;
	printf("pcm_play %p\n",audData->audio);
	audData->thread_t = std::thread(&tpAudioInterface::threadAudio, this);
	return 0;
}

tpBool tpAudioInterface::isOpen()
{
	tpAudioInfData *audData = static_cast<tpAudioInfData *>(data_);
	return (audData->running==true? TP_TRUE : TP_FALSE);
}

int tpAudioInterface::closeDevice()
{
	tpAudioInfData *audData = static_cast<tpAudioInfData *>(data_);
	Audio_Set_Close(audData->user);
	while(!Audio_State_Is_Exit(audData->user))			//
		usleep(100);
	return Audio_Device_Close(audData->audio);
}

int tpAudioInterface::setVolume(tpUInt8 volume)
{
	tpAudioInfData *audData = static_cast<tpAudioInfData *>(data_);
	if(!audData->user)
		return -1;
	return Audio_Set_Volume(audData->user,volume);
}

int tpAudioInterface::getVolume()
{
	tpAudioInfData *audData = static_cast<tpAudioInfData *>(data_);
	if(!audData->user)
		return -1;
	return Audio_Get_Volume(audData->user);
}

int tpAudioInterface::setSpeed(float speed)
{
	tpAudioInfData *audData = static_cast<tpAudioInfData *>(data_);
	if(!audData->user)
		return -1;
	return Audio_Set_Speed(audData->user,speed);
}

int tpAudioInterface::getSpeed()
{
	tpAudioInfData *audData = static_cast<tpAudioInfData *>(data_);
	if(!audData->user)
		return -1;
	return Audio_Get_Speed(audData->user);
}

int tpAudioInterface::setPosition(tpUInt32 position)
{
	tpAudioInfData *audData = static_cast<tpAudioInfData *>(data_);
	if(!audData->user)
		return -1;
	return Audio_Set_Position(audData->user,position);
}

int tpAudioInterface::getPosition()
{
	tpAudioInfData *audData = static_cast<tpAudioInfData *>(data_);
	if(!audData->user)
		return -1;
	if(!audData->audio)
		return -1;
	return Audio_Get_Position(audData->user,audData->audio);
}

tpUInt32 tpAudioInterface::getDuration()
{
	tpAudioInfData *audData = static_cast<tpAudioInfData *>(data_);
	if(!audData->user)
		return 0;
	double val=Audio_Get_Length(audData->user);
	if(val<0)
		return 0;
	tpUInt32 duration=(tpUInt32)(val+0.5);
	return duration;
}

int tpAudioInterface::addFile(const tpString& file)
{
	return addFile(file.c_str());
}
int tpAudioInterface::addFile(const char *file)
{
	tpAudioInfData *audData = static_cast<tpAudioInfData *>(data_);
	if(!audData->user)
		return -1;
	return Audio_Add_File(audData->user,file);
}

int tpAudioInterface::deleteFile(const tpString& file)
{
	return deleteFile(file.c_str());
}
int tpAudioInterface::deleteFile(const char *file)
{
	tpAudioInfData *audData = static_cast<tpAudioInfData *>(data_);
	if(!audData->user)
		return -1;
	return Audio_Del_File(audData->user,file);
}

int tpAudioInterface::setFile(const tpString& file)
{
	return setFile(file.c_str());
}
int tpAudioInterface::setFile(const char *file)
{
	tpAudioInfData *audData = static_cast<tpAudioInfData *>(data_);
	if(!audData->user)
		return -1;
	return Audio_Set_Play(audData->user,file);
}

int tpAudioInterface::playStart()
{
	tpAudioInfData *audData = static_cast<tpAudioInfData *>(data_);
	if(!audData->user || !audData->audio)
		return -1;
	return Audio_Set_Start(audData->user,NULL);
}

int tpAudioInterface::playContinue()
{
	tpAudioInfData *audData = static_cast<tpAudioInfData *>(data_);
	if(!audData->user || !audData->audio)
		return -1;
	return Audio_Set_Continue(audData->user);
}

int tpAudioInterface::playPause()
{
	tpAudioInfData *audData = static_cast<tpAudioInfData *>(data_);
	if(!audData->user || !audData->audio)
		return -1;
	return Audio_Set_Suspend(audData->user);
}

int tpAudioInterface::playStop()
{
	tpAudioInfData *audData = static_cast<tpAudioInfData *>(data_);
	if(!audData->user || !audData->audio)
		return -1;
	return Audio_Set_Stop(audData->user);
}

int tpAudioInterface::playNext()
{
	tpAudioInfData *audData = static_cast<tpAudioInfData *>(data_);
	if(!audData->user || !audData->audio)
		return -1;
	return Audio_Play_Next(audData->user);
}

int tpAudioInterface::playLast()
{
	tpAudioInfData *audData = static_cast<tpAudioInfData *>(data_);
	if(!audData->user || !audData->audio)
		return -1;
	return Audio_Play_Last(audData->user);
}

int tpAudioInterface::setSampleParame(SampleRate rate, SampleChannel channel, SampleBits bits)
{
	tpAudioInfData *audData = static_cast<tpAudioInfData *>(data_);
	if(!audData->user)
		return -1;
	if(Audio_Set_Hard_Params(audData->audio,audData->user,(uint32_t)rate,(uint16_t)channel,(uint16_t)bits)<0);
		return -1;

	return 0;
}

int tpAudioInterface::playStream(tpUInt8 *data,tpUInt32 frames,tpInt64 offset,tpInt32 delay)
{
	tpAudioInfData *audData = static_cast<tpAudioInfData *>(data_);
	if(!audData->user || !audData->audio)
		return -1;
	if(!audData->hard_params)
	{
		std::cerr << "Uninitialized hardware\n";
		return -1;
	}
	return Audio_Write_Stream(audData->audio, audData->user, audData->hard_params,(uint8_t *)data,(uint32_t)frames,(int)offset,(int)delay);

}

int tpAudioInterface::setNonblock(tpBool nonblock)
{
	tpAudioInfData *audData = static_cast<tpAudioInfData *>(data_);
	if(!audData->user)
		return -1;
	uint8_t nonblock_u8 = (nonblock==true?1:0);
	return Audio_Set_Nonblock(audData->audio,audData->user,nonblock_u8);
}


int tpAudioInterface::getMaxVolume()
{
	return USER_CONF_VOLUME_MAX;
}

int tpAudioInterface::getMinVolume()
{
	return USER_CONF_VOLUME_MIN;
}

float tpAudioInterface::getMaxSpeed()
{
	return USER_CONF_SPEED_MAX;
}

float tpAudioInterface::getMinSpeed()
{
	return USER_CONF_SPEED_MIN;
}


static void callback_get_audio_list(AudioCardDevice *device, void *user_data)
{
	if(!device)
		return ;
	tpList<tpString> *list=static_cast<tpList<tpString> *>(user_data);
	tpString card=tpString(device->hw) + tpString(" ") + tpString(device->name);
	list->push_back(card);
}

tpList<tpString> tpAudioInterface::getDevices()
{
	tpList<tpString> list;
	Audio_Get_Device_List(callback_get_audio_list,&list);
	return list;
}

tpBool tpAudioInterface::isPlayEnd()
{
	tpAudioInfData *audData = static_cast<tpAudioInfData *>(data_);
	if(!audData)
	{
		return TP_TRUE;
	}
	if(Audio_Get_Is_Playing(audData->user)==false)
		return TP_TRUE;
	return TP_FALSE;
}

