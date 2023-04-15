#pragma once

/* TODO memory arena */
// see https://www.rfleury.com/p/untangling-lifetimes-the-arena-allocator

struct mem_arena_t;
typedef struct mem_arena_t mem_arena_t;

mem_arena_t       mem_arena_alloc  (u64 size_in_bytes);
mem_arena_t       mem_arena_create (void* buf, u64 size_in_bytes);
void*             mem_arena_push   (mem_arena_t* arena, u64 size);
void              mem_arena_pop_to (mem_arena_t* arena, void* buf);
void              mem_arena_pop_by (mem_arena_t* arena, u64 bytes);
void              mem_arena_free   (mem_arena_t* arena);
u64               mem_arena_get_pos(mem_arena_t* arena);

/* TODO new API */
// mem_arena_t* mem_arena_reserve(u64 size_in_bytes);
// mem_arena_t* mem_arena_subarena(mem_arena_t* arena, u64 size_in_bytes); /* no commit */

/* helper */
// mem_arena_t* mem_arena_default();

/* TODO scratch arenas */

/* arena macro helpers */
#define ARENA_PUSH_ARRAY(arena, type, count) (type*) mem_arena_push((arena), sizeof(type)*(count))
#define ARENA_PUSH_STRUCT(arena, type)       ARENA_PUSH_ARRAY((arena), type, 1)

#ifdef BASIC_IMPLEMENTATION
// NOTE: we could store the mem_arena struct inside the allocation itself
struct mem_arena_t /* TODO reserve & commit */
{
    u8* base; // memory address
    u64 pos;  // offset from base gives memory address
    u64 cap;
    /* debug */
    // ...
};

mem_arena_t mem_arena_alloc(u64 size_in_bytes)
{
    mem_arena_t arena;
    arena.base = (u8*) mem_alloc(size_in_bytes); /* committing & reserving for now */
    arena.pos  = 0;
    arena.cap  = size_in_bytes;
    return arena;
}
mem_arena_t mem_arena_create(void* buf, u64 size_in_bytes)
{
    /* NOTE right now user needs to make sure arena fits inside buffer */
    mem_arena_t arena;
    arena.base = (u8*) buf;
    arena.pos  = 0;
    arena.cap  = size_in_bytes;
    return arena;
}
void* mem_arena_push(mem_arena_t* arena, u64 size)
{
    void* buf = NULL;
    if ((arena->pos + size) <= arena->cap)
    {
        buf = (void*) (arena->base + arena->pos);
        arena->pos = arena->pos + size;

        /* TODO handle committing here */
    }
    ASSERT(buf);
    return buf;
}
void mem_arena_pop_to(mem_arena_t* arena, void* buf)
{
    ASSERT(arena->base <= (u8*) buf);
    ASSERT((arena->base + arena->cap) >= (u8*) buf);
    u64 new_pos =  (u8*) buf - (u8*) arena->base;
    u64 diff    = arena->pos - new_pos;
    if (diff > 0)
    {
        arena->pos = new_pos;
        mem_zero_out(arena->base + new_pos, diff);
        /* TODO handle committing here */
    }
}
void mem_arena_pop_by(mem_arena_t* arena, u64 bytes)
{
    ASSERT((arena->pos - bytes) >= 0);
    mem_zero_out(arena->base + arena->pos, bytes);
    arena->pos -= bytes;
}
void mem_arena_free(mem_arena_t* arena)
{
    mem_free((void*) arena->base, arena->cap);
}
u64 mem_arena_get_pos(mem_arena_t* arena) { return arena->pos; }
#endif // BASIC_IMPLEMENTATION
