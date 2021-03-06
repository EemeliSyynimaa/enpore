#ifndef PLATFORM_H
#define PLATFORM_H

#include <stdint.h>

typedef float       f32;
typedef double      f64;

typedef int8_t      s8;
typedef int16_t     s16;
typedef int32_t     s32;
typedef int64_t     s64;

typedef uint8_t     u8;
typedef uint16_t    u16;
typedef uint32_t    u32;
typedef uint64_t    u64;

typedef int32_t     b32;

typedef struct
{
    f32 x;
    f32 y;
} v2f;

typedef struct
{
    s32 x;
    s32 y;
} v2i;

typedef struct
{
    s32 width;
    s32 height;
    s32 *data;
} bitmap_t;

#endif