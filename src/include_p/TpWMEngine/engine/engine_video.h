/* liucy has been here，but nothing to see and nothing left ^_^!*/

/*
** Copyright (c) 2007-2021 By Alexander.King.
**
** Permission is hereby granted, free of charge, to any person obtaining a
** copy of this software and/or associated documentation files (the
** "Materials"), to deal in the Materials without restriction, including
** without limitation the rights to use, copy, modify, merge, publish,
** distribute, sublicense, and/or sell copies of the Materials, and to
** permit persons to whom the Materials are furnished to do so, subject to
** the following conditions:
**
** The above copyright notice and this permission notice shall be included
** in all copies or substantial portions of the Materials.
**
** THE MATERIALS ARE PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
** EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
** MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.
** IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY
** CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT,
** TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE
** MATERIALS OR THE USE OR OTHER DEALINGS IN THE MATERIALS.
*/
#ifndef __ENGINE_VIDEO_H
#define __ENGINE_VIDEO_H

#include "base.h"
#include "engine.h"

typedef struct{
	ITpFormat format;
	tpUInt32 width;
	tpUInt32 height;
	tpUInt32 stride;
	
	void  *matrix;

	tpUInt32 Rmask;
	tpUInt32 Gmask;
	tpUInt32 Bmask;
	tpUInt32 Amask;
	
	tpBool accelerated;//TP_TRUE==harware, TP_FALSE==software
}PiVideoSurfaceInfo;

typedef struct _TP_VIDEO_DRIVER
{
	tpBool (*video_driver_init)(tpInt32 , tpUInt32, tpUInt32, void*, tpBool);
	tpBool (*video_driver_reinit)(tpInt32 , tpUInt32, tpUInt32, void*);
	tpBool (*video_surface_info)(PiVideoSurfaceInfo*);
	tpBool (*video_do_fill)(PiRect*, tpUInt32, tpBool update);
	tpBool (*video_do_blit)(IPiSurface*, PiRect*, PiRect*, tpBool);
	tpBool (*video_do_copy_blit)(IPiSurface*, PiRect*, IPiSurface *, PiRect *);
	tpBool (*video_do_update)(PiRect*);
	tpBool (*video_do_updates)(PiRect*, tpInt32);
	tpBool (*video_do_save)(const tpChar *, IPiSurface *);
	tpBool (*video_do_capture)(void *, tpUInt32 , tpUInt32 *, tpUInt32 *, tpUInt32 *, tpUInt32 *);
	tpBool (*video_driver_hardware)(void);
	tpBool (*video_driver_show_wait)(IPiSurface *);
	tpBool (*video_driver_enable_switch)(tpBool);
	tpBool (*video_driver_set_cursor)(IPiSurface*, tpInt32, tpInt32);
	tpBool (*video_driver_set_mouse_position)(tpInt32, tpInt32);
	void   (*video_driver_set_logical_size)(tpUInt32 , tpUInt32 );
	void   (*video_driver_get_logical_size)(tpUInt32*, tpUInt32*);
	void   (*video_driver_get_physical_metrics)(tpFloat*, tpFloat*);
	void   (*video_mouse_show)(tpBool);
	void   (*video_driver_uninit)(void);
	
	void *priv;
}PiVideoDriver;

#ifdef __cplusplus
extern "C"{
#endif

/**
*register video driver
**/
extern DECLSPEC PiVideoDriver * STDCALL register_video_driver(void*);

#ifdef __cplusplus
}
#endif

#endif
