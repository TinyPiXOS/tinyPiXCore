/*///------------------------------------------------------------------------------------------------------------------------//
		视频服务管理
说 明 : 
日 期 : 2025.1.8

/*///------------------------------------------------------------------------------------------------------------------------//

#ifdef __cplusplus
extern "C" {
#endif
#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include "video_play.h"
#include "video_codec.h"
#include "video_manage.h"
#include "audio_codec.h"



int Video_Test()
{
	struct VideoHardParam display;
	struct MediaCodecParam codec_v,codec_a;
	Video_Get_File_Info("/home/pix/Media/test_video.mp4",&codec_v,&codec_a);

	Video_Play_File(&display,&codec_v,&codec_a);

	Video_Free_File(&codec_v,&codec_a);
	return 0;
}

#ifdef __cplusplus
}
#endif