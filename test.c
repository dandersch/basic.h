#include "basic.h"
#include "basic.h"

#include <stdio.h>

extern void test_for_multiple_definition();

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

        /*                           */
        /* TODO TEST DEBUG FUNCTIONS */
        /*                           */
        STATIC_ASSERT((2+2==4), "All good");
        //STATIC_ASSERT((2+2==5), "ignorance is strength");

        ASSERT(1 == 1);
        //ASSERT(0 == 1);

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
        m3f_print(m3f_mul(mat2, m3f_inv(mat2))); // should equal identity matrix

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
        /* TODO TEST MEMORY FUNCTIONS */
        /*                            */

        // TODO comment out once working on MSVC
        //u64 offset = OFFSET_OF(v3f, x);
        //ASSERT(offset == 0);
        //offset = OFFSET_OF(v3f, y);
        //ASSERT(offset == 4);

        return 0;
}
