#ifndef _TP_RECORD_DEVICE_H_
#define _TP_RECORD_DEVICE_H_

#ifdef __cplusplus
extern "C" {
#endif



struct MediaAudioHandle ;
struct MediaParams;
struct MediaCodecParam;

struct MediaParams *record_config_creat()  __attribute__((used));
void record_config_free(struct MediaParams *conf) __attribute__((used));

struct MediaAudioHandle *Audio_Record_Open(const char *device) __attribute__((used));
int Record_Set_Start(struct MediaParams *conf,const char *file) __attribute__((used));
int Record_Set_Stop(struct MediaParams *conf) __attribute__((used));
int Audio_Record_Main(struct MediaAudioHandle *pcm,struct MediaParams *conf) __attribute__((used));



#ifdef __cplusplus
}
#endif

#endif
