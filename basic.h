#pragma once

/* idea: put together a replacement for the c standard library, consisting of
 * own code & public domain libraries */

/* TODO:
 * [-] dynamic array
 * [ ] hashtable
 * [ ] strings & string api
 * [ ] (pseudo) random number generator
 * [ ] file & filepath operations
 * [ ] introspection (maybe with registries)
 * [ ] threads
 *
 * for what else to add and how to do it, see
   https://nullprogram.com/blog/2023/02/11/
   https://nullprogram.com/blog/2023/02/15/
 */

#include "platform.h"

#include "buildtype.h" /* depends on detected buildtype from platform.h */
#include "maths.h"     /* depends on typedefs from platform.h */

#ifdef BASIC_IMPLEMENTATION
  #define MEMORY_IMPLEMENTATION
#endif
#include "memory/memory.h"

#ifdef BASIC_IMPLEMENTATION
  #define MEM_ARENA_IMPLEMENTATION
  //#define MEM_ARENA_OS_ALLOC(size) mem_alloc(size)
  //#define MEM_ARENA_OS_FREE(ptr) mem_free(size)
  #define MEM_ARENA_OS_RESERVE(size)      mem_reserve(NULL, size)
  #define MEM_ARENA_OS_COMMIT(ptr,size)   mem_commit(ptr, size)
  #define MEM_ARENA_OS_RELEASE(ptr,size)  mem_release(ptr, size)
  #define MEM_ARENA_OS_DECOMMIT(ptr,size) mem_decommit(ptr, size)
#endif
#include "arena/arena.h"

#include "basic_dynarr.h"    /* depends on basic_memory.h */

/* standalones: these do not depend on other headers or on each other */
#include "macros.h"
#include "ext/utlist.h"      /* linked list macros (thirdparty) */
#include "log/log.h"
