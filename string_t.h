#include "basic.h"

/* see https://joellaity.com/2020/01/31/string.html */
typedef struct string_t
{
    #define SHORT_MODE  ((len_ss & (u8(1)<<7)) == 0)
    #define LONG_MODE   ((cap & (u64(1)<<63)) == 1)

    union {
        struct /* small string */
        {
            u8   len_ss;  // MSB is a flag
            char buf[16];
        };
        struct /* long string */
        {
            u64   cap;    // MSB is a flag; real capacity goes up to 127
            u64   len_ls;
            char* ptr;
        };
    };

    u64   len()   { if (SHORT_MODE) { return len_ss; } else { return len_ls; } }
    char* c_str() { if (SHORT_MODE) { return buf; } else { return ptr; } }

    char& operator[](u32 idx)
    {
        if (true /* check for short string mode */)
        {
            if (idx < (sizeof(buf)/sizeof(char))) { /* reallocate into long string */ }
            return buf[idx];
        }
        else
        {
            /* ASSERT(idx < cap) */
            return ptr[idx];
        }
    }
} string_t;

// NOTE: not valid in MSVC C++
#define S(str) (string_t) { (sizeof(str)/sizeof(str[0])) - 1, str }

//string_t string_string0_to_string(char* str) { return {str, strlen(str)}; };
//string_t string_slice(string_t string, u32 from, u32 to);

// usage example:
// string_t hello_world = S("Hello World!");
