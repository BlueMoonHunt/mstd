#ifndef MSTD_IMPLEMENTATION
#define MSTD_IMPLEMENTATION

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>
#include <assert.h>
#include <stdalign.h>

#if defined(_MSC_VER)
#define COMPILER_MSVC 1
#elif defined(__clang__)
#define COMPILER_CLANG 1
#elif defined(__GNUC__)
#define COMPILER_GCC 1
#else
#warning "Unknown Compiler"
#endif

#if defined(_WIN32)
#define OS_WINDOWS 1
#elif defined(__linux__)
#define OS_LINUX 1
#elif defined(__APPLE__) && defined(__MACH__)
#define OS_MAC 1
#else
#warning "Unknown Operating System"
#endif

#if defined(_M_AMD64) || defined(__amd64__) || defined(__x86_64__)
#define ARCH_X64 1
#elif defined(_M_IX86) || defined(__i386__)
#define ARCH_X86 1
#elif defined(_M_ARM) || defined(__arm__) || defined(__aarch64__)
#define ARCH_ARM 1
#else
#warning "Unknown Architecture"
#endif

////////////////////////////////
// types

typedef uint8_t  u8;
typedef uint16_t u16;
typedef uint32_t u32;
typedef uint64_t u64;
typedef uintptr_t uaddress;

typedef int8_t  i8;
typedef int16_t i16;
typedef int32_t i32;
typedef int64_t i64;
typedef intptr_t iaddress;

typedef i8 b8;
typedef i16 b16;
typedef i32 b32;
typedef i64 b64;

typedef float f32;
typedef double f64;

#define enum_class(type, size_type) size_type

#if defined(__clang__) || defined(__GNUC__)
    typedef __uint128_t u128;
#elif defined(_MSC_VER) && defined(_M_X64)
    typedef struct {
        u64 low;
        u64 high;
    } _u128;
    #define u128 alignas(16) _u128
#else
    #error "No supported 128-bit integer implementation found."
#endif

////////////////////////////////
// math and utils

#define unreferenced_parameter(x) { (x) = (x); }

#define align_up_pow2(x,b)   (((x) + (b) - 1)&(~((b) - 1)))
#define align_down_pow2(x,b) ((x)&(~((b) - 1)))
#define align_up_pad_pow2(x,b)  ((0-(x)) & ((b) - 1))
#define is_pow2(x)          ((x)!=0 && ((x)&((x)-1))==0)
#define is_pow2_or_0(x)    ((((x) - 1)&(x)) == 0)

#define extract_bit(word, idx) (((word) >> (idx)) & 1)
#define extract8(word, pos)   (((word) >> ((pos) << 3)) & UINT8_MAX)
#define extract16(word, pos)  (((word) >> ((pos) << 4)) & UINT16_MAX)
#define extract32(word, pos)  (((word) >> ((pos) << 5)) & UINT32_MAX)

#define min(a, b) ((a) < (b)) ? a : b
#define max(a, b) ((a) > (b)) ? a : b
#define clamp(low, high, value) min(max(low, value), high)

inline uint64_t rotate_right_u64(uint64_t value, unsigned int shift);

#define KB(value) ((value) << 10)
#define MB(value) ((value) << 20)
#define GB(value) ((value) << 30)
#define TB(value) ((value) << 40)

////////////////////////////////
// memory

b32 mem_is_pow2(void* memory);
b32 mem_is_aligned(void* memory, u64 alignment);
b32 mem_compare(void* data1, void* data2, u64 count);
void mem_copy(void* destination, void* source, u64 size);
void mem_set(void* data, u8 value, u64 size);
#define mem_zero(data, size) mem_set(data, 0, size)

////////////////////////////////
// Arena

typedef enum ArenaFlag ArenaFlag;
enum ArenaFlag {
    arena_flag_none = 0,
    arena_flag_allocate_large_pages = 1 << 0,
};

typedef struct ArenaCreateInfo ArenaCreateInfo;
struct ArenaCreateInfo {
    u64 reserve_size;
    u64 commit_size;
    b32 allocate_large_pages;
};

typedef struct Arena Arena;
typedef struct ArenaTemp ArenaTemp;

Arena* arena_alloc(const ArenaCreateInfo create_info);
void arena_release(Arena* arena);
void* arena_push(Arena* arena, const u64 size, const u64 align);
void arena_clear(Arena* arena);

ArenaTemp* arena_temp_begin(Arena* arena);
void arena_temp_end(ArenaTemp* arena_temp);

#define arena_scratch(arena) \
    for (ArenaTemp *_temp_ = arena_temp_begin(arena); \
         _temp_ != NULL; \
         arena_temp_end(_temp_), _temp_ = NULL)

#define arena_push_array(arena, Type, count) (Type*)arena_push((arena), sizeof(Type)*(count), alignof(Type))
#define arena_push_struct(arena, Type) (Type*)arena_push((arena), sizeof(Type), alignof(Type))

////////////////////////////////
// OS
typedef struct SystemInfo SystemInfo;
struct SystemInfo {
    u64 page_size;
    u64 large_page_size;
    u64 allocation_granularity;
    u32 processors;
    u32 numa_nodes;
    u64 cache_line_size;
};

typedef struct ProcessInfo ProcessInfo;
struct ProcessInfo {
    u32 pid;
    u32 group_id;
    u64 affinity_mask;
    u64 page_file_limit;
};

SystemInfo* os_get_system_info(Arena* arena);
ProcessInfo* os_get_process_info(Arena* arena);

u64 os_get_page_size();
u64 os_get_large_page_size();
void* os_reserve(u64 size);
void* os_reserve_large(u64 size);
b32 os_commit(void* ptr, u64 size);
b32 os_commit_large(void* ptr, u64 size);
void os_decommit(void* ptr, u64 size);
void os_release(void* ptr, u64 size);
b64 os_get_random_bits();


////////////////////////////////
// random
typedef struct RandomNumberGenerator RandomNumberGenerator;
struct RandomNumberGenerator {
    u128 state;
    u128 inc;
};
inline void random_update_pcg_state(RandomNumberGenerator* rng);
void random_seed_pcg(RandomNumberGenerator* rng, const u64 state, const u64 stream);
u64 random_pcg(RandomNumberGenerator* rng);
u64 random_in_range(u64 low, u64 high, RandomNumberGenerator* rng);

#define random_from_os() os_get_random_bits()
#define random_seed(rng, state, stream) random_seed_pcg(rng,(state),(stream))
#define random(rng) random_pcg(rng)

////////////////////////////////
// strings: u8 u16

typedef struct str8 str8;
struct str8 {
    u8* str;
    u64 size;
};

typedef struct str16 str16;
struct str16 {
    u16* str;
    u64 size;
};
b32 char_is_space(u8 c);
b32 char_is_upper(u8 c);
b32 char_is_lower(u8 c);
b32 char_is_alphabet(u8 c);
b32 char_is_digit(u8 c, u32 base);
u8 char_to_lower(u8 c);
u8 char_to_upper(u8 c);
u64 cstr8_length(u8* c);
u64 cstr16_length(u16* c);

str8 _str8(u8* str, u64 size);
str16 _str16(u16* str, u64 size);

#define str8(literal) _str8((u8*)(literal), sizeof(literal) - 1)
#define str16(literal) _str16((u16*)(literal), sizeof(literal) >> 1 - 1)

str8 str8_from_cstr(const char* str);
str16 str16_from_cstr(const u16* str);

str8 str8_copy(Arena* arena, const str8 str);
str16 str16_copy(Arena* arena, const str16 str);

str8 str8_to_lower(Arena* arena, const str8 str);
str8 str8_to_upper(Arena* arena, const str8 str);
str8 str8_concat(Arena* arena, const str8 a, const str8 b);
b32 str8_equal(const str8 a, const str8 b);

#ifdef __cplusplus
}
#endif

#endif // MSTD_IMPLEMENTATION
