
#include "tpJsonDocument.h"
#include "tpJsonObject.h"
#include "tpFile.h"
#include "tpSound.h"
#include "tpAudioDevice.h"

#define DEVICE_CONFIG_FILE_PATH	"/System/conf/deviceConfig.conf"
#define AUDIO_DEFAULT_CARD	"defaultCard"

struct tpSoundData{
	tpString card;
	tpAudioInterface *audio;
	tpSoundData()
	{
		card="";
		audio=nullptr;
	};
};

tpSound::tpSound(const tpString &name)
{
	data_=new tpSoundData;
	tpSoundData *data = static_cast<tpSoundData *>(data_);
	data->card=name;
	
}

tpSound::tpSound(tpAudioInterface *audio)
{
	data_=new tpSoundData;
	tpSoundData *data = static_cast<tpSoundData *>(data_);
	assert(&audio != nullptr);  // 确保传入对象地址有效
	data->audio=audio;
}

tpSound::~tpSound()
{
	tpSoundData *data = static_cast<tpSoundData *>(data_);

}

static void callback_get_audio_list(AudioCardDevice *device, void *user_data)
{
	if(!device)
		return ;
	tpList<tpString> *list=static_cast<tpList<tpString> *>(user_data);
	tpString card=tpString(device->hw) + tpString(" ") + tpString(device->name);
	list->push_back(card);
}

tpList<tpString>tpSound::getDevices()
{
	tpList<tpString> list;
	Audio_Get_Device_List(callback_get_audio_list,&list);
	return list;
}

//使用中的设备
tpString tpSound::getUsedDevice()
{
	tpString config_file(DEVICE_CONFIG_FILE_PATH);
	tpFile fp_conf(config_file);
	if(!fp_conf.open(tpFile::ReadOnly))
	{
		fprintf(stderr,"[Warning]:The configuration file does not exist, but it does not affect playback\n");
		return "default";
	}

	tpString conf_json=fp_conf.readAll();
	if(conf_json.empty())
	{
		fp_conf.close();
		return "default";
	}
	tpJsonDocument json_doc = tpJsonDocument::fromJson(conf_json);
	tpJsonObject json_root=json_doc.object();
	tpJsonObject json_audio=json_root.value("audio").toObject();
	tpString card=json_audio.value(AUDIO_DEFAULT_CARD).toString();

	fp_conf.close();
	
	return card;
}

tpBool tpSound::setUsedDevice(const tpString& name)
{
	tpString config_file(DEVICE_CONFIG_FILE_PATH);
	tpFile fp_conf(config_file);
	tpJsonObject json_root;
	tpJsonObject json_audio;
	if(fp_conf.open(tpFile::ReadOnly))
	{
		tpString conf_json=fp_conf.readAll();
		tpJsonDocument json_doc_l = tpJsonDocument::fromJson(conf_json);
		json_root=json_doc_l.object();
		json_audio=json_root.value("audio").toObject();
		fp_conf.close();
	}
	

	json_audio.insert(AUDIO_DEFAULT_CARD, name);
	json_root.insert("audio",json_audio);
	tpJsonDocument json_doc_n(json_root);
	tpString json_string=json_doc_n.toJson();

	tpString config_tmp=tpString(DEVICE_CONFIG_FILE_PATH)+tpString(".tmp");
	tpFile fp_tmp(config_tmp);
	if(!fp_tmp.open(tpFile::Append))
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


int tpSound::setAudio(tpAudioInterface *audio)
{
	tpSoundData *data = static_cast<tpSoundData *>(data_);

	data->audio=audio;
}

tpAudioInterface *tpSound::getAudio()
{
	tpSoundData *data = static_cast<tpSoundData *>(data_);
	return data->audio;
}


int tpSound::setVolume(tpUInt8 volume)
{
	tpSoundData *data = static_cast<tpSoundData *>(data_);
	if(!data || !data->audio)
		return -1;
	data->audio->setVolume(volume);
}

int tpSound::getVolume()
{
	tpSoundData *data = static_cast<tpSoundData *>(data_);
	if(!data || !data->audio)
		return 0;
	return data->audio->getVolume();
}

int tpSound::setSystemVolume(tpUInt8 volume)
{
	tpSoundData *data = static_cast<tpSoundData *>(data_);
	return Audio_Set_System_Volume(volume,data->card.c_str());
}

int tpSound::getSystemVolume()
{
	tpSoundData *data = static_cast<tpSoundData *>(data_);
	return Audio_Get_System_Volume(data->card.c_str());
}