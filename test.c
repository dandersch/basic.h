#define BASIC_IMPLEMENTATION
#include "basic.h"
#include "basic.h" // testing double include

#include <stdio.h>

extern void test_for_multiple_definition();

PUSH_STRUCT_PACK(1)
typedef struct test_align_packed
{
    int   a; //   4B
    char  b; // + 1B
    float c; // + 4B
             // = 9B because of packing
} test_align_packed;
POP_STRUCT_PACK()
typedef struct test_align_unpacked
{
    int   a; //    4B
    char  b; // +  1B
    float c; // +  4B
             // = 12B bc of std alignment
} test_align_unpacked;

int main(int argc, char** argv)
{
    test_for_multiple_definition();

    /*                         */
    /* TEST PLATFORM DETECTION */
    /*  (OS/COMPILER/STD/...)  */
    /*                         */
    const char* os = platform_os_string(platform_detect_os());
    printf("OS:...........%s\n", os);

    printf("COMPILER:.....");
#if defined(COMPILER_GCC)
    printf("gcc\n");
#elif defined(COMPILER_CLANG)
    printf("clang\n");
#elif defined(COMPILER_MSVC)
    printf("msvc\n");
#elif defined(COMPILER_MINGW)
    printf("mingw\n");
#endif

    printf("ARCH:.........");
#if defined(ARCH_X64)
    printf("x64\n");
#elif defined(ARCH_X86)
    printf("x86\n");
#elif defined(ARCH_ARM)
    printf("arm\n");
#endif

    printf("LANGUAGE:.....");
#if defined(LANGUAGE_CPP)
    printf("C++\n");
#elif defined(LANGUAGE_C)
    printf("C\n");
#endif

    printf("STANDARD:.....");
#if   defined(STANDARD_C89)
    printf("C89\n");
#elif defined(STANDARD_C99)
    printf("C99\n");
#elif defined(STANDARD_C11)
    printf("C11\n");
#elif defined(STANDARD_C17)
    printf("C17\n");
#elif defined(STANDARD_Cxx98)
    printf("C++98\n");
#elif defined(STANDARD_Cxx03)
    printf("C++03\n");
#elif defined(STANDARD_Cxx11)
    printf("C++11\n");
#elif defined(STANDARD_Cxx14)
    printf("C++14\n");
#elif defined(STANDARD_Cxx17)
    printf("C++17\n");
#elif defined(STANDARD_Cxx20)
    printf("C++20\n");
#endif

    /* TEST BUILD TYPE */
    printf("BUILD_TYPE:...");
#if defined(BUILD_DEBUG)
    printf("DEBUG\n");
#elif defined(BUILD_RELEASE)
    printf("RELEASE\n");
#elif defined(BUILD_CUSTOM)
    printf("CUSTOM\n");
#else
    #error "No build type set or detected."
#endif

    /*                      */
    /* TEST DEBUG FUNCTIONS */
    /*                      */
    STATIC_ASSERT((2+2==4), "All good");
    //STATIC_ASSERT((2+2==5), "ignorance is strength");

    ASSERT(!debug_running_under_debugger()); /* NOTE: run in debugger to test this */

    ASSERT(1 == 1);
    //ASSERT(0 == 1);


    /*              */
    /* TEST PRAGMAS */
    /*              */

    /* Test if we can ignore warnings in a custom scope by pushing and popping */
    PUSH_WARNINGS()
    WARNING_TO_IGNORE("-Wshadow", 6244)
    WARNING_TO_IGNORE("-Wshadow", 6246)
    // no warning/error of shadowing a variable should pop up here
    int a = 0;
    {
            int a = 0;
    }
    POP_WARNINGS()

    /* Test #pragma packing */
    ASSERT( 9 == sizeof(test_align_packed));
    ASSERT(12 == sizeof(test_align_unpacked));

    /*                           */
    /* TODO TEST MATH FUNCTIONS  */
    /*                           */
    v3f vec1 = {0.2f,  0.5, 0.9f};
    v3f vec2 = {3.3f, -0.5, 0.1f};

    m3f iden =
    {
        1.0f, 0.0f, 0.0f,
        0.0f, 1.0f, 0.0f,
        0.0f, 0.0f, 1.0f,
    };

    m3f mat  =
    {
         9.0f, 0.3f,  1.8f,
        -0.2f, 8.0f, 20.0f,
         3.0f, 0.0f, -1.0f,
    };

    m3f mat2 =
    {
         2.0f, 3.3f,  8.8f,
         2.2f, 1.0f, -2.8f,
         3.8f, 7.2f, -5.0f,
    };


#if defined(LANGUAGE_CPP)
    v3f vec3 = vec1 + vec2;
    f32 dot = vec1.dot(vec2);

    ASSERT(iden[0][0] == 1.0f);
    v3f from_mat = iden[2];
    ASSERT(from_mat[0] == 0.0f);
    ASSERT(from_mat[1] == 0.0f);
    ASSERT(from_mat[2] == 1.0f);

#else
    v3f vec3 = v3f_add(vec1, vec2);
    f32 dot = v3f_dot(vec1,vec2);
#endif
    m3f iden_test = m3f_mul(mat2, m3f_inv(mat2)); // should equal identity matrix (within an EPSILON range)
    ASSERT((iden_test.e[0][0] + EPSILON) >= iden.e[0][0] || (iden_test.e[0][0] - EPSILON) <= iden.e[0][0]);
    ASSERT((iden_test.e[1][1] + EPSILON) >= iden.e[1][1] || (iden_test.e[1][1] - EPSILON) <= iden.e[1][1]);
    ASSERT((iden_test.e[2][2] + EPSILON) >= iden.e[2][2] || (iden_test.e[2][2] - EPSILON) <= iden.e[2][2]);
    ASSERT((iden_test.e[0][1] + EPSILON) >= iden.e[0][1] || (iden_test.e[0][1] - EPSILON) <= iden.e[0][1]);

    ASSERT(vec3.x == 3.5f);
    ASSERT(vec3.y == 0.0f);
    ASSERT(vec3.z == 1.0f);

    ASSERT(lerp(0, 0.25, 1)      == 0.25);
    ASSERT(lerp(200, 0.5, 300)   == 250);
    ASSERT(unlerp(100, 125, 150) == unlerp(0.5, 0.75, 1.0));
    ASSERT(linear_remap(125, 100, 150, 0.5 , 1.0) == 0.75);

    ASSERT(2.8f == CLAMP(vec3.z,  2.8f, vec3.x));
    ASSERT(3.5f == CLAMP(vec3.z,  3.7f, vec3.x));
    ASSERT(1.0f == CLAMP(vec3.z, -0.7f, vec3.x));

    ASSERT(120   == MAX(  120,  90));
    ASSERT(-0.3f == MIN(-0.3f, 1.0f));

    /*                            */
    /* TODO TEST THREAD FUNCTIONS */
    /*                            */
    static thread_var u32 thread_thing = 0; // TODO test properly with threads

    /*                            */
    /* TODO TEST MEMORY FUNCTIONS */
    /*                            */

    /* "power of 2" macros */
    STATIC_ASSERT(CHECK_IF_POW2(16), "");
    STATIC_ASSERT(!CHECK_IF_POW2(18), "");

    ASSERT(NEXT_ALIGN_POW2(12,16) == 16);
    ASSERT(NEXT_ALIGN_POW2(18,16) == 32);
    ASSERT(NEXT_ALIGN_POW2(34,32) == 64);
    ASSERT(PREV_ALIGN_POW2(34,32) == 32);

    /* test offset of macro */
    u64 offset = OFFSET_OF(v3f, x);
    ASSERT(offset == 0);
    offset = OFFSET_OF(v3f, y);
    ASSERT(offset == 4);

    /* test defer statement */
    #ifdef defer
    i32 x = 3;
    {
        defer(x *= 2);
        x -= 1;
    }
    ASSERT(x == 4)
    #endif

    /*             */
    /* TEST MEMORY */
    /*             */
    u64 buf_size  = MEGABYTES(1);
    u8* buf       = (u8*) mem_reserve(buf_size);
    ASSERT(buf);
    b32 committed = mem_commit(buf, buf_size);
    ASSERT(committed);
    u8* buf_2     = (u8*) mem_alloc(buf_size);
    ASSERT(buf_2);

    /* test if memory is initialized to zero */
    for (u32 i = 0; i < buf_size; i++)
    {
        ASSERT(!buf[i]);
        ASSERT(!buf_2[i]);
    }

    /* freeing memory */
    mem_decommit(buf, buf_size);
    mem_release(buf, buf_size);
    mem_free(buf_2, buf_size);

    /* memory arenas */
    mem_arena_t arena = mem_arena_alloc(buf_size);
    u8* arena_buf     = (u8*) mem_arena_push(&arena, KILOBYTES(4));
    for (u32 i = 0; i < KILOBYTES(4); i++) { ASSERT(!arena_buf[i]); }
    mem_arena_pop_by(&arena, KILOBYTES(1));
    ASSERT(mem_arena_get_pos(&arena) == KILOBYTES(3));

    /* test if memory after popping & pushing is still zeroed */
    mem_arena_push(&arena, KILOBYTES(1));
    for (u32 i = 0; i < KILOBYTES(4); i++) { ASSERT(!arena_buf[i]); }

    test_align_unpacked* struct_test = ARENA_PUSH_STRUCT(&arena, test_align_unpacked);
    u32* number_arr = ARENA_PUSH_ARRAY(&arena, u32, 256);
    for (u32 i = 0; i < 256; i++) { ASSERT(!number_arr[i]); }

    mem_arena_pop_to(&arena, number_arr);
    number_arr = ARENA_PUSH_ARRAY(&arena, u32, 256);
    for (u32 i = 0; i < 256; i++) { ASSERT(!number_arr[i]); }

    /* arena within an arena */
    u8* sub_arena_buf     = (u8*) mem_arena_push(&arena, KILOBYTES(1));
    mem_arena_t sub_arena = mem_arena_create(sub_arena_buf, KILOBYTES(1));
    test_align_unpacked* struct_arr = ARENA_PUSH_ARRAY(&arena, test_align_unpacked, 256);

    /* provoke an overflow */
    //mem_arena_push(&sub_arena, KILOBYTES(3));
    //mem_arena_push(&arena,     MEGABYTES(10));

    mem_arena_free(&arena);

    return 0;
}
