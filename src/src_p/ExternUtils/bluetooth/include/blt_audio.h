#ifndef _BLT_AUDIO_H_
#define _BLT_AUDIO_H_

#ifdef	__cplusplus
extern "C" {
#endif

#include <stdint.h>
#include "lib/freedesktop_systemd.h"

//"a2dp_sink" "hsp_ag"/"hfp_ag"   
typedef enum{
	BLUET_AUDIO_PROFILE_A2DP_SINK,
}BluetAudioProfileType;


typedef enum{
	BLUET_AUDIO_CODEC_SBC,
}BluetAudioCodecType;

//启动bluealsa守护进程
int bluet_audio_start_blue_alsa(DesktopSystem *system,GError **error);
int bluet_audio_stop_blue_alsa(DesktopSystem *system,GError **error);
int bluet_audio_restart_blue_alsa(DesktopSystem *system,GError **error);

uint8_t bluet_audio_blue_alsa_is_runing(DesktopSystem *system,GError **error);


#ifdef	__cplusplus
}
#endif

#endif

