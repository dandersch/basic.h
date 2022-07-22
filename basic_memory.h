#pragma once

/* common macros */
#define ARRAY_COUNT(arr)  (sizeof(arr)/sizeof(arr[0]))

#if !defined(COMPILER_MSVC)
    #define OFFSET_OF(type, member) __builtin_offsetof(type, member)
#else
    #define OFFSET_OF(s,m) ((size_t)&(((s*)0)->m)) // used by MSVC
#endif

#define KILOBYTES(val) (         (val) * 1024LL)
#define MEGABYTES(val) (KILOBYTES(val) * 1024LL)
#define GIGABYTES(val) (MEGABYTES(val) * 1024LL)
#define TERABYTES(val) (GIGABYTES(val) * 1024LL)

/* alignment / power-of-2 macros */
#define CHECK_IF_POW2(x)          (((x)&((x)-1))==0)
// some bit hacking to get the next alignment value.
// e.g.: NEXT_ALIGN_POW2(12,16) == 16, NEXT_ALIGN_POW2(18,16) == 32
#define NEXT_ALIGN_POW2(x,align) (((x) + (align) - 1)&~((align) - 1))
#define PREV_ALIGN_POW2(x,align) ((x)&~((align) - 1))

/* add a defer statement for C++11 and up */
#if defined(LANGUAGE_CPP) && (STANDARD_VERSION >= 2011)
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

/* TODO memory arena */

struct mem_arena_t
{
    void* base_addr;
    void* curr_addr;
    u64   total_size;
};
// NOTE: we can store the mem_arena struct inside the allocation itself

//mem_arena_t mem_arena_create(u64 size_in_bytes) {}
// mem_arena_allocate / mem_arena_push
// mem_arena_free     / mem_arena_pop
// mem_arena_destroy  / mem_arena_release_arena
