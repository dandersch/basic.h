#pragma once

/* idea: put together a replacement for the c standard library, consisting of
 * own code & public domain libraries */

/* TODO:
 * [-] dynamic array
 * [ ] hashtable
 * [ ] custom imgui
 * [ ] (pseudo) random number generator
 * [ ] file & filepath operations
 * [ ] introspection (maybe with registries)
 * [ ] threads
 * [ ] json/csv/... parser
 * [ ] crossplatform audio library (miniaudio?)
 * [ ] custom, crossplatform opengl loader
 * [ ] custom, opengl 2d renderer
 * [ ] stb_img, stb_truetype
 *
 * for what else to add and how to do it, see
   https://nullprogram.com/blog/2023/02/11/
   https://nullprogram.com/blog/2023/02/15/
 */

#include "basic_platform.h"

#include "basic_buildtype.h" // depends on basic_platform.h  TODO maybe rename to basic_build.h

#include "basic_math.h"      // depends on basic_platform.h


#include "basic_memory.h"    // standalone


//#define MEM_ARENA_OS_ALLOC(size) mem_alloc(size)
//#define MEM_ARENA_OS_FREE(ptr) mem_free(size)
#define MEM_ARENA_OS_RESERVE(size)      mem_reserve(NULL, size)
#define MEM_ARENA_OS_COMMIT(ptr,size)   mem_commit(ptr, size)
#define MEM_ARENA_OS_RELEASE(ptr,size)  mem_release(ptr, size)
#define MEM_ARENA_OS_DECOMMIT(ptr,size) mem_decommit(ptr, size)

#ifdef BASIC_IMPLEMENTATION
  #define MEM_ARENA_IMPLEMENTATION
#endif

#include "basic_arena.h"     // standalone


#include "basic_dynarr.h"    // depends on basic_memory.h

/* standalones: these do not depend on other headers */
#include "basic_macro.h"
#include "ext/utlist.h"      /* linked list macros (thirdparty) */
#include "log/log.h"
