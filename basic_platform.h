#pragma once 

/* 
 * includes detection of the OS, compiler, architecture, language std and
 * creates appropriate #defines for those platforms + some runtime detection
 * functions. Here are the most important #defines:
 * - PLATFORM_WIN32|LINUX|MACOS
 * - COMPILER_GCC|CLANG|MINGW|MSVC
 * - LANGUAGE_C|CPP
 * - STANDARD_C89|C99|C11|Cxx11|Cxx14|Cxx17|Cxx20|..
 * - STANDARD_VERSION (can go from 1989 to 2020)
 * - EXPORT (for cross-platform export declaration)
 * - CDECL  (specify c calling convention cross-platform)
 * - ...
 */

/* os detection */
typedef enum operating_system_e
{
    OPERATING_SYSTEM_UNKNOWN,
    OPERATING_SYSTEM_LINUX,
    OPERATING_SYSTEM_WIN32,
    OPERATING_SYSTEM_MACOS,
    OPERATING_SYSTEM_COUNT,
} operating_system_e;

// NOTE: maybe rename these to OP_SYS_WIN32 or similar...
#if defined(_WIN32)
    #define PLATFORM_WIN32
#elif defined(__gnu_linux__)
    #define PLATFORM_LINUX
#elif defined(__APPLE__) && defined(__MACH__)
    #define PLATFORM_MACOS // NOTE: untested
#else
    #error "No supported platform (OS) detected."
# endif

// extra "platforms" we might be interested in
#if defined(__EMSCRIPTEN__)
    #define PLATFORM_WEB // NOTE: untested
#endif

/* compiler detection */
#if defined(__GNUC__)
    #define COMPILER_GCC
    #ifdef __clang__ // NOTE clang also defines __GNUC__ since it implements the gcc extensions
        #define COMPILER_CLANG
        #undef COMPILER_GCC
    #endif
    #ifdef __MINGW32__ // NOTE mingw also defines __GNUC__
        #define COMPILER_MINGW
        #undef COMPILER_GCC
    #endif
#elif defined(_MSC_VER)
    #define COMPILER_MSVC
#else
    #error "No supported compiler detected."
#endif

/* architecture detection */
#if defined(_M_X64) || defined(__amd64__)
    #define ARCH_X64
#elif defined(__i386__) || defined(_M_IX86)
    #define ARCH_X86
#elif defined(__arm__) || defined(_M_ARM)
    #define ARCH_ARM   // NOTE: untested  
#elif defined(__aarch64__)
    #define ARCH_ARM64 // NOTE: untested
#else
//    #warning "Architecture not detected." // TODO not working on MSVC
#endif

/* define export declarations for .dll & .so files */
// NOTE: untested
// TODO COMPILER_CLANG?
#ifdef COMPILER_GCC
    // NOTE GCC exports every symbol to the ELF by default (so no keyword would be needed), 
    // unless -fvisibility=hidden is specified, then below keyword is needed for exporting
    #if __GNUC__ >= 4 // NOTE: taken from SDL
        #define EXPORT __attribute__((visibility("default")))
    #else
        #define EXPORT
    #endif
#elif defined(COMPILER_MSVC)
    #define EXPORT __declspec(dllexport)
#elif defined(COMPILER_MINGW)
    // NOTE apparently you can use both __attribute__() & __declspec() with mingw
    #define EXPORT __declspec(dllexport)
#endif

/* define c calling convention macro */
// NOTE: untested
#if defined(COMPILER_MSVC)
    #define CDECL  __cdecl
#elif defined(COMPILER_GCC)
    //#define CDECL __attribute__((__cdecl__)) // NOTE this define can cause warnings
    #define CDECL
#elif defined(COMPILER_MINGW)
    // NOTE mingw doesn't seem to care if this is defined or not
    #define CDECL  __cdecl
#elif defined(COMPILER_CLANG)
    // TODO define CDECL for COMPILER_CLANG, break compilation in the meantime
    #define CDECL  not_defined
#endif

// TODO Both __cplusplus and __STDC_VERSION__, __STDC__ are not defined by MSVC
// instead, it defines _MSC_VER, which can give us Visual C++ version (whatever that is..)
#if defined(__cplusplus)
    #define LANGUAGE_CPP

    #define STANDARD_VERSION (__cplusplus/100)

    #if   __cplusplus >= 199701L && __cplusplus < 200400L
        // NOTE: __cplusplus == 199711 for -std=c++98/c++03 for both clang & gcc
        #define STANDARD_Cxx98
        #define STANDARD_Cxx03
    #elif __cplusplus >= 201101L && __cplusplus < 201200L
        #define STANDARD_Cxx11
    #elif __cplusplus >= 201401L && __cplusplus < 201500L
        #define STANDARD_Cxx14
    #elif __cplusplus >= 201701L && __cplusplus < 201800L
        #define STANDARD_Cxx17
    #elif __cplusplus >= 202001L && __cplusplus < 202100L
        #define STANDARD_Cxx20
    #else
//        #warning "C++ standard not detected" // TODO not working on MSVC
    #endif
#elif defined(__STDC_VERSION__)
    #define LANGUAGE_C

    #define STANDARD_VERSION (__STDC_VERSION__/100)

    #if   __STDC_VERSION__ >= 198901L && __STDC_VERSION__ < 199000L
        #define STANDARD_C89 // NOTE: not supported
    #elif __STDC_VERSION__ >= 199901L && __STDC_VERSION__ < 200000L
        #define STANDARD_C99
    #elif __STDC_VERSION__ >= 201101L && __STDC_VERSION__ < 201200L
        #define STANDARD_C11
    #elif __STDC_VERSION__ >= 201701L && __STDC_VERSION__ < 201800L
        #define STANDARD_C17
    #else
//        #warning "C standard not detected" // TODO not working on MSVC
    #endif
#else
//    #warning "Language not detected (C or C++)" // TODO not working on MSVC
#endif

/* functions for runtime detection */
inline static operating_system_e platform_detect_os()
{
    operating_system_e os = OPERATING_SYSTEM_UNKNOWN;
#if   defined(PLATFORM_WIN32)
    os = OPERATING_SYSTEM_WIN32;
#elif defined(PLATFORM_LINUX)
    os = OPERATING_SYSTEM_LINUX;
#elif defined(PLATFORM_MACOS)
    os = OPERATING_SYSTEM_MACOS;
#endif
    return os;
}
inline static const char* platform_os_string(operating_system_e os)
{
    const char* string = "Unknown";
    switch (os)
    {
      case OPERATING_SYSTEM_LINUX: { string = "Linux";   } break;
      case OPERATING_SYSTEM_WIN32: { string = "Windows"; } break;
      case OPERATING_SYSTEM_MACOS: { string = "MacOS";   } break;
      default: { } break; // UNREACHABLE
    }
    return string;
}
