/*
    TpWM 内部私有的结构体定义
*/

#ifndef __TP_DATA_DEFINE_H
#define __TP_DATA_DEFINE_H

#include "typesDef.h"
#include <time.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <wchar.h>
#include <unistd.h>

typedef struct PiPoint 
{
    tpInt32 x;
    tpInt32 y;
} PiPoint;

typedef struct PiRect
{
    tpInt32 x;
    tpInt32 y;
    tpInt32 w;
    tpInt32 h;
} PiRect;

typedef struct PiSize
{
    tpInt32 w;
    tpInt32 h;
} PiSize;

/**RGBA defination*/
typedef struct PiRGBA
{
    tpUInt8 r;
    tpUInt8 g;
    tpUInt8 b;
    tpUInt8 a;
} PiRGBA;

#endif
