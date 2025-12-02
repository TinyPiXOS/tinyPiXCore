#ifndef _AUDIO_RECORD_H_
#define _AUDIO_RECORD_H_

#ifdef __cplusplus
extern "C"
{
#endif

#include "audio_play.h"




struct MediaAudioHandle *Audio_Record_Open(const char *device);
int Audio_Record_Main(struct MediaAudioHandle *pcm,struct MediaParams *conf);
int Audio_Record_Test(struct MediaAudioHandle *pcm,const char *file);

int Record_Set_Start(struct MediaParams *conf,const char *file);
int Record_Set_Stop(struct MediaParams *conf);

#ifdef __cplusplus
}
#endif

#endif
