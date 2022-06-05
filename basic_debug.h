
/* asserts */
#ifdef ENABLE_ASSERTS
#ifdef COMPILER_MINGW
    #define _POSIX // needed for mingw to include SIGTRAP
#endif
#include <signal.h> // for SIGTRAP
#include <stdio.h>  // for fprintf, stderr

    #ifdef COMPILER_MSVC
        #define DEBUG_BREAK() DebugBreak()
    #else
        #define DEBUG_BREAK() raise(SIGTRAP)
    #endif

    // NOTE: if __func__ is not supported, we might be able to use __FUNCTION__
    #define _ASSERT_STRING "Assert failed for '%s' in file '%s', function: '%s' at line '%d'\n"
    #define ASSERT(expr)                                                      \
    if (expr) { }                                                             \
    else                                                                      \
    {                                                                         \
        fprintf(stderr, _ASSERT_STRING, #expr, __FILE__, __func__, __LINE__); \
        DEBUG_BREAK();                                                        \
    }

    #define UNREACHABLE(msg, ...) { fprintf(stderr,msg,##__VA_ARGS__); ASSERT(false); }
#else
    #define ASSERT(expr)          (void)0
    #define UNREACHABLE(msg, ...) (void)0
#endif

/* static_assert */
#if   defined(LANGUAGE_CPP) && STANDARD_VERSION >= 2011
    // C++11 has built-in static_assert
    #define STATIC_ASSERT(expr, msg) static_assert(expr, msg)
#elif defined(LANGUAGE_C) && STANDARD_VERSION >= 2011
    #define STATIC_ASSERT(expr, msg) _Static_assert(expr, msg)
#else
//    #warning "No STATIC_ASSERT for compiler" // TODO not working on MSVC
    #define STATIC_ASSERT(expr, msg)
    // TODO portable static_assert?
    //#define static_assert(c,l) typedef u64 Glue(l,__LINE__) [(c)?1:-1]
    //#define Glue_(A,B) A##B
    //#define Glue(A,B) Glue_(A,B)
#endif

/* TODO Logger */

//enum log_severity_e
//{
//    LOG_INFO,
//    LOG_WARNING,
//    LOG_CRITICAL,
//    LOG_SEVERITY_COUNT,
//};
//
//enum log_target_e
//{
//    LOG_CONSOLE,
//    LOG_FILE,
//    LOG_TARGET_COUNT,
//};
//
//// TODO maybe use a macro with LOG(msg, severity, target, ...) printf(msg, __VA_ARGS__)
//void debug_log(string_t str, log_severity_e severity, log_target_e target)
//{
//    switch (severity)
//    {
//    }
//
//    switch (target)
//    {
//      case LOG_CONSOLE: { printf("%s", string_null_terminate(str)); } break;
//      case LOG_FILE: { } break;
//    }
//}
