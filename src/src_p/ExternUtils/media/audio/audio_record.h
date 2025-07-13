#ifndef _AUDIO_RECORD_H_
#define _AUDIO_RECORD_H_

#include "audio_play.h"




PIAudioConf *Audio_Record_Open(const char *device);
int Audio_Record_Main(PIAudioConf *pcm,struct MediaParams *conf);
int Audio_Record_Test(PIAudioConf *pcm,const char *file);

int Record_Set_Start(struct MediaParams *conf,const char *file);
int Record_Set_Stop(struct MediaParams *conf);

#endif
