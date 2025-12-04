
#include "TpJsonDocument.h"
#include "TpJsonObject.h"
#include "TpFile.h"
#include "TpSound.h"
#include "TpMediaDevice.h"

#define DEVICE_CONFIG_FILE_PATH	"/System/conf/deviceConfig.conf"
#define AUDIO_DEFAULT_CARD	"defaultCard"

struct TpSoundData{
	TpString card;
	TpAudioInterface *audio;
	TpSoundData()
	{
		card="";
		audio=nullptr;
	};
};


//获取单个声卡的配置信息
static TpJsonObject getAudioCardAllConfig()
{
	return TpJsonObject();
}

//获取声卡的配置信息中的指定值(object)
static TpJsonObject getAudioCardConfig()
{
	return TpJsonObject();
}

//设置声卡的配置信息中的指定值(object)
static TpJsonObject setAudioCardConfig()
{
	return TpJsonObject();
}


TpSound::TpSound(const TpString &name)
{
	data_=new TpSoundData;
	TpSoundData *data = static_cast<TpSoundData *>(data_);
	data->card=name;
	
}

TpSound::TpSound(TpAudioInterface *audio)
{
	data_=new TpSoundData;
	TpSoundData *data = static_cast<TpSoundData *>(data_);
	assert(&audio != nullptr);  // 确保传入对象地址有效
	data->audio=audio;
}

TpSound::~TpSound()
{
	TpSoundData *data = static_cast<TpSoundData *>(data_);
	if(!data)
		return ;
	delete(data);
}

static void callback_get_audio_list(AudioCardDevice *device, void *user_data)
{
	if(!device)
		return ;
	TpList<TpString> *list=static_cast<TpList<TpString> *>(user_data);
	if(!list)
		return ;
	TpString card=TpString(device->hw) + TpString(" ") + TpString(device->name);
	list->push_back(card);
}

TpList<TpString>TpSound::getDevices()
{
	TpList<TpString> list;
	Audio_Get_Device_List(callback_get_audio_list,&list);
	return list;
}

//使用中的设备
TpString TpSound::getUsedDevice()
{
	TpString config_file(DEVICE_CONFIG_FILE_PATH);
	TpFile fp_conf(config_file);
	if(!fp_conf.open(TpFile::ReadOnly))
	{
		fprintf(stderr,"[Warning]:The configuration file does not exist, but it does not affect playback\n");
		return "default";
	}

	TpString conf_json=fp_conf.readAll();
	if(conf_json.empty())
	{
		fp_conf.close();
		return "default";
	}
	TpJsonDocument json_doc = TpJsonDocument::fromJson(conf_json);
	TpJsonObject json_root=json_doc.object();
	TpJsonObject json_audio=json_root.value("audio").toObject();
	TpString card=json_audio.value(AUDIO_DEFAULT_CARD).toString();

	fp_conf.close();
	
	return card;
}

tpBool TpSound::setUsedDevice(const TpString& name)
{
	TpString config_file(DEVICE_CONFIG_FILE_PATH);
	TpFile fp_conf(config_file);
	TpJsonObject json_root;
	TpJsonObject json_audio;
	if(fp_conf.open(TpFile::ReadOnly))
	{
		TpString conf_json=fp_conf.readAll();
		TpJsonDocument json_doc_l = TpJsonDocument::fromJson(conf_json);
		json_root=json_doc_l.object();
		json_audio=json_root.value("audio").toObject();
		fp_conf.close();
	}
	

	json_audio.insert(AUDIO_DEFAULT_CARD, name);
	json_root.insert("audio",json_audio);
	TpJsonDocument json_doc_n(json_root);
	TpString json_string=json_doc_n.toJson();

	TpString config_tmp=TpString(DEVICE_CONFIG_FILE_PATH)+TpString(".tmp");
	TpFile fp_tmp(config_tmp);
	if(!fp_tmp.open(TpFile::Append))
	{
		fprintf(stderr,"[Error]:The sound card configuration file cannot be opened\n");
		return TP_FALSE;
	}
	if(fp_tmp.write(json_string)<0)
	{
		fprintf(stderr,"[Error]:set defaut sound card error\n");
		return TP_FALSE;
	}

	if(!fp_tmp.rename(config_tmp,config_file))
	{
		fprintf(stderr,"[Error]:rename file error\n");
		return TP_FALSE;
	}
	fp_tmp.close();
	return TP_TRUE;
}


int TpSound::setAudio(TpAudioInterface *audio)
{
	TpSoundData *data = static_cast<TpSoundData *>(data_);
	if(!data)
		return -1;
	data->audio=audio;
	return 0;
}

TpAudioInterface *TpSound::getAudio()
{
	TpSoundData *data = static_cast<TpSoundData *>(data_);
	if(!data)
		return nullptr;
	return data->audio;
}


int TpSound::setVolume(tpUInt8 volume)
{
	TpSoundData *data = static_cast<TpSoundData *>(data_);
	if(!data || !data->audio)
		return -1;
	return data->audio->setVolume(volume);
}

int TpSound::getVolume()
{
	TpSoundData *data = static_cast<TpSoundData *>(data_);
	if(!data || !data->audio)
		return 0;
	return data->audio->getVolume();
}

int TpSound::setSystemVolume(tpUInt8 volume)
{
	TpSoundData *data = static_cast<TpSoundData *>(data_);
	if(!data)
		return -1;
	return Audio_Set_System_Volume(volume,data->card.c_str());
}

int TpSound::getSystemVolume()
{
	TpSoundData *data = static_cast<TpSoundData *>(data_);
	if(!data)
		return -1;
	return Audio_Get_System_Volume(data->card.c_str());
}