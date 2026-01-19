#ifndef _AUDIO_HARD_H_
#define _AUDIO_HARD_H_

#ifdef __cplusplus
extern "C" {
#endif


#include <stdint.h>
#include <alsa/asoundlib.h>



typedef struct AudioCardDevice{
	char *hw;
	char *name;
}AudioCardDevice;


typedef void (*AudioGetCardCallback)(AudioCardDevice *device, void* user_data);


int Audio_Get_Device_List(AudioGetCardCallback callback,void *userdata);

int Audio_Set_System_Volume(uint8_t volume,const char *name);
int Audio_Get_System_Volume(const char *name);

#ifdef __cplusplus
}
#endif

#endif
