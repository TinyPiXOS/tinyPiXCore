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

#ifndef __IDS_H
#define __IDS_H

#include "object.h"

#ifdef __cplusplus
extern "C"{
#endif

/**ids create**/
extern DECLSPEC IPiIDS * STDCALL ids_create(unsigned int length);
/**ids free**/
extern DECLSPEC void STDCALL ids_free(IPiIDS *ids);

/**ids find emtpy id resource**/
extern DECLSPEC int STDCALL ids_find_empty(IPiIDS *ids);//return -1, could not find
/**ids find object by id**/
extern DECLSPEC IPiObject* STDCALL ids_find_object_by_id(IPiIDS *ids, int id);//return -1, could not find
/**ids find object by sock**/
extern DECLSPEC IPiObject* STDCALL ids_find_object_by_sock(IPiIDS *ids, IPiTCPSocket *sock);//return -1, could not find

/**ids release id resource**/
extern DECLSPEC tpBool STDCALL ids_realse_id(IPiIDS *ids, int id);
/**ids release id resource**/
extern DECLSPEC tpBool STDCALL ids_realse_id_by_sock(IPiIDS *ids, IPiTCPSocket *sock);
/**ids release id resource**/
extern DECLSPEC tpBool STDCALL ids_realse_id_by_object(IPiIDS *ids, IPiObject *object);
/**ids use id resource**/
extern DECLSPEC tpBool STDCALL ids_use_id(IPiIDS *ids, IPiObject *object, int id);

/**ids alloc id resource and use**/
extern DECLSPEC int STDCALL ids_alloc_id(IPiIDS *ids, IPiObject *object);//return -1, could not find

#ifdef __cplusplus
}
#endif

#endif
