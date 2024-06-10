
/* asserts */
#ifdef ENABLE_ASSERTS
    #ifdef COMPILER_MINGW
        #define _POSIX // needed for mingw to include SIGTRAP
    #endif
#include <stdio.h>  // for fprintf, stderr

    // NOTE way to check for builtins: __has_builtin(__builtin_debugtrap)

    // here a list of ways to implement a DEBUG_BREAK macro and on what OS+compiler they work
    // see https://github.com/scottt/debugbreak for more info
    //   __builtin_debugtrap();              linux: clang(++)                   windows: clang-cl.exe clang(++)
    //   __builtin_trap();                   linux: clang(++) gcc(++) mingw(++) windows: clang(++) clang-cl.exe
    //   #include <signal.h> raise(SIGTRAP); linux: clang(++) gcc(++) mingw(++) windows: none
    //   DebugBreak()                        linux: none                        windows: cl.exe clang-cl.exe
    //   __debugbreak()                      linux: none                        windows: cl.exe clang-cl.exe clang(++)
    //   __asm__ volatile("int $0x03");      trap instruction, x86 specific
    #ifdef COMPILER_MSVC
        #define DEBUG_BREAK() __debugbreak()
    #else
        #if defined(ARCH_X86) || defined(ARCH_X64)
            #define DEBUG_BREAK() __asm__ volatile("int $0x03")
        #else
            WARNING("No DEBUG_BREAK() defined for this architecture.")
        #endif
    #endif

    // NOTE: __func__ seems to be supported everywhere (alternative in case: __FUNCTION__)
    #define _ASSERT_STRING "Assert failed for '%s' in file '%s', function: '%s' at line '%d'\n"
    #define ASSERT(expr)                                                      \
    if (expr) { }                                                             \
    else                                                                      \
    {                                                                         \
        fprintf(stderr, _ASSERT_STRING, #expr, __FILE__, __func__, __LINE__); \
        DEBUG_BREAK();                                                        \
    }

    // TODO try to use __builtin_unreachable
    #define UNREACHABLE(msg, ...) do { fprintf(stderr,msg,##__VA_ARGS__); ASSERT(0); } while (0)

    #define UNIMPLEMENTED UNREACHABLE("Call to unimplemented function %s", __FUNCTION__)

#else
    #define ASSERT(expr)          (void)0
    #define UNREACHABLE(msg, ...) (void)0
#endif

/*
 * TODO
 * compile-time macro for functions that aren't finished. Usage: TODO int my_func() { }
 */
#if defined(LANGUAGE_CPP)
  // works with GCC, G++, CLANG++, MSVC++, CLANG++.exe
  #define TODO [[deprecated("not implemented")]]
#elif defined(LANGUAGE_C) && !defined(COMPILER_MSVC)
  // works with GCC, G++, CLANG++, CLANG, CLANG.exe, CLANG++.exe
  #define TODO __attribute__((error("not implemented")))
#else
  // no equivalent when compiling ".c" files with msvc
  #define TODO WARNING("no UNIMPLEMENTED macro available")
#endif

/* static_assert */
#if defined(LANGUAGE_CPP) && STANDARD_VERSION >= 2011
    // C++11 has built-in static_assert
    #define STATIC_ASSERT(expr, msg) static_assert(expr, msg)
#elif defined(LANGUAGE_C) && STANDARD_VERSION >= 2011
    #define STATIC_ASSERT(expr, msg) _Static_assert(expr, msg)
#else
    // portable static_assert, but message string is lost
    #define STATIC_ASSERT(expr, msg) typedef char static_assertion[(expr)?1:-1]
#endif

#if defined(BUILD_DEBUG)
    #if defined(PLATFORM_WIN32)
        //#include <debugapi.h>
        #include <windows.h>
        static int debug_running_under_debugger()
        {
            return IsDebuggerPresent();
        }
    #else
        #include <sys/ptrace.h>
        static int debug_running_under_debugger()
        {
            static int debugger_detected;
            static int debugger_present = 0;
            if (!debugger_detected)
            {
                debugger_detected = 1;
                debugger_present = (ptrace(PTRACE_TRACEME, 0, 0, 0) == -1);
            }
            return debugger_present;
        }
    #endif
#endif
