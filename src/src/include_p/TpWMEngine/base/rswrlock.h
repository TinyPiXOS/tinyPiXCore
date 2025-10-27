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
#ifndef __RS_WRLOCK_H
#define __RS_WRLOCK_H

#include "typesDef.h"

TP_DEF_VOID_TYPE_VAR(IPiWRLock);

#ifdef __cplusplus
extern "C"{
#endif

/**create shared rwlock**/
extern DECLSPEC IPiWRLock *STDCALL rsrwlock_shared_do_create(const char *lockname, tpBool isCreate);

/**read shared rwlock**/
extern DECLSPEC IPiWRLock *STDCALL rsrwlock_shared_do_read(const char *lockname);

/**create rwlock**/
extern DECLSPEC IPiWRLock *STDCALL rsrwlock_do_create();

/**read lock**/
extern DECLSPEC tpInt32 STDCALL rsrwlock_do_rdlock(IPiWRLock *rwlock);

/**read try lock**/
extern DECLSPEC tpInt32 STDCALL rsrwlock_do_try_rdlock(IPiWRLock *rwlock);

/**read wait lock**/
extern DECLSPEC tpInt32 STDCALL rsrwlock_do_rlock(IPiWRLock *rwlock, tpUInt32 ms);

/**write lock**/
extern DECLSPEC tpInt32 STDCALL rsrwlock_do_rwlock(IPiWRLock *rwlock);

/**write try lock**/
extern DECLSPEC tpInt32 STDCALL rsrwlock_do_try_rwlock(IPiWRLock *rwlock);

/**write wait lock**/
extern DECLSPEC tpInt32 STDCALL rsrwlock_do_wlock(IPiWRLock *rwlock, tpUInt32 ms);

/**unlock**/
extern DECLSPEC tpInt32 STDCALL rsrwlock_do_unlock(IPiWRLock *rwlock);

/**destroy rwlock**/
extern DECLSPEC void STDCALL rsrwlock_do_destroy(IPiWRLock *rwlock);

#ifdef __cplusplus
}
#endif

#endif
