#pragma once

/* common macros */
#define ARRAY_COUNT(arr)  (sizeof(arr)/sizeof(arr[0]))

#if !defined(COMPILER_MSVC)
    #define OFFSET_OF(type, member) __builtin_offsetof(type, member)
#else
    #define OFFSET_OF(s,m) ((size_t)&(((s*)0)->m))
#endif

#define KILOBYTES(val) (         (val) * 1024LL)
#define MEGABYTES(val) (KILOBYTES(val) * 1024LL)
#define GIGABYTES(val) (MEGABYTES(val) * 1024LL)
#define TERABYTES(val) (GIGABYTES(val) * 1024LL)

/* alignment / power-of-2 macros */
#define CHECK_IF_POW2(x)          (((x)&((x)-1))==0)
// some bit hacking to get the next alignment value.
// e.g.: NEXT_ALIGN_POW2(12,16) == 16, NEXT_ALIGN_POW2(18,16) == 32
#define NEXT_ALIGN_POW2(x,align) (((x) + (align) - 1) & ~((align) - 1))
#define PREV_ALIGN_POW2(x,align) ((x) & ~((align) - 1))

/* align e.g. a memory address to its next page boundary */
#define ALIGN_TO_NEXT_PAGE(val) NEXT_ALIGN_POW2((uintptr_t) val, mem_pagesize())
#define ALIGN_TO_PREV_PAGE(val) PREV_ALIGN_POW2((uintptr_t) val, mem_pagesize())

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
u64   mem_pagesize(); /* pagesize in bytes */

/* helper macros */
#define MEM_ZERO_OUT_STRUCT(s) mem_zero_out((s), sizeof(*(s)))
#define MEM_ZERO_OUT_ARRAY(a)  mem_zero_out((a), sizeof(a))

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
  u64  mem_pagesize()
  {
      SYSTEM_INFO si;
      GetSystemInfo(&si);
      return si.dwPageSize;
  }
#elif defined(PLATFORM_LINUX) || defined(PLATFORM_MACOS)
  #include <stdlib.h>   /* for malloc */
  #include <string.h>   /* for memset, memcpy, memcmp */
  #include <sys/mman.h> /* for mmmap, mprotect, madvise */
  #include <unistd.h>   /* for getpagesize() */
  #include <errno.h>    /* TODO only for debugging */
  /* NOTE: right now we only use mmap & mprotect for reserving & committing
     memory and no malloc(), because it seems that we can't easily mix e.g. calls like void*
     buf=malloc w/ subsequent calls to munmap(buf) or calls to buf = mmap with
     subsequent calls to free(buf) */
  /* TODO find out if malloc is slower/faster than mmap & mprotect */
  // TODO use mremap() with MREMAP_MAYMOVE for moving memory (use same protection flags)
  // see http://www.smallbulb.net/2018/809-reserve-virtual-memory
  void* mem_reserve(u64 size)
  {
      /* TODO try to get a fixed memory address in debug builds */
      //#if BUILD_DEBUG
      //void* mem = mmap(GIGABYTES(256), size, PROT_NONE, MAP_PRIVATE | MAP_FIXED | MAP_ANONYMOUS, -1, 0);
      //#else
      void* mem = mmap(NULL, size, PROT_NONE, MAP_PRIVATE | MAP_ANONYMOUS, -1, 0);
      //#endif
      //void* mem = malloc(size);
      //mem_zero_out(mem, size);
      return mem;
  }
  b32 mem_commit(void* ptr, u64 size)
  {
      /* TODO breaking compat with 32bit architecture here because we use u64 as a pointer */
      /* TODO check if VirtualAlloc has a similar requirement  */

      //const uintptr_t PAGESIZE = sysconf(_SC_PAGE_SIZE);
      ////printf("%u\n", sizeof(uintptr_t));
      //uintptr_t next_page_addr = (((uintptr_t) ptr) & ~(PAGESIZE-1));
      //uintptr_t prev_page_addr = next_page_addr - PAGESIZE;
      //uintptr_t offset_from_prev_page = ((uintptr_t) ptr) - next_page_addr;
      //ptr  = (void*) next_page_addr;
      //size = size + offset_from_prev_page;

      /* TODO mprotect fails if addr is not aligned to a page boundary and if
       * length (size) is not a multiple of the page size as returned by sysconf(). */
      uintptr_t prev_page_boundary           = ALIGN_TO_PREV_PAGE(ptr);
      uintptr_t next_page_boundary           = ALIGN_TO_NEXT_PAGE(ptr);
      uintptr_t size_as_multiple_of_pagesize = ALIGN_TO_NEXT_PAGE(size);
      ptr  = (void*) prev_page_boundary;
      size = size_as_multiple_of_pagesize;

      uintptr_t commit_begin = (uintptr_t) ptr;
      uintptr_t commit_end   = (uintptr_t) ptr + size;
      commit_begin           = ALIGN_TO_PREV_PAGE(commit_begin);
      commit_end             = ALIGN_TO_NEXT_PAGE(commit_end);
      size                   = commit_end - commit_begin;
      ptr                    = (void*) commit_begin;

      /* TODO: overcommitting here by 1 pagesize solves some edge cases - find out why */
      size                  += mem_pagesize();

      ASSERT(!(size % mem_pagesize()));
      ASSERT(!(((uintptr_t) ptr) % mem_pagesize()));

      i32 i = mprotect(ptr, size, PROT_READ | PROT_WRITE);
      ASSERT(i == 0);
      return (i == 0);
  }
  void* mem_alloc(u64 size)
  {
      void* mem  = mem_reserve(size);
      b32 result = mem_commit(mem, size);
      return mem;
  }
  b32 mem_decommit(void* ptr, u64 size)
  {
      /* NOTE: This doesn't seem to do anything and is not supported by all compilers */
      //i32 result = madvise(ptr, size, MADV_DONTNEED);

      i32 result     = mprotect(ptr, size, PROT_NONE);
      return (result == 0);
  }
  void mem_release(void* ptr,  u64 size)
  {
      munmap(ptr, size);
  }
  void mem_free(void* ptr,  u64 size)
  {
      mem_decommit(ptr, size);
      mem_release(ptr, size);
  }
  void mem_zero_out(void* ptr, u64 size)
  {
      memset(ptr, 0, size);
  }
  b32 mem_equal(void* buf_a, void* buf_b, u64 size_in_bytes)
  {
      return (memcmp(buf_a, buf_b, size_in_bytes) == 0);
  }
  void mem_copy(void* dst, void* src, u64 size_in_bytes)
  {
      memcpy(dst, src, size_in_bytes);
  }
  u64  mem_pagesize()
  {
      return sysconf(_SC_PAGE_SIZE);
  }
#endif
#endif // BASIC_IMPLEMENTATION

/* TODO FILE OPERATIONS  */
