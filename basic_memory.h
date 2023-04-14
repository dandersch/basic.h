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

/* TYPE_OF macro for all compilers except MSVC in C-mode */
#if defined(LANGUAGE_CPP)
  #define TYPE_OF(e) decltype(e)
#elif !defined(COMPILER_MSVC)
  /* NOTE: non standard gcc extension, but works everywhere except MSVC w/ C */
  #define TYPE_OF(e) __typeof__(e)
#else
  #undef TYPE_OF
  #define TYPE_OF(e) STATIC_ASSERT(0, "TYPE_OF macro doesn't work w/ MSVC in C");
#endif

/* memory is guaranteed to be initialized to zero */
void* mem_reserve (u64 size);
b32   mem_commit  (void* ptr,   u64 size);
void* mem_alloc   (u64 size);               /* reserve & commit  */
b32   mem_decommit(void* ptr,   u64 size);
void  mem_release (void* ptr,   u64 size);
void  mem_free    (void* ptr,   u64 size);  /* decommit & release */
void  mem_zero_out(void* ptr,   u64 size);
b32   mem_equal   (void* buf_a, void* buf_b, u64 size_in_bytes);
void  mem_copy    (void* dst,   void* src,   u64 size_in_bytes);

/* helper macros */
#define MEM_ZERO_OUT_STRUCT(s) mem_zero_out((s), sizeof(*(s)))
#define MEM_ZERO_OUT_ARRAY(a)  mem_zero_out((a), sizeof(a))

/* TODO scratch arena */

#ifdef BASIC_IMPLEMENTATION
#if defined(PLATFORM_WIN32)
  #include <windows.h>
  void* mem_reserve(u64 size)
  {
      void* mem = VirtualAlloc(0, size, MEM_RESERVE, PAGE_READWRITE);
      return mem;
  }
  b32 mem_commit(void* ptr, u64 size)
  {
      b32 result = (VirtualAlloc(ptr, size, MEM_COMMIT, PAGE_READWRITE) != 0);
      return result;
  }
  void* mem_alloc(u64 size)
  {
      void* mem = VirtualAlloc(0, size, MEM_COMMIT, PAGE_READWRITE);
      return mem;
  }
  b32 mem_decommit(void* ptr, u64 size)
  {
      return VirtualFree(ptr, size, MEM_DECOMMIT);
  }
  void mem_release(void* ptr,  u64 size)
  {
      VirtualFree(ptr, 0, MEM_RELEASE);
  }
  void mem_free(void* ptr, u64 size)
  {
      VirtualFree(ptr, size, MEM_DECOMMIT);
      VirtualFree(ptr, 0, MEM_RELEASE);
  }
  void mem_zero_out(void* ptr, u64 size)
  {
      SecureZeroMemory(ptr, size);
  }
  b32 mem_equal(void* buf_a, void* buf_b, u64 size_in_bytes)
  {
      /* NOTE RtlCompareMemory works differently from memcmp (it returns number
       * of bytes matched), so we limit our memory compare function to just test
       * for equality right now */
      return (RtlCompareMemory(buf_a, buf_b, size_in_bytes) == size_in_bytes);
  }
  void mem_copy(void* dst, void* src, u64 size_in_bytes)
  {
      RtlCopyMemory(dst, src, size_in_bytes);
  }
#elif defined(PLATFORM_LINUX) || defined(PLATFORM_MACOS)
  #include <stdlib.h>   /* for malloc */
  #include <string.h>   /* for memset */
  #include <sys/mman.h> /* for mmmap, mprotect, madvise */
  /* NOTE: right now we only use mmap & mprotect for reserving & committing
     memory, because it seems that we can't easily mix e.g. calls like void*
     buf=malloc w/ subsequent calls to munmap(buf) or calls to buf = mmap with
     subsequent calls to free(buf) */
  /* TODO find out if malloc is slower/faster than mmap & mprotect */
  // TODO use mremap() with MREMAP_MAYMOVE for moving memory (use same protection flags)
  // see http://www.smallbulb.net/2018/809-reserve-virtual-memory
  void* mem_reserve(u64 size)
  {
      // TODO find out what MAP_PRIVATE does
      void* mem = mmap(NULL, size, PROT_NONE, MAP_PRIVATE | MAP_ANONYMOUS, -1, 0);
      //void* mem = malloc(size);
      //mem_zero_out(mem, size);
      return mem;
  }
  b32 mem_commit(void* ptr, u64 size)
  {
      /* TODO breaking compat with 32bit architecture here because we use u64 as a pointer */
      /* TODO mprotect fails if addr is not multiple of the page size as returned by sysconf() */
      /* TODO check if VirtualAlloc has a similar requirement  */
      const u64 PAGESIZE = 4096; // TODO get this programmatically
      u64 next_page_addr = (((u64)ptr) & ~(PAGESIZE-1));
      u64 prev_page_addr = next_page_addr - 4096;
      u64 offset_from_prev_page = ((u64) ptr) - next_page_addr;
      ptr  = (void*) next_page_addr;
      size = size + offset_from_prev_page;

      i32 i = mprotect(ptr, size, PROT_READ | PROT_WRITE);
      ASSERT(i == 0);
      //mem_zero_out(ptr, size); // TODO maybe we don't have to zero out here because mmap does it for us ?
      return (i == 0);
  }
  void* mem_alloc(u64 size)
  {
      void* mem  = mem_reserve(size);
      b32 result = mem_commit(mem, size);
      //void* mem = malloc(size);
      //mem_zero_out(mem, size);
      return mem;
  }
  b32 mem_decommit(void* ptr, u64 size)
  {
      /* NOTE: This doesn't seem to do anything and is not supported by all compilers */
      //i32 result = madvise(ptr, size, MADV_DONTNEED);

      i32 result     = mprotect(ptr, size, PROT_NONE);
      return (result == 0);
  }
  void mem_release(void* ptr,  u64 size) { munmap(ptr, size); }
  void mem_free(void* ptr,  u64 size)
  {
      // free(ptr);
      mem_decommit(ptr, size);
      mem_release(ptr, size);
  }
  void mem_zero_out(void* ptr, u64 size) { memset(ptr, 0, size); }
  b32 mem_equal(void* buf_a, void* buf_b, u64 size_in_bytes)
  {
      return (memcmp(buf_a, buf_b, size_in_bytes) == 0);
  }
  void mem_copy(void* dst, void* src, u64 size_in_bytes)
  {
      memcpy(dst, src, size_in_bytes);
  }
#endif
#endif // BASIC_IMPLEMENTATION

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

/* TODO FILE OPERATIONS  */
