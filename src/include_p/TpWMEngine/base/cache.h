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
#ifndef __CACHE_H
#define __CACHE_H

#include "typesDef.h"

#ifdef __cplusplus
extern "C"{
#endif

TP_DEF_VOID_TYPE_VAR(IPiCache);

/**
*load all contents from a spicy file to memory
*to fasten the read/write.
*/
extern DECLSPEC IPiCache * STDCALL cache_file(const char *filename);

/**
*create a new cache file
*/
extern DECLSPEC IPiCache * STDCALL cache_create(const char *filename, unsigned int size);

/**
*get size from cache
*/
extern DECLSPEC unsigned int STDCALL cache_size(IPiCache *cache);

/**
*get address from cache
*/
extern DECLSPEC void * STDCALL cache_address(IPiCache *cache);

/**
*free cache from memory
*/
extern DECLSPEC void STDCALL cache_free(IPiCache *cache);


#ifdef __cplusplus
}
#endif

#endif
