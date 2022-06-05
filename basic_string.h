
typedef struct string_t {
  char* buf;
  u32   len;
} string_t;

#define S(string) (string_t){X, sizeof(string)/sizeof(string[0]) - 1}

//string_t string_slice(string_t string, u32 from, u32 to);

// usage example:
// string_t hello_world = S("Hello World!");

// TODO printf replacement?
