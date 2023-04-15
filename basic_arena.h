#pragma once

/* TODO memory arena */
// see https://www.rfleury.com/p/untangling-lifetimes-the-arena-allocator

struct mem_arena_t;
typedef struct mem_arena_t mem_arena_t;

mem_arena_t*      mem_arena_alloc  (u64 size_in_bytes);
void*             mem_arena_push   (mem_arena_t* arena, u64 size);
void              mem_arena_pop_to (mem_arena_t* arena, void* buf);
void              mem_arena_pop_by (mem_arena_t* arena, u64 bytes);
void              mem_arena_free   (mem_arena_t* arena);
u64               mem_arena_get_pos(mem_arena_t* arena);
mem_arena_t*      mem_arena_reserve(u64 size_in_bytes); /* TODO */

// TODO replace one with the other
mem_arena_t*      mem_arena_create  (void* buf, u64 size_in_bytes);
mem_arena_t*      mem_arena_subarena(mem_arena_t* base, u64 size); /* like a push w/o committing memory */

/* helper */
// mem_arena_t* mem_arena_default();
#define ARENA_PUSH_ARRAY(arena, type, count) (type*) mem_arena_push((arena), sizeof(type)*(count))
#define ARENA_PUSH_STRUCT(arena, type)       ARENA_PUSH_ARRAY((arena), type, 1)
#define ARENA_BUFFER(arena, pos)             ((void*) ((((u8*) arena) + sizeof(mem_arena_t)) + pos))

/* TODO scratch arenas */

#ifdef BASIC_IMPLEMENTATION
// TODO store the mem_arena struct inside the allocation itself and take out the u8* base
struct mem_arena_t /* TODO reserve & commit */
{
    u64 pos;
    u64 cap;
    u64 commit_pos;

    #ifdef BUILD_DEBUG
    u32 depth;         // base arena has depth 0
    u64 commit_amount;
    #endif
};

mem_arena_t* mem_arena_reserve(u64 size_in_bytes)
{
    mem_arena_t* arena = (mem_arena_t*) mem_reserve(NULL, size_in_bytes + sizeof(mem_arena_t));

    /* commit enough to write the arena metadata */
    mem_commit((void*) arena, sizeof(mem_arena_t)); // TODO handle error
    arena->pos  = 0;
    arena->cap  = size_in_bytes;
    // TODO record the commit pos into the arena

    return arena;
}
mem_arena_t* mem_arena_alloc(u64 size_in_bytes)
{
    /* committing & reserving for now */
    mem_arena_t* arena = (mem_arena_t*) mem_alloc(size_in_bytes + sizeof(mem_arena_t));
    arena->pos         = 0;
    arena->cap         = size_in_bytes;
    return arena;
}
mem_arena_t* mem_arena_create(void* buf, u64 size_in_bytes)
{
    /* NOTE right now user needs to make sure arena fits inside buffer */
    mem_arena_t* arena = (mem_arena_t*) buf;
    arena->pos         = 0;
    arena->cap         = size_in_bytes;
    return arena;
}
mem_arena_t* mem_arena_subarena(mem_arena_t* base, u64 size)
{
    /* push on an arena w/o committing memory */
    mem_arena_t* subarena = NULL;
    if ((base->pos + size) <= base->cap)
    {
        subarena  = (mem_arena_t*) ARENA_BUFFER(base, base->pos);
        base->pos = base->pos + size;
    }
    else
    {
        UNREACHABLE("Couldn't fit subarena\n");
    }

    /* commit enough to write the subarena metadata */
    mem_commit((void*) subarena, sizeof(mem_arena_t)); // TODO handle error
    subarena->pos         = 0;
    subarena->cap         = size;
    // TODO record the commit pos into the arena

    return subarena;
}
void* mem_arena_push(mem_arena_t* arena, u64 size)
{
    void* buf = NULL;
    if ((arena->pos + size) <= arena->cap)
    {
        buf = ARENA_BUFFER(arena, arena->pos);
        arena->pos = arena->pos + size;

        /* TODO handle committing here */
    }
    ASSERT(buf);
    return buf;
}
void mem_arena_pop_to(mem_arena_t* arena, void* buf)
{
    ASSERT((uintptr_t) arena <= (uintptr_t) buf);
    ASSERT(((uintptr_t) arena + arena->cap) >= (uintptr_t) buf);
    u64 new_pos =  (u8*) buf - (u8*) arena;
    u64 diff    = arena->pos - new_pos;
    if (diff > 0)
    {
        arena->pos = new_pos;
        mem_zero_out(arena + new_pos, diff);
        /* TODO handle committing here */
    }
}
void mem_arena_pop_by(mem_arena_t* arena, u64 bytes)
{
    ASSERT((arena->pos - bytes) >= 0);
    mem_zero_out(arena + arena->pos, bytes);
    arena->pos -= bytes;
}
void mem_arena_free(mem_arena_t* arena)
{
    mem_free((void*) arena, arena->cap);
}
u64 mem_arena_get_pos(mem_arena_t* arena)
{
    return arena->pos;
}
#endif // BASIC_IMPLEMENTATION
