#pragma once

/* define common, useful macros such as
 *    scoped_begin_end(start, end) : runs start at begin and end at end of the following scope
 *    scoped_defer(...) : runs all given expressions at the end of the following scope in the order given
 *    defer(...) : runs given code at end of the scope (only available in C++11 and up)
 */

/* helper macros */
#define TOKEN_PASTE(a, b) a##b
#define CONCAT(a,b) TOKEN_PASTE(a,b)
#define UNIQUE_VAR(name) CONCAT(name, __LINE__)

/*
 * SCOPED MACROS: these macros require you to open a scope afterwards {}.
 * The prefixed "scoped_" is there to act as a mnemonic
 */

/* Usage: scoped_begin_end(printf("First"), printf("Last\n")) { ... } */
#define scoped_begin_end(start, end) \
    for (int UNIQUE_VAR(_i_) = (start, 0); !UNIQUE_VAR(_i_); (UNIQUE_VAR(_i_) += 1), end)

/*
 * NOTE: scoped_defer(printf("3"), printf("2"), printf("1")) prints out 321, for
 * a real defer keyword we would want the reverse (stack-like behaviour). To
 * achieve an output of 123, you could chain the macro like so:
 *     scoped_defer(printf("3"))
 *     scoped_defer(printf("2"))
 *     scoped_defer(printf("1")) { ... }
 * Note that having them on the same line can cause a shadowing warning
 */
#define scoped_defer(...) \
    for (int UNIQUE_VAR(_i_) = 0; !UNIQUE_VAR(_i_); (UNIQUE_VAR(_i_) += 1), __VA_ARGS__)

/* add a (real) defer statement for C++11 and up */
#if defined(__cplusplus) && ((__cplusplus >= 201100) || (_MSVC_LANG >= 201100))
    template <typename F>
    struct privDefer {
        F f;
        privDefer(F f_in) : f(f_in) {}
        ~privDefer() { f(); }
    };

    template <typename F>
    privDefer<F> defer_func(F f) {
        return privDefer<F>(f);
    }

    #define DEFER_1(x, y) x##y
    #define DEFER_2(x, y) DEFER_1(x, y)
    #define DEFER_3(x)    DEFER_2(x, __COUNTER__)
    #define defer(code)   auto DEFER_3(_defer_) = defer_func([&](){code;})
#endif
