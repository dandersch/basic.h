#pragma once

/* common macros */
#define ARRAY_COUNT(arr)  (sizeof(arr)/sizeof(arr[0]))

#if !defined(COMPILER_MSVC)
    #define OFFSET_OF(type, member) __builtin_offsetof(type, member)
#else
    #define OFFSET_OF(s,m) ((size_t)&(((s*)0)->m)) // used by MSVC
#endif

/* TODO memory arena */
