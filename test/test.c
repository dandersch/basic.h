#define LOG_USE_SHORT_NAMES_GLOBALLY
#define LOG_USE_PLAIN_ENTRY_FILE
#define LOG_ENTRY_FILE "log_entries.h"
#ifdef __MINGW32__
      #define LOG_USE_NO_COLOR
#endif
#define BASIC_IMPLEMENTATION
#include "../basic/basic.h"
#include "../basic/basic.h" // testing double include

/* NOTE invalid sizes on 32bit arch */
#define RES_MEM_ENTITIES    ALIGN_TO_NEXT_PAGE(GIGABYTES(4))
#define RES_MEM_GAME_TEMP   ALIGN_TO_NEXT_PAGE(MEGABYTES(100))
#define RES_MEM_GAME        RES_MEM_ENTITIES + RES_MEM_GAME_TEMP

#define RES_MEM_TEXTURES    ALIGN_TO_NEXT_PAGE(GIGABYTES(2))
#define RES_MEM_MESHES      ALIGN_TO_NEXT_PAGE(GIGABYTES(2))
#define RES_MEM_RENDERER    RES_MEM_TEXTURES + RES_MEM_MESHES

#define RES_MEM_FILES       ALIGN_TO_NEXT_PAGE(MEGABYTES(512))
#define RES_MEM_NETWORK     ALIGN_TO_NEXT_PAGE(KILOBYTES(10))
#define RES_MEM_PLATFORM    RES_MEM_FILES + RES_MEM_NETWORK

#define RES_MEM_APPLICATION RES_MEM_GAME + RES_MEM_RENDERER + RES_MEM_PLATFORM

/* proof-of-concept: defining memory sizes using x-macros */
#define MEMORY_GAME \
  X(ENTITIES, GIGABYTES(4)  ) \
  X(SOUND,    MEGABYTES(2)  ) \
  X(TEMP,     KILOBYTES(16) )

#define X(name, memory) memory +
const u64 MEMORY_OVERALL_GAME = MEMORY_GAME + 0;

#define MEMORY_RENDERER \
  X(TEXTURES, MEGABYTES(256) ) \
  X(MESHES,   MEGABYTES(40)  )

const u64 MEMORY_OVERALL_RENDERER = MEMORY_RENDERER + 0;
const u64 MEMORY_OVERALL = MEMORY_GAME MEMORY_RENDERER + 0;
#undef X

int log_verbosity_level = LOG_EVERYTHING;


void test_math();
int main(int argc, char** argv)
{
    /* TEST PLATFORM DETECTION */
    {
        LOG(INFO|PLATFORM, "OS:...........%s", platform_os_string(platform_detect_os()));
        LOG(INFO|PLATFORM, "COMPILER:.....%s", platform_compiler_string(platform_detect_compiler()));
        LOG(INFO|PLATFORM, "ARCH:.........%s", ARCHITECTURE_STRING);
        LOG(INFO|PLATFORM, "STANDARD:.....%s", C_STANDARD_STRING);
        LOG(INFO|PLATFORM, "BUILD_TYPE:...%s", BUILD_TYPE_STRING);
    }

    /* TEST DEBUG FUNCTIONS */
    {
        STATIC_ASSERT((2+2==4), "All good");
        //STATIC_ASSERT((2+2==5), "ignorance is strength");

        //ASSERT(!debug_running_under_debugger()); /* NOTE: run in debugger to test this */

        ASSERT(1 == 1);
        //ASSERT(0 == 1);
    }

    /* TEST PRAGMAS */
    {
        /* Test if we can ignore warnings in a custom scope by pushing and popping */
        PUSH_WARNINGS()
        WARNING_TO_IGNORE("-Wshadow", 6244)
        WARNING_TO_IGNORE("-Wshadow", 6246)
        // no warning/error of shadowing a variable should pop up here
        int a = 0;
        {
                int a = 0;
        }

        #ifdef COMPILER_TCC
        #pragma pack(1) // NOTE: tcc doesnt support do_pragma's
        #endif
        PUSH_STRUCT_PACK(1)
        typedef struct test_align_packed
        {
            int   a; //   4B
            char  b; // + 1B
            float c; // + 4B
                     // = 9B because of packing
        } test_align_packed;
        POP_STRUCT_PACK()
        #ifdef COMPILER_TCC
        #pragma pack(8) // NOTE: tcc doesnt support do_pragma's
        #endif
        typedef struct test_align_unpacked
        {
            int   a; //    4B
            char  b; // +  1B
            float c; // +  4B
                     // = 12B bc of std alignment
        } test_align_unpacked;
        POP_WARNINGS()

        /* Test #pragma packing */
        ASSERT( 9 == sizeof(test_align_packed));
        ASSERT(12 == sizeof(test_align_unpacked));
    }

    /* TEST THREAD STUFF */
    {
        #if !defined(COMPILER_TCC) // TODO
        static thread_local u32 thread_thing = 0; // TODO test properly with threads
        #endif
    }

    /* TEST MATH FUNCTIONS  */
    test_math();


    /* TEST MEMORY MACROS */
    {
        /* "power of 2" macros */
        STATIC_ASSERT(CHECK_IF_POW2(16), "");
        STATIC_ASSERT(!CHECK_IF_POW2(18), "");

        ASSERT(NEXT_ALIGN_POW2(12,16) == 16);
        ASSERT(NEXT_ALIGN_POW2(18,16) == 32);
        ASSERT(NEXT_ALIGN_POW2(34,32) == 64);
        ASSERT(PREV_ALIGN_POW2(34,32) == 32);

        void* test_ptr = (void*) 49802293lu;
        ASSERT((ALIGN_TO_NEXT_PAGE(test_ptr) - ALIGN_TO_PREV_PAGE(test_ptr)) == mem_pagesize());
        ASSERT(ALIGN_TO_NEXT_PAGE(test_ptr) > (uintptr_t) test_ptr);
        ASSERT(ALIGN_TO_PREV_PAGE(test_ptr) < (uintptr_t) test_ptr);

        ASSERT((ALIGN_TO_NEXT_PAGE(49802293) - ALIGN_TO_PREV_PAGE(49802293)) == mem_pagesize());

        /* test offset of macro */
        u64 offset = OFFSET_OF(v3f, x);
        ASSERT(offset == 0);
        offset = OFFSET_OF(v3f, y);
        ASSERT(offset == 4);

        /* test defer statement */
        #if defined(LANGUAGE_CPP) && (STANDARD_VERSION >= 2011)
        i32 x = 3;
        {
            defer(x *= 2);
            x -= 1;
        }
        ASSERT(x == 4)
        #endif
    }

    /* TEST MEMORY ALLOCATION */
    {
        u64 buf_size_reserved  = MEGABYTES(8);

        void* fixed_address = NULL;
        #if defined(BUILD_DEBUG) && defined(ARCH_X64)
        fixed_address = (void*) GIGABYTES(256); // 0x4000000000
        #endif
        u8* buf       = (u8*) mem_reserve(fixed_address, buf_size_reserved);
        ASSERT(buf);
        #if defined(BUILD_DEBUG) && defined(ARCH_X64)
        ASSERT(buf == fixed_address);
        #endif

        u64 buf_size_committed = KILOBYTES(12);
        b32 committed          = mem_commit(buf, buf_size_committed);
        ASSERT(committed);

        u8* buf_2              = (u8*) mem_alloc(buf_size_committed);
        ASSERT(buf_2);

        /* test if memory is initialized to zero */
        for (u32 i = 0; i < buf_size_committed; i++)
        {
            ASSERT(!buf[i]);
            ASSERT(!buf_2[i]);
        }

        /* copying and comparing memory */
        for (u32 i = 0; i < buf_size_committed; i++)
        {
            buf[i] = 'a';
        }
        ASSERT(!mem_equal(buf, buf_2, buf_size_committed));
        mem_copy(buf_2, buf, buf_size_committed);
        ASSERT(mem_equal(buf, buf_2, buf_size_committed));

        /* freeing memory */
        b32 decommitted = mem_decommit(buf, buf_size_committed);
        ASSERT(decommitted);
        // for (u32 i = 0; i < buf_size_committed; i++) { buf[0] = 'b'; } // should cause SEGV
        mem_release(buf,  buf_size_committed);
        mem_free(buf_2,   buf_size_committed);
    }

    /* TEST COMMON MACROS */
    {
        /* test scoped_begin_end */
        int a = 0; // compute a = (((3 * 5) - 3) / 6)
        scoped_begin_end(a = 3, a /= 6)
        {
            a *= 5;
            a -= 3;
        }
        ASSERT(a == 2);

        /* test scoped_defer */
        int b = 0; // compute b = (((10 + 2) / 6) - 1)
        scoped_defer(b += 2, b /= 3, b -= 1)
        {
            b = 10;
        }
        ASSERT(b == 3)

        /* chained scoped_defer for more defer-like behaviour */
        int c = 0; // compute c = ((((10 - 1)) / 3) + 2)
        scoped_defer(c += 2)
        scoped_defer(c /= 3)
        scoped_defer(c -= 1)
        {
            c = 10;
        }
        ASSERT(c == 5)

        char  real_array[8];
        char* decayed_array = real_array;
        ASSERT(ARRAY_COUNT(real_array) == 8);

        MEM_ZERO_OUT_ARRAY(real_array);
        // MEM_ZERO_OUT_ARRAY(decayed_array); // should fail

        PUSH_WARNINGS()
        WARNING_TO_IGNORE("-Wsizeof-pointer-div", 6384)
        //DO_PRAGMA(warning(disable : 6384))
        //DO_PRAGMA(GCC diagnostic ignored "-Wsizeof-pointer-div")
        /* NOTE should cause an error with warnings enabled */
        ASSERT(ARRAY_COUNT(decayed_array) == 0);
        POP_WARNINGS()
    }

    /* TEST ARENAS */
    {
        mem_arena_t* arena = mem_arena_reserve(MEGABYTES(1));
        u8* arena_buf     = (u8*) mem_arena_push(arena, KILOBYTES(4));
        for (u32 i = 0; i < KILOBYTES(4); i++) { ASSERT(!arena_buf[i]); }
        mem_arena_pop_by(arena, KILOBYTES(1));
        ASSERT(mem_arena_get_pos(arena) == KILOBYTES(3));

        /* test if memory after popping & pushing is still zeroed */
        mem_arena_push(arena, KILOBYTES(1));
        for (u32 i = 0; i < KILOBYTES(4); i++) { ASSERT(!arena_buf[i]); }

        struct test_align_unpacked
        {
            int   a; //    4B
            char  b; // +  1B
            float c; // +  4B
                     // = 12B bc of std alignment
        };
        struct test_align_unpacked* struct_test = ARENA_PUSH_STRUCT(arena, struct test_align_unpacked);
        u32* number_arr = ARENA_PUSH_ARRAY(arena, u32, 256);
        for (u32 i = 0; i < 256; i++) { ASSERT(!number_arr[i]); }

        mem_arena_pop_to(arena, number_arr);
        number_arr = ARENA_PUSH_ARRAY(arena, u32, 256);
        for (u32 i = 0; i < 256; i++) { ASSERT(!number_arr[i]); }

        /* provoke an overflow */
        //mem_arena_push(&sub_arena, KILOBYTES(3));
        //mem_arena_push(&arena,     MEGABYTES(10));
    }

    /* TEST ARENA RESERVING & COMMITTING */
    {
        mem_arena_t* arena   = mem_arena_reserve(KILOBYTES(32));
        u8* arena_buf_1      = (u8*) mem_arena_push(arena, KILOBYTES(4));
        ASSERT(arena_buf_1);
        for (u32 i = 0; i < KILOBYTES(4); i++) { ASSERT(!arena_buf_1[i]); }

        u8* arena_buf_2      = (u8*) mem_arena_push(arena, KILOBYTES(16));
        ASSERT(arena_buf_2);
        for (u32 i = 0; i < KILOBYTES(16); i++) { ASSERT(!arena_buf_2[i]); }
    }

    /* TEST SUBARENAS */
    {
        mem_arena_t* base_arena     = mem_arena_reserve(RES_MEM_APPLICATION);
        mem_arena_t* platform_arena = mem_arena_subarena(base_arena, RES_MEM_PLATFORM);
        ASSERT(platform_arena);
        mem_arena_t* renderer_arena = mem_arena_subarena(base_arena, RES_MEM_RENDERER);
        ASSERT(renderer_arena);
        mem_arena_t* game_arena     = mem_arena_subarena(base_arena, RES_MEM_GAME);
        ASSERT(game_arena);

        // TODO find a way to turn overallocating the base arena into a compile time error
        //mem_arena_t* test_arena     = mem_arena_subarena(base_arena, 1); // should fail

        //u8* test_buf = (u8*) mem_arena_push(game_arena, KILOBYTES(5)); // push beyond pagesize
        //for (u32 i = 0; i < KILOBYTES(8); i++) { test_buf[i] = 'a'; }
    }

    /* TEST LINKED LIST MACROS */
    {
        PUSH_WARNINGS()
        WARNING_TO_IGNORE("-Wshadow", 6244)
        WARNING_TO_IGNORE("-Wshadow", 6246)
        typedef struct node_t /* for testing linked list macros */
        {
            int            data;
            struct node_t* next;
        } node_t;
        POP_WARNINGS()

        mem_arena_t* arena = mem_arena_reserve(MEGABYTES(1));
        node_t* first  = (node_t*) mem_alloc(sizeof(node_t));
        first->data    = 1;
        node_t* second = ARENA_PUSH_STRUCT(arena, node_t);
        second->data   = 2;
        node_t* third  = (node_t*) mem_alloc(sizeof(node_t));
        third->data    = 3;

        LL_APPEND(first, second);
        LL_APPEND(first, third);

        #if defined(LANGUAGE_C) && defined(COMPILER_MSVC)
        node_t* idx; /* no typeof macro */
        #else
        TYPE_OF(first) idx;
        #endif
        i32 val = 1;
        LL_FOREACH(first, idx)
        {
            ASSERT(idx->data == val);
            val++;
        }

        mem_arena_free(arena);
    }

    /* TEST DYNAMIC ARRAY */
    {
        i32* array              = (i32*) dynarr_create(sizeof(i32));
        dynarr_header_t* header = dynarr_header(array);
        //printf("%lu %lu\n", header->len, header->cap);

        for (i32 i = 0; i < 20000; i++)
        {
            //printf("%lu %lu\n", header->len, header->cap);
            dynarr_push(array, i);
        }

        //printf("%lu %lu\n", dynarr_len(array), header->cap);
        for (i32 i = 0; i < dynarr_len(array); i++)
        {
            //printf("%i ", array[i]);
        }
    }

    return 0;
}

/* NOTE: C++20 error if math.h is included  */
#define HANDMADE_MATH_PROVIDE_MATH_FUNCTIONS
#define HMM_SINF  sinf
#define HMM_COSF  cosf
#define HMM_TANF  tanf
#define HMM_SQRTF sqrtf
#define HMM_ACOSF acosf
#include "ext/HandmadeMath.h"
void test_math() {
    /* helper macros */
    #define VEC_EQUAL(v,hmm) ((v.x == hmm.X) && (v.y == hmm.Y) && (v.z == hmm.Z))

    v3f vec1          = {0.2f,  0.5, 0.9f};
    HMM_Vec3 hmm_vec1 = {0.2f,  0.5, 0.9f};
    v3f vec2          = {3.3f, -0.5, 0.1f};
    HMM_Vec3 hmm_vec2 = {3.3f, -0.5, 0.1f};

    ASSERT(VEC_EQUAL(vec1, hmm_vec1));
    ASSERT(VEC_EQUAL(vec2, hmm_vec2));

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

    if (0) { m3f_print(mat); }

    #if defined(LANGUAGE_CPP)
    v3f vec3 = vec1 + vec2;
    f32 dot  = vec1.dot(vec2);

    ASSERT(iden[0][0] == 1.0f);
    v3f from_mat = iden[2];
    ASSERT(from_mat[0] == 0.0f);
    ASSERT(from_mat[1] == 0.0f);
    ASSERT(from_mat[2] == 1.0f);
    #else
    v3f vec3 = v3f_add(vec1, vec2);
    f32 dot  = v3f_dot(vec1,vec2);
    #endif

    HMM_Vec3 hmm_vec3 = HMM_AddV3(hmm_vec1, hmm_vec2);
    ASSERT(VEC_EQUAL(vec3, hmm_vec3));
    ASSERT(dot == HMM_DotV3(hmm_vec1, hmm_vec2));

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

    /* TODO implement and test further math functions */
}
