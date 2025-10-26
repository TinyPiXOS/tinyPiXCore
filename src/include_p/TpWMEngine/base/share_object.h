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
#ifndef __SHARE_OBJECT_H
#define __SHARE_OBJECT_H

#include "typesDef.h"
#include "protocol.h"
#include "map.h"
#include "mutex.h"

typedef enum
{
	WM_NONE = -1,
	WM_DESK,
	WM_WIN,
	WM_USE_FLOAT,
	WM_SYS_FLOAT,
	WM_LAYER_NUMBERS,
} PiWMLayer;

typedef struct{
	pthread_mutex_t mutex;
	pthread_mutexattr_t attr;
	tpBool inited;
}PiMutex;

typedef struct{
	tpBool actived;
	tpBool visible;
	tpBool moved;
	PiRect rect;
	tpUInt8 alpha;
	
	PiMutex process_mutex;
	tpChar title[TOKEN_RING_LENGTH];
}PiObjectAttr;

typedef struct{
	IPiMap *map;
	PiObjectAttr *object_attr;
}PiShareObject;

#ifdef __cplusplus
extern "C"{
#endif

//create share object
extern DECLSPEC PiShareObject* STDCALL object_do_share_create(const tpChar *filename, tpBool isCreate);

//read share object
extern DECLSPEC PiShareObject* STDCALL object_do_share_read(const tpChar *filename);

//destroy share object
extern DECLSPEC void STDCALL object_do_share_destroy(PiShareObject *share_object);

//get process mutex 
extern DECLSPEC IPiMutex *STDCALL object_do_share_mutex(PiShareObject *share_object, tpBool isServer);
#ifdef __cplusplus
}
#endif

#endif
