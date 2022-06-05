#pragma once

/* determine build type */
#if !defined(BUILD_DEBUG) && !defined(BUILD_RELEASE) && !defined(BUILD_CUSTOM)
// if not set by -DBUILD_*, we try to figure out whether to build DEBUG/RELEASE
// via conventional macros set by IDEs/compilers/etc.
    #if defined(NDEBUG) || defined(__OPTIMIZE__)
        #define BUILD_RELEASE
    #elif defined(DEBUG) || defined(_DEBUG)
        #define BUILD_DEBUG
    #else
        #define BUILD_DEBUG
    #endif
#endif

/* enable/disable features based on build type */
#if defined(BUILD_DEBUG)
    #define ENABLE_ASSERTS
    #define ENABLE_LOGGING
#elif defined(BUILD_RELEASE)
    #undef  ENABLE_ASSERTS
    #undef  ENABLE_LOGGING
#elif defined(BUILD_CUSTOM)
    /* user defined via -DENABLE_* */
#endif


#include "basic_platform.h"
#include "basic_debug.h"
#include "basic_math.h"
#include "basic_memory.h"
#include "basic_string.h"

#ifdef BASIC_IMPLEMENTATION // NOTE unused
#endif
