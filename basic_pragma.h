// MSVC syntax: #pragma warning( disable : 4507 34; once : 4385; error : 164 )
// MSVC keywords:
//   default
//   disable
//   error
//   once
//   suppress (disables warning only for next line)
// GCC syntax: #pragma GCC diagnostic warning "-Wname"
// GCC keywords:
//   warning
//   error
//   ignored

/* cross-platform #pragma's */
#if defined(_MSC_VER) // NOTE: so we can be included before basic_platform.h
    #define DO_PRAGMA(x) __pragma(x)

    #define WARNING_TO_ENABLE(flag,code) DO_PRAGMA(warning(default : code))
    #define WARNING_TO_ERROR(flag,code)  DO_PRAGMA(warning(error   : code))
    #define WARNING_TO_IGNORE(flag,code) DO_PRAGMA(warning(disable : code))
    #define PUSH_WARNINGS() DO_PRAGMA(warning(push))
    #define POP_WARNINGS()  DO_PRAGMA(warning(pop))

    #define WARNING(msg)    DO_PRAGMA(message(msg))

    #define PUSH_STRUCT_PACK(n) DO_PRAGMA(pack(push,n))
    #define POP_STRUCT_PACK()   DO_PRAGMA(pack(pop))
#else
    #define DO_PRAGMA(x) _Pragma (#x)
    #define WARNING_TO_ENABLE(flag,code) DO_PRAGMA(GCC diagnostic warning flag)
    #define WARNING_TO_ERROR(flag,code)  DO_PRAGMA(GCC diagnostic error   flag)
    #define WARNING_TO_IGNORE(flag,code) DO_PRAGMA(GCC diagnostic ignored flag)

    #define PUSH_WARNINGS() DO_PRAGMA(GCC diagnostic push)
    #define POP_WARNINGS()  DO_PRAGMA(GCC diagnostic pop)

    // NOTE: to not show "expanded from macro.." in gcc: -ftrack-macro-expansion=0 -fno-diagnostics-show-caret
    #define WARNING(msg) DO_PRAGMA(GCC warning msg)

    #define PUSH_STRUCT_PACK(n) DO_PRAGMA(pack(push,n))
    #define POP_STRUCT_PACK()   DO_PRAGMA(pack(pop))
#endif
