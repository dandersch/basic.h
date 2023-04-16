#pragma once

struct mem_arena_t;
typedef struct mem_arena_t mem_arena_t;

/* see
   https://www.gingerbill.org/article/2019/02/08/memory-allocation-strategies-002/
*/

/* api */
mem_arena_t*      mem_arena_reserve(u64 size_in_bytes);              /* create an arena w/ reserved memory       */
void*             mem_arena_push    (mem_arena_t* arena, u64 size);  /* push on the arena, committing if needed  */
void*             mem_arena_place   (mem_arena_t* arena, u64 size);  /* push on the arena w/o committing memory  */
mem_arena_t*      mem_arena_subarena(mem_arena_t* base, u64 size);   /* pushes on an arena w/o committing memory */
void              mem_arena_pop_to  (mem_arena_t* arena, void* buf); /* TODO doesn't decommit for now */
void              mem_arena_pop_by  (mem_arena_t* arena, u64 bytes);
void              mem_arena_free    (mem_arena_t* arena);

/* helper */
mem_arena_t*      mem_arena_default ();
u64               mem_arena_get_pos (mem_arena_t* arena);
#define ARENA_PUSH_ARRAY(arena, type, count) (type*) mem_arena_push((arena), sizeof(type)*(count))
#define ARENA_PUSH_STRUCT(arena, type)       ARENA_PUSH_ARRAY((arena), type, 1)
#define ARENA_BUFFER(arena, pos)             ((void*) ((((u8*) arena) + sizeof(mem_arena_t)) + pos))

#define ARENA_DEFAULT_RESERVE_SIZE MEGABYTES(4)

/* TODO scratch arenas */

#ifdef BASIC_IMPLEMENTATION
struct mem_arena_t
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
    arena->pos        = 0;
    arena->cap        = size_in_bytes;
    arena->commit_pos = 0;

    #ifdef BUILD_DEBUG
    arena->depth         = 0;
    arena->commit_amount = 0;
    #endif

    return arena;
}
mem_arena_t* mem_arena_subarena(mem_arena_t* base, u64 size)
{
    /* push on an arena w/o committing memory */
    mem_arena_t* subarena = NULL;
    if ((base->pos + size) <= base->cap)
    {
        subarena         = (mem_arena_t*) ARENA_BUFFER(base, base->pos);
        base->pos        = base->pos + size;
        base->commit_pos = base->pos + size; // NOTE we advance the commit_pos
                                             // here even though we don't commit
                                             // the memory - maybe we should
                                             // rename this member
    }
    else
    {
        UNREACHABLE("Couldn't fit subarena\n");
    }

    /* commit enough to write the subarena metadata */
    mem_commit((void*) subarena, sizeof(mem_arena_t)); // TODO handle error
    subarena->pos         = 0;
    subarena->cap         = size;
    subarena->commit_pos  = 0; // TODO should we set it to 0 or to sizeof(mem_arena_t)

    #ifdef BUILD_DEBUG
    subarena->depth         = base->depth + 1;
    subarena->commit_amount = 0;
    #endif

    return subarena;
}
void* mem_arena_push(mem_arena_t* arena, u64 size)
{
    void* buf   = NULL;
    u64 push_to = arena->pos + size;
    if (push_to <= arena->cap)
    {
        buf = ARENA_BUFFER(arena, arena->pos);
        arena->pos = push_to;

        /* handle committing */
        if (arena->commit_pos <= arena->pos)
        {
            b32 committed = mem_commit(ARENA_BUFFER(arena, arena->commit_pos), size);
            ASSERT(committed);
            arena->commit_pos += size;

            #ifdef BUILD_DEBUG
            arena->commit_amount += size;
            #endif
        }
    }
    else
    {
        UNREACHABLE("Overstepped capacity of arena");
    }
    ASSERT(buf);
    return buf;
}
void mem_arena_pop_to(mem_arena_t* arena, void* buf)
{
    ASSERT((uintptr_t) arena <= (uintptr_t) buf);
    ASSERT(((uintptr_t) arena + arena->cap) >= (uintptr_t) buf);
    u64 new_pos =  (u8*) buf - (u8*) ARENA_BUFFER(arena, 0);
    u64 diff    = arena->pos - new_pos;
    if (diff > 0)
    {
        arena->pos = new_pos;
        mem_zero_out(ARENA_BUFFER(arena, arena->pos), diff);

        /* TODO handle decommitting here */
    }
}
void mem_arena_pop_by(mem_arena_t* arena, u64 bytes)
{
    ASSERT((arena->pos - bytes) >= 0);
    mem_arena_pop_to(arena, ARENA_BUFFER(arena, arena->pos - bytes));
}
void mem_arena_free(mem_arena_t* arena)
{
    mem_free((void*) arena, arena->cap);
}

mem_arena_t* mem_arena_default()
{
    mem_arena_t* default_arena = mem_arena_reserve(ARENA_DEFAULT_RESERVE_SIZE);
    return default_arena;
}
u64 mem_arena_get_pos(mem_arena_t* arena)
{
    return arena->pos;
}
#endif // BASIC_IMPLEMENTATION
