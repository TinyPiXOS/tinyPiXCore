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
#ifndef __MUTEX_H
#define __MUTEX_H

#include "TpWMEngine/dataDefine.h"

TP_DEF_VOID_TYPE_VAR(IPiMutex);
TP_DEF_VOID_TYPE_VAR(IPiMutexAddress);

#ifdef __cplusplus
extern "C"{
#endif

/**
*attach a share mutex, commonly you should be careful to use this function
*/
extern DECLSPEC IPiMutex * STDCALL mutex_attach(IPiMutexAddress *address, tpBool isServer);

/**mutex inited **/
extern DECLSPEC int STDCALL mutex_inited(IPiMutex *mutex);

/**
*dettach a share mutex, commonly you should be careful to use this function
*/
extern DECLSPEC void STDCALL mutex_deattach(IPiMutexAddress *address);

/**set consisit_np **/
extern DECLSPEC int STDCALL mutex_consistent(IPiMutex *mutex);

/*
*try lock mutex
*/
extern DECLSPEC int STDCALL mutex_trylock(IPiMutex *mutex);

/**
*
*time lock mutex
*/
extern DECLSPEC int STDCALL mutex_timelock(IPiMutex *mutex, unsigned int time);//second unit

/**
*create mutex
*/
extern DECLSPEC IPiMutex * STDCALL mutex_create();

/**
*lock mutex
*/
extern DECLSPEC int STDCALL mutex_lock(IPiMutex *mutex);

/**
*unlock mutex
*/
extern DECLSPEC int STDCALL mutex_unlock(IPiMutex *mutex);

/**
*free mutex
*/
extern DECLSPEC int STDCALL mutex_free(IPiMutex *mutex);


#ifdef __cplusplus
}
#endif

#endif
