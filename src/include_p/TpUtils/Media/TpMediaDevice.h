#ifndef _TP_MEDIA_DEVICE_H_
#define _TP_MEDIA_DEVICE_H_

#ifdef __cplusplus
extern "C" {
#endif

#include "Media/Audio/audio_play.h"
#include "Media/Audio/hard.h"
#include "Media/Media/media.h"
#include "Media/Media/media_play.h"
#include "Media/Media/media_codec.h"
#include "Media/Video/video_play.h"

#ifdef __cplusplus
}
#endif


#include "TpString.h"

class TpMediaDevice
{
public: 
    TpMediaDevice(){};
    ~TpMediaDevice(){};
public:
    static TpString getFormatName(const TpString& audio_name);

};


#endif