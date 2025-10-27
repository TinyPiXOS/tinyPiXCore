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
#ifndef __MAP_H
#define __MAP_H

#include "TpWMEngine/dataDefine.h"

TP_DEF_VOID_TYPE_VAR(IPiMap);

#ifdef __cplusplus
extern "C"{
#endif

//create share map between process
extern DECLSPEC IPiMap *STDCALL map_do_create(const char *mapname, int size, tpBool isCreate);

//read share map between process
extern DECLSPEC IPiMap *STDCALL map_do_read(const char *mapname);

//refresh share map between process
extern DECLSPEC tpBool STDCALL map_do_refresh(IPiMap *map);

//resize map between process
extern DECLSPEC tpBool STDCALL map_do_resize(IPiMap *map, int size);

//get address 
extern DECLSPEC void *STDCALL map_get_address(IPiMap *map);

//get insize
extern DECLSPEC int STDCALL map_get_insize(IPiMap *map);

//get alsize
extern DECLSPEC int STDCALL map_get_alsize(IPiMap *map);

//get map name
extern DECLSPEC const char *STDCALL map_get_mapname(IPiMap *map);

//destroy map
extern DECLSPEC void STDCALL map_do_destroy(IPiMap *map);

#ifdef __cplusplus
}
#endif

#endif
