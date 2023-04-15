#pragma once

/* idea: put together a replacement for the c standard library, consisting of
 * own code & public domain libraries */

/* TODO:
 * [X] linked list macros
 * [ ] hashtable
 * [ ] dynamic array
 * [ ] custom imgui
 * [ ] logging
 * [ ] a string replacement (no null terminated strings)
 * [ ] (pseudo) random number generator
 * [ ] file & filepath operations
 * [ ] introspection (maybe with registrars)
 * [ ] threads
 * [ ] json/csv/... parser
 * [ ] sorting algorithms
 * [ ] way to get a window up crossplatform
 * [ ] cross platform event system
 * [ ] crossplatform audio library (miniaudio?)
 * [ ] custom, crossplatform opengl loader
 * [ ] custom, opengl 2d renderer
 * [ ] stb_img, stb_truetype
 *
 * for what else to add and how to do it, see
   https://nullprogram.com/blog/2023/02/11/
   https://nullprogram.com/blog/2023/02/15/
 */

#include "basic_pragma.h"
#include "basic_types.h"
#include "basic_buildtype.h"
#include "basic_platform.h"
#include "basic_warnings.h"
#include "basic_debug.h"
#include "basic_math.h"
#include "basic_thread.h"
#include "basic_memory.h"
#include "basic_arena.h"
#include "basic_linkedlist.h"
#include "basic_dynarr.h"
//#include "basic_string.h"
