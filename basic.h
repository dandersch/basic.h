#pragma once

/* idea: put together a replacement for the c standard library, consisting of
 * own code & public domain libraries */

/* TODO:
 * [-] dynamic array
 * [ ] hashtable
 * [ ] strings & string api
 * [ ] (pseudo) random number generator
 * [ ] file & filepath operations
 * [ ] threads
 *
 * for what else to add and how to do it, see
   https://nullprogram.com/blog/2023/02/11/
   https://nullprogram.com/blog/2023/02/15/
 */

#include "platform/platform.h"

/* enable/disable features based on build type */
#if defined(BUILD_DEBUG)
    #define ENABLE_ASSERTS
    #define ENABLE_LOGGING
    #define ENABLE_WARNINGS
#elif defined(BUILD_RELEASE)
    #define ENABLE_WARNINGS
    #undef ENABLE_ASSERTS
    #undef ENABLE_LOGGING
#elif defined(BUILD_CUSTOM)
    /* user defined via -DENABLE_* */
#endif

#ifdef ENABLE_WARNINGS
  /* error when shadowing variables */
  WARNING_TO_ERROR("-Wshadow", 6246) // NOTE: MSVC needs /analyze flag
  WARNING_TO_ERROR("-Wshadow", 6244) // NOTE: MSVC needs /analyze flag

  /* error when doing sizeof(pointer)/sizeof(pointer[0]) */
  WARNING_TO_ERROR("-Wsizeof-pointer-div", 6384) // NOTE: MSVC needs /analyze flag

  /* warning for: float f = (3/4); // == 0 when expecting 0.75 */
  /* NOTE: This also warns about a whole lot of other (reasonable) conversions */
  //WARNING_TO_ENABLE("-Wconversion", Unknown) // Warn for implicit conversions that may alter a value.

  /* Other warnings to consider: */
  //WARNING_TO_IGNORE("-Wunknown-pragmas", 4068)
#endif

#ifdef ENABLE_LOGGING /* NOTE: unused */
#endif

#ifndef ENABLE_ASSERTS
  #define ASSERT (void)0
  #define UNREACHABLE(msg, ...) (void)0
#endif

#include "maths.h"     /* depends on typedefs from platform.h */

#ifdef BASIC_IMPLEMENTATION
  #define MEMORY_IMPLEMENTATION
#endif
#include "memory/memory.h"

#ifdef BASIC_IMPLEMENTATION
  #define MEM_ARENA_IMPLEMENTATION
  //#define MEM_ARENA_OS_ALLOC(size)        mem_alloc(size)
  //#define MEM_ARENA_OS_FREE(ptr)          mem_free(size)
  #define MEM_ARENA_OS_RESERVE(size)      mem_reserve(NULL, size)
  #define MEM_ARENA_OS_COMMIT(ptr,size)   mem_commit(ptr, size)
  #define MEM_ARENA_OS_RELEASE(ptr,size)  mem_release(ptr, size)
  #define MEM_ARENA_OS_DECOMMIT(ptr,size) mem_decommit(ptr, size)
#endif
#include "memory/mem_arena.h"

#include "dynarr.h"    /* depends on memory.h */

/* standalones: these do not depend on other headers or on each other */
#include "macros.h"
#include "ext/utlist.h"      /* linked list macros (thirdparty) */
#include "log/log.h"
