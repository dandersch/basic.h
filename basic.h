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

#include "log/log.h"

#include "basic_pragma.h"
#include "basic_types.h"
#include "basic_buildtype.h"
#include "basic_platform.h"
#include "basic_warnings.h"
#include "basic_debug.h"
#include "basic_math.h"
#include "basic_thread.h"
#include "basic_memory.h"
#include "basic_macro.h"
#include "basic_arena.h"
#include "basic_linkedlist.h"
#include "basic_dynarr.h"
//#include "basic_string.h"
