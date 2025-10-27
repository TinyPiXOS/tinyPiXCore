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
#ifndef __WM_LIST_H
#define __WM_LIST_H

#include "pixdef.h"

#ifdef __cplusplus
extern "C"{
#endif

/**wmlist create**/
extern DECLSPEC IPiWMlist * STDCALL wmlist_create(int maxnumber);//defautl 10240
/**wmlist free**/
extern DECLSPEC void STDCALL wmlist_free(IPiWMlist *wmlist, tpBool freeObject);

/**add an object to wmlist**/
extern DECLSPEC tpBool STDCALL wmlist_add_object(IPiWMlist *wmlist, IPiObject *object);

/**remove an object form wmlist**/
extern DECLSPEC tpBool STDCALL wmlist_remove_object(IPiWMlist *wmlist, IPiObject *object);
/**remove an object form wmlist by pid**/
extern DECLSPEC tpBool STDCALL wmlist_remove_object_by_pid(IPiWMlist *wmlist, pid_t pid, tpBool freeObject);
/**remove an object form wmlist by id**/
extern DECLSPEC tpBool STDCALL wmlist_remove_object_by_id(IPiWMlist *wmlist, int id);
/**remove an object form wmlist by sock**/
extern DECLSPEC tpBool STDCALL wmlist_remove_object_by_sock(IPiWMlist *wmlist, IPiTCPSocket *sock);

/**find an object by id**/
extern DECLSPEC IPiObject * STDCALL wmlist_find_object_by_id(IPiWMlist *wmlist, int id);
/**find an object by sock**/
extern DECLSPEC IPiObject * STDCALL wmlist_find_object_by_sock(IPiWMlist *wmlist, IPiTCPSocket *sock);
/**find an object by point**/
extern DECLSPEC IPiObject * STDCALL wmlist_find_object_by_point(IPiWMlist *wmlist, PiPoint point, PiWMLayer layer);//only visible = true and acitved = true, and consider colorkey and change shape
/**find an object by x and y**/
extern DECLSPEC IPiObject * STDCALL wmlist_find_object_by_position(IPiWMlist *wmlist, int x, int y, PiWMLayer layer);//only visible = true and acitved = true, and consider colorkey and change shape
/**find the top visible object for win not float**/
extern DECLSPEC IPiObject * STDCALL wmlist_find_object_by_top_visible(IPiWMlist *wmlist, IPiObject *object);
/**find desktop**/
extern DECLSPEC IPiObject * STDCALL wmlist_find_object_first(IPiWMlist *wmlist);

/**is full**/
extern DECLSPEC tpBool STDCALL wmlist_is_full(IPiWMlist *wmlist);
/**is empty**/
extern DECLSPEC tpBool STDCALL wmlist_is_empty(IPiWMlist *wmlist);
/**numbers**/
extern DECLSPEC int STDCALL wmlist_has_numbers(IPiWMlist *wmlist);

/**bring object in wmlist position to top**/
extern DECLSPEC tpBool STDCALL wmlist_bring_object_to_top(IPiWMlist *wmlist, IPiObject *object);
/**bring object in wmlist position to bottom**/
extern DECLSPEC tpBool STDCALL wmlist_bring_object_to_bottom(IPiWMlist *wmlist, IPiObject *object);

/**visit wmlist by callback**/
extern DECLSPEC tpBool STDCALL wmlist_visit(IPiWMlist *wmlist, VisitCallbackFunction visitFun, int args, void *args1, void *args2, void *args3);

#ifdef __cplusplus
}
#endif

#endif
