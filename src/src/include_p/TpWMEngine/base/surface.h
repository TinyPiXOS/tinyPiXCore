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
#ifndef __SURFACE_H
#define __SURFACE_H

#include "TpWMEngine/dataDefine.h"

TP_DEF_VOID_TYPE_VAR(IPiSurface);

#ifdef __cpluscplus
extern "C"{
#endif

/************************************Surface Function DEF*****************************************/
/**
function name: surface_create, aim to create a surface, according to the followings
@bits		: if not NULL, denotes the buffer comes from others, surface will share it with 
			  others. otherwise, create a inner buffer.
@width		: the width for surface
@height 	: the hegiht for surface
@format		: the bitsperpixel, such as 8, 16, 24 and 32
@Rmask		: the mask of RED, default 0  //eg. 0xff000000
@Gmask		: the mask of GREEN, default 0  //eg. 0x00ff0000
@Bmask		: the mask of BLUE, default 0   //eg. 0x0000ff00
@Amask		: the mask of ALPHA, default 0  //eg. 0x000000ff

return value: return a not-NULL pointer, otherwise NULL
**/

extern DECLSPEC IPiSurface *STDCALL surface_create(void *bits, unsigned int width, unsigned int height, 
											ItpFormat format, 
											unsigned int  Rmask, unsigned int  Gmask, unsigned int  Bmask, unsigned int Amask);

/**
function name: clone a new surface according to the orignal surface 
			  others. otherwise, create a inner buffer.
@surface		: orignal surface
@share			: decide to share surface matrix that be cloned
return value: return a not-NULL pointer, otherwise NULL
**/

extern DECLSPEC IPiSurface *STDCALL surface_clone(IPiSurface *surface, tpBool share);

/**
function name: get surface structure size of header only.
@surface		: orignal surface

return value: if surface is ok , would return a const number, otherwise 0
**/

extern DECLSPEC int STDCALL surface_header_size();

/**
function name: caculate the stride which denotes the bytes per line
@width		: the width for surface
@depth		: the bitsperpixel, such as 16, 24 and 32

return value: if failed, return zero. otherwise return the fact bytes per line
*/
extern DECLSPEC unsigned int STDCALL surface_cal_stride(int width, ItpFormat format);

/**
function name: set_alpha, aim to set alpha for surface
@surface	: the exist surface pointer
@alpha		: the alpha value ranges from 0-255

return value: if failed, return TP_FALSE. otherwise return TP_TRUE
*/
extern DECLSPEC tpBool STDCALL surface_set_alpha(IPiSurface *surface, unsigned char alpha);

/**
function name: set alpha enable for surface
@surf		: the exist surface pointer
@enable		: if TP_TRUE, enable alpha mode. otherwise not
*/
extern DECLSPEC void STDCALL surface_set_alpha_enable(IPiSurface *surface, tpBool enable);

/**
function name: surface_get_alpha, aim to get alpha for surface
@surface		: the exist surface pointer

return value: if failed, return 255. otherwise return true alpha value
*/
extern DECLSPEC unsigned char STDCALL surface_get_alpha(IPiSurface *surface);

/**
function name: get alpha enable state for surface
@surface		: the exist surface pointer

return value:  return alpha enable true state, if surface is NULL, return TP_FALSE
*/
extern DECLSPEC tpBool STDCALL surface_get_alpha_enable(IPiSurface *surface);

/**
function name:set the color key (transparent pixel)
@surface	: the exist surface pointer
@colorkey	: the colorkey value for R-G-B-A or A-R-G-B

return value: if failed, return TP_FALSE. otherwise return TP_TRUE
*/
extern DECLSPEC tpBool STDCALL surface_set_colorkey(IPiSurface *surface, unsigned int colorkey);

/**
function name: set colorkey for surface
@surface	: the exist surface pointer
@enable		: if TP_TRUE, enable alpha mode. otherwise not
*/
extern DECLSPEC void STDCALL surface_set_colorkey_enable(IPiSurface *surface, tpBool enable);

/**
function name: surface_get_colorkey, aim to get colorkey for surface
@surf		: the exist surface pointer

return value: if failed, return default 0. otherwise return true colorkey value
*/
extern DECLSPEC unsigned int STDCALL surface_get_colorkey(IPiSurface *surface);

/**
function name: surface_get_colorkey, aim to get colorkey enable state for surface
@surf		: the exist surface pointer

return value:  return colorkey enable true state, if surf is NULL, return TP_FALSE
*/
extern DECLSPEC tpBool STDCALL surface_get_colorkey_enable(IPiSurface *surface);

/**
function name: surface_set_cliprect, aim to set cliprect for surface
@surf		: the exist surface pointer

return value:  if surf is NULL or w = 0 || h = 0 for rect, will return TP_FALSE, otherwise return TP_TRUE
*/
extern DECLSPEC tpBool STDCALL surface_set_cliprect(IPiSurface *surface, PiRect rect);

/**
function name: surface_get_cliprect, aim to get cliprect for surface
@surf		: the exist surface pointer

return value:  if surf is NULL , will return TP_FALSE, otherwise return true cliprect
*/
extern DECLSPEC PiRect STDCALL surface_get_cliprect(IPiSurface *surface);

/**
function name: surface_get_width, aim to get width for surface
@surf		: the exist surface pointer

return value:  if surf is NULL , will return 0
*/
extern DECLSPEC unsigned int STDCALL surface_get_width(IPiSurface *surface);

/**
function name: surface_get_height, aim to get height for surface
@surf		: the exist surface pointer

return value:  if surf is NULL , will return 0
*/
extern DECLSPEC unsigned int STDCALL surface_get_height(IPiSurface *surface);

/**
function name: surface_get_stride, aim to get stride for surface
@surf		: the exist surface pointer

return value:  if surf is NULL , will return 0
*/
extern DECLSPEC unsigned int STDCALL surface_get_stride(IPiSurface *surface);

/**
function name: surface_get_format, aim to get image format for surface
@surf		: the exist surface pointer

return value:  if surf is NULL , will return TP_RGB_UNKOWN
*/
extern DECLSPEC ItpFormat STDCALL surface_get_format(IPiSurface *surface);

/**
function name: surface_get_size, aim to get image size for surface
@surf		: the exist surface pointer

return value:  if surf is NULL , will return 0
*/
extern DECLSPEC unsigned int STDCALL surface_get_size(IPiSurface *surface);

/**
function name: surface_get_bits, aim to get image buffer for surface
@surf		: the exist surface pointer

return value:  if surf is NULL , will return NULL. also if width = 0 || height = 0 for surf, will return NULL.
*/
extern DECLSPEC void* STDCALL surface_get_matrix(IPiSurface *surface);

/**
*function name : rapaidx_surface_swap_width_height, aim to swap the width and the height to surface
*/
extern DECLSPEC tpBool STDCALL surface_swap_width_height(IPiSurface *surf);
/**
fuction name : surface_set_lightness, aim to set surface lightness
**/
extern DECLSPEC void STDCALL surface_set_lightness(IPiSurface *surf, unsigned char lightness);
/**
fuction name : surface_get_lightness, aim to get surface lightness
**/
extern DECLSPEC unsigned char STDCALL surface_get_lightness(IPiSurface *surf);

/**get Rmask, Gmask, Bmask, Amask;
*/
extern DECLSPEC unsigned int STDCALL surface_get_rmask(IPiSurface *surface);
extern DECLSPEC unsigned int STDCALL surface_get_gmask(IPiSurface *surface);
extern DECLSPEC unsigned int STDCALL surface_get_bmask(IPiSurface *surface);
extern DECLSPEC unsigned int STDCALL surface_get_amask(IPiSurface *surface);
extern DECLSPEC tpBool STDCALL surface_get_rgba_mask(IPiSurface *surface, unsigned int *Rmask, unsigned int *Gmask, unsigned int *Bmask, unsigned int *Amask);

/**surface fill**/
extern DECLSPEC void STDCALL surface_clear(IPiSurface *surface, unsigned int color, int use_alpha);

/**map color**/
extern DECLSPEC unsigned int STDCALL surface_map_rgba(IPiSurface *surface, unsigned char r, unsigned char g, unsigned char b, unsigned char a);

/**
function name: free an exist surface
@surface		:  the exist surface pointer
*/
extern DECLSPEC void STDCALL surface_free(IPiSurface *surface);
/************************************Surface Function END*****************************************/

#ifdef __cpluscplus
}
#endif

#endif
