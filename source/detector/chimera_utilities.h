typedef int8_t  i8;
typedef int16_t i16;
typedef int32_t i32;
typedef int64_t i64;

typedef int8_t  s8;
typedef int16_t s16;
typedef int32_t s32;
typedef int64_t s64;

typedef uint8_t  u8;
typedef uint16_t u16;
typedef uint32_t u32;
typedef uint64_t u64;

typedef float   r32;
typedef double  r64;

typedef r32 f32;
typedef r64 f64;

typedef i8  b8;
typedef i16 b16;
typedef i32 b32;
typedef i64 b64;

#define bytes(n)     (n)
#define kilobytes(n) (bytes(n)*1024)
#define megabytes(n) (kilobytes(n)*1024)
#define gigabytes(n) (megabytes(n)*1024)

#define alloc_heap_memory(n) malloc(n)
#define free_heap_memory(m)  free(m)

#define foreach(i, lim) for(u64 (i) = 0; (i) < (u64)(lim); ++(i))

#define global static

