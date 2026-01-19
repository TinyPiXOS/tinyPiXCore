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
#ifndef __RS_SURF_H
#define __RS_SURF_H

#include "typesDef.h"
#include "surface.h"

TP_DEF_VOID_TYPE_VAR(IPiWRSurface);

#ifdef __cplusplus
extern "C"{
#endif

/**create wrsurface**/
extern DECLSPEC IPiWRSurface *STDCALL rssurface_do_create(const tpChar *surfacename,
														  tpUInt32 width, 
														  tpUInt32 height, 
														  ITpFormat format, 
														  tpUInt32  Rmask,
														  tpUInt32  Gmask, 
														  tpUInt32  Bmask, 
														  tpUInt32  Amask,
														  tpBool isCreate);

/**read wrsurface**/
extern DECLSPEC IPiWRSurface *STDCALL rssurface_do_read(const tpChar *surfacename);

/**read wrsurface by parameters**/
extern DECLSPEC IPiWRSurface *STDCALL rssurface_do_read_by_parameters(const tpChar *surfacename,
																	  tpUInt32 width, 
																	  tpUInt32 height, 
																	  ITpFormat format, 
																	  tpUInt32  Rmask,
																	  tpUInt32  Gmask, 
																	  tpUInt32  Bmask, 
																	  tpUInt32  Amask);
/**save map as a snap**/
extern DECLSPEC tpBool STDCALL rssurface_do_snap(IPiWRSurface *wrsurface, const tpChar *snapname);

/**change surface size**/
extern DECLSPEC tpBool STDCALL rssurface_do_resize(IPiWRSurface *wrsurface, tpUInt32 width, tpUInt32 height);

/**get rect from surface**/
extern DECLSPEC PiRect STDCALL rssurface_get_rect(IPiWRSurface *wrsurface);

/**set alpha for surface**/
extern DECLSPEC tpBool STDCALL rssurface_do_alpha(IPiWRSurface *wrsurface, tpUInt8 alpha);

/**get surface**/
extern DECLSPEC IPiSurface *STDCALL rssurface_get_surface(IPiWRSurface *wrsurface, tpBool refresh);

/**destory wrsurface**/
extern DECLSPEC void STDCALL rssurface_do_destroy(IPiWRSurface *wrsurface);

#ifdef __cplusplus
}
#endif

#endif
