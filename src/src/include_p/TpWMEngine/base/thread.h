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
#ifndef __THREAD_H
#define __THREAD_H

#include "typesDef.h"

TP_DEF_VOID_TYPE_VAR(IPiThread);

#ifdef __cplusplus
extern "C" {
#endif

/**
*create thread
*/
extern DECLSPEC IPiThread * STDCALL thread_create(int (STDCALL *fn)(void*), void *data);

/**
*thread id
*/
extern DECLSPEC unsigned int STDCALL thread_id();

/**
*notice : please use one of the thread_wait or thread_kill at once, or will occurs segement fault.
*/

/**
*wait thread to stop and free thread
*/
extern DECLSPEC void STDCALL thread_wait(IPiThread *thread, void *status);

/**
*force to kill thread and free thread
*/
extern DECLSPEC void STDCALL thread_kill(IPiThread *thread);

/**
*force to kill thread and free thread
*/
extern DECLSPEC tpBool STDCALL thread_set_sigmask(int signal_mask, int thread_mask);


#ifdef __cplusplus
}
#endif

#endif
