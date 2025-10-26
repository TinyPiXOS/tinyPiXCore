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
#ifndef __RECT_H
#define __RECT_H

#include "TpWMEngine/dataDefine.h"

#ifdef __cplusplus
extern "C"{
#endif

/**
*create rect by width and height
*/
extern DECLSPEC PiRect STDCALL rect_size_create(int x, int y, unsigned int w, unsigned int h);

/**
*create rect by coordinate
*/
extern DECLSPEC PiRect STDCALL rect_coor_create(int x1, int y1, int x2, int y2);

/**
*caculate interact s1 with s2
*/
extern DECLSPEC tpBool  STDCALL rect_interact(PiRect *s1, PiRect *s2, PiRect *result);

/**
*caculate union s1 with s2
*/
extern DECLSPEC tpBool STDCALL rect_union(PiRect *s1, PiRect *s2, PiRect *result);

/**
*whether a int is inside a rect with x and y
*/
extern DECLSPEC tpBool STDCALL xy_in_rect(int x, int y, PiRect *rect);

/**
*whether a point is inside a rect with a point
*/
extern DECLSPEC tpBool STDCALL point_in_rect(PiPoint p, PiRect *rect);

/**
*check rect is validate
*/
extern DECLSPEC tpBool STDCALL rect_is_validate(PiRect *rect);

/**
*rect is equal ?
*/
extern DECLSPEC tpBool STDCALL rect_equals(PiRect *src_rect, PiRect *dst_rect);

/**
*rect includes other ?
*/
extern DECLSPEC tpBool STDCALL rect_in_rect(PiRect *src_rect, PiRect *dst_rect);

#ifdef __cplusplus
}
#endif


#endif
