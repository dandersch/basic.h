#pragma once

// vector types
union v2u
{
    struct { u32 x; u32 y; };
    struct { u32 u; i32 v; };
    u32 vec[2];
};
union v3u
{
    struct { u32 x; u32 y; u32 z; };
    u32 vec[3];
};
union v2i
{
    struct { i32 x; i32 y; };
    struct { i32 u; i32 v; };
    i32 vec[2];
};
union v3i
{
    struct { i32 x; i32 y; i32 z; };
    i32 vec[3];
};

// some other useful types
struct rect_t
{
    i32 left, top;
    i32 w, h;
};
struct rectf_t
{
    f32 left, top;
    f32 w, h;
};
struct color_t // NOTE maybe add a 24bit version
{
    u8 r; u8 g; u8 b; u8 a;
};
struct colorf_t // TODO add some implicit conversion functions
{
    f32 r; f32 g; f32 b; f32 a;
    //operator color_t() const { return {(u8)(255*r),(u8)(255*g),(u8)(255*b),(u8)(255*a)}; }
};
