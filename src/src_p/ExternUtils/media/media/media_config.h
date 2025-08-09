#ifndef _MEDIA_CONFIG_H_
#define _MEDIA_CONFIG_H_

#ifdef __cplusplus
extern "C" {
#endif

#define MEDIA_SDL_ENABLE	1			//是否使用SDL
#define DEBUG_MEDIA_CODEC				//媒体编解码调试


#ifdef MEDIA_SDL_ENABLE
#include <SDL2/SDL.h>
#endif




#ifdef __cplusplus
}
#endif

#endif
