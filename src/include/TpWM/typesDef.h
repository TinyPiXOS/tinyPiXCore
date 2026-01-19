#ifndef __TYPES_DEF_H
#define __TYPES_DEF_H

#include <locale.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

#ifdef __cplusplus
#include <cstdint>
#endif

// only support for linux
#ifndef DECLSPEC
#define DECLSPEC __attribute__((visibility("default")))
#endif

#define STDCALL
#define TP_DEF_VOID_TYPE_VAR(v) \
    typedef void v

#define TP_INVALIDATE_VALUE -1

#ifndef TP_MIN
#define TP_MIN(x, y) ((x) > (y) ? (y) : (x))
#endif

#ifndef TP_MAX
#define TP_MAX(x, y) ((x) < (y) ? (y) : (x))
#endif

#ifndef TP_ABS
#define TP_ABS(x) ((x) < 0 ? (-(x)) : (x))
#endif

#ifndef M_PI
#define M_PI 3.1415926535897932384626433832795
#endif

#define _RGB(r, g, b) (((r) << 24) | ((g) << 16) | ((b) << 8) | 0xff)
#define _RGBA(r, g, b, a) (((r) << 24) | ((g) << 16) | ((b) << 8) | (a) & 0xff)

#define _R(c) (((c) >> 24) & 0xff)
#define _G(c) (((c) >> 16) & 0xff)
#define _B(c) (((c) >> 8) & 0xff)
#define _A(c) (((c)) & 0xff)

typedef enum
{
    TP_FALSE,
    TP_TRUE,
} tpBool;

#ifndef __cplusplus

typedef unsigned char tpUChar;
typedef unsigned char tpUInt8;
typedef unsigned short tpUInt16;
typedef unsigned int tpUInt32;
typedef unsigned long tpUInt64;

typedef char tpChar;
typedef char tpInt8;
typedef short tpInt16;
typedef int tpInt32;
typedef long tpInt64;

typedef float tpFloat;
typedef double tpDouble;

#else

typedef unsigned char tpUChar;
typedef uint8_t tpUInt8;
typedef uint16_t tpUInt16;
typedef uint32_t tpUInt32;
typedef uint64_t tpUInt64;

typedef char tpChar;
typedef int8_t tpInt8;
typedef int16_t tpInt16;
typedef int32_t tpInt32;
typedef int64_t tpInt64;

typedef float tpFloat;
typedef double tpDouble;

#endif

/**rotate defination**/
typedef enum
{
    TP_ROT_NONE = -1, // auto defination
    TP_ROT_0 = 0,
    TP_ROT_90 = 90,
    TP_ROT_90X = -90,
    TP_ROT_180 = 180,
    TP_ROT_180X = -180,
    TP_ROT_270 = 270,
    TP_ROT_270X = -270,
    TP_ROT_360 = 360,
} ITpRotateType,
    ITpRotate;

typedef enum
{
    TP_RGB_UNKOWN = -1,
    TP_RGB_8 = 8,   // only support RGB  for 256 colors
    TP_RGB_16 = 16, // only support RGB  for 16 true color
    TP_RGB_24 = 24, // only support RGB  for 24 true color
    TP_RGB_32 = 32, // only support RGBA for 32 true color
} ITpFormat;

#endif
