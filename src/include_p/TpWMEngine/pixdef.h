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
#ifndef __PIX_DEF_H
#define __PIX_DEF_H

#include "base.h"
#include "engine_driver.h"

TP_DEF_VOID_TYPE_VAR(IPiObject);
TP_DEF_VOID_TYPE_VAR(IPiZorderLayer);
TP_DEF_VOID_TYPE_VAR(IPiWMlist);
TP_DEF_VOID_TYPE_VAR(IPiConfig);
TP_DEF_VOID_TYPE_VAR(IPiIDS);
TP_DEF_VOID_TYPE_VAR(IPiService);
TP_DEF_VOID_TYPE_VAR(IPiServer);

typedef tpBool (*VisitCallbackFunction)(IPiObject*, pid_t, tpInt32, tpInt32, IPiTCPSocket *, tpInt32, void*, void*, void*);
typedef tpBool (*videoDoBlit)(IPiSurface*, PiRect*, PiRect*, tpBool);
typedef tpBool (*videoCopyStrenchBlit)(IPiSurface*, PiRect*, IPiSurface *, PiRect *);
typedef tpBool (*videoSurfaceInfo)(PiVideoSurfaceInfo *);
typedef tpBool (*videoDoUpdate)(PiRect*);
typedef tpBool (*videoDoUpdates)(PiRect*, tpInt32);
typedef tpBool (*videoDoFill)(PiRect*, tpUInt32, tpBool update);
typedef tpBool (*videoDoReInit)(tpInt32 , tpUInt32, tpUInt32, void*);
typedef tpBool (*videoDoSave)(const tpChar *, IPiSurface *);
typedef tpBool (*videoDoCapture)(void  *pixels, tpUInt32 stride, tpUInt32 *Rmask, tpUInt32 *Gmask, tpUInt32 *Bmask, tpUInt32 *Amask);
typedef tpBool (*videoSetCursor)(IPiSurface*, tpInt32, tpInt32);
typedef tpBool (*videoSetMousePosition)(tpInt32, tpInt32);
typedef void   (*videoSetLogicalSize)(tpUInt32, tpUInt32);
typedef void   (*videoGetLogicalSize)(tpUInt32*, tpUInt32*);
typedef void   (*videoGetMetrics)(tpFloat*, tpFloat*);
typedef tpBool (*videoShowWait)(IPiSurface*);
typedef tpBool (*videoSwitch)(tpBool);
typedef tpBool (*videoHardware)(void);

static int layer_limit[] = {1, 991, 16, 16};


#endif
