#pragma once

/* implementation of a dynamic array that actually just reserves a huge chunk of
 * memory and commits subchunks of it when it reaches capacity, i.e. no
 * reallocations take place (for now).
 *
 * Downside to this approach is that you can't have a huge number of dynamic
 * arrays at the same time in the program. E.g.: For a RESERVE_SIZE of 4 GB:
 *
 *                              4GB =      8589934592 bits reserved per dynamic array
 * 2**48 bits virtual address space = 281474976710656 bits available in x64
 *     281474976710656 / 8589934592 =           32768 dynamic arrays possible
 */

/* Example usage code:

       i32_array vals = dynarr_create(i32);
       dynarr_put(vals, 2);
       dynarr_put(vals, 3);
       dynarr_put(vals, 5);
       for (int i=0; i < dynarr_len(vals); ++i) // DYNARR_FOREACH
       {
           printf("%d ", array[i]);
       }
*/


/* NOTE 4GB is max for 32bit  */
#define DYNARR_RESERVE_SIZE     MEGABYTES(1) // maybe let the user decide for each array and store in header
#define DYNARR_INITIAL_CAPACITY 100          // nr of elements that can fit after initial commit

/* api */
void*   dynarr_create (u64 elem_size);

#define dynarr_push(arr, val) (dynarr_maybe_grow_by_n(arr,1), (arr)[dynarr_header(arr)->len++] = (val))
#define dynarr_back(arr)      ((arr)[dynarr_header(arr)->len-1])

#define dynarr_len(a)       ((a) ? (dynarr_header(a))->len : 0)
#define dynarr_pop(a)       (dynarr_header(a)->len--, (a)[dynarr_header(a)->len])
#define dynarr_free(a)      ((void) ((a) ? realloc(stbds_header(a),0) : 0), (a)=NULL)

#define dynarr_del(a,i)     dynarr_arrdeln(a,i,1)
#define dynarr_insert(a)    (stbds_arrinsn((a),(i),1), (a)[i]=(v))

#ifdef BASIC_IMPLEMENTATION
typedef struct dynarr_header_t
{
    u64  len;
    u64  cap; /* == commited memory */
    u64  elem_size;
} dynarr_header_t;

dynarr_header_t* dynarr_header(void* arr)
{
   dynarr_header_t* header = (dynarr_header_t*) arr - 1;
   return header;
}

void dynarr_grow_by_n(void* arr, u64 n)
{
    dynarr_header_t* header = dynarr_header(arr);
    void* array_end         = ((u8*) arr) + (header->cap * header->elem_size);
    header->cap             = header->cap + n;
    u64 grow_size           = header->elem_size * n;
    mem_commit(array_end, grow_size);   /* TODO error handling */
}

#define GROW_BY_FACTOR 100
void dynarr_maybe_grow_by_n(void* arr, u64 n)
{
    dynarr_header_t* header = dynarr_header(arr);
    if ((header->len + n) >= header->cap)
    {
        dynarr_grow_by_n(arr, n * GROW_BY_FACTOR);
    }
}

void* dynarr_create(u64 elem_size)
{
    dynarr_header_t* header = (dynarr_header_t*) mem_reserve(NULL, DYNARR_RESERVE_SIZE);
    mem_commit(header, (DYNARR_INITIAL_CAPACITY * elem_size) + sizeof(dynarr_header_t));   /* TODO error handling */
    header->len       = 0;
    header->cap       = DYNARR_INITIAL_CAPACITY;
    header->elem_size = elem_size;
    void* buf_after_header =  ((u8*) header) + sizeof(dynarr_header_t);
    return buf_after_header;
}
#endif // BASIC_IMPLEMENTATION
