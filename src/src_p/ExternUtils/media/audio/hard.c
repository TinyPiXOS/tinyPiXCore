/*///------------------------------------------------------------------------------------------------------------------------//
		硬件相关的接口
说 明 : 
日 期 : 2024.11.26

/*///------------------------------------------------------------------------------------------------------------------------//
#ifdef __cplusplus
extern "C" {
#endif
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <stdint.h>
#include <stdlib.h>
#include <alsa/asoundlib.h>
#include "tpAudioDevice.h"
#include "hard.h"

struct MediaVolumeHard{
	snd_mixer_t *mixer_handle;     // 混音器句柄
	snd_mixer_selem_id_t *selem_id; // 混音器元素ID
	snd_mixer_elem_t *elem;         // 混音器元素
	long min, max;                  // 音量范围
};


//
static int open_vloume_hard(const char *name,struct MediaVolumeHard *mixer)
{
	snd_mixer_selem_id_t *selem_id; // 混音器元素ID
	snd_mixer_elem_t *elem;         // 混音器元素
	long min=0, max=0;                  // 音量范围
	int err;
	printf("open device:%s\n",name);
	// 打开混音器
	if ((err = snd_mixer_open(&mixer->mixer_handle, 0)) < 0) {
		fprintf(stderr, "Error opening mixer: %s\n", snd_strerror(err));
		return -1;
	}

	// 附加到默认的声卡
	if(name==NULL)
		name="default";
	if ((err = snd_mixer_attach(mixer->mixer_handle, name)) < 0) {
		fprintf(stderr, "Error attaching to default device: %s\n", snd_strerror(err));
		snd_mixer_close(mixer->mixer_handle);
		return -1;
	}

	// 注册混音器
	if ((err = snd_mixer_selem_register(mixer->mixer_handle, NULL, NULL)) < 0) {
		fprintf(stderr, "Error registering mixer: %s\n", snd_strerror(err));
		snd_mixer_close(mixer->mixer_handle);
		return -1;
	}

	// 加载混音器
	if ((err = snd_mixer_load(mixer->mixer_handle)) < 0) {
		fprintf(stderr, "Error loading mixer: %s\n", snd_strerror(err));
		snd_mixer_close(mixer->mixer_handle);
		return -1;
	}

	// 查找混音器元素
	for (elem = snd_mixer_first_elem(mixer->mixer_handle);elem;elem = snd_mixer_elem_next(elem)) 
	{
		if (snd_mixer_selem_is_active(elem) &&snd_mixer_selem_has_playback_volume(elem)) 
		{

			mixer->elem = elem;

			// 获取此 mixer 的音量范围
			if ((err = snd_mixer_selem_get_playback_volume_range(elem, &min, &max)) < 0) {
				fprintf(stderr, "Error getting volume range: %s\n", snd_strerror(err)); 
				snd_mixer_close(mixer->mixer_handle);
				return -1;
			}

			mixer->min = min;
			mixer->max = max;

			// 打印出哪个被找到：
			printf("Using mixer control '%s'\n",snd_mixer_selem_get_name(elem));

			return 1;
		}
	}
	
//	printf("volume now%d ( %d ~ %d )\n",current_volume,min,max);
//	mixer->elem=elem;
//	mixer->mixer_handle=mixer_handle;
//	mixer->selem_id=selem_id;
	mixer->min=min;
	mixer->max=max;
	return 0;
}

static int close_volume_hard(struct MediaVolumeHard *mixer)
{
	snd_mixer_close(mixer->mixer_handle);
}

//设置声卡音量
//当前程序可用，设置全局音量
//name:Master根据需要更换
//index:0,可能也需要更换
static int set_snd_volume(uint8_t volume,const char *name)
{
	int err=0;
	
	struct MediaVolumeHard mixer;
	if(open_vloume_hard(name,&mixer)<0)
		return -1;

	// 设置音量值
	long volume_set=(long)((mixer.max-mixer.min)*volume/100+mixer.min);
	if ((err = snd_mixer_selem_set_playback_volume_all(mixer.elem, volume_set)) < 0) {
		fprintf(stderr, "Error setting volume: %s\n", snd_strerror(err));
		close_volume_hard(&mixer);
		return -1;
	}
	printf("Volume set %s to %ld (%d/100)\n", name,volume_set,volume);

	// 关闭混音器
	close_volume_hard(&mixer);
	return 0;
}


//获取声卡硬件音量
//音量是分通道设置的，但是目前仅开放了统一设置的接口，所以获取时候暂时以作省道为准
static int get_snd_volume(const char *name)
{
	int err=0;
	long volume_get;
	int volume;
	struct MediaVolumeHard mixer;
	if(open_vloume_hard(name,&mixer)<0)
		return -1;
	if (snd_mixer_selem_get_playback_volume(mixer.elem, SND_MIXER_SCHN_FRONT_LEFT, &volume_get) < 0) {
		fprintf(stderr, "Error getting current volume\n");
		close_volume_hard(&mixer);
		return -1;
	}
	close_volume_hard(&mixer);
	volume=(volume_get-mixer.min)*100/(mixer.max-mixer.min);
	//printf("Volume get %s %ld(%d/100)\n",name,volume_get,volume);
	return volume;
}

static int get_audio_card_list(AudioGetCardCallback callback,void *userdata)
{
	int err=0;
	int card_idx = -1;
    
    while (snd_card_next(&card_idx) >= 0 && card_idx >= 0) {
        snd_ctl_t* ctl;
        char hw_name[32];
        snprintf(hw_name, sizeof(hw_name), "hw:%d", card_idx);
        
        if (snd_ctl_open(&ctl, hw_name, 0) != 0) 
			continue;
		int dev_idx = -1;
		while (snd_ctl_pcm_next_device(ctl, &dev_idx) >= 0 && dev_idx >= 0) 
		{
			snd_pcm_info_t* info;
			snd_pcm_info_alloca(&info);
			snd_pcm_info_set_device(info, dev_idx);
			snd_pcm_info_set_stream(info, SND_PCM_STREAM_PLAYBACK);
			
			if (snd_ctl_pcm_info(ctl, info) == 0) {

				AudioCardDevice card;
				char device[32];
				snprintf(device,sizeof(device),"hw:%d,%d",card_idx,dev_idx);
				card.hw=strdup(device);
				card.name=strdup(snd_pcm_info_get_name(info));
				if(callback)
					callback(&card,userdata);
				printf("  Device %d: %s (ID: hw:%d,%d)\n", dev_idx,snd_pcm_info_get_name(info), card_idx, dev_idx);
				if(card.hw)	free(card.hw);
				if(card.name) free(card.name);
			}
		}
		snd_ctl_close(ctl);
    }
    return 0;
}


//从传进来的pcm获取其对应的卡
static char *get_hw_card(const char *name)
{
	if(strncmp(name,"hw:",3)!=0)
	{
		return strdup(name);
	}
		
	else
	{
		char *card=strdup(name);
		char *p=strchr(card,',');
		if (p != NULL) {
    		*p = '\0'; 
		}
		return card;
	}
}



int Audio_Set_System_Volume(uint8_t volume,const char *name)
{
	if(volume>100)
		volume=100;
	if(!name)
		return set_snd_volume(volume,name);
	else
	{
		char *card=get_hw_card(name);
		int val=set_snd_volume(volume,card);
		free(card);
		return val;
	}
}

int Audio_Get_System_Volume(const char *name)
{
	if(!name)
		return get_snd_volume(name);
	else
	{
		char *card=get_hw_card(name);
		int val=get_snd_volume(card);
		free(card);
		return val;
	}
}

int Audio_Get_Device_List(AudioGetCardCallback callback,void *userdata)
{
	return get_audio_card_list( callback, userdata);
}







#ifdef __cplusplus
}
#endif