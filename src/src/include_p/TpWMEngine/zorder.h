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
#ifndef __ZORDER_H
#define __ZORDER_H

#include "wmlist.h"
#include "ids.h"
#include "object.h"

#ifdef __cplusplus
extern "C"{
#endif

/**zorders create**/
extern DECLSPEC IPiZorderLayer * STDCALL zorder_create();
/**zorders free**/
extern DECLSPEC void STDCALL zorder_free(IPiZorderLayer *zorders, tpBool freeObject);

/**add an object to layer**/
extern DECLSPEC tpBool STDCALL zorder_add_object(IPiZorderLayer *zorders, IPiObject *object);

/**remove an object form layer**/
extern DECLSPEC tpBool STDCALL zorder_remove_object(IPiZorderLayer *zorders, IPiObject *object);
/**remove an object form layer by pid**/
extern DECLSPEC tpBool STDCALL zorder_remove_object_by_pid(IPiZorderLayer *zorders, pid_t pid, tpBool freeObject);
/**remove an object form layer by id**/
extern DECLSPEC tpBool STDCALL zorder_remove_object_by_id(IPiZorderLayer *zorders, int id);
/**remove an object form layer by sock**/
extern DECLSPEC tpBool STDCALL zorder_remove_object_by_sock(IPiZorderLayer *zorders, IPiTCPSocket *sock);

/**find an object by id**/
extern DECLSPEC IPiObject * STDCALL zorder_find_object_by_id(IPiZorderLayer *zorders, int id);
/**find an object by sock**/
extern DECLSPEC IPiObject * STDCALL zorder_find_object_by_sock(IPiZorderLayer *zorders, IPiTCPSocket *sock);
/**find an object by point**/
extern DECLSPEC IPiObject * STDCALL zorder_find_object_by_point(IPiZorderLayer *zorders, PiPoint point);//only visible = true and acitved = true, and consider colorkey and change shape
/**find an object by x and y**/
extern DECLSPEC IPiObject * STDCALL zorder_find_object_by_position(IPiZorderLayer *zorders, int x, int y);//only visible = true and acitved = true, and consider colorkey and change shape
/**find the top visible object for win not float**/
extern DECLSPEC IPiObject * STDCALL zorder_find_object_by_top_visible(IPiZorderLayer *zorders, IPiObject *object, PiWMLayer start, PiWMLayer end);

/**find desktop**/
extern DECLSPEC IPiObject * STDCALL zorder_find_object_first(IPiZorderLayer *zorders, PiWMLayer layer);

/**is full**/
extern DECLSPEC tpBool STDCALL zorder_is_full(IPiZorderLayer *zorders, PiWMLayer layer);
/**is empty**/
extern DECLSPEC tpBool STDCALL zorder_is_empty(IPiZorderLayer *zorders, PiWMLayer layer);

/**bring object in layer position to top**/
extern DECLSPEC tpBool STDCALL zorder_bring_object_to_top(IPiZorderLayer *zorders, IPiObject *object);
/**bring object in layer position to bottom**/
extern DECLSPEC tpBool STDCALL zorder_bring_object_to_bottom(IPiZorderLayer *zorders, IPiObject *object);
/**get numbers**/
extern DECLSPEC int STDCALL zorder_has_numbers(IPiZorderLayer *zorders, PiWMLayer layer);

/**visit layer by callback**/
extern DECLSPEC tpBool STDCALL zorder_visit(IPiZorderLayer *zorders, VisitCallbackFunction visitFun, PiWMLayer start, PiWMLayer end, int args, void *args1, void *args2, void *args3);

#ifdef __cplusplus
}
#endif

#endif
