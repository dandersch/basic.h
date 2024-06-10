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

#include "basic_types.h"     // maybe merge
#include "basic_math.h"      // these two

#include "basic_memory.h"    // standalone
#include "basic_arena.h"     // depends on basic_memory.h
#include "basic_dynarr.h"    // depends on basic_memory.h

/* standalones: these do not depend on other headers */
#include "basic_macro.h"
#include "ext/utlist.h"      /* linked list macros (thirdparty) */
#include "log/log.h"
