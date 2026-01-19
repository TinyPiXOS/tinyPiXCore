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
#ifndef __CACHE_EX_H
#define __CACHE_EX_H

#include "base.h"

TP_DEF_VOID_TYPE_VAR(IPiSharedEx);

#ifdef __cplusplus
extern "C"{
#endif

/**create share ex information between server and client, only use once**/
extern DECLSPEC IPiSharedEx *STDCALL cache_ex_map_surface_create(const char *filename, unsigned int width, unsigned int height, ITpFormat format,
																 unsigned int Rmask, unsigned int Gmask, unsigned int Bmask, unsigned int Amask);
/**open the existed information **/
extern DECLSPEC IPiSharedEx *STDCALL cache_ex_map_surface_read(const char *filename);
/**resize the maptrix **/
extern DECLSPEC IPiSharedEx *STDCALL cache_ex_map_resize(IPiSharedEx *shareEx, unsigned int nwidth, unsigned int nheight, ITpFormat format);
/**get the new surface**/
extern DECLSPEC IPiSurface *STDCALL cache_ex_map_get_surface(IPiSharedEx *shareEx);
/**get matrix size**/
extern DECLSPEC int STDCALL cache_ex_map_get_matrix_size(IPiSharedEx *shareEx);
/**get resize state**/
extern DECLSPEC tpBool STDCALL cache_ex_map_get_resized(IPiSharedEx *shareEx);
/**set resize state**/
extern DECLSPEC tpBool STDCALL cache_ex_map_set_resized(IPiSharedEx *shareEx, tpBool resized);
/**share ex free**/
extern DECLSPEC void STDCALL cache_ex_map_free(IPiSharedEx *shareEx, tpBool isServer);
/**get share ex surface, directly**/
extern DECLSPEC IPiSurface* STDCALL cache_ex_map_surface(IPiSharedEx *shareEx);

#ifdef __cplusplus
}
#endif

#endif
