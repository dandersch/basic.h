#pragma once

/* includes typedefs, #defines, math types and operations */

// NOTE: here are some ways in which these math operations can be slow:
// - too many indirections could lead to failed inlining by compilers
// - no considerations for caching
// - pass by value everywhere
// - no SIMD
// - used algorithms might not be optimal

/* u32, f32, ... types */
#include <stdint.h>   // TODO find a way to avoid this include
typedef int8_t   i8;
typedef int16_t  i16;
typedef int32_t  i32;
typedef int64_t  i64;

typedef int8_t   s8;
typedef int16_t  s16;
typedef int32_t  s32;
typedef int64_t  s64;

typedef uint8_t  u8;
typedef uint16_t u16;
typedef uint32_t u32;
typedef uint64_t u64;

typedef int8_t   b8;
typedef int16_t  b16;
typedef int32_t  b32;
typedef int64_t  b64;

typedef float    f32;
typedef double   f64;

/* redefine the static keyword for use in specific contexts */
#if !defined(internal)
    #define internal static // for 'static' functions
#else
    #warning "'internal' already defined. Make sure it's set to 'static'."
#endif
#if !defined(local)
    #define local    static // for 'static' variables inside a function
#else
    #warning "'local' already defined. Make sure it's set to 'static'."
#endif
#if !defined(global)
    #define global   static // for 'static' variables w/ global scope in a unity build
#else
    #warning "'global' already defined. Make sure it's set to 'static'."
#endif

/* numerical limits */
#include <float.h>  // for FLT_MAX/MIN
#define U8_MIN  0
#define U8_MAX  255
#define U16_MIN 0
#define U16_MAX 65535
#define U32_MIN 0
#define U32_MAX 4294967295
#define U64_MIN 0
#define U64_MAX 18446744073709551615
#define I32_MIN ((i32) 0x80000000)
#define I32_MAX ((i32) 0x7FFFFFFF)
#define S32_MIN ((s32) 0x80000000)
#define S32_MAX ((s32) 0x7FFFFFFF)
#define F32_MIN -FLT_MAX
#define F32_MAX FLT_MAX

/* mathematical constants */
// TODO find out the double precision versions (PI_F64,...)
#define PI_F32      3.14159265359f
#define PI          PI_F32
#define TAU_F32     6.28318530718f 
#define TAU         TAU_F32
#define EULER_F32   2.71828182846f
#define EULER       EULER_F32
#define EPSILON_F32 1.1920929e-7f
#define EPSILON     EPSILON_F32

/* vector types and operations */
typedef union v2f
{
    struct { f32 x; f32 y; };
    struct { f32 u; f32 v; };
    f32 e[2];

#if defined(LANGUAGE_CPP)
    inline f32& operator[](const i32& idx) { return e[idx]; }
    inline f32 dot(const v2f rhs);
#endif
} v2f;

inline static v2f v2f_add(const v2f lhs, const v2f rhs) { return (v2f){lhs.x + rhs.x, lhs.y + rhs.y}; }
inline static v2f v2f_sub(const v2f lhs, const v2f rhs) { return (v2f){lhs.x - rhs.x, lhs.y - rhs.y}; }
inline static v2f v2f_mul(const v2f lhs, const v2f rhs) { return (v2f){lhs.x * rhs.x, lhs.y * rhs.y}; }
inline static v2f v2f_div(const v2f lhs, const v2f rhs) { return (v2f){lhs.x / rhs.x, lhs.y / rhs.y}; }
inline static v2f v2f_add_s(const v2f lhs, const f32 scalar) { return v2f_add(lhs, (v2f){scalar, scalar}); }
inline static v2f v2f_sub_s(const v2f lhs, const f32 scalar) { return v2f_sub(lhs, (v2f){scalar, scalar}); }
inline static v2f v2f_mul_s(const v2f lhs, const f32 scalar) { return v2f_mul(lhs, (v2f){scalar, scalar}); }
inline static v2f v2f_div_s(const v2f lhs, const f32 scalar) { return v2f_div(lhs, (v2f){scalar, scalar}); }
inline static f32 v2f_dot(v2f lhs, v2f rhs) { return (lhs.x * rhs.x) + (lhs.y * rhs.y); }
// TODO cross product
// TODO length/magnitude
#if defined(LANGUAGE_CPP)
    inline v2f operator+(v2f lhs, v2f rhs) { return v2f_add(lhs,rhs); }
    inline v2f operator-(v2f lhs, v2f rhs) { return v2f_sub(lhs,rhs); }
    inline v2f operator*(v2f lhs, v2f rhs) { return v2f_mul(lhs,rhs); }
    inline v2f operator/(v2f lhs, v2f rhs) { return v2f_div(lhs,rhs); }
    inline v2f operator+(v2f lhs, f32 scalar) { return v2f_add_s(lhs, scalar); }
    inline v2f operator-(v2f lhs, f32 scalar) { return v2f_sub_s(lhs, scalar); }
    inline v2f operator*(v2f lhs, f32 scalar) { return v2f_mul_s(lhs, scalar); }
    inline v2f operator/(v2f lhs, f32 scalar) { return v2f_div_s(lhs, scalar); }
    inline f32 v2f::dot(const v2f rhs) { return v2f_dot(*this, rhs); }
#endif

typedef union v3f
{
    struct { f32 x; f32 y; f32 z; };
    f32 e[3];

#if defined(LANGUAGE_CPP)
    inline f32& operator[](const i32& idx) { return e[idx]; }
    inline f32 dot(const v3f rhs);
#endif
} v3f;

inline static v3f v3f_add(const v3f lhs, const v3f rhs) { return (v3f){lhs.x + rhs.x, lhs.y + rhs.y, lhs.z + rhs.z}; }
inline static v3f v3f_sub(const v3f lhs, const v3f rhs) { return (v3f){lhs.x - rhs.x, lhs.y - rhs.y, lhs.z - rhs.z}; }
inline static v3f v3f_mul(const v3f lhs, const v3f rhs) { return (v3f){lhs.x * rhs.x, lhs.y * rhs.y, lhs.z * rhs.z}; }
inline static v3f v3f_div(const v3f lhs, const v3f rhs) { return (v3f){lhs.x / rhs.x, lhs.y / rhs.y, lhs.z / rhs.z}; }
inline static v3f v3f_add_s(const v3f lhs, const f32 scalar) { return v3f_add(lhs, (v3f){scalar, scalar, scalar}); }
inline static v3f v3f_sub_s(const v3f lhs, const f32 scalar) { return v3f_sub(lhs, (v3f){scalar, scalar, scalar}); }
inline static v3f v3f_mul_s(const v3f lhs, const f32 scalar) { return v3f_mul(lhs, (v3f){scalar, scalar, scalar}); }
inline static v3f v3f_div_s(const v3f lhs, const f32 scalar) { return v3f_div(lhs, (v3f){scalar, scalar, scalar}); }
inline static f32 v3f_dot(v3f lhs, v3f rhs) { return (lhs.x * rhs.x) + (lhs.y * rhs.y) + (lhs.z * rhs.z); }
#if defined(LANGUAGE_CPP)
    inline v3f operator+(v3f lhs, v3f rhs) { return v3f_add(lhs,rhs); }
    inline v3f operator-(v3f lhs, v3f rhs) { return v3f_sub(lhs,rhs); }
    inline v3f operator*(v3f lhs, v3f rhs) { return v3f_mul(lhs,rhs); }
    inline v3f operator/(v3f lhs, v3f rhs) { return v3f_div(lhs,rhs); }
    inline v3f operator+(v3f lhs, f32 scalar) { return v3f_add_s(lhs, scalar); }
    inline v3f operator-(v3f lhs, f32 scalar) { return v3f_sub_s(lhs, scalar); }
    inline v3f operator*(v3f lhs, f32 scalar) { return v3f_mul_s(lhs, scalar); }
    inline v3f operator/(v3f lhs, f32 scalar) { return v3f_div_s(lhs, scalar); }
    inline f32 v3f::dot(const v3f rhs) { return v3f_dot(*this, rhs); }
#endif

/* TODO matrix types and operations */
typedef struct m3f
{
    f32 e[3][3]; // NOTE: ROW MAJOR - E[ROW][COLUMN]!!!
#if defined(LANGUAGE_CPP)
    inline v3f operator[](const i32 idx) { ASSERT(idx < 3); return (v3f){e[idx][0],e[idx][1],e[idx][2]}; };
    inline m3f inverse();
#endif
} m3f;

inline static m3f m3f_mul(m3f lhs, m3f rhs)
{
    m3f res = {0};

    res.e[0][0] = lhs.e[0][0] * rhs.e[0][0] + lhs.e[0][1] * rhs.e[1][0] + lhs.e[0][2] * rhs.e[2][0];
    res.e[0][1] = lhs.e[0][0] * rhs.e[0][1] + lhs.e[0][1] * rhs.e[1][1] + lhs.e[0][2] * rhs.e[2][1];
    res.e[0][2] = lhs.e[0][0] * rhs.e[0][2] + lhs.e[0][1] * rhs.e[1][2] + lhs.e[0][2] * rhs.e[2][2];

    res.e[1][0] = lhs.e[1][0] * rhs.e[0][0] + lhs.e[1][1] * rhs.e[1][0] + lhs.e[1][2] * rhs.e[2][0];
    res.e[1][1] = lhs.e[1][0] * rhs.e[0][1] + lhs.e[1][1] * rhs.e[1][1] + lhs.e[1][2] * rhs.e[2][1];
    res.e[1][2] = lhs.e[1][0] * rhs.e[0][2] + lhs.e[1][1] * rhs.e[1][2] + lhs.e[1][2] * rhs.e[2][2];

    res.e[2][0] = lhs.e[2][0] * rhs.e[0][0] + lhs.e[2][1] * rhs.e[1][0] + lhs.e[2][2] * rhs.e[2][0];
    res.e[2][1] = lhs.e[2][0] * rhs.e[0][1] + lhs.e[2][1] * rhs.e[1][1] + lhs.e[2][2] * rhs.e[2][1];
    res.e[2][2] = lhs.e[2][0] * rhs.e[0][2] + lhs.e[2][1] * rhs.e[1][2] + lhs.e[2][2] * rhs.e[2][2];

    return res;
};

inline static m3f m3f_mul_s(m3f mat, f32 scalar)
{
    mat.e[0][0] *= scalar; mat.e[0][1] *= scalar; mat.e[0][2] *= scalar;
    mat.e[1][0] *= scalar; mat.e[1][1] *= scalar; mat.e[1][2] *= scalar;
    mat.e[2][0] *= scalar; mat.e[2][1] *= scalar; mat.e[2][2] *= scalar;
    return mat;
}

inline static f32 m3f_det(m3f mat)
{
    f32 det = (mat.e[0][0] * mat.e[1][1] * mat.e[2][2]) +
              (mat.e[0][1] * mat.e[1][2] * mat.e[2][0]) +
              (mat.e[0][2] * mat.e[1][0] * mat.e[2][1]) -
              (mat.e[2][0] * mat.e[1][1] * mat.e[0][2]) -
              (mat.e[2][1] * mat.e[1][2] * mat.e[0][0]) -
              (mat.e[2][2] * mat.e[1][0] * mat.e[0][1]);
    return det;
}

inline static m3f m3f_transpose(m3f mat)
{
    m3f t = {0};
    t.e[0][0] = mat.e[0][0];
    t.e[1][1] = mat.e[1][1];
    t.e[2][2] = mat.e[2][2];
    t.e[0][1] = mat.e[1][0];
    t.e[0][2] = mat.e[2][0];
    t.e[1][0] = mat.e[0][1];
    t.e[2][0] = mat.e[0][2];
    t.e[1][2] = mat.e[2][1];
    t.e[2][1] = mat.e[1][2];
    return t;
}

inline static m3f m3f_inv(m3f mat)
{
    m3f res = {0};

    // algorithm taken from wikipedia "Invertible Matrix::Inversion of 3x3 matrices"
    f32 A =  (mat.e[1][1] * mat.e[2][2] - mat.e[1][2] * mat.e[2][1]);
    f32 B = -(mat.e[1][0] * mat.e[2][2] - mat.e[1][2] * mat.e[2][0]);
    f32 C =  (mat.e[1][0] * mat.e[2][1] - mat.e[1][1] * mat.e[2][0]);
    f32 D = -(mat.e[0][1] * mat.e[2][2] - mat.e[0][2] * mat.e[2][1]);
    f32 E =  (mat.e[0][0] * mat.e[2][2] - mat.e[0][2] * mat.e[2][0]);
    f32 F = -(mat.e[0][0] * mat.e[2][1] - mat.e[0][1] * mat.e[2][0]);
    f32 G =  (mat.e[0][1] * mat.e[1][2] - mat.e[0][2] * mat.e[1][1]);
    f32 H = -(mat.e[0][0] * mat.e[1][2] - mat.e[0][2] * mat.e[1][0]);
    f32 I =  (mat.e[0][0] * mat.e[1][1] - mat.e[0][1] * mat.e[1][0]);

    m3f tmp =
    {
        A, B, C,
        D, E, F,
        G, H, I,
    };

    f32 det = mat.e[0][0] * A + mat.e[0][1] * B + mat.e[0][2] * C;

    ASSERT(det != 0.0f); // if true, matrix is singular and not invertible

    f32 inv_det = 1.0f/det;

    res = m3f_mul_s(m3f_transpose(tmp), inv_det);

    return res;
}

#if defined(LANGUAGE_CPP)
    inline m3f operator*(m3f lhs, m3f rhs) { return m3f_mul(lhs, rhs); };
    inline m3f operator*(m3f lhs, f32 rhs) { return m3f_mul_s(lhs, rhs); };
    inline m3f m3f::inverse()              { return m3f_inv(*this); }
#endif

inline static void m3f_print(m3f mat)
{
    for (u32 i = 0; i < 3; i++)
    {
        for (u32 j = 0; j < 3; j++)
        {
            printf("%f ", mat.e[i][j]);
        }
        printf("\n");
    }
}

// TODO matrix-vector-multiplication

// TODO m4f

/* common operations */
inline static f32 lerp(f32 min, f32 interpolant, f32 max) { return (min + interpolant * (max - min)); }
inline static f32 unlerp(f32 min, f32 value, f32 max) { return (min != max) ? (value - min)/(max - min) : 0.0f; }
inline static f32 linear_remap(f32 val, f32 a_min, f32 a_max, f32 b_min, f32 b_max) { return lerp(b_min, unlerp(a_min, val, a_max), b_max);}

#define MIN(x,y) (((x)<(y)) ? (x):(y))
#define MAX(x,y) (((x)>(y)) ? (x):(y))
#define CLAMP(min,val,max) (((val) < (min)) ? (min) : ((max)<(val)) ? (max) : (val))

// TODO
// i32 abs(i32 val)
// f32 absf(f32 val)
// f64 absd(f64 val)
// i32 sign(i32 val)
// f32 signf(f32 val)
//
// f32 ceil(f32 val)
// f32 floor(f32 val)
