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
#ifndef __OBJECT_H
#define __OBJECT_H

#include "pixdef.h"

#ifdef __cplusplus
extern "C"{
#endif

/**object create**/
extern DECLSPEC IPiObject* STDCALL obeject_create(pid_t pid, PiWMLayer layer, IPiTCPSocket *sock);
/**object attrib confirm**/
extern DECLSPEC tpBool  STDCALL obeject_confirm(IPiObject *object, PiRect rect, tpBool visible, void *args);
/**object free**/
extern DECLSPEC void STDCALL object_free(IPiObject *object);

/**object get title**/
extern DECLSPEC const tpChar *STDCALL object_get_title(IPiObject *object);
/**object active**/
extern DECLSPEC void STDCALL object_set_active(IPiObject *object, tpBool actived);
/**get object active**/
extern DECLSPEC tpBool STDCALL object_get_active(IPiObject *object);
/**object visible**/
extern DECLSPEC void STDCALL object_set_visible(IPiObject *object, tpBool visible);
/**get object visible**/
extern DECLSPEC tpBool STDCALL object_get_visible(IPiObject *object);
/**object alpha**/
extern DECLSPEC void STDCALL object_set_alpha(IPiObject *object, tpUInt8 alpha);
/**get object alpha**/
extern DECLSPEC tpUInt8 STDCALL object_get_alpha(IPiObject *object);
/**object moved**/
extern DECLSPEC void STDCALL object_set_moved(IPiObject *object, tpBool moved);
/**get object moved**/
extern DECLSPEC tpBool STDCALL object_get_moved(IPiObject *object);
/**get object surface**/
extern DECLSPEC IPiSurface* STDCALL object_get_surface(IPiObject *object);
/**get object process mutex**/
extern DECLSPEC IPiMutex* STDCALL object_get_mutex(IPiObject *object);
/**process lock**/
extern DECLSPEC tpInt32 STDCALL object_lock_surface(IPiObject *object);
/**process unlock**/
extern DECLSPEC tpInt32 STDCALL object_unlock_surface(IPiObject *object);
/**process trylock **/
extern DECLSPEC tpInt32 STDCALL object_try_lock_surface(IPiObject *object);
/**process timelock**/
extern DECLSPEC tpInt32 STDCALL object_time_lock_surface(IPiObject *object, tpUInt32 time);
/**set object rect**/
extern DECLSPEC void STDCALL object_set_rect(IPiObject *object, PiRect rect);
/**get object rect**/
extern DECLSPEC PiRect STDCALL object_get_rect(IPiObject *object);
/**set object position**/
extern DECLSPEC void STDCALL object_set_position(IPiObject *object, PiPoint point);
/**get object position**/
extern DECLSPEC PiPoint STDCALL object_get_position(IPiObject *object);
/**get object sock**/
extern DECLSPEC IPiTCPSocket *STDCALL object_get_sock(IPiObject *object);
/**object id**/
extern DECLSPEC tpInt32 STDCALL object_set_id(IPiObject *object, tpInt32 id);
/**get object id**/
extern DECLSPEC tpInt32 STDCALL object_get_id(IPiObject *object);
/**get object pid**/
extern DECLSPEC pid_t STDCALL object_get_pid(IPiObject *object);
/**get object layer**/
extern DECLSPEC PiWMLayer STDCALL object_get_layer(IPiObject *object);
/**object send command**/
extern DECLSPEC tpBool STDCALL object_send_command(IPiObject *object, PiXPacketCmd *packet, tpBool direct);

#ifdef __cplusplus
}
#endif

#endif
