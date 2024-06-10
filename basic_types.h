#pragma once

/* u32, f32, ... types */
#include <stdint.h>   // TODO find a way to avoid this include
typedef int8_t    i8;
typedef int16_t   i16;
typedef int32_t   i32;
typedef int64_t   i64;

typedef int8_t    s8;
typedef int16_t   s16;
typedef int32_t   s32;
typedef int64_t   s64;

typedef uint8_t   u8;
typedef uint16_t  u16;
typedef uint32_t  u32;
typedef uint64_t  u64;

typedef int8_t    b8;
typedef int16_t   b16;
typedef int32_t   b32;
typedef int64_t   b64;

typedef float     f32;
typedef double    f64;

// typedef uintptr_t umm; /* stand-in for void* */

/* numerical limits */
#include <float.h>  // for FLT_MAX/MIN
#define U8_MIN  0
#define U8_MAX  255
#define U16_MIN 0
#define U16_MAX 65535
#define U32_MIN 0
#define U32_MAX 4294967295
#define U64_MIN 0
#define U64_MAX 18446744073709551615
#define I32_MIN ((i32) 0x80000000)
#define I32_MAX ((i32) 0x7FFFFFFF)
#define S32_MIN ((s32) 0x80000000)
#define S32_MAX ((s32) 0x7FFFFFFF)
#define F32_MIN -FLT_MAX
#define F32_MAX FLT_MAX
