#pragma once

/* thread local storage */
// TODO how do these compare vs. C++11's thread_local?
#if defined(COMPILER_MSVC)
    #define thread_var __declspec(thread)
#else
    #define thread_var __thread
#endif
