#pragma once

/* common macros */
#define ARRAY_COUNT(arr)  (sizeof(arr)/sizeof(arr[0]))

// TODO doesn't work on MSVC
//#define OFFSET_OF(type, member) __builtin_offsetof(type, member)

/* TODO memory arena */
