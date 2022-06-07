/* determine build type */
#if !defined(BUILD_DEBUG) && !defined(BUILD_RELEASE) && !defined(BUILD_CUSTOM)
    // if not set by -DBUILD_*, we try to figure out whether to build DEBUG/RELEASE
    // via conventional macros set by IDEs/compilers/etc.
    #if defined(NDEBUG) || defined(__OPTIMIZE__)
        #define BUILD_RELEASE
    #elif defined(DEBUG) || defined(_DEBUG)
        #define BUILD_DEBUG
    #else
        #define BUILD_DEBUG // default to debug if all else fails
    #endif
#endif

/* enable/disable features based on build type */
#if defined(BUILD_DEBUG)
    #define ENABLE_ASSERTS
    #define ENABLE_LOGGING
    #define ENABLE_WARNINGS
#elif defined(BUILD_RELEASE)
    #define ENABLE_WARNINGS
    #undef ENABLE_ASSERTS
    #undef ENABLE_LOGGING
#elif defined(BUILD_CUSTOM)
    /* user defined via -DENABLE_* */
#endif
