#pragma once 

/* 
 * includes detection of the OS, compiler, architecture, language std and
 * creates appropriate #defines for those platforms + some runtime detection
 * functions. Here are the most important #defines:
 * - PLATFORM_{WIN32|LINUX|MACOS}
 * - COMPILER_{GCC|CLANG|MINGW|MSVC|TCC}
 * - LANGUAGE_{C|CPP}
 * - STANDARD_{C89|C99|C11|C17|Cxx98|Cxx03|Cxx11|Cxx14|Cxx17|Cxx20}
 * - STANDARD_VERSION: Integer between 1989 and 2020
 * - EXPORT: for export declaration, syntax: EXPORT void func()
 * - CDECL:  c calling convention, syntax: void CDECL func()
 * - ...
 */

/* os detection */
#if defined(_WIN32)
    #define PLATFORM_WIN32
#elif defined(__gnu_linux__) || defined(__linux__)
    #define PLATFORM_LINUX
#elif defined(__APPLE__) && defined(__MACH__)
    #define PLATFORM_MACOS // NOTE: untested
#else
    #error "No supported platform (OS) detected."
# endif

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

#elif defined(__clang__) // NOTE for clang++ on windows
    #define COMPILER_CLANG
#elif defined(__TINYC__)
    #define COMPILER_TCC
#elif defined(_MSC_VER)
    #define COMPILER_MSVC
#else
    #error "No supported compiler detected."
#endif

/* architecture detection */
#if defined(_M_X64) || defined(__amd64__)
    #define ARCH_X64
#elif defined(__i386__) || defined(_M_IX86) || defined(__x86_64__)
    #define ARCH_X86
#elif defined(__arm__) || defined(_M_ARM)
    #define ARCH_ARM   // NOTE: untested  
#elif defined(__aarch64__)
    #define ARCH_ARM64 // NOTE: untested
#else
    #warning "Architecture not detected"
#endif

/* define export declarations for .dll & .so files */
#if defined(COMPILER_GCC)
    // NOTE GCC exports every symbol to the ELF by default, unless -fvisibility=hidden is specified
    #if __GNUC__ >= 4 // NOTE: taken from SDL
        #define EXPORT __attribute__((visibility("default")))
    #else
        #define EXPORT
    #endif
#elif defined(COMPILER_CLANG)
    #define EXPORT __attribute__((visibility("default")))
#elif defined(COMPILER_MSVC)
    #define EXPORT __declspec(dllexport)
#elif defined(COMPILER_MINGW)
    // NOTE apparently you can use both __attribute__() & __declspec() with mingw
    #define EXPORT __declspec(dllexport)
#elif defined(COMPILER_TCC) && defined(PLATFORM_WIN32)
    #define EXPORT __attribute__(dllexport)
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
#elif defined(COMPILER_TCC)
    #define CDECL __attribute__(cdecl) // TODO untested
#endif

#if defined(__cplusplus)
    #define LANGUAGE_CPP

    #if defined(_MSVC_LANG)
        #define STANDARD_VERSION (_MSVC_LANG/100)
    #else
        #define STANDARD_VERSION (__cplusplus/100)
    #endif

    // NOTE: maybe we don't need this...
    #if   STANDARD_VERSION >= 1997 && STANDARD_VERSION < 2004
        // NOTE: __cplusplus == 199711 for -std=c++98/c++03 for both clang & gcc
        #define STANDARD_Cxx98
        #define STANDARD_Cxx03
    #elif STANDARD_VERSION >= 2011 && STANDARD_VERSION < 2012
        #define STANDARD_Cxx11
    #elif STANDARD_VERSION >= 2014 && STANDARD_VERSION < 2015
        #define STANDARD_Cxx14
    #elif STANDARD_VERSION >= 2017 && STANDARD_VERSION < 2018
        #define STANDARD_Cxx17
    #elif STANDARD_VERSION >= 2020 && STANDARD_VERSION < 2021
        #define STANDARD_Cxx20
    #else
        WARNING("C++ standard not detected")
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
        WARNING("C standard not detected")
    #endif
#else
    WARNING("Language not detected (C or C++)")
#endif

/* functions / strings for runtime detection */

#define PLATFORMS(X)        \
    X(UNKNOWN, "Unknown")   \
    X(LINUX,   "Linux")     \
    X(WIN32,   "Windows")   \
    X(MACOS,   "MacOS")     \
    X(COUNT,   "Invalid")

#define OPERATING_SYSTEM_ENUM(e, ...) OPERATING_SYSTEM_##e,
typedef enum operating_system_e
{
    PLATFORMS(OPERATING_SYSTEM_ENUM)
} operating_system_e;

#define COMPILERS(X)      \
    X(UNKNOWN, "Unknown") \
    X(CLANG,   "Clang")   \
    X(GCC,     "GCC")     \
    X(MINGW,   "MinGW")   \
    X(MSVC,    "MSVC")    \
    X(TCC,     "TCC")     \
    X(COUNT,   "Invalid")

#define COMPILER_ENUM(e, ...) COMPILER_TYPE_##e, // NOTE: _TYPE_ to not clash with the (more important) macro
typedef enum compiler_e
{
    COMPILERS(COMPILER_ENUM)
} compiler_e;

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

#define OPERATING_SYSTEM_STRING(e, str, ...) case OPERATING_SYSTEM_##e: { string = str; } break;
inline static const char* platform_os_string(operating_system_e os)
{
    const char* string = "Unknown";
    switch (os)
    {
        PLATFORMS(OPERATING_SYSTEM_STRING);
        default: { string = "Invalid"; } break; // UNREACHABLE
    }
    return string;
}

inline static compiler_e platform_detect_compiler()
{
    compiler_e compiler = COMPILER_TYPE_UNKNOWN;
#if   defined(COMPILER_CLANG)
    compiler = COMPILER_TYPE_CLANG;
#elif defined(COMPILER_GCC)
    compiler = COMPILER_TYPE_GCC;
#elif defined(COMPILER_MINGW)
    compiler = COMPILER_TYPE_MINGW;
#elif defined(COMPILER_MSVC)
    compiler = COMPILER_TYPE_MSVC;
#elif defined(COMPILER_TCC)
    compiler = COMPILER_TYPE_TCC;
#endif
    return compiler;
}

#define COMPILER_STRING(e, str, ...) case COMPILER_TYPE_##e: { string = str; } break;
inline static const char* platform_compiler_string(compiler_e compiler)
{
    const char* string = "Unknown";
    switch (compiler)
    {
        COMPILERS(COMPILER_STRING);
        default: { string = "Invalid"; } break; // UNREACHABLE
    }
    return string;
}

#if defined(ARCH_X64)
  #define ARCHITECTURE_STRING "x64"
#elif defined(ARCH_X86)
  #define ARCHITECTURE_STRING "x86"
#elif defined(ARCH_ARM)
  #define ARCHITECTURE_STRING "ARM"
#endif

#if   defined(STANDARD_C89)
  #define C_STANDARD_STRING "C89"
#elif defined(STANDARD_C99)
  #define C_STANDARD_STRING "C99"
#elif defined(STANDARD_C11)
  #define C_STANDARD_STRING "C11"
#elif defined(STANDARD_C17)
  #define C_STANDARD_STRING "C17"
#elif defined(STANDARD_Cxx98)
  #define C_STANDARD_STRING "C++98"
#elif defined(STANDARD_Cxx03)
  #define C_STANDARD_STRING "C++03"
#elif defined(STANDARD_Cxx11)
  #define C_STANDARD_STRING "C++11"
#elif defined(STANDARD_Cxx14)
  #define C_STANDARD_STRING "C++14"
#elif defined(STANDARD_Cxx17)
  #define C_STANDARD_STRING "C++17"
#elif defined(STANDARD_Cxx20)
  #define C_STANDARD_STRING "C++20"
#endif

#if defined(BUILD_DEBUG)
  #define BUILD_TYPE_STRING "DEBUG"
#elif defined(BUILD_RELEASE)
  #define BUILD_TYPE_STRING "RELEASE"
#elif defined(BUILD_CUSTOM)
  #define BUILD_TYPE_STRING "CUSTOM"
#else
  #error "No build type set or detected."
#endif
